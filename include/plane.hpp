#ifndef ROHAN_PLANE_HPP_
#define ROHAN_PLANE_HPP_

#include <glm/vec3.hpp>

#include "rhdefs.hpp"

namespace rohan {

    class Plane {
      public:

        Plane(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2);

        Plane(const glm::vec3& pos, const glm::vec3& normal);

        bool
        intersects_line(const glm::vec3& p0, const glm::vec3& p1, glm::vec3* intersection) const;

      private:

        glm::vec3 m_normal;
        f32       m_d; // -ax - by - cz = d
    };

} // namespace rohan

#endif
