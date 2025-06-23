#ifndef ROHAN_DEFS_HPP_
#define ROHAN_DEFS_HPP_

#include <cstdint>

#define RAPI __attribute__((visibility("default")))

namespace rohan {

    using u8  = uint8_t;
    using u32 = uint32_t;
    using i32 = int32_t;
    using u64 = uint64_t;

    using f32 = float;

} // namespace rohan

#endif
