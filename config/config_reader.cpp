#include <fstream>
#include <cassert>

#include <boost/lexical_cast.hpp>

#include "../string.h"

#include "config_reader.h"

using namespace config;

ConfigReader::ConfigReader() {

}

ConfigReader::ConfigReader(const std::string& filename) {
    load(filename);
}

void ConfigReader::save(const std::string& filename) {
    std::ofstream s(filename.c_str());

    for(std::map<std::string, Option>::iterator it = groups_.begin();
        it != groups_.end(); ++it) {

        s << "[" << (*it).first << "]" << std::endl;

        for(Option::iterator set = (*it).second.begin(); set != (*it).second.end(); ++set) {
            try {
                bool v = boost::any_cast<bool>((*set).second);
                s << (*set).first << "=" << ((v)?"true":"false") << std::endl;
                continue;
            } catch(boost::bad_any_cast& e) {}

            try {
                double v = boost::any_cast<double>((*set).second);
                s << (*set).first << "=" << v << std::endl;
                continue;
            } catch(boost::bad_any_cast& e) {}

            try {
                int v = boost::any_cast<int>((*set).second);
                s << (*set).first << "=" << v << std::endl;
                continue;
            } catch(boost::bad_any_cast& e) {}

            try {
                std::string v = boost::any_cast<std::string>((*set).second);
                s << (*set).first << "=" << v << std::endl;
                continue;
            } catch(boost::bad_any_cast& e) {}

            assert(0 && "Unsupported type");
        }
    }
}

void ConfigReader::load(const std::string& filename) {
    std::ifstream s(filename.c_str());

    if(!s.good()) {
        throw config::IOError("Could not load the file: " + filename);
    }

    std::string current_group;
    std::string line;
    while(std::getline(s, line)) {
        line = str::strip(line);

        if(str::starts_with(line, "#")) continue;

        if(str::starts_with(line, "[")) {
            if(str::ends_with(line, "]")) {
                current_group = str::slice(line, 1, -1);
            }
        } else {
            if(str::contains(line, "=")) {
                if(current_group.empty()) {
                    throw IOError("Bad config file");
                }

                std::vector<std::string> parts = str::split(line, "=", 1);

                assert(parts.size() == 2);

                std::string key = parts.at(0);
                std::string value = parts.at(1);

                if(str::lower(value) == "true") {
                    set_setting<bool>(current_group, key, true);
                } else if (str::lower(value) == "false") {
                    set_setting<bool>(current_group, key, false);
                } else {
                    try {
                        double v = boost::lexical_cast<double>(value);
                        set_setting<double>(current_group, key, v);
                    } catch(boost::bad_lexical_cast& e) {
                        try {
                            int v = boost::lexical_cast<int>(value);
                            set_setting<int>(current_group, key, v);
                        } catch(boost::bad_lexical_cast& e) {
                            set_setting<std::string>(current_group, key, value);
                        }
                    }
                }
            } else {
                throw IOError("Bad config file");
            }
        }
    }
}
