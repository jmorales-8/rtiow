#ifndef RTWEEKEND_HPP
#define RTWEEKEND_HPP

#include <cmath>
#include <limits>
#include <memory>
#include <random>

// Usings

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees)
{
    return degrees * pi / 180.0;
}

inline double random_double()
{
    // Returns a random real in [0,1).
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max)
{
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

inline double clamp(double x, double min, double max)
{
    if (x < min)
        return min;
    if (x > max)
        return max;
    return x;
}

// Common Headers

#include "math/ray.hpp"
#include "math/vec3.hpp"
#include "math/interval.hpp"
#include "scene/hittable.hpp"

#endif // RTWEEKEND_HPP
