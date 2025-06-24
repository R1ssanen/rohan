#include "state.hpp"

namespace rohan {

    RenderConfig& RenderState::config() noexcept { return m_config; }

    RenderState&  RenderState::config(const RenderConfig& config) noexcept {
        m_config = config;
        return *this;
    }

    Program&     RenderState::program() noexcept { return *m_program; }

    RenderState& RenderState::program(Program& program) noexcept {
        m_program = &program;
        return *this;
    }

} // namespace rohan
