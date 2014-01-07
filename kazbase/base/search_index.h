#ifndef SEARCH_INDEX_H_INCLUDED
#define SEARCH_INDEX_H_INCLUDED

#include <string>
#include <set>
#include <map>
#include "kazbase/string.h"
#include "kazbase/list_utils.h"

template<typename T>
class SearchIndex {
public:
    void add_to_index(const T& item, const std::string& term);
    void remove_from_index(const T& item);

    std::set<T> search(const std::string& query) const;

private:
    std::map<std::string, std::set<T>> index_;
};

template<typename T>
void SearchIndex<T>::add_to_index(const T& item, const std::string& term) {
    std::string buffer;

    //First store the item against the whole term

    //FIXME: Handle term.decode("utf-8")
    for(char c: str::lower(term)) {
        buffer.push_back(c);
        index_[buffer].insert(item);
    }

    //Now store against each word in the term
    for(std::string word: str::split(term)) {
        buffer = "";
        for(char c: str::lower(word)) {
            buffer.push_back(c);
            index_[buffer].insert(item);
        }
    }
}

template<typename T>
std::set<T> SearchIndex<T>::search(const std::string& query) const {
    return container::const_get(index_, str::lower(query), std::set<T>());
}

template<typename T>
void SearchIndex<T>::remove_from_index(const T& item) {

    for(std::string key: container::keys(index_)) {
        if(container::contains(index_[key], item)) {
            index_[key].erase(item);
            if(index_[key].empty()) {
                index_.erase(key);
            }
        }
    }
}

#endif // SEARCH_INDEX_H_INCLUDED
