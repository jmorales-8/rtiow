// Local includes
#include "rtweekend.hpp"

#include "scene/material.hpp"
#include "scene/hittable_list.hpp"
#include "scene/sphere.hpp"
#include "scene/camera.hpp"
#include "image/image_exporter.hpp"

// ImGui includes
#include "imgui.h"

// STL includes
#include <stdio.h>

// External includes
#include <argparse/argparse.hpp>

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

    auto world = jmrtiow::scene::random_scene();
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
                pixel_color += jmrtiow::scene::ray_color(r, world, max_depth);
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
