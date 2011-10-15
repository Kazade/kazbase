#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <string>
#include <map>
#include <vector>

namespace type {

template<typename T, typename U>
class MultiValueMap {
private:
    std::map<T, std::vector<U> > map_;

public:
    U get(const T& key, const U& def=U()) const {
        typename std::map<T, std::vector<U> >::const_iterator it = map_.find(key);
        if(it == map_.end()) {
            return def;
        }

        if((*it).second.empty()) {
            return def;
        }

        return (*it).second.at(0);
    }

    std::vector<U> get_list(const T& key, const std::vector<U>& def) const {
        typename std::map<T, std::vector<U> >::const_iterator it = map_.find(key);
        if(it == map_.end()) {
            return def;
        }

        if((*it).empty()) {
            return def;
        }

        return (*it);
    }

    void set(const T& key, const U& value) {
        std::vector<U> values;
        values.push_back(value);
        map_[key] = values;
    }

    void set_list(const T& key, const std::vector<U>& values) {
        map_[key] = values;
    }

    void append(const T& key, const U& value) {
        map_[key].push_back(value);
    }

    std::vector<U>& operator[](const T& key) {
        return map_[key];
    }

    typedef typename std::map<T, std::vector<U> >::const_iterator const_iterator;
    typedef typename std::map<T, std::vector<U> >::iterator iterator;

    const_iterator begin() const {
        return map_.begin();
    }

    const_iterator end() const {
        return map_.end();
    }

    iterator begin() {
        return map_.begin();
    }

    iterator end() {
        return map_.end();
    }

    bool empty() const {
        return map_.empty();
    }
};

typedef MultiValueMap<std::string, std::string> MultiValueDict;

}


#endif // TYPES_H_INCLUDED
