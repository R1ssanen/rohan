#ifndef ROHAN_CONFIG_HPP_
#define ROHAN_CONFIG_HPP_

#include <iomanip>
#include <iostream>

#include "barycentric.hpp"
#include "rhdefs.hpp"

namespace rohan {

    enum class DepthPolicy {
        LESS,
        GREATER,
        LESS_EQ,
        GREATER_EQ,
    };

    enum class DrawMode {
        TRIANGLE,
        LINE,
    };

    inline std::ostream& operator<<(std::ostream& stream, DepthPolicy method) {
        switch (method) {
        case DepthPolicy::LESS: stream << "less-than"; break;
        case DepthPolicy::GREATER: stream << "greater-than"; break;
        case DepthPolicy::LESS_EQ: stream << "less-or-equal"; break;
        case DepthPolicy::GREATER_EQ: stream << "greater-or-equal"; break;
        default: stream << "unknown"; break;
        }

        return stream;
    }

    inline std::ostream& operator<<(std::ostream& stream, DrawMode mode) {
        switch (mode) {
        case DrawMode::TRIANGLE: stream << "triangle"; break;
        case DrawMode::LINE: stream << "line"; break;
        default: stream << "unknown"; break;
        }

        return stream;
    }

} // namespace rohan

namespace rohan {

    class RAPI RenderConfig {
      public:

        RenderConfig&       cull_backfaces(bool value) noexcept;
        bool                cull_backfaces() const noexcept;

        RenderConfig&       winding_order(WindingOrder winding) noexcept;
        WindingOrder        winding_order() const;

        RenderConfig&       test_depth(bool value) noexcept;
        bool                test_depth() const noexcept;

        RenderConfig&       depth_policy(DepthPolicy method) noexcept;
        DepthPolicy         depth_policy() const noexcept;

        RenderConfig&       draw_mode(DrawMode mode) noexcept;
        DrawMode            draw_mode() const noexcept;

        RenderConfig&       depth_range(f32 minz, f32 maxz) noexcept;
        RenderConfig&       depth_range(const glm::vec2& range) noexcept;
        std::pair<f32, f32> depth_range() const noexcept;

      private:

        WindingOrder m_winding_order = WindingOrder::CW;
        DepthPolicy  m_depth_policy  = DepthPolicy::LESS;
        DrawMode     m_draw_mode     = DrawMode::TRIANGLE;
        f32          m_minz = 0.f, m_maxz = 1.f;
        bool         m_cull_backfaces = true;
        bool         m_test_depth     = true;
    };

} // namespace rohan

namespace rohan {

    inline std::ostream& operator<<(std::ostream& stream, const RenderConfig& config) {
        stream << std::boolalpha;
        stream << "[rohan::config]" << '\n'
               << '\t' << "depth_range: " << config.depth_range().first << ", "
               << config.depth_range().second << '\n'
               << '\t' << "winding: " << config.winding_order() << '\n'
               << '\t' << "depth_method: " << config.depth_policy() << '\n'
               << '\t' << "backface_culling: " << config.cull_backfaces() << '\n'
               << '\t' << "depth_test: " << config.test_depth();
        return stream;
    }

} // namespace rohan

#endif
