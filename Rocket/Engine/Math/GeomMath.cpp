#include "Math/GeomMath.h"

#include <cstdlib>
#include <ctime>

using namespace Rocket;

Random::Random() { Seed(0); }
Random::Random(uint32_t seed) { Seed(seed); }

void Random::Seed(uint32_t s)
{
    if (s == 0)
    {
        s = (uint32_t)clock();
    }

    // Fill the buffer with some basic random numbers
    for (unsigned i = 0; i < 17; i++)
    {
        // Simple linear congruential generator
        s = s * 2891336453 + 1;
        buffer[i] = s;
    }

    // Initialize pointers into the buffer
    p1 = 0;  p2 = 10;
}

uint32_t Random::RotL(uint32_t n, uint32_t r)
{
    return	(n << r) | (n >> (32 - r));
}

uint32_t Random::RotR(uint32_t n, uint32_t r)
{
    return	(n >> r) | (n << (32 - r));
}

uint32_t Random::RandomBits()
{
    uint32_t result;

    // Rotate the buffer and store it back to itself
    result = buffer[p1] = RotL(buffer[p2], 13) + RotR(buffer[p1], 9);

    // Rotate pointers
    if (--p1 < 0) p1 = 16;
    if (--p2 < 0) p2 = 16;

    // Return result
    return result;
}

Real Random::RandomReal()
{
    // Get the random number
    unsigned bits = RandomBits();
#if 0
    // Set up a reinterpret structure for manipulation
    union {
        Real value;
        uint32_t word;
    } convert;

    // Now assign the bits to the word. This works by fixing the ieee
    // sign and exponent bits (so that the size of the result is 1-2)
    // and using the bits to create the fraction part of the float.
    convert.word = (bits >> 9) | 0x3f800000;
#else
    // Set up a reinterpret structure for manipulation
    union {
        Real value;
        uint32_t words[2];
    } convert;

    // Now assign the bits to the words. This works by fixing the ieee
    // sign and exponent bits (so that the size of the result is 1-2)
    // and using the bits to create the fraction part of the float. Note
    // that bits are used more than once in this process.
    convert.words[0] =  bits << 20; // Fill in the top 16 bits
    convert.words[1] = (bits >> 12) | 0x3FF00000; // And the bottom 20
#endif

    // And return the value
    return convert.value - 1.0;
}

Real Random::RandomReal(Real scale)
{
    return RandomReal() * scale;
}

Real Random::RandomReal(Real min, Real max)
{
    return RandomReal() * (max-min) + min;
}

Real Random::RandomBinomial(Real scale)
{
    return (RandomReal()-RandomReal()) * scale;
}

uint32_t Random::RandomInt(uint32_t max)
{
    return RandomBits() % max;
}

Real3 Random::RandomVector(Real scale)
{
    return Real3(
        RandomBinomial(scale),
        RandomBinomial(scale),
        RandomBinomial(scale)
    );
}

Real3 Random::RandomVector(const Real3& scale)
{
    return Real3(
        RandomBinomial(scale[0]),
        RandomBinomial(scale[1]),
        RandomBinomial(scale[2])
    );
}

Real3 Random::RandomVector(const Real3& min, const Real3& max)
{
    return Real3(
        RandomReal(min[0], max[0]),
        RandomReal(min[1], max[1]),
        RandomReal(min[2], max[2])
    );
}

Real3 Random::RandomXZVector(Real scale)
{
    return Real3(
        RandomBinomial(scale),
        0,
        RandomBinomial(scale)
    );
}

Real3 Random::RandomXYVector(Real scale)
{
    return Real3(
        RandomBinomial(scale),
        RandomBinomial(scale),
        0
    );
}

Real3 Random::RandomYZVector(Real scale)
{
    return Real3(
        0,
        RandomBinomial(scale),
        RandomBinomial(scale)
    );
}

Quaternion Random::RandomQuaternion()
{
    Quaternion q(
        RandomReal(),
        RandomReal(),
        RandomReal(),
        RandomReal()
    );
    q.normalize();
    return q;
}
