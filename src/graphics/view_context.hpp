#ifndef GRAPHICS_VIEW_CONTEXT_HPP
#define GRAPHICS_VIEW_CONTEXT_HPP

#include <stdint.h>
#include "../math/vec3.hpp"

namespace jmrtiow::graphics
{
    /// @brief Context for a view being rendered.
    struct view_context
    {
    public:
        /// @brief Width of the desired view
        uint32_t width;
        /// @brief Height of the desired view
        uint32_t height;
        /// @brief X location to start from
        uint32_t x;
        /// @brief Y location to start from
        uint32_t y;
        /// @brief Pixel data of the view
        math::color3** data;
        /// @brief Number of pixels per row in the view
        uint32_t data_width;
        /// @brief Number of pixels per column in the view
        uint32_t data_height;
        /// @brief The number of completed iterations this view has rendered
        uint32_t iteration;
    };
}

#endif // GRAPHICS_VIEW_CONTEXT_HPP
