#include "transform.hpp"

#include <algorithm>
#include <glm/mat4x4.hpp>

namespace rohan {

    glm::vec4 clip_to_ndc_zo(glm::vec4 p, u32 width, u32 height, f32 minz, f32 maxz) {
        p.w = 1.f / p.w;
        p.x *= p.w;
        p.y *= p.w;
        p.z *= p.w;

        p.x = std::clamp(0.5f + p.x * 0.5f, 0.f, 1.f);
        p.y = std::clamp(0.5f - p.y * 0.5f, 0.f, 1.f);
        p.z = std::clamp(0.5f + p.z * 0.5f * (maxz - minz) + minz, 0.f, 1.f);

        return p;
    }

    glm::vec3 clip_to_world(glm::vec4 p, const glm::mat4& inv_view_proj) {
        p = inv_view_proj * p;
        return glm::vec3(p) / p.w;
    }

    // right-handed system, -z, +y
    glm::vec3
    screen_to_world(const glm::vec3& p, f32 inv_w_2, f32 inv_h_2, const glm::mat4& inv_view_proj) {
        return screen_to_world(p.x, p.y, p.z, inv_w_2, inv_h_2, inv_view_proj);
    }

    // right-handed system, -z, +y
    glm::vec3 screen_to_world(
        f32 screen_x, f32 screen_y, f32 ndc_z, f32 inv_w_2, f32 inv_h_2,
        const glm::mat4& inv_view_proj
    ) {

        f32 ndc_x = screen_x * inv_w_2 - 1.f;
        f32 ndc_y = 1.f - screen_y * inv_h_2;

        return clip_to_world(glm::vec4(ndc_x, ndc_y, ndc_z, 1.f), inv_view_proj);
    }

    glm::vec4 ndc_to_screen(glm::vec4 p, u32 width, u32 height) {
        p.x = std::trunc(p.x * width - 0.5f);
        p.y = std::trunc(p.y * height - 0.5f);
        return p;
    }

} // namespace rohan
