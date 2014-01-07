#include <ctime>
#include <cstdlib>

#include "random.h"

void seed() {
    srand((unsigned)time(0));
}

float random_float(float min, float max) {
    return min + (float)rand()/((float)RAND_MAX/(max-min));
}

int32_t random_int(int32_t min, int32_t max) {
    return min + (float)rand()/((float)RAND_MAX/(max-min));
}

