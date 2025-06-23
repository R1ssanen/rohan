#ifndef MT_BARYCENTRIC_HPP_
#define MT_BARYCENTRIC_HPP_

#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>
#include <iostream>

#include "rhdefs.hpp"

namespace rohan {

    enum class WindingOrder { CW, CCW };

    inline std::ostream& operator<<(std::ostream& stream, WindingOrder winding) {
        switch (winding) {
        case WindingOrder::CW: stream << "clockwise"; break;
        case WindingOrder::CCW: stream << "counter-clockwise"; break;
        default: stream << "unknown"; break;
        }

        return stream;
    }

} // namespace rohan

namespace rohan {

    class Barycentric {
      public:

        Barycentric(
            const glm::vec4& p0, const glm::vec4& p1, const glm::vec4& p2,
            WindingOrder winding = WindingOrder::CW, bool skip_if_backface = true
        ) noexcept;

        glm::vec3 slope_x() const noexcept;

        glm::vec3 slope_y(f32 left_slope) const noexcept;

        glm::vec3 coordinate(f32 x, f32 y) const noexcept;

        bool      backface() const noexcept { return m_is_backface; }

        bool      valid() const noexcept { return m_is_valid; }

      private:

        glm::mat3 m_mat;
        glm::vec3 m_slope_x;
        glm::vec3 m_slope_y;
        f32       m_inv_area_2;

        bool      m_is_backface = false;
        bool      m_is_valid    = true;
    };

} // namespace rohan

#endif
