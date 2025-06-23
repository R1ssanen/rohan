#ifndef ROHAN_DRAWCALL_HPP_
#define ROHAN_DRAWCALL_HPP_

#include <algorithm>
#include <span>
#include <vector>

#include "barycentric.hpp"
#include "frame.hpp"
#include "render.hpp"
#include "shader.hpp"
#include "state.hpp"
#include "transform.hpp"
#include "vertex_type.hpp"

namespace rohan {

    template <class VertexType>
        requires is_vertex<VertexType>
    void FrameU32::draw(
        RenderState& state, FrameF32& depth_buffer, const std::vector<VertexType>& vertices,
        const std::vector<u32>& indices
    ) {

        std::vector<VertexType> transformed;
        transformed.reserve(vertices.size());

        for (const auto& vertex : vertices) transformed.push_back(vertex);

        auto hash = [](u32 x) -> u32 {
            x += (x << 10);
            x ^= (x >> 6);
            x += (x << 3);
            x ^= (x >> 11);
            x += (x << 15);
            return x;
        };

        u64 triangle_id = 0;
        for (u64 i = 0; i < indices.size(); i += 3, triangle_id = hash(i)) {

            glm::vec4 p0      = transformed[indices[i + 0]].position();
            glm::vec4 p1      = transformed[indices[i + 1]].position();
            glm::vec4 p2      = transformed[indices[i + 2]].position();

            auto [minz, maxz] = state.config().depth_range();
            p0                = clip_to_ndc_zo(p0, m_width, m_height, minz, maxz);
            p1                = clip_to_ndc_zo(p1, m_width, m_height, minz, maxz);
            p2                = clip_to_ndc_zo(p2, m_width, m_height, minz, maxz);

            render_triangle_direct(*this, depth_buffer, state, p0, p1, p2);
            continue;

            p0 = ndc_to_screen(p0, m_width, m_height);
            p1 = ndc_to_screen(p1, m_width, m_height);
            p2 = ndc_to_screen(p2, m_width, m_height);

            render_debug_line_direct(*this, p0.x, p0.y, p1.x, p1.y, 0x00dca07f);
            render_debug_line_direct(*this, p1.x, p1.y, p2.x, p2.y, 0x00dca07f);
            render_debug_line_direct(*this, p2.x, p2.y, p0.x, p0.y, 0x00dca07f);
        }
    }

} // namespace rohan

#endif
