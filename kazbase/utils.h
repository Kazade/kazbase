#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

template<typename T>
uintptr_t id(T& obj) {
    uintptr_t result = (uintptr_t) &obj;
    return result;
}

#endif // UTILS_H
