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
    unicode(const char16_t* utf16_string);

    unicode(int32_t n, char32_t c);
    unicode(int32_t n, char16_t c);
    unicode(int32_t n, char c);

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
    unicode strip(const unicode& things=unicode(" \t\r\n")) const;
    unicode lstrip() const;
    unicode rstrip() const;
    unicode swap_case() const;
    unicode replace(const unicode& thing, const unicode& replacement);

    bool contains(const unicode& thing) const;
    bool contains(const std::string& thing) const;
    bool contains(const char* thing) const;

    unicode slice(int32_t beg, int32_t end);
    unicode slice(int32_t beg, void* null);
    unicode slice(void* null, int32_t end);

    bool empty() const { return string_.empty(); }
    bool starts_with(const unicode& thing) const;
    bool ends_with(const unicode& thing) const;

    std::vector<unicode> split(const unicode& on) const;

    unicode join(const std::vector<unicode>& parts) const;
    unicode join(const std::vector<std::string>& parts) const;

    struct Counter {
        Counter(uint32_t c): c(c) {}
        uint32_t c;
    };

    template<typename T>
    unicode format(T value) {
        unicode token = unicode("{" + boost::lexical_cast<std::string>(0) + "}");
        unicode result = this->replace(token, boost::lexical_cast<std::string>(value));
        return result;
    }

    template<typename T>
    unicode format(Counter count, T value) {
        unicode token = unicode("{" + boost::lexical_cast<std::string>(count.c) + "}");
        unicode result = this->replace(token, boost::lexical_cast<std::string>(value));
        return result;
    }

    template<typename T, typename... Args>
    unicode format(T value, Args&... args) {
        unicode token = unicode("{" + boost::lexical_cast<std::string>(0) + "}");
        unicode result = this->replace(token, boost::lexical_cast<std::string>(value));
        return result.format(Counter(1), args...);
    }

    template<typename T, typename... Args>
    unicode format(Counter count, T value, Args&... args) {
        unicode token = unicode("{" + boost::lexical_cast<std::string>(count.c) + "}");
        unicode result = this->replace(token, boost::lexical_cast<std::string>(value));
        return result.format(Counter(count.c + 1), args...);
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

    ustring::reverse_iterator rbegin() { return string_.rbegin(); }
    ustring::reverse_iterator rend() { return string_.rend(); }
    ustring::const_reverse_iterator rbegin() const { return string_.rbegin(); }
    ustring::const_reverse_iterator rend() const { return string_.rend(); }

    uint32_t count(const unicode& str) const;

    //Conversion functions
    int32_t to_int() const;
    float to_float() const;
    double to_double() const;
    bool to_boolean() const;
private:
    ustring string_;
};

using _u = unicode;

#endif // UNICODE_H
