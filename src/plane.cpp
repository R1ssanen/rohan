#include "plane.hpp"

#include <glm/detail/func_geometric.inl>
#include <glm/vec3.hpp>

#include "rhdefs.hpp"

namespace rohan {

    Plane::Plane(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2) {
        m_normal = glm::normalize(glm::cross(p1 - p0, p2 - p0));
        m_d      = -glm::dot(p0, m_normal);
    }

    Plane::Plane(const glm::vec3& pos, const glm::vec3& normal) {
        m_normal = normal;
        m_d      = -glm::dot(pos, normal);
    }

    bool Plane::intersects_line(
        const glm::vec3& p0, const glm::vec3& p1, glm::vec3* intersection
    ) const {
        glm::vec3 direction    = p1 - p0;
        f32       discriminant = glm::dot(m_normal, direction);

        if (discriminant == 0) return false;
        else if (!intersection)
            return true;

        f32 t         = (m_d - glm::dot(m_normal, p0)) / discriminant;
        *intersection = p0 + glm::normalize(direction) * t;
        return true;
    }

} // namespace rohan
