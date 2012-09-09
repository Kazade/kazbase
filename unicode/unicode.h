#ifndef UNICODE_H
#define UNICODE_H

namespace unicode {

std::wstring decode(const std::string& str, const std::string& encoding="utf-8");
std::string encode(const std::string& str, const std::string& encoding="utf-8");

}

#endif // UNICODE_H
