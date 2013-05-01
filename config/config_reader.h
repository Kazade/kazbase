#ifndef CONFIG_READER_H_INCLUDED
#define CONFIG_READER_H_INCLUDED

#include <iostream>
#include <string>
#include <map>
#include <stdexcept>
#include <boost/any.hpp>
#include "../exceptions.h"

namespace config {

class TypeError : public std::logic_error {
public:
    TypeError(const std::string& s):
        std::logic_error(s) {}
};

class InvalidSectionError : public std::logic_error {
public:
    InvalidSectionError(const std::string& s):
        std::logic_error(s) {}
};

class InvalidSettingError : public std::logic_error {
public:
    InvalidSettingError(const std::string& s):
        std::logic_error(s) {}
};

class ConfigReader {
public:
    ConfigReader();
    ConfigReader(const std::string& filename);


    template<typename T>
    void set_setting(const std::string& group, const std::string& key, T value) {
        groups_[group][key] = value;
    }
    
    template<typename T>
    T get_setting(const std::string& group, const std::string& key) {
        std::map<std::string, Option>::iterator g = groups_.find(group);

        if(g == groups_.end()) {
            throw InvalidSectionError(group);
        }

        Option::iterator s = (*g).second.find(key);
        if(s == (*g).second.end()) {
            throw InvalidSettingError(key);
        }

        try {
            return boost::any_cast<T>((*s).second);
        } catch(boost::bad_any_cast& e) {
            throw TypeError(e.what());
        }
    }

    void save(const std::string& filename);
    void load(const std::string& filename);
private:
    typedef std::map<std::string, boost::any> Option;
    std::map<std::string, Option> groups_;
    std::string filename_;


};





}

#endif // CONFIG_READER_H_INCLUDED
