#include <fstream>
#include <cassert>

#include <boost/lexical_cast.hpp>

#include "../string.h"

#include "config_reader.h"

using namespace config;

ConfigReader::ConfigReader() {

}

ConfigReader::ConfigReader(const unicode& filename) {
    load(filename);
}

void ConfigReader::save(const unicode& filename) {
    std::ofstream s(filename.encode().c_str());

    for(auto it = groups_.begin();
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
                unicode v = boost::any_cast<unicode>((*set).second);
                s << (*set).first << "=" << v << std::endl;
                continue;
            } catch(boost::bad_any_cast& e) {}

            assert(0 && "Unsupported type");
        }
    }
}

void ConfigReader::load(const unicode& filename) {
    std::ifstream s(filename.encode());

    if(!s) {
        throw IOError(_u("Could not load the file: ") + filename);
    }

    unicode current_group;
    std::string tmp_line;
    while(std::getline(s, tmp_line)) {
        unicode line = unicode(tmp_line).strip();

        if(line.starts_with("#")) {
            continue;
        }

        if(line.starts_with("[")) {
            if(line.ends_with("]")) {
                current_group = line.slice(1, -1);
            }
        } else {
            if(line.contains("=")) {
                if(current_group.empty()) {
                    throw IOError("Bad config file");
                }

                std::vector<unicode> parts = line.split("=", 1);

                assert(parts.size() == 2);

                unicode key = parts.at(0);
                unicode value = parts.at(1);

                if(value.lower() == "true") {
                    set_setting<bool>(current_group, key, true);
                } else if (value.lower() == "false") {
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
                            set_setting<unicode>(current_group, key, value);
                        }
                    }
                }
            } else {
                throw IOError("Bad config file");
            }
        }
    }
}
