#include "state.hpp"

namespace rohan {

    RenderConfig& RenderState::config() noexcept { return m_config; }

    RenderState&  RenderState::config(const RenderConfig& config) noexcept {
        m_config = config;
        return *this;
    }

} // namespace rohan
