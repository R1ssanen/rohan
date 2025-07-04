#ifndef ROHAN_SIMD_VECTOR_HPP_
#define ROHAN_SIMD_VECTOR_HPP_

#include <immintrin.h>

#include "rhdefs.hpp"

#if defined(_MSC_VER)
#    define RINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#    define RINLINE inline __attribute__((always_inline))
#else
#    define RINLINE inline
#endif

#ifndef NDEBUG
#    define RASSERT_ALIGNMENT(ptr, align)                                                          \
        if ((reinterpret_cast<uintptr_t>(ptr) & (align - 1)) != 0) {                               \
            std::cerr << "error: alignment of " << align << " bytes broken by " << ptr << '\n';    \
            abort();                                                                               \
        }
#else
#    define RASSERT_ALIGNMENT(ptr, align)
#endif

namespace rohan {

    struct U256 reinterpret_u256(const struct F256&);

}

namespace rohan {

    struct U256 {
      public:

        __m256i m_v;

      public:

        U256(const __m256i& v) : m_v(v) { }

        U256(u32 i) : m_v(_mm256_set1_epi32(i)) { }

        // MEMORY

        RINLINE U256 load(const u32* ptr) const noexcept {
            return _mm256_load_si256(reinterpret_cast<const __m256i*>(ptr));
        }

        RINLINE void store_aligned(u32* ptr) const noexcept {
            RASSERT_ALIGNMENT(ptr, 32)
            store_aligned(reinterpret_cast<__m256i*>(ptr));
        }

        RINLINE void store_aligned(__m256i* ptr) const noexcept {
            RASSERT_ALIGNMENT(ptr, 32)
            _mm256_stream_si256(ptr, m_v);
        }

        RINLINE void store_aligned(u32* ptr, const U256& mask) const noexcept {
            RASSERT_ALIGNMENT(ptr, 32)
            _mm256_maskstore_epi32(reinterpret_cast<i32*>(ptr), mask.m_v, m_v);
        }

        // LOGIC

        RINLINE bool is_zero() const noexcept { return _mm256_testz_si256(m_v, m_v) != 0; }

        // BITWISE

        RINLINE U256 operator<<(u32 count) const noexcept { return _mm256_slli_epi32(m_v, count); }

        RINLINE U256 operator>>(u32 count) const noexcept { return _mm256_srli_epi32(m_v, count); }

        RINLINE U256 operator&(const U256& v) const noexcept {
            return _mm256_and_si256(m_v, v.m_v);
        }

        RINLINE U256 operator|(const U256& v) const noexcept { return _mm256_or_si256(m_v, v.m_v); }

        // ARITHMETIC

        RINLINE U256 operator+(const U256& v) const noexcept {
            return _mm256_add_epi32(m_v, v.m_v);
        }

        RINLINE U256 operator-(const U256& v) const noexcept {
            return _mm256_sub_epi32(m_v, v.m_v);
        }

        RINLINE U256 operator*(const U256& v) const noexcept {
            return _mm256_mul_epi32(m_v, v.m_v);
        }

        // ASSIGN

        RINLINE void operator+=(const U256& v) noexcept { m_v = _mm256_add_epi32(m_v, v.m_v); }

        RINLINE void operator-=(const U256& v) noexcept { m_v = _mm256_sub_epi32(m_v, v.m_v); }

        RINLINE void operator*=(const U256& v) noexcept { m_v = _mm256_mul_epi32(m_v, v.m_v); }
    };

    RINLINE U256 interleave_rgb(const U256& r, const U256& g, const U256& b) {
        U256 byte = 0xff;
        // __m128i r_shift = _mm_slli_epi32(_mm_and_si128(r, byte), 24);
        // __m128i g_shift = _mm_slli_epi32(_mm_and_si128(g, byte), 16);
        // __m128i b_shift = _mm_slli_epi32(_mm_and_si128(b, byte), 8);

        return ((r & byte) << 24) | ((g & byte) << 16) | ((b & byte) << 8) | byte;

        // return _mm_or_si128(r_shift, _mm_or_si128(g_shift, _mm_or_si128(b_shift, byte)));
    }

} // namespace rohan

namespace rohan {

    struct F256 {
      public:

        __m256 m_v;

      public:

        F256(const __m256& v) : m_v(v) { }

        F256(f32 f) : m_v(_mm256_set1_ps(f)) { }

        RINLINE explicit operator U256() const noexcept { return _mm256_cvtps_epi32(m_v); }

        // MEMORY

        RINLINE F256 load(const f32* ptr) { return _mm256_load_ps(ptr); }

        RINLINE void store_aligned(f32* ptr) { _mm256_stream_ps(ptr, m_v); }

        RINLINE void store_aligned(f32* ptr, const U256& mask) {
            _mm256_maskstore_ps(ptr, mask.m_v, m_v);
        }

        // ARITHMETIC

        RINLINE F256 operator+(const F256& v) const noexcept { return _mm256_add_ps(m_v, v.m_v); }

        RINLINE F256 operator-(const F256& v) const noexcept { return _mm256_sub_ps(m_v, v.m_v); }

        RINLINE F256 operator*(const F256& v) const noexcept { return _mm256_mul_ps(m_v, v.m_v); }

        RINLINE F256 operator/(const F256& v) const noexcept { return _mm256_div_ps(m_v, v.m_v); }

        // ASSIGN

        RINLINE void operator+=(const F256& v) noexcept { m_v = _mm256_add_ps(m_v, v.m_v); }

        RINLINE void operator-=(const F256& v) noexcept { m_v = _mm256_sub_ps(m_v, v.m_v); }

        RINLINE void operator*=(const F256& v) noexcept { m_v = _mm256_mul_ps(m_v, v.m_v); }

        RINLINE void operator/=(const F256& v) noexcept { m_v = _mm256_div_ps(m_v, v.m_v); }

        // COMPARE

        RINLINE U256 operator<(const F256& v) const noexcept {
            return reinterpret_u256(_mm256_cmp_ps(m_v, v.m_v, _CMP_LT_OQ));
        }

        RINLINE U256 operator>(const F256& v) const noexcept {
            return reinterpret_u256(_mm256_cmp_ps(m_v, v.m_v, _CMP_GT_OQ));
        }

        RINLINE U256 operator<=(const F256& v) const noexcept {
            return reinterpret_u256(_mm256_cmp_ps(m_v, v.m_v, _CMP_LE_OQ));
        }

        RINLINE U256 operator>=(const F256& v) const noexcept {
            return reinterpret_u256(_mm256_cmp_ps(m_v, v.m_v, _CMP_GE_OQ));
        }

        // SPECIAL

        F256 trilerp(const F256& w0, const F256& w1, const F256& w2) {
            return _mm256_fmadd_ps(
                w0.m_v, m_v, _mm256_fmadd_ps(w1.m_v, m_v, _mm256_mul_ps(w2.m_v, m_v))
            );
        }
    };

} // namespace rohan

namespace rohan {

    RINLINE U256 reinterpret_u256(const F256& v) { return _mm256_castps_si256(v.m_v); }

} // namespace rohan

#endif
