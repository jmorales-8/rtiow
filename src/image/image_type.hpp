#ifndef IMAGE_IMAGE_TYPE_HPP
#define IMAGE_IMAGE_TYPE_HPP

namespace jmrtiow::image
{
    enum class image_type
    {
        Unknown = 0,
        PNG,
        JPG,
        BMP,
        TGA,
        HDR,
        PPM,
        WEBP
    };
}

#endif // IMAGE_IMAGE_TYPE_HPP
