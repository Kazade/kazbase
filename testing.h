#ifndef TESTING_H
#define TESTING_H

#include <vector>
#include <tr1/functional>
#include <stdexcept>
#include <boost/any.hpp>
#include <iostream>

#include "kglt/kazbase/unicode.h"
#include "kglt/kazbase/exceptions.h"

class TestCase {
public:
    virtual ~TestCase() {}

    virtual void set_up() {}
    virtual void tear_down() {}

    template<typename T, typename U>
    void assert_equal(T expected, U actual) {
        if(expected != actual) {
            throw AssertionError(unicode("{0} does not match {1}").format(actual, expected).encode());
        }
    }

    template<typename T>
    void assert_true(T actual) {
        if(!bool(actual)) {
            throw AssertionError(unicode("{0} is not true").format(actual).encode());
        }
    }

    template<typename T>
    void assert_false(T actual) {
        if(bool(actual)) {
            throw AssertionError(unicode("{0} is not false").format(actual).encode());
        }
    }

    template<typename T, typename U, typename V>
    void assert_close(T expected, U actual, V difference) {
        if(actual < expected - difference ||
           actual > expected + difference) {
            throw AssertionError(unicode("{0} is not close enough to {1}").format(actual, expected).encode());
        }
    }
};

class TestRunner {
public:
    template<typename T, typename U>
    void register_case(std::vector<U> methods, std::vector<std::string> names) {
        std::tr1::shared_ptr<T> instance(new T());

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
        std::cout << "Running " << tests_.size() << " tests" << std::endl;
        for(std::tr1::function<void ()> test: tests_) {
            try {
                std::cout << "\t" << names_[ran] << std::endl;
                test();
            } catch(AssertionError& e) {
                std::cout << "\t\t" << e.what() << std::endl;
                ++failed;
            } catch(...) {
                ++failed;
            }
            ++ran;
        }

        if(!failed) {
            std::cout << "All tests passed" << std::endl;
        } else {
            std::cout << failed << " tests failed" << std::endl;
        }

        return failed;
    }

private:
    std::vector<boost::any> instances_;
    std::vector<std::tr1::function<void()> > tests_;
    std::vector<std::string> names_;
};


#endif // TESTING_H
