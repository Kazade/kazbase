#ifndef TESTING_H
#define TESTING_H

#define UNW_LOCAL_ONLY

#include <vector>
#include <functional>
#include <stdexcept>
#include <boost/any.hpp>
#include <iostream>

#include "unicode.h"
#include "exceptions.h"
#include "logging.h"
#include "file_utils.h"

#define assert_equal(expected, actual) _assert_equal((expected), (actual), __FILE__, __LINE__)
#define assert_false(actual) _assert_false((actual), __FILE__, __LINE__)
#define assert_true(actual) _assert_true((actual), __FILE__, __LINE__)
#define assert_close(expected, actual, difference) _assert_close((expected), (actual), (difference), __FILE__, __LINE__)
#define assert_is_null(actual) _assert_is_null((actual), __FILE__, __LINE__)
#define assert_is_not_null(actual) _assert_is_not_null((actual), __FILE__, __LINE__)

class TestCase {
public:
    virtual ~TestCase() {}

    virtual void set_up() {}
    virtual void tear_down() {}

    template<typename T, typename U>
    void _assert_equal(T expected, U actual, unicode file, int line) {
        if(expected != actual) {
            auto file_and_line = std::make_pair(file, line);
            throw AssertionError(file_and_line, unicode("{0} does not match {1}").format(actual, expected).encode());
        }
    }

    template<typename T>
    void _assert_true(T actual, unicode file, int line) {
        if(!bool(actual)) {
            auto file_and_line = std::make_pair(file, line);
            throw AssertionError(file_and_line, unicode("{0} is not true").format(bool(actual) ? "true" : "false").encode());
        }
    }

    template<typename T>
    void _assert_false(T actual, unicode file, int line) {
        if(bool(actual)) {
            auto file_and_line = std::make_pair(file, line);
            throw AssertionError(file_and_line, unicode("{0} is not false").format(bool(actual) ? "true" : "false").encode());
        }
    }

    template<typename T, typename U, typename V>
    void _assert_close(T expected, U actual, V difference, unicode file, int line) {
        if(actual < expected - difference ||
           actual > expected + difference) {
            auto file_and_line = std::make_pair(file, line);
            throw AssertionError(file_and_line, unicode("{0} is not close enough to {1}").format(actual, expected).encode());
        }
    }

    template<typename T>
    void _assert_is_null(T* thing, unicode file, int line) {
        if(thing != nullptr) {
            auto file_and_line = std::make_pair(file, line);
            throw AssertionError(file_and_line, "Pointer was not NULL");
        }
    }

    template<typename T>
    void _assert_is_not_null(T* thing, unicode file, int line) {
        if(thing == nullptr) {
            auto file_and_line = std::make_pair(file, line);
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
            std::function<void()> func = std::bind(method, instance.get());
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
        for(std::function<void ()> test: tests_) {
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
                    if(os::path::exists(e.file)) {
                        std::vector<unicode> lines = file_utils::read_lines(e.file);
                        if(e.line <= lines.size()) {
                            std::cout << lines[e.line - 1].encode() << std::endl << std::endl;
                        }
                    }
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
    std::vector<std::function<void()> > tests_;
    std::vector<std::string> names_;
};


#endif // TESTING_H
