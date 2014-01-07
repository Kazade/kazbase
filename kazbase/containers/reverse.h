#ifndef REVERSE_H_INCLUDED
#define REVERSE_H_INCLUDED

/**
    Allows us to iterate a container in reverse using C++0x range
    based for loops

    e.g.
    for(int i: reverse(integers)) {

    }
*/
template <typename Container>
class ReverseWrapper {
private:
    Container& c_;

public:
    typedef typename Container::reverse_iterator reverse_iterator;
    typedef typename Container::const_reverse_iterator const_reverse_iterator;

    ReverseWrapper(Container& c) : c_(c) { }
    reverse_iterator begin() { return c_.rbegin(); }
    const_reverse_iterator begin() const { return c_.rbegin(); }

    reverse_iterator end() { return c_.rend(); }
    const_reverse_iterator end() const { return c_.rend(); }
};

template <typename Container>
ReverseWrapper<Container> reverse(Container& c) {
    return ReverseWrapper<Container>(c);
}

#endif // REVERSE_H_INCLUDED
