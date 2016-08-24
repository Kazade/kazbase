#ifndef KAZBASE_EXCEPTIONS_H
#define KAZBASE_EXCEPTIONS_H

#include <typeinfo>
#include <stdexcept>
#include <string>

#include "unicode.h"

class NotImplementedError : public std::runtime_error {
public:
    NotImplementedError(const unicode& file, const int line):
        std::runtime_error(_u("Not implemented error at: {0}:{1}").format(file, line).encode()) {}
};

class RuntimeError : public std::runtime_error {
public:
    RuntimeError(const unicode& what):
        std::runtime_error(what.encode()) {}
};

class IOError : public std::runtime_error {
public:
    IOError(const char* what):
        std::runtime_error(what) {}

    IOError(const unicode& what):
        std::runtime_error(what.encode()) {}

    IOError(const std::string& what):
        std::runtime_error(what) { }
};

class LogicError : public std::logic_error {
public:
    LogicError(const unicode& what):
        std::logic_error(what.encode()) {}
};

class TypeError : public std::runtime_error {
public:
    TypeError(const unicode& what):
        std::runtime_error(what.encode()) {}
};

class ValueError : public std::runtime_error {
public:
    ValueError(const unicode& what):
        std::runtime_error(what.encode()) {}
};

class AssertionError : public LogicError {
public:
    AssertionError(const unicode& what):
        LogicError(what),
        file(""),
        line(-1) {
    }

    AssertionError(const std::pair<unicode, int> file_and_line, const unicode& what):
        LogicError(what),
        file(file_and_line.first),
        line(file_and_line.second) {

    }

    ~AssertionError() noexcept (true) {

    }

    unicode file;
    int line;
};

class FileNotFoundError : public IOError {
public:
    FileNotFoundError(const unicode& path):
        IOError(_u("Unable to find file: {0}").format(path).encode()) {}
};

class NetworkError : public IOError {
public:
    NetworkError(const unicode& what):
        IOError(what) {}
};

template<typename T>
class DoesNotExist : public std::runtime_error {
public:
    DoesNotExist():
        std::runtime_error(std::string(typeid(T).name()) + " instance does not exist") {
    }

    template<typename U>
    DoesNotExist(const U& what):
        std::runtime_error(_u("{0} instance does not exist: {1}").format(typeid(T).name(), what).encode()) {
    }
};

#endif

