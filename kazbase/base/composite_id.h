#ifndef COMPOSITE_ID_H_INCLUDED
#define COMPOSITE_ID_H_INCLUDED

#include <cassert>

#include "../logging/logging.h"
#include "../parse/tree_parser.h"
#include "../exceptions.h"
#include "../unicode.h"

namespace base {

//FIXME: Don't accept pointers!
template<typename T, typename U>
class CompositeID {
private:
    T first_;
    U second_;

    bool is_null_;
public:
    typedef T first_type;
    typedef U second_type;

    CompositeID():
        is_null_(true) {}

    CompositeID(const T& f, const U& s):
        first_(f),
        second_(s),
        is_null_(false) {}

    bool operator==(const CompositeID& rhs) const {
        if(is_null_ || rhs.is_null_) {
            throw ValueError("Attempted a comparison with a NULL composite ID");
        }
        return first_ == rhs.first_ && second_ == rhs.second_;
    }

    bool operator!=(const CompositeID& rhs) const {
        return !(*this == rhs);
    }

    bool operator<(const CompositeID& rhs) const {
        if(is_null_ || rhs.is_null_) {
            throw ValueError("Attempted a comparison with a NULL composite ID");
        }

        if(first() < rhs.first()) {
            return true;
        } else if(first() == rhs.first()) {
            if(second() < rhs.second()) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    bool is_null() const { return is_null_; }

    T first() const { return first_; }
    U second() const { return second_; }

    std::string to_string() {
        return to_string(*this);
    }

    static std::string to_string(const CompositeID& c) {
        if(c.is_null_) {
            throw ValueError("Attempted to convert a NULL composite ID to a string");
        }

        //FIXME: Check that pipe doesn't exist in first() or second()
        return _u("[[{0}][{1}]]").format(c.first(), c.second()).encode();
    }

    static CompositeID from_string(const std::string& c) {
        T f;
        U s;

        L_DEBUG("String: " + c);
        parse::TreeParser parser("\\[", "\\]");
        parse::Node n = parser.parse(c);

        assert(!n.child_nodes.empty());


        L_DEBUG("Child count: " + _u("{0}").format(n.child_nodes.size()));
        std::vector<std::string> parts;

        L_DEBUG(n.full_contents);
        L_DEBUG(n.child_nodes[0].full_contents);
      /*  L_DEBUG(n.child_nodes[0].child_nodes.at(0).full_contents);
        L_DEBUG(n.child_nodes[0].child_nodes.at(1).full_contents);*/

        parts.push_back(n.child_nodes[0].inner_contents);
        parts.push_back(n.child_nodes[1].inner_contents);

        std::string first = parts[0];
        std::string second = parts[1];

        std::stringstream fs(first);
        fs >> f;

        std::stringstream ss(second);
        ss >> s;

        return CompositeID(f, s);
    }
};

template<typename T, typename U>
std::ostream& operator<< (std::ostream& out, const CompositeID<T, U>& c) {
    out << CompositeID<T, U>::to_string(c);
    return out;
}

template<typename T, typename U>
std::istream& operator>> (std::istream& in, CompositeID<T, U>& c) {
    std::string result;
    in >> result;
    c = CompositeID<T, U>::from_string(result);
    return in;
}

}
#endif // COMPOSITE_ID_H_INCLUDED
