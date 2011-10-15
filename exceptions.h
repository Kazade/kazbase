#ifndef KAZBASE_EXCEPTIONS_H
#define KAZBASE_EXCEPTIONS_H

#include <typeinfo>
#include <stdexcept>

class IOError : public std::runtime_error {
public:
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
        LogicError(what) {}
};

class FileNotFoundError : public IOError {
public:
    FileNotFoundError(const std::string& path):
        IOError("Unable to find file: " + path) {}
};

class NetworkError : public std::runtime_error {
public:
    NetworkError(const std::string& what):
        std::runtime_error(what) {}
};

template<typename T>
class DoesNotExist : public std::runtime_error {
public:
    DoesNotExist():
        std::runtime_error(std::string(typeid(T).name()) + " instance does not exist") {

    }
};

#endif

