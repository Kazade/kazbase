#ifndef RANDOM_H
#define RANDOM_H

#include <cstdint>

void seed();
float random_float(float min, float max);
int32_t random_int(int32_t min, int32_t max);

#endif // RANDOM_H
