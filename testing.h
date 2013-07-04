#ifndef TESTING_H
#define TESTING_H

#define UNW_LOCAL_ONLY

#include <libunwind.h>
#include <vector>
#include <tr1/functional>
#include <stdexcept>
#include <boost/any.hpp>
#include <iostream>

#include "unicode.h"
#include "exceptions.h"
#include "logging.h"

class TestCase {
public:
    virtual ~TestCase() {}

    virtual void set_up() {}
    virtual void tear_down() {}

    std::pair<unicode, int> get_file_and_line() {
        unw_cursor_t cursor;
        unw_context_t uc;
        unw_word_t offp, ip, sp;

        unw_getcontext(&uc);
        unw_init_local(&cursor, &uc);

        unw_step(&cursor);
        unw_step(&cursor);

        char name[256];
        name[0] = '\0';
        unw_get_proc_name(&cursor, name, 256, &offp);
        unw_get_reg(&cursor, UNW_REG_IP, &ip);
        unw_get_reg(&cursor, UNW_REG_SP, &sp);

        unicode command = _u("/usr/bin/addr2line -C -e {0} -f -i {1:x}").format(os::path::exe_path(), (long)ip);

        FILE* f = popen(command.encode().c_str(), "r");
        if(f) {
            char buf[256];
            fgets(buf, 256, f);
            fgets(buf, 256, f);
            if(buf[0] != '?') {
                unicode file = _u(buf).split(":")[0];
                int line = std::stoi(_u(buf).split(":")[1].encode());
                return std::make_pair(file, line);
            }
        }

        return std::make_pair("", -1);
    }

    template<typename T, typename U>
    void assert_equal(T expected, U actual) {
        if(expected != actual) {
            auto file_and_line = get_file_and_line();
            throw AssertionError(file_and_line, unicode("{0} does not match {1}").format(actual, expected).encode());
        }
    }

    template<typename T>
    void assert_true(T actual) {
        if(!bool(actual)) {
            auto file_and_line = get_file_and_line();
            throw AssertionError(file_and_line, unicode("{0} is not true").format(bool(actual) ? "true" : "false").encode());
        }
    }

    template<typename T>
    void assert_false(T actual) {
        if(bool(actual)) {
            auto file_and_line = get_file_and_line();
            throw AssertionError(file_and_line, unicode("{0} is not false").format(bool(actual) ? "true" : "false").encode());
        }
    }

    template<typename T, typename U, typename V>
    void assert_close(T expected, U actual, V difference) {
        if(actual < expected - difference ||
           actual > expected + difference) {
            auto file_and_line = get_file_and_line();
            throw AssertionError(file_and_line, unicode("{0} is not close enough to {1}").format(actual, expected).encode());
        }
    }

    template<typename T>
    void assert_is_null(T* thing) {
        if(thing != nullptr) {
            auto file_and_line = get_file_and_line();
            throw AssertionError(file_and_line, "Pointer was not NULL");
        }
    }

    template<typename T>
    void assert_is_not_null(T* thing) {
        if(thing == nullptr) {
            auto file_and_line = get_file_and_line();
            throw AssertionError(file_and_line, "Pointer was unexpectedly NULL");
        }
    }
};

class TestRunner {
public:
    template<typename T, typename U>
    void register_case(std::vector<U> methods, std::vector<std::string> names) {
        std::shared_ptr<T> instance(new T());

        instances_.push_back(instance); //Hold on to it

        for(std::string name: names) {
            names_.push_back(name);
        }

        for(U& method: methods) {
            std::tr1::function<void()> func = std::tr1::bind(method, instance.get());
            tests_.push_back([=]() {
                instance->set_up();
                func();
                instance->tear_down();
            });
        }
    }

    int32_t run() {
        int failed = 0;
        int ran = 0;
        int crashed = 0;
        std::cout << std::endl << "Running " << tests_.size() << " tests" << std::endl << std::endl;
        for(std::tr1::function<void ()> test: tests_) {
            try {
                std::string output = "    " + names_[ran];

                for(int i = output.length(); i < 71; ++i) {
                    output += " ";
                }

                std::cout << output;
                test();
                std::cout << "\033[32m" << "   OK   " << std::endl;
            } catch(AssertionError& e) {
                std::cout << "\033[33m" << " FAILED " << std::endl;
                std::cout << "        " << e.what() << std::endl;
                if(!e.file.empty()) {
                    std::cout << "        " << e.file << ":" << e.line << std::endl;
                }
                ++failed;
            } catch(std::exception& e) {
                std::cout << "\033[31m" << " EXCEPT " << std::endl;
                std::cout << "        " << e.what() << std::endl;
                ++crashed;
            }
            std::cout << "\033[37m";
            ++ran;
        }

        std::cout << "-----------------------" << std::endl;
        if(!failed && !crashed) {
            std::cout << "All tests passed" << std::endl << std::endl;
        } else {
            if(failed) {
                std::cout << failed << " tests failed";
            }
            if(crashed) {
                if(failed) {
                    std::cout << ", ";
                }
                std::cout << crashed << " tests crashed";
            }
            std::cout << std::endl << std::endl;
        }

        return failed;
    }

private:
    std::vector<boost::any> instances_;
    std::vector<std::tr1::function<void()> > tests_;
    std::vector<std::string> names_;
};


#endif // TESTING_H
