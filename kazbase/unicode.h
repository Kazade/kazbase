#ifndef UNICODE_H
#define UNICODE_H

#include <string>
#include <vector>
#include <sstream>

typedef std::basic_string<char32_t> ustring;

class unicode {
public:
    typedef ustring::size_type size_type;

    unicode() {}
    unicode(const unicode& rhs):
        string_(rhs.string_){

    }

    unicode& operator=(const unicode& rhs);
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
    unicode strip() const;
    unicode lstrip() const;
    unicode rstrip() const;

    unicode strip(const unicode& things) const;
    unicode lstrip(const unicode& things) const;
    unicode rstrip(const unicode& things) const;
    unicode swap_case() const;
    unicode replace(const unicode& thing, const unicode& replacement) const;

    unicode lpad(int32_t indent);
    unicode rpad(int32_t count);

    bool contains(const unicode& thing) const;
    bool contains(const std::string& thing) const;
    bool contains(const char* thing) const;
    bool contains(const wchar_t ch) const;

    void push_back(const wchar_t c);

    unicode slice(int32_t beg, int32_t end) const;
    unicode slice(int32_t beg, void* null) const;
    unicode slice(void* null, int32_t end) const;

    bool empty() const { return string_.empty(); }
    bool starts_with(const unicode& thing) const;
    bool ends_with(const unicode& thing) const;

    std::vector<unicode> split(const unicode& on, int32_t count=-1, bool keep_empty=true) const;

    unicode join(const std::vector<unicode>& parts) const;
    unicode join(const std::vector<std::string>& parts) const;

    struct Counter {
        Counter(uint32_t c): c(c) {}
        uint32_t c;
    };

    template<typename T>
    unicode format(T value) {
        std::stringstream ss;
        ss << value;
        return _do_format(0, ss.str());
    }

    template<typename T>
    unicode format(Counter count, T value) {
        std::stringstream ss;
        ss << value;
        return _do_format(count.c, ss.str());
    }

    template<typename T, typename... Args>
    unicode format(T value, const Args&... args) {
        std::stringstream ss;
        ss << value;
        return _do_format(0, ss.str()).format(Counter(1), args...);
    }

    template<typename T, typename... Args>
    unicode format(Counter count, T value, const Args&... args) {
        std::stringstream ss;
        ss << value;
        return _do_format(count.c, ss.str()).format(Counter(count.c + 1), args...);
    }

    unicode _do_format(uint32_t counter, const std::string& value);

    bool operator==(const unicode& rhs) const {
        return string_ == rhs.string_;
    }

    bool operator!=(const unicode& rhs) const {
        return !(*this == rhs);
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

    std::size_t find(const unicode& what) const {
        return string_.find(what.string_);
    }

    std::size_t rfind(const unicode& what) const {
        return string_.rfind(what.string_);
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

    unicode operator+(const unicode& rhs) const {
        unicode result;
        result += *this;
        result += rhs;
        return result;
    }

    unicode operator*(const uint32_t rhs) const {
        unicode result;
        for(uint32_t i = 0; i < rhs; ++i) {
            result += *this;
        }
        return result;
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
    ustring to_ustring() const { return string_; }
private:
    ustring string_;
};

std::ostream& operator<< (std::ostream& os, const unicode& str);
bool operator==(const char* c_str, const unicode& uni_str);
bool operator!=(const char* c_str, const unicode& uni_str);
unicode operator+(const char* c_str, const unicode& uni_str);

namespace std {
    template<>
    struct hash<unicode> {
        size_t operator()(const unicode& str) const {
            hash<ustring> make_hash;
            return make_hash(str.to_ustring());
        }
    };
}

typedef unicode _u;

unicode humanize(int i);

#endif // UNICODE_H
