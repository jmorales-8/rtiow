#include "rtweekend.h"

#include "color.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"

#include <stdio.h>

double hit_sphere(const point3& center, double radius, const ray& r)
{
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;
    auto discriminiant = half_b * half_b - 4 * a * c;
    if (discriminiant < 0) 
    {
        return -1.0;
    }
    else
    {
        return (-half_b - sqrt(discriminiant)) / (2.0 * a);
    }

}

color3 ray_color(const ray& r, const hittable& world) {
    hit_record rec;
    if (world.hit(r, 0, infinity, rec)) {
        return 0.5 * (rec.normal + color3(1, 1, 1));
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y + 1.0);
    return (1.0 - t) * color3(1.0, 1.0, 1.0) + t * color3(0.5, 0.7, 1.0);
}

int main(int argc, char** argv)
{

    // Image

    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    // World

    hittable_list world;
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // Camera

    auto viewport_height = 2.0;
    auto viewport_width = aspect_ratio * viewport_height;
    auto focal_length = 1.0;

    auto origin = point3(0, 0, 0);
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

    // Render

    printf_s("P3\n%d %d\n255\n", image_width, image_height);

    for (int j = image_height - 1; j >= 0; j--)
    {
        fprintf_s(stderr, "\rScanlines remaining: %d", j);
        fflush(stderr);

        for (int i = 0; i < image_width; i++)
        {
            auto u = double(i) / (image_width - 1);
            auto v = double(j) / (image_height - 1);
            ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
            color3 pixel_color = ray_color(r, world);
            write_color(std::cout, pixel_color);
        }
    }

    fprintf_s(stderr, "\nDone.\n");

    return 0;
}