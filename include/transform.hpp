#ifndef ROHAN_TRANSFORM_HPP_
#define ROHAN_TRANSFORM_HPP_

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "rhdefs.hpp"

namespace rohan {

    RAPI glm::vec4 clip_to_ndc_zo(glm::vec4 p, u32 width, u32 height, f32 minz, f32 maxz);

    RAPI glm::vec3 clip_to_world(glm::vec4 p, const glm::mat4& inv_view_proj);

    // right-handed system, -z, +y
    RAPI glm::vec3 screen_to_world(
        f32 screen_x, f32 screen_y, f32 ndc_z, f32 inv_w_2, f32 inv_h_2,
        const glm::mat4& inv_view_proj
    );

    // right-handed system, -z, +y
    RAPI glm::vec3
    screen_to_world(const glm::vec3& p, f32 inv_w_2, f32 inv_h_2, const glm::mat4& inv_view_proj);

    // z and w untouched
    RAPI glm::vec4 ndc_to_screen(glm::vec4 p, u32 width, u32 height);

} // namespace rohan

#endif
