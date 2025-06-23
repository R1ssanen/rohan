#include "config.hpp"

#include "rhdefs.hpp"

namespace rohan {

    RenderConfig& RenderConfig::cull_backfaces(bool value) noexcept {
        m_cull_backfaces = value;
        return *this;
    }

    bool          RenderConfig::cull_backfaces() const noexcept { return m_cull_backfaces; }

    RenderConfig& RenderConfig::winding_order(WindingOrder winding) noexcept {
        m_winding_order = winding;
        return *this;
    }

    WindingOrder  RenderConfig::winding_order() const { return m_winding_order; }

    RenderConfig& RenderConfig::test_depth(bool value) noexcept {
        m_test_depth = value;
        return *this;
    }

    bool          RenderConfig::test_depth() const noexcept { return m_test_depth; }

    RenderConfig& RenderConfig::depth_policy(DepthPolicy method) noexcept {
        m_depth_policy = method;
        return *this;
    }

    DepthPolicy   RenderConfig::depth_policy() const noexcept { return m_depth_policy; }

    RenderConfig& RenderConfig::draw_mode(DrawMode mode) noexcept {
        m_draw_mode = mode;
        return *this;
    }

    DrawMode      RenderConfig::draw_mode() const noexcept { return m_draw_mode; }

    RenderConfig& RenderConfig::depth_range(f32 minz, f32 maxz) noexcept {
        m_minz = minz, m_maxz = maxz;
        return *this;
    }

    RenderConfig& RenderConfig::depth_range(const glm::vec2& range) noexcept {
        m_minz = range.x, m_maxz = range.y;
        return *this;
    }

    std::pair<f32, f32> RenderConfig::depth_range() const noexcept {
        return std::make_pair(m_minz, m_maxz);
    }

} // namespace rohan
