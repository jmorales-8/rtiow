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
            auto r = double(i) / (image_width - 1);
            auto g = double(j) / (image_height - 1);
            auto b = 0.25;

            int ir = static_cast<int>(255.999 * r);
            int ig = static_cast<int>(255.999 * g);
            int ib = static_cast<int>(255.999 * b);

            printf_s("%d %d %d\n", ir, ig, ib);
        }
    }

    fprintf_s(stderr, "\nDone.\n");

    return 0;
}