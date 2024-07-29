// Local includes
#include "rtweekend.hpp"

#include "scene/hittable_list.hpp"
#include "scene/sphere.hpp"
#include "scene/camera.hpp"
#include "scene/material.hpp"
#include "image/image_exporter.hpp"

// STL includes
#include <stdio.h>

// External includes
#include <argparse/argparse.hpp>

namespace jmrtiow
{
    double hit_sphere(const math::point3 &center, double radius, const math::ray &r)
    {
        math::vec3 oc = r.origin() - center;
        auto a = r.direction().length_squared();
        auto half_b = math::dot(oc, r.direction());
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

    math::color3 ray_color(const math::ray &r, const scene::hittable &world, int depth)
    {
        scene::hit_record rec;

        // If we've exceeded the math::ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return math::color3(0, 0, 0);

        if (world.hit(r, 0.001, infinity, rec))
        {

#if 0   // Alternate diffuse form
        math::point3 target = rec.p + random_in_hemisphere(rec.normal);
#elif 0 // True scene::lambertian reflection
            math::point3 target = rec.p + rec.normal + random_unit_vector();
#elif 0 // Random math::ray reflection
            math::point3 target = rec.p + rec.normal + random_in_unit_sphere();
#endif
            math::ray scattered;
            math::color3 attenuation;
            if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, world, depth - 1);
            return math::color3(0, 0, 0);
        }

        math::vec3 unit_direction = unit_vector(r.direction());
        auto t = 0.5 * (unit_direction.y + 1.0);
        return (1.0 - t) * math::color3(1.0, 1.0, 1.0) + t * math::color3(0.5, 0.7, 1.0);
    }

    scene::hittable_list random_scene()
    {
        scene::hittable_list world;

        auto ground_material = make_shared<scene::lambertian>(math::color3(0.5, 0.5, 0.5));
        world.add(make_shared<scene::sphere>(math::point3(0, -1000, 0), 1000, ground_material));

        for (int a = -11; a < 11; a++)
        {
            for (int b = -11; b < 11; b++)
            {
                auto choose_mat = random_double();
                math::point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

                if ((center - math::point3(4, 0.2, 0)).length() > 0.9)
                {
                    shared_ptr<scene::material> sphere_material;

                    if (choose_mat < 0.8)
                    {
                        // diffuse
                        auto albedo = math::color3::random() * math::color3::random();
                        sphere_material = make_shared<scene::lambertian>(albedo);
                        world.add(make_shared<scene::sphere>(center, 0.2, sphere_material));
                    }
                    else if (choose_mat < 0.95)
                    {
                        // scene::metal
                        auto albedo = math::color3::random(0.5, 1);
                        auto fuzz = random_double(0, 0.5);
                        sphere_material = make_shared<scene::metal>(albedo, fuzz);
                        world.add(make_shared<scene::sphere>(center, 0.2, sphere_material));
                    }
                    else
                    {
                        // glass
                        sphere_material = make_shared<scene::dielectric>(1.5);
                        world.add(make_shared<scene::sphere>(center, 0.2, sphere_material));
                    }
                }
            }
        }

        auto material1 = make_shared<scene::dielectric>(1.5);
        world.add(make_shared<scene::sphere>(math::point3(0, 1, 0), 1.0, material1));

        auto material2 = make_shared<scene::lambertian>(math::color3(0.4, 0.2, 0.1));
        world.add(make_shared<scene::sphere>(math::point3(-4, 1, 0), 1.0, material2));

        auto material3 = make_shared<scene::metal>(math::color3(0.7, 0.6, 0.5), 0.0);
        world.add(make_shared<scene::sphere>(math::point3(4, 1, 0), 1.0, material3));

        return world;
    }

}

int main(int argc, char **argv)
{
    using namespace jmrtiow;

    argparse::ArgumentParser argparser("rtiow", "0.1");

    argparser.add_argument("--image-type", "-t")
        .default_value(std::string{"png"})
        .choices("png", "jpg", "jpeg", "bmp", "tga", "hdr", "ppm", "webp")
        .nargs(1)
        .help("The type of image to output [choices: png, jpg, jpeg, bmp, tga, hdr, ppm, webp]")
        .metavar("TYPE");

    argparser.add_argument("--filepath", "-f")
        .help("The file location to output to")
        .metavar("PATH")
        .required();

    try
    {
        argparser.parse_args(argc, argv);
    }
    catch (const std::exception &err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << argparser;
        std::exit(1);
    }

    std::string filepath = argparser.get<std::string>("--filepath");
    std::string image_type_string = argparser.get("--image-type");
    image::image_type image_type_selection = image::image_type::Unknown;

    if (image_type_string.compare("png") == 0)
    {
        image_type_selection = image::image_type::PNG;
    }
    else if (image_type_string.compare("jpg") == 0 || image_type_string.compare("jpeg") == 0)
    {
        image_type_selection = image::image_type::JPG;
    }
    else if (image_type_string.compare("bmp") == 0)
    {
        image_type_selection = image::image_type::BMP;
    }
    else if (image_type_string.compare("tga") == 0)
    {
        image_type_selection = image::image_type::TGA;
    }
    else if (image_type_string.compare("hdr") == 0)
    {
        image_type_selection = image::image_type::HDR;
    }
    else if (image_type_string.compare("ppm") == 0)
    {
        image_type_selection = image::image_type::PPM;
    }
    else if (image_type_string.compare("webp") == 0)
    {
        image_type_selection = image::image_type::WEBP;
    }

    // Image

    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 10;
    const int max_depth = 25;

    // World

    auto world = random_scene();
    // hittable_list world;

#if 0   // First scene (three spheres and a green ground
    auto material_ground = make_shared<scene::lambertian>(math::color3(0.8, 0.8, 0.0));
    auto material_center = make_shared<scene::lambertian>(math::color3(0.1, 0.2, 0.5));
    auto material_left  = make_shared<scene::dielectric>(1.5);
    auto material_right = make_shared<scene::metal>(math::color3(0.8, 0.6, 0.2), 0.0);

    world.add(make_shared<scene::sphere>(math::point3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<scene::sphere>(math::point3(0.0, 0.0, -1.0), 0.5, material_center));
    world.add(make_shared<scene::sphere>(math::point3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<scene::sphere>(math::point3(-1.0, 0.0, -1.0), -0.45, material_left)); // bubble effect on the left scene::sphere.
    world.add(make_shared<scene::sphere>(math::point3(1.0, 0.0, -1.0), 0.5, material_right));
#elif 0 // Second scene (two spheres, blue and red)

    auto R = cos(pi / 4);
    auto material_left = make_shared<scene::lambertian>(math::color3(0, 0, 1));
    auto material_right = make_shared<scene::lambertian>(math::color3(1, 0, 0));

    world.add(make_shared<scene::sphere>(math::point3(-R, 0, -1), R, material_left));
    world.add(make_shared<scene::sphere>(math::point3(R, 0, -1), R, material_right));
#endif

    // scene::camera cam(math::point3(-2, 2, 1), math::point3(0, 0, -1), math::vec3(0, 1, 0), 90, aspect_ratio);
    // scene::camera cam(math::point3(-2, 2, 1), math::point3(0, 0, -1), math::vec3(0, 1, 0), 20, aspect_ratio);
    // math::point3 lookfrom(3, 3, 2);
    // math::point3 lookat(0, 0, -1);
    // math::vec3 vup(0, 1, 0);
    // auto dist_to_focus = (lookfrom - lookat).length();
    // auto aperture = 2.0;

    math::point3 lookfrom(13, 2, 3);
    math::point3 lookat(0, 0, 0);
    math::vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    scene::camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    // Render

    // Image data as R,G,B math::vec3, no alpha.

    std::vector<math::color3> image_data{};

    for (int j = image_height - 1; j >= 0; j--)
    {
        std::cerr << std::format("\rScanlines remaining: {} - {:.1f}%    ", j, 100.0 - (100.0f * (double(j) / (image_height - 1))));
        fflush(stderr);

        for (int i = 0; i < image_width; i++)
        {
            math::color3 pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s)
            {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                math::ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }

            // Normalize the color samples and gamma correct before passing off to pixel data.
            pixel_color *= (1.0f / samples_per_pixel);
            pixel_color.r = sqrt(pixel_color.r);
            pixel_color.g = sqrt(pixel_color.g);
            pixel_color.b = sqrt(pixel_color.b);

            image_data.push_back(pixel_color);
        }
    }

    std::cerr << "\nExporting to file...\n";
    image::image_exporter exporter;
    exporter.export_data(filepath, image_type_selection, image_data, image_width, image_height);
    std::cerr << "Done!\n";

    return 0;
}
