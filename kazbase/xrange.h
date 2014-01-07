#ifndef XRANGE_H
#define XRANGE_H

#include <iostream>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <cassert>

template <class T>
boost::iterator_range<boost::counting_iterator<T> > xrange(T to)
{
    //these assertions are somewhat problematic:
    //might produce warnings, if T is unsigned
    assert(T() <= to);
    return boost::make_iterator_range(boost::counting_iterator<T>(0), boost::counting_iterator<T>(to));
}

template <class T>
boost::iterator_range<boost::counting_iterator<T> > xrange(T from, T to)
{
    assert(from <= to);
    return boost::make_iterator_range(boost::counting_iterator<T>(from), boost::counting_iterator<T>(to));
}

//iterator that can do increments in steps (positive and negative)
template <class T>
class xrange_iterator:
    public boost::iterator_facade<xrange_iterator<T>, const T, std::forward_iterator_tag>
{
    T value, incr;
public:
    xrange_iterator(T value, T incr = T()): value(value), incr(incr) {}
private:
    friend class boost::iterator_core_access;
    void increment() { value += incr; }
    bool equal(const xrange_iterator& other) const
    {
        //this is probably somewhat problematic, assuming that the "end iterator"
        //is always the right-hand value?
        return (incr >= 0 && value >= other.value) || (incr < 0 && value <= other.value);
    }
    const T& dereference() const { return value; }
};

template <class T>
boost::iterator_range<xrange_iterator<T> > xrange(T from, T to, T increment)
{
    assert((increment >= T() && from <= to) || (increment < T() && from >= to));
    return boost::make_iterator_range(xrange_iterator<T>(from, increment), xrange_iterator<T>(to));
}

#endif // XRANGE_H
