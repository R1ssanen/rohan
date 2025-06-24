#ifndef ROHAN_RENDER_HPP_
#define ROHAN_RENDER_HPP_

#include <glm/vec4.hpp>

#include "frame.hpp"
#include "rhdefs.hpp"
#include "state.hpp"

namespace rohan {

    RAPI void render_triangle_direct(
        RenderState& state, FrameU32& color_buffer, FrameF32& depth_buffer, glm::vec4 p0,
        glm::vec4 p1, glm::vec4 p2
    );

    RAPI void
    render_debug_line_direct(FrameU32& color_buffer, u32 x0, u32 y0, u32 x1, u32 y1, u32 color);

} // namespace rohan

#endif
