#ifndef LIST_UTILS_H
#define LIST_UTILS_H

#include <vector>
#include <set>
#include <algorithm>
#include <map>
#include <tr1/unordered_map>
#include "exceptions.h"

namespace container {

template<typename T>
bool contains(const std::vector<T>& haystack, const T& needle) {
    typename std::vector<T>::const_iterator it = std::find(haystack.begin(), haystack.end(), needle);
    return it != haystack.end();
}

template<typename T>
bool contains(const std::set<T>& haystack, const T& needle) {
    typename std::set<T>::const_iterator it = std::find(haystack.begin(), haystack.end(), needle);
    return it != haystack.end();
}

template<typename T, typename U>
bool contains(const std::map<T, U>& haystack, const T& needle) {
    typename std::map<T, U>::const_iterator it = haystack.find(needle);
    return it != haystack.end();
}

template<typename T, typename U>
std::set<T> keys(const std::tr1::unordered_map<T, U>& container) {
    std::set<T> result;
    for(std::pair<T, U> i: container) {
        result.insert(i.first);
    }
    return result;
}

template<typename T, typename U>
std::set<T> keys(const std::map<T, U>& container) {
    std::set<T> result;
    for(typename std::map<T, U>::const_iterator it = container.begin(); it != container.end(); ++it) {
        result.insert((*it).first);
    }
    return result;
}

template<typename T, typename U>
U const_get(const std::map<T, U>& container, const T& key, const U& default_value=U()) {
    typename std::map<T, U>::const_iterator it = container.find(key);
    if(it == container.end()) {
        return default_value;
    }

    return (*it).second;
}

}

#endif
