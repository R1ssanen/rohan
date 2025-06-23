#ifndef ROHAN_FRAME_HPP_
#define ROHAN_FRAME_HPP_

#include <span>

#include "buffer.hpp"
#include "rhdefs.hpp"
#include "state.hpp"
#include "vertex_type.hpp"

namespace rohan {

    struct FrameF32 : public Buffer<f32> {
        using Buffer<f32>::Buffer;

        /*template <vertex_type VertexType>
        void draw_indexed(
            const RenderConfig& config, Frame<f32>& depth_buffer, const VertexType* vertices,
            u64 vertex_count, const u32* indices, u64 index_count
        );*/
    };

    struct FrameU32 : public Buffer<u32> {
        using Buffer<u32>::Buffer;

        template <class VertexType>
            requires is_vertex<VertexType>
        void draw(
            RenderState& state, FrameF32& depth_buffer, const std::vector<VertexType>& vertices,
            const std::vector<u32>& indices
        );
    };

} // namespace rohan

#endif
