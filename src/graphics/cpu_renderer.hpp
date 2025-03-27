#ifndef GRAPHICS_CPU_RENDERER_HPP
#define GRAPHICS_CPU_RENDERER_HPP

#include "renderer_context.hpp"
#include "../math/vec3.hpp"
#include "../scene/hittable_list.hpp"
#include "../rtweekend.hpp"

namespace jmrtiow::graphics
{
    class cpu_renderer
    {
    public:
        void render(const renderer_context& context, math::color3** image_data);
    };

    void cpu_renderer::render(const renderer_context& context, math::color3** image_data)
    {
        for (int j = 0; j < context.height; j++)
        {
            for (int i = 0; i < context.width; i++)
            {
                math::color3 pixel_color(0, 0, 0);
                auto u = (i + random_double()) / (context.width - 1);
                auto v = (j + random_double()) / (context.height - 1);
                math::ray r = context.camera->get_ray(u, v);
                pixel_color += jmrtiow::scene::ray_color(r, (*context.scene), context.max_depth);

                // For better readability.
                auto& image_data_element = (*image_data)[j * context.width + i];

                // Pixel data is normalized, be sure to un-normalize it before averaging.
                image_data_element = image_data_element * image_data_element;

                // Blend color with source.
                image_data_element = context.blend_callback(image_data_element, pixel_color);;

                // Normalize the color samples and gamma correct before passing off to pixel data.
                image_data_element.r = sqrt(image_data_element.r);
                image_data_element.g = sqrt(image_data_element.g);
                image_data_element.b = sqrt(image_data_element.b);
            }

            if (*context.pause)
            {
                break;
            }
        }
    }
}

#endif // GRAPHICS_CPU_RENDERER_HPP
