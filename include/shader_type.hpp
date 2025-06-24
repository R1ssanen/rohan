#ifndef ROHAN_SHADER_TYPE_HPP_
#define ROHAN_SHADER_TYPE_HPP_

#include <algorithm>
#include <cassert>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <variant>
#include <vector>

#include "rhdefs.hpp"
#include "vertex_type.hpp"

namespace rohan {

    struct UniformBuffer {

        using primitive_t =
            std::variant<u32, i32, u64, f32, glm::vec2, glm::vec3, glm::vec4, void*>;

        std::vector<primitive_t>                 uniforms;
        std::vector<std::pair<std::string, u64>> index_table;

      private:

        u64 get_index(std::string_view name) const {
            auto it = std::find_if(index_table.begin(), index_table.end(), [name](const auto& p) {
                return p.first == name;
            });

            assert(it != index_table.end() && "error: can't find named uniform.");
            return it->second;
        }

      public:

        template <typename T> void push_uniform(T uniform, std::string_view name = "") {
            u64 size = uniforms.size();
            if (name.empty()) index_table.emplace_back(std::to_string(size), size);
            else
                index_table.emplace_back(name, size);

            uniforms.emplace_back(std::move(uniform));
        }

        template <typename T> void set_uniform(T uniform, u64 index) { uniforms[index] = uniform; }

        template <typename T> void set_uniform(T uniform, std::string_view name) {
            uniforms[get_index(name)] = uniform;
        }

        template <typename T> auto get_uniform(u64 index) const {
            if constexpr (std::is_pointer_v<T>)
                return reinterpret_cast<T>(std::get<void*>(uniforms[index]));
            else
                return std::get<T>(uniforms[index]);
        }

        template <typename T> auto get_uniform(std::string_view name) const {
            return get_uniform<T>(get_index(name));
        }
    };

    using frag_method_t   = void(const UniformBuffer&, u64, u32&);
    using vertex_method_t = glm::vec4(const UniformBuffer&, u64, const glm::vec4&);

    struct Program {
        UniformBuffer    uniforms;
        frag_method_t*   frag_shader;
        vertex_method_t* vertex_shader;

        void             operator()(u64 id, u32& frag_out) { frag_shader(uniforms, id, frag_out); }

        glm::vec4        operator()(u64 id, const glm::vec4& pos_in) {
            return vertex_shader(uniforms, id, pos_in);
        }
    };

}; // namespace rohan

#endif
