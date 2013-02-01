#ifndef UNICODE_H
#define UNICODE_H

#include <boost/lexical_cast.hpp>
#include <string>
#include <vector>

typedef std::basic_string<char32_t> ustring;

class unicode {
public:
    unicode() {}
    unicode(const unicode& rhs):
        string_(rhs.string_){

    }

    unicode& operator=(const unicode& rhs) {
        this->string_ = rhs.string_;
        return *this;
    }

    unicode(const char* utf8_string);
    unicode(const std::string& utf8_string);
    unicode(char32_t* unicode_string);

    template<class InputIterator>
    unicode(InputIterator begin, InputIterator end):
        string_(begin, end) {

    }

    std::wstring::size_type length() const {
        return string_.size();
    }

    std::string encode() const;

    unicode capitalize() const;
    unicode title() const;

    unicode lower() const;
    unicode upper() const;
    unicode strip() const;
    unicode lstrip() const;
    unicode rstrip() const;
    unicode swap_case() const;
    unicode replace(const unicode& thing, const unicode& replacement);

    bool empty() const { return string_.empty(); }
    bool starts_with(const unicode& thing);
    bool ends_with(const unicode& thing);

    std::vector<unicode> split(const unicode& on) const;
    unicode join(const std::vector<unicode>& parts) const;

    //FIXME: VARIADIC TEMPLATES!
    template<typename T>
    unicode format(T value) {
        unicode token = unicode(std::string("{0}"));
        return this->replace(token, boost::lexical_cast<std::string>(value));
    }

    template<typename T, typename U>
    unicode format(T v1, U v2) {
        unicode token = unicode(std::string("{0}"));
        unicode result = this->replace(token, boost::lexical_cast<std::string>(v1));

        token = unicode(std::string("{1}"));
        return result.replace(token, boost::lexical_cast<std::string>(v2));
    }

    template<typename T, typename U, typename V>
    unicode format(T v1, U v2, V v3) {
        unicode token = unicode(std::string("{0}"));
        unicode result = this->replace(token, boost::lexical_cast<std::string>(v1));

        token = unicode(std::string("{1}"));
        result = result.replace(token, boost::lexical_cast<std::string>(v2));

        token = unicode(std::string("{2}"));
        result = result.replace(token, boost::lexical_cast<std::string>(v3));

        return result;
    }

    //fixme: make unicode work with lexical_cast rather than overloading
    unicode format(unicode v1, unicode v2) {
        return format(v1.encode(), v2.encode());
    }

    unicode format(unicode value) {
        return format(value.encode());
    }

    bool operator==(const unicode& rhs) const {
        return string_ == rhs.string_;
    }

    unicode& operator=(const std::string& rhs) {
        //Automatically convert UTF-8 strings to unicode
        *this = unicode(rhs);
        return *this;
    }

    unicode& operator=(const char* rhs) {
        //Automatically convert UTF-8 strings to unicode
        *this = unicode(rhs);
        return *this;
    }

    char32_t& operator[](ustring::size_type pos) {
        return string_[pos];
    }

    const char32_t& operator[](ustring::size_type pos) const {
        return string_[pos];
    }

    unicode& operator+=(const unicode& rhs) {
        string_.append(rhs.string_);
        return *this;
    }

    bool operator<(const unicode& rhs) const {
        //FIXME: need to do a proper lexigraphical compare - probably
        return encode() < rhs.encode();
    }

    ustring::iterator begin() { return string_.begin(); }
    ustring::iterator end() { return string_.end(); }
    ustring::const_iterator begin() const { return string_.begin(); }
    ustring::const_iterator end() const { return string_.end(); }
private:
    ustring string_;
};

#endif // UNICODE_H
