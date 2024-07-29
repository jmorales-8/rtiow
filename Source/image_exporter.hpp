#ifndef IMAGE_EXPORTER_HPP
#define IMAGE_EXPORTER_HPP

#include <vector>
#include <fstream>
#include <format>
#include <stdint.h>

#include "color.hpp"
#include "image_type.hpp"
#include "exceptions/not_implemented.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace
{
    struct color3byte
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
};

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

    std::vector<color3byte> convert_to_bytes(const std::vector<color3>& image_data);
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
    // First we change the doubles to bytes from 0-255.
    auto pixel_data = convert_to_bytes(image_data);

    auto writer = [](void *context, void *data, int size)
    {
        auto out = static_cast<std::ostream*>(context);
        (*out).write(static_cast<char*>(data), size);
    };

    int return_code = stbi_write_png_to_func(writer, &out, image_width, image_height, 3, pixel_data.data(), 0);

    return return_code != 0;
}

bool image_exporter::export_jpg(std::ostream &out, image_type file_type, const std::vector<color3> &image_data, int image_width, int image_height)
{
    // First we change the doubles to bytes from 0-255.
    auto pixel_data = convert_to_bytes(image_data);

    auto writer = [](void *context, void *data, int size)
    {
        auto out = static_cast<std::ostream*>(context);
        (*out).write(static_cast<char*>(data), size);
    };

    int return_code = stbi_write_jpg_to_func(writer, &out, image_width, image_height, 3, pixel_data.data(), 100);

    return return_code != 0;
}

bool image_exporter::export_bmp(std::ostream &out, image_type file_type, const std::vector<color3> &image_data, int image_width, int image_height)
{
    // First we change the doubles to bytes from 0-255.
    auto pixel_data = convert_to_bytes(image_data);

    auto writer = [](void *context, void *data, int size)
    {
        auto out = static_cast<std::ostream*>(context);
        (*out).write(static_cast<char*>(data), size);
    };

    int return_code = stbi_write_bmp_to_func(writer, &out, image_width, image_height, 3, pixel_data.data());

    return return_code != 0;
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

    auto pixel_data = convert_to_bytes(image_data);

    for (auto &&pixel : pixel_data)
    {
        // Write the translated [0,255] value of each color component.
        // Cast to int for the actual data to be inserted into the file instead of utf8 characters.
        out << static_cast<int>(pixel.r) << ' '
        << static_cast<int>(pixel.g) << ' '
        << static_cast<int>(pixel.b) << '\n';
    }

    return true;
}

bool image_exporter::export_webp(std::ostream &out, image_type file_type, const std::vector<color3> &image_data, int image_width, int image_height)
{
    throw not_implemented(__PRETTY_FUNCTION__);
}

inline std::vector<color3byte> image_exporter::convert_to_bytes(const std::vector<color3> &image_data)
{
    std::vector<color3byte> bytes{};

    for (auto &&pixel : image_data)
    {
        color3byte c;

        c.r = static_cast<uint8_t>(256 * clamp(pixel.r, 0.0, 0.999));
        c.g = static_cast<uint8_t>(256 * clamp(pixel.g, 0.0, 0.999));
        c.b = static_cast<uint8_t>(256 * clamp(pixel.b, 0.0, 0.999));

        bytes.push_back(c);
    }

    return bytes;
}

#endif // IMAGE_EXPORTER_HPP
