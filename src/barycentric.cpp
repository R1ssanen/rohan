#include "barycentric.hpp"

#include <glm/mat3x3.hpp>
#include <glm/vec4.hpp>

namespace rohan {

    Barycentric::Barycentric(
        const glm::vec4& p0, const glm::vec4& p1, const glm::vec4& p2, WindingOrder winding,
        bool skip_if_backface
    ) noexcept {

        const f32 determinant = p0.x * (p1.y - p2.y) + p1.x * (p2.y - p0.y) + p2.x * (p0.y - p1.y);

        if (determinant == 0) {
            m_is_valid = false;
            return;
        }

        switch (winding) {
        case WindingOrder::CW:
            if (determinant < 0) {
                m_is_backface = true;
                if (skip_if_backface) return;
            }
            break;

        case WindingOrder::CCW:
            if (determinant > 0) {
                m_is_backface = true;
                if (skip_if_backface) return;
            }
            break;

        default: return;
        }

        m_mat = glm::mat3(
            { p1.x * p2.y - p2.x * p1.y, p1.y - p2.y, p2.x - p1.x },
            { p2.x * p0.y - p0.x * p2.y, p2.y - p0.y, p0.x - p2.x },
            { p0.x * p1.y - p1.x * p0.y, p0.y - p1.y, p1.x - p0.x }
        );

        m_inv_area_2 = 1.f / determinant;
        m_slope_x    = glm::vec3(p1.y - p2.y, p2.y - p0.y, p0.y - p1.y) * m_inv_area_2;
        m_slope_y    = glm::vec3(p2.x - p1.x, p0.x - p2.x, p1.x - p0.x) * m_inv_area_2;
    }

    glm::vec3 Barycentric::slope_x(void) const noexcept { return m_slope_x; }

    glm::vec3 Barycentric::slope_y(f32 left_slope) const noexcept {
        return m_slope_y + m_slope_x * left_slope;
    }

    glm::vec3 Barycentric::coordinate(f32 x, f32 y) const noexcept {
        return m_inv_area_2 * glm::vec3(1.f, x, y) * m_mat;
    }

} // namespace rohan
