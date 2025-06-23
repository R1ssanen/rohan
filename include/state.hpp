#ifndef ROHAN_STATE_HPP_
#define ROHAN_STATE_HPP_

#include "config.hpp"

namespace rohan {

    class RenderState {
      public:

        RenderConfig& config() noexcept;
        RenderState&  config(const RenderConfig& config) noexcept;

      private:

        RenderConfig m_config;
    };

} // namespace rohan

#endif
