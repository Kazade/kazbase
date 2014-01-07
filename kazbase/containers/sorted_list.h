#ifndef SORTED_LIST_H_INCLUDED
#define SORTED_LIST_H_INCLUDED

#include <list>

namespace container {

template<typename T,
         typename Compare=std::less<T>,
         typename Allocator=std::allocator<T> >
class sorted_list {
private:
    std::list<T, Allocator> list_;
    Compare cmp_;
public:
    typedef T& reference;
    typedef const T& const_reference;
    typedef typename std::list<T>::iterator iterator;
    typedef typename std::list<T>::const_iterator const_iterator;
    typedef typename std::list<T>::size_type size_type;
    typedef typename std::list<T>::reverse_iterator reverse_iterator;
    typedef typename std::list<T>::const_reverse_iterator const_reverse_iterator;

    explicit sorted_list(const Compare& comp=Compare(), const Allocator& alloc = Allocator()):
        list_(alloc),
        cmp_(comp) {}
    explicit sorted_list(size_type n, const T& value=T(), const Compare& comp=Compare(), const Allocator& alloc=Allocator()):
        list_(n, value, alloc),
        cmp_(comp) {}

    template<typename InputIterator>
    sorted_list(InputIterator first, InputIterator last, const Compare& comp=Compare(), const Allocator& alloc = Allocator()):
        list_(first, last, alloc),
        cmp_(comp) {

        //Sort the list after initializing it with the range
        list_.sort(cmp_);
    }

    sorted_list(const sorted_list<T, Compare, Allocator>& other):
        list_(other.list_),
        cmp_(other.cmp) {}

    sorted_list<T, Compare, Allocator>& operator=(const sorted_list<T, Compare, Allocator>& rhs) {
        list_ = rhs.list_;
        cmp_ = rhs.cmp_;
        return *this;
    }

    template<typename InputIterator>
    void assign(InputIterator first, InputIterator last) {
        list_.assign(first, last);
        list_.sort(cmp_); //Make sure we remain sorted
    }

    void assign(size_type n, const T& u) {
        list_.assign(n, u);
        list_.sort(cmp_);
    }

    reference back() {
        return list_.back();
    }

    const_reference back() const {
        return list_.back();
    }

    iterator begin() { return list_.begin(); }
    const_iterator begin() const { return list_.begin(); }
    void clear() { list_.clear(); }
    bool empty() const { return list_.empty(); }
    iterator end() { return list_.end(); }
    const_iterator end() const { return list_.end(); }
    iterator erase(iterator position) { return list_.erase(position); }
    iterator erase(iterator first, iterator last) { return list_.erase(first, last); }
    reference front() { return list_.front(); }
    const_reference front() const { return list_.front(); }

    size_type position(iterator it) {
        iterator i = it;
        size_type pos = 0;

        //Go through the list backwards from the iterator until the beginning
        while(i != list_.begin()) {
            i--;
            pos++;
        }
        return (pos); //Make zero-based
    }

    iterator insert(const T& x) {
        iterator ret = list_.insert(list_.end(), x);
        list_.sort(cmp_); //Doesn't invalidate the iterator
        return ret;
    }

    void insert(size_type n, const T& x) {
        std::list<T> to_insert(n, x);
        to_insert.sort(cmp_);
        list_.merge(to_insert, cmp_);
    }

    template <class InputIterator>
    void insert (InputIterator first, InputIterator last) {
        std::list<T> to_insert(first, last);
        to_insert.sort(cmp_);
        list_.merge(to_insert, cmp_);
    }

    size_type max_size() const { return list_.max_size(); }
    void pop_back() { list_.pop_back(); }
    void pop_front() { list_.pop_front(); }
    void remove(const T& value) { list_.remove(value); }

    template<typename Predicate>
    void remove_if(Predicate pred) { list_.remove_if(pred); }
    void resize(size_type sz, T c= T()) {
        list_.resize(sz, c);
        list_.sort(cmp_);
    }

    size_type size() const { return list_.size(); }
    void swap(sorted_list<T, Compare, Allocator>& lst) {
        list_.swap(lst);
    }
    void unique() { list_.unique(); }
};

}

#endif // SORTED_LIST_H_INCLUDED
