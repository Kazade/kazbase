#ifndef NOISE_H
#define NOISE_H

#include <random>
#include <array>

namespace noise {

class Perlin {
public:
    Perlin(uint32_t seed=0);

    double noise(double x) const { return noise(x, 0, 0); }
    double noise(double x, double y) const { return noise(x, y, 0); }
    double noise(double x, double y, double z) const;

private:
    std::array<int, 512> p;

    struct Quaternion {
        float x, y, z, w;
    };

    Quaternion rotation_; //Used to reduce the risk of passing in round integers into the noise function

    Quaternion from_eular(float pitch, float yaw, float roll);
};

class PerlinOctave {
public:
    PerlinOctave(int octaves, uint32_t seed=0);

    double noise(double x) const { return noise(x, 0, 0); }
    double noise(double x, double y) const { return noise(x, y, 0); }
    double noise(double x, double y, double z) const;

private:
    Perlin perlin_;
    int octaves_;
};

}

#endif // NOISE_H
