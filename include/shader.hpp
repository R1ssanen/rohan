#ifndef ROHAN_SHADER_HPP_
#define ROHAN_SHADER_HPP_

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "rhdefs.hpp"

namespace rohan {

    class VertexShader {
      public:

        virtual glm::vec4& main(u64, void*) = 0;
    };

    class FragShader {
      public:

        virtual void main(u64, glm::vec3&, u32&) = 0;
    };

} // namespace rohan

namespace rohan {

    class StandardVertex1 : public VertexShader {
      public:

        glm::vec4& main(u64 id, void* data) override {
            auto vertex = reinterpret_cast<Vertex*>(data);
            vertex->pos = mat_transform * vertex->pos;
            return vertex->pos;
        }

        glm::mat4 mat_transform;

      private:

        struct Vertex {
            glm::vec4 pos;
        };
    };

    class StandardFrag1 : public FragShader {
      public:

        void main(u64 id, glm::vec3& bc, u32& out) override { out = 0xff00ffff; }
    };

}; // namespace rohan

#endif
