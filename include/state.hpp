#ifndef ROHAN_STATE_HPP_
#define ROHAN_STATE_HPP_

#include "config.hpp"
#include "rhdefs.hpp"
#include "shader_type.hpp"

namespace rohan {

    class RAPI RenderState {
      public:

        RenderConfig& config() noexcept;
        RenderState&  config(const RenderConfig& config) noexcept;

        Program&      program() noexcept;
        RenderState&  program(Program& program) noexcept;

      private:

        RenderConfig m_config;
        Program*     m_program;
    };

} // namespace rohan

#endif
