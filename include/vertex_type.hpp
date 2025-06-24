#ifndef ROHAN_VERTEX_TYPE_HPP_
#define ROHAN_VERTEX_TYPE_HPP_

#include <glm/vec4.hpp>

namespace rohan {

    template <typename T>
    concept interpolable = requires(T a, T b, f32 t) {
        { a.interpolate(b, t) } -> std::same_as<T>;
    };

    template <typename T>
    concept has_position = requires(T t) {
        { t.position() } -> std::same_as<const glm::vec4&>;
    };

    template <typename T>
    concept is_vertex = interpolable<T> && has_position<T>;

} // namespace rohan

#endif
