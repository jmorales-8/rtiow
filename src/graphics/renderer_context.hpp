#ifndef GRAPHICS_RENDERER_CONTEXT_HPP
#define GRAPHICS_RENDERER_CONTEXT_HPP

#include <stdint.h>
#include "../scene/hittable.hpp"
#include "../scene/camera.hpp"

namespace jmrtiow::graphics
{
    /// @brief Rendering context for all renderers
    struct renderer_context
    {
    public:
        /// @brief Maximum depth a ray will travel
        uint32_t max_depth;
        /// @brief Number of samples to render and average per pixel (set to -1 for cumulative sampling)
        uint32_t samples_per_pixel;
        /// @brief Bool to stop the renderer
        bool* pause;
        /// @brief Pointer to the scene to render
        scene::hittable* scene;
        /// @brief Pointer to the camera to use
        scene::camera* camera;
        /// @brief Blending function to use if samples_per_pixel is not 1
        std::function<math::color3(const math::color3&, const math::color3&, const uint32_t&)> blend_callback;
    };
}

#endif // GRAPHICS_RENDERER_CONTEXT_HPP
