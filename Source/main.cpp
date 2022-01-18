#include "rtweekend.hpp"

#include "color.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "camera.hpp"
#include "material.hpp"

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

color3 ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color3(0, 0, 0);

    if (world.hit(r, 0.001, infinity, rec)) {

#if 0 // Alternate diffuse form
        point3 target = rec.p + random_in_hemisphere(rec.normal);
#elif 0 // True lambertian reflection
        point3 target = rec.p + rec.normal + random_unit_vector();
#elif 0 // Random ray reflection
        point3 target = rec.p + rec.normal + random_in_unit_sphere();
#endif
        ray scattered;
        color3 attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return color3(0, 0, 0);
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
    const int samples_per_pixel = 70; 
    const int max_depth = 35;

    // World

    hittable_list world;
    
    auto material_ground = make_shared<lambertian>(color3(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color3(0.7, 0.3, 0.3));
    auto material_left  = make_shared<metal>(color3(0.8, 0.8, 0.8), 0.3);
    auto material_right = make_shared<metal>(color3(0.8, 0.6, 0.2), 1.0);

    world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

    // Camera

    camera cam{};

    // Render

    printf_s("P3\n%d %d\n255\n", image_width, image_height);

    for (int j = image_height - 1; j >= 0; j--)
    {
        fprintf_s(stderr, "\rScanlines remaining: %d - %.1f%%    ", j, 100.0 - (100.0f * (double(j) / (image_height - 1))));
        fflush(stderr);

        for (int i = 0; i < image_width; i++)
        {
            color3 pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    fprintf_s(stderr, "\nDone.\n");

    return 0;
}