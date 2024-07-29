#ifndef IMAGE_EXPORTER_HPP
#define IMAGE_EXPORTER_HPP

#include <vector>
#include <fstream>
#include <format>
#include "color.hpp"
#include "image_type.hpp"
#include "exceptions/not_implemented.hpp"

class image_exporter
{
public:
    bool export_data(std::ostream &out, image_type file_type, const std::vector<color3>& image_data, int image_width, int image_height);
    bool export_data(std::string filepath, image_type file_type, const std::vector<color3>& image_data, int image_width, int image_height);

private:
    bool export_png(std::ostream &out, image_type file_type, const std::vector<color3>& image_data, int image_width, int image_height);
    bool export_jpg(std::ostream &out, image_type file_type, const std::vector<color3>& image_data, int image_width, int image_height);
    bool export_bmp(std::ostream &out, image_type file_type, const std::vector<color3>& image_data, int image_width, int image_height);
    bool export_tga(std::ostream &out, image_type file_type, const std::vector<color3>& image_data, int image_width, int image_height);
    bool export_hdr(std::ostream &out, image_type file_type, const std::vector<color3>& image_data, int image_width, int image_height);
    bool export_ppm(std::ostream &out, image_type file_type, const std::vector<color3>& image_data, int image_width, int image_height);
    bool export_webp(std::ostream &out, image_type file_type, const std::vector<color3>& image_data, int image_width, int image_height);
};

bool image_exporter::export_data(std::ostream &out, image_type file_type, const std::vector<color3>& image_data, int image_width, int image_height)
{
    switch (file_type)
    {
    case image_type::PNG:
        return export_png(out, file_type, image_data, image_width, image_height);
        break;
    case image_type::JPG:
        return export_jpg(out, file_type, image_data, image_width, image_height);
        break;
    case image_type::BMP:
        return export_bmp(out, file_type, image_data, image_width, image_height);
        break;
    case image_type::TGA:
        return export_tga(out, file_type, image_data, image_width, image_height);
        break;
    case image_type::HDR:
        return export_hdr(out, file_type, image_data, image_width, image_height);
        break;
    case image_type::PPM:
        return export_ppm(out, file_type, image_data, image_width, image_height);
        break;
    case image_type::WEBP:
        return export_webp(out, file_type, image_data, image_width, image_height);
        break;
    default:
        throw not_implemented("Unknown file type export started", __PRETTY_FUNCTION__);
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

bool image_exporter::export_png(std::ostream &out, image_type file_type, const std::vector<color3> &image_data, int image_width, int image_height)
{
    throw not_implemented(__PRETTY_FUNCTION__);
}

bool image_exporter::export_jpg(std::ostream &out, image_type file_type, const std::vector<color3> &image_data, int image_width, int image_height)
{
    throw not_implemented(__PRETTY_FUNCTION__);
}

bool image_exporter::export_bmp(std::ostream &out, image_type file_type, const std::vector<color3> &image_data, int image_width, int image_height)
{
    throw not_implemented(__PRETTY_FUNCTION__);
}

bool image_exporter::export_tga(std::ostream &out, image_type file_type, const std::vector<color3> &image_data, int image_width, int image_height)
{
    throw not_implemented(__PRETTY_FUNCTION__);
}

bool image_exporter::export_hdr(std::ostream &out, image_type file_type, const std::vector<color3> &image_data, int image_width, int image_height)
{
    throw not_implemented(__PRETTY_FUNCTION__);
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

bool image_exporter::export_webp(std::ostream &out, image_type file_type, const std::vector<color3> &image_data, int image_width, int image_height)
{
    throw not_implemented(__PRETTY_FUNCTION__);
}

#endif // IMAGE_EXPORTER_HPP
