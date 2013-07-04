#ifndef KAZBASE_EXCEPTIONS_H
#define KAZBASE_EXCEPTIONS_H

#include <typeinfo>
#include <stdexcept>
#include <string>
#include <boost/lexical_cast.hpp>

#include "logging.h"

class NotImplementedError : public std::runtime_error {
public:
    NotImplementedError(const std::string& file, const int line):
        std::runtime_error("Not implemented error at: " + file + ":" + boost::lexical_cast<std::string>(line)) {}
};

class RuntimeError : public std::runtime_error {
public:
    RuntimeError(const std::string& what):
        std::runtime_error(what) {}
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
    LogicError(const std::string& what):
        std::logic_error(what) {}
};

class TypeError : public std::runtime_error {
public:
    TypeError(const std::string& what):
        std::runtime_error(what) {}
};

class ValueError : public std::runtime_error {
public:
    ValueError(const std::string& what):
        std::runtime_error(what) {}
};

class AssertionError : public LogicError {
public:
    AssertionError(const std::string& what):
        LogicError(what),
        file(""),
        line(-1) {
    }

    AssertionError(const std::pair<unicode, int> file_and_line, const std::string& what):
        LogicError(what),
        file(file_and_line.first),
        line(file_and_line.second) {

    }

    unicode file;
    int line;
};

class FileNotFoundError : public IOError {
public:
    FileNotFoundError(const unicode& path):
        IOError(_u("Unable to find file: ") + path) {}
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
        L_ERROR(std::string(typeid(T).name()) + " instance does not exist");
    }

    template<typename U>
    DoesNotExist(const U& what):
        std::runtime_error(std::string(typeid(T).name()) + " instance does not exist: " + boost::lexical_cast<std::string>(what)) {
        L_ERROR(std::string(typeid(T).name()) + " instance does not exist: " + boost::lexical_cast<std::string>(what));
    }
};

#endif

