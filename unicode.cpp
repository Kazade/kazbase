#include "utf8.h"
#include "unicode.h"
#include "string.h"

#include <boost/algorithm/string.hpp>

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
