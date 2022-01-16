#include "color.hpp"
#include "vec3.hpp"

#include <stdio.h>

int main(int argc, char** argv)
{

    // Image

    const int image_width = 256;
    const int image_height = 256;

    // Render

    printf_s("P3\n%d %d\n255\n", image_width, image_height);

    for (int j = image_height - 1; j >= 0; j--)
    {
        fprintf_s(stderr, "\rScanlines remaining: %d", j);
        fflush(stderr);

        for (int i = 0; i < image_width; i++)
        {
            color3 pixel_color(double(i) / (image_width - 1), double(j)/(image_height-1), 0.25);
            write_color(std::cout, pixel_color);
        }
    }

    fprintf_s(stderr, "\nDone.\n");

    return 0;
}