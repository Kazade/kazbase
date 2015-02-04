#ifndef CONFIG_READER_H_INCLUDED
#define CONFIG_READER_H_INCLUDED

#include <iostream>
#include <string>
#include <unordered_map>
#include <stdexcept>

#include "../any/any.h"
#include "../unicode.h"
#include "../exceptions.h"

namespace config {

class TypeError : public std::logic_error {
public:
    TypeError(const unicode& s):
        std::logic_error(s.encode()) {}
};

class InvalidSectionError : public std::logic_error {
public:
    InvalidSectionError(const unicode& s):
        std::logic_error(s.encode()) {}
};

class InvalidSettingError : public std::logic_error {
public:
    InvalidSettingError(const unicode& s):
        std::logic_error(s.encode()) {}
};

class ConfigReader {
public:
    ConfigReader();
    ConfigReader(const unicode& filename);


    template<typename T>
    void set_setting(const unicode& group, const unicode& key, T value) {
        groups_[group][key] = value;
    }
    
    template<typename T>
    T get_setting(const unicode& group, const unicode& key) {
        auto g = groups_.find(group);

        if(g == groups_.end()) {
            throw InvalidSectionError(group);
        }

        Option::iterator s = (*g).second.find(key);
        if(s == (*g).second.end()) {
            throw InvalidSettingError(key);
        }

        try {
            return kazbase::any_cast<T>((*s).second);
        } catch(kazbase::bad_any_cast& e) {
            throw TypeError(e.what());
        }
    }

    void save(const unicode& filename);
    void load(const unicode& filename);
private:
    typedef std::unordered_map<unicode, kazbase::any> Option;
    std::unordered_map<unicode, Option> groups_;
    unicode filename_;


};





}

#endif // CONFIG_READER_H_INCLUDED
