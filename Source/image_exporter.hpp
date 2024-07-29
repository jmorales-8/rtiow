#ifndef IMAGE_EXPORTER_HPP
#define IMAGE_EXPORTER_HPP

#include <vector>
#include <fstream>
#include <format>
#include "color.hpp"
#include "image_type.hpp"

class image_exporter
{
public:
    bool export_data(std::ostream &out, image_type file_type, const std::vector<color3>& image_data, int image_width, int image_height);
    bool export_data(std::string filepath, image_type file_type, const std::vector<color3>& image_data, int image_width, int image_height);

private:
    bool export_ppm(std::ostream &out, image_type file_type, const std::vector<color3>& image_data, int image_width, int image_height);
};

bool image_exporter::export_data(std::ostream &out, image_type file_type, const std::vector<color3>& image_data, int image_width, int image_height)
{
    switch (file_type)
    {
    case image_type::PPM:
        return export_ppm(out, file_type, image_data, image_width, image_height);
        break;

    default:
        std::cerr << "Invalid filetype specified when saving image...\n";
        return false;
        break;
    }
}

bool image_exporter::export_data(std::string filepath, image_type file_type, const std::vector<color3>& image_data, int image_width, int image_height)
{
    std::ofstream file_stream(filepath, std::ios::trunc);

    if (!file_stream.is_open())
    {
        return false;
    }

    return export_data(file_stream, file_type, image_data, image_width, image_height);
}

bool image_exporter::export_ppm(std::ostream &out, image_type file_type, const std::vector<color3> &image_data, int image_width, int image_height)
{
    out << std::format("P3\n{} {}\n255\n", image_width, image_height);

    for (auto &&pixel : image_data)
    {
        auto r = pixel.r;
        auto g = pixel.g;
        auto b = pixel.b;

        // Write the translated [0,255] value of each color component.
        out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
            << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
            << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
    }

    return true;
}

#endif // IMAGE_EXPORTER_HPP
