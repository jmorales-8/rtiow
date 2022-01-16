#ifndef COLOR_H
#define COLOR_H

#include "vec3.hpp"

#include <iostream>

void write_color(std::ostream& out, color3 pixel_color) {
    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(255.999 * pixel_color.r) << ' '
        << static_cast<int>(255.999 * pixel_color.g) << ' '
        << static_cast<int>(255.999 * pixel_color.b) << '\n';
}

#endif