#ifndef GRAPHICS_RENDERER_CONTEXT_HPP
#define GRAPHICS_RENDERER_CONTEXT_HPP

#include <stdint.h>
#include "../scene/hittable.hpp"
#include "../scene/camera.hpp"

namespace jmrtiow::graphics
{
    struct renderer_context
    {
    public:
        uint32_t width;
        uint32_t height;
        uint32_t max_depth;
        uint32_t samples_per_pixel;
        bool* pause;
        scene::hittable* scene;
        scene::camera* camera;
        std::function<math::color3(const math::color3&, const math::color3&)> blend_callback;
    };
}

#endif // GRAPHICS_RENDERER_CONTEXT_HPP
