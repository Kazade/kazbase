#include "utf8.h"
#include "unicode.h"
#include "string.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

unicode::unicode(int32_t n, char32_t c) {
    string_ = std::basic_string<char32_t>(n, c);
}

unicode::unicode(int32_t n, char16_t c) {
    std::basic_string<char16_t> s(n, c);

    *this = unicode(s.c_str());
}

unicode::unicode(int32_t n, char c) {
    std::string s(n, c);

    *this = unicode(s.c_str());
}

unicode::unicode(const char* utf8_string):
    unicode(std::string(utf8_string)) {
}

unicode::unicode(const std::string& utf8_string) {
    utf8::utf8to32(utf8_string.begin(), utf8_string.end(), std::back_inserter(string_));
}

unicode::unicode(const char16_t* utf16_string) {
    std::string tmp;

    std::basic_string<char16_t> s16(utf16_string);
    utf8::utf16to8(s16.begin(), s16.end(), std::back_inserter(tmp));
    utf8::utf8to32(tmp.begin(), tmp.end(), std::back_inserter(string_));
}

unicode::unicode(char32_t* utf32_string) {
    string_ = ustring(utf32_string);
}

bool unicode::contains(const unicode& thing) const {
    return contains(encode());
}

bool unicode::contains(const std::string& thing) const {
    std::string enc = encode();
    return enc.find(thing) != std::string::npos;
}

bool unicode::contains(const char *thing) const {
    return contains(std::string(thing));
}

std::string unicode::encode() const {
    std::string result;
    utf8::utf32to8(string_.begin(), string_.end(), std::back_inserter(result));
    return result;
}

unicode unicode::replace(const unicode& to_find, const unicode& to_replace) {
    //FIXME: THis currently encodes to utf-8 does the replacement, and then decodes it.. that's a bit shit
    std::string final_s(encode());
    boost::replace_all(final_s, std::string(to_find.encode()), std::string(to_replace.encode()));
    return unicode(final_s);
}

unicode unicode::lower() const {
    //FIXME: WORK WITH UNICODE
    std::string final_s(encode());
    final_s = str::lower(final_s);
    return unicode(final_s);
}

std::vector<unicode> unicode::split(const unicode &on) const {
    //FIXME: WORK WITH UNICODE
    std::string final_s(encode());

    std::vector<unicode> result;
    for(std::string part: str::split(final_s, on.encode())) {
        result.push_back(unicode(part));
    }
    return result;
}

unicode unicode::join(const std::vector<std::string>& parts) const {
    std::vector<unicode> tmp_parts;
    for(std::string p: parts) {
        tmp_parts.push_back(unicode(p));
    }

    return join(tmp_parts);
}

unicode unicode::join(const std::vector<unicode>& parts) const {
    if(parts.empty()) {
        return unicode("");
    }

    unicode final_string;
    for(unicode p: parts) {
        if(p.empty()) {
            continue;
        }
        final_string += p;
        final_string += *this;
    }

    return unicode(final_string.begin(), final_string.begin() + (final_string.length() - string_.length()));
}

unicode unicode::slice(int32_t beg, int32_t end) const {
    //Handle negative indexing
    if(beg < 0) {
        beg = length() - beg;
    }

    if(end < 0) {
        end = length() - end;
    }

    //Keep within range
    if(beg > length()) {
        beg = length() - 1;
    }

    if(end > length()) {
        end = length() - 1;
    }

    return unicode(begin() + beg, begin() + end);
}

unicode unicode::slice(int32_t beg, void* null) const {
    assert(!null);
    return slice(beg, length());
}

unicode unicode::slice(void* null, int32_t end) const {
    assert(!null);
    return slice(0, end);
}

int32_t unicode::to_int() const {
    return boost::lexical_cast<int32_t>(encode());
}

float unicode::to_float() const {
    return boost::lexical_cast<float>(encode());
}

double unicode::to_double() const {
    return boost::lexical_cast<double>(encode());
}

bool unicode::to_boolean() const {
    return lower() == "yes" || lower() == "true";
}

uint32_t unicode::count(const unicode& str) const {
    if(length() == 0) return 0;

    uint32_t count = 0;
    //FIXME Use this->find when it's implemented
    for(size_t offset = string_.find(str.string_);
        offset != std::string::npos;
        offset = string_.find(str.string_, offset + str.string_.length())) {
        ++count;
    }

    return count;
}

bool unicode::starts_with(const unicode& thing) const {
    return std::mismatch(thing.begin(), thing.end(), begin()).first == thing.end();
}

bool unicode::ends_with(const unicode& thing) const {
    return std::mismatch(thing.rbegin(), thing.rend(), rbegin()).first == thing.rend();
}

unicode unicode::strip(const unicode& things) const {
    unicode result = *this;

    result.string_.erase(result.string_.find_last_not_of(things.string_) + 1);
    result.string_.erase(result.begin(), result.begin() + result.string_.find_first_not_of(things.string_));

    return result;
}
