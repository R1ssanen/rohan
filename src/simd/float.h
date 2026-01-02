#ifndef ROHAN_SIMD_FLOAT_H_
#define ROHAN_SIMD_FLOAT_H_

#if !defined __AVX2__
#error "AVX2 instructions required."
#else

#include <immintrin.h>
#include <stdbool.h>

#include "int.h"

typedef __m256 rvecf;

static inline rvecf rvm_add_f32(rvecf a, rvecf b)
{
    return _mm256_add_ps(a, b);
}

static inline rvecf rvm_div_f32(rvecf a, rvecf b)
{
    return _mm256_div_ps(a, b);
}

static inline rvecf rvm_mul_f32(rvecf a, rvecf b)
{
    return _mm256_mul_ps(a, b);
}

static inline rvecf rvm_sub_f32(rvecf a, rvecf b)
{
    return _mm256_sub_ps(a, b);
}

#if defined __FMA__
static inline rvecf rvm_fma_f32(rvecf a, rvecf b, rvecf c)
{
    return _mm256_fmadd_ps(a, b, c);
}

static inline rvecf rvm_fms_f32(rvecf a, rvecf b, rvecf c)
{
    return _mm256_fmsub_ps(a, b, c);
}
#else
static inline rvecf rvm_fma_f32(rvecf a, rvecf b, rvecf c)
{
    return _mm256_add_ps(_mm256_mul_ps(a, b), c);
}

static inline rvecf rvm_fms_f32(rvecf a, rvecf b, rvecf c)
{
    return _mm256_sub_ps(_mm256_mul_ps(a, b), c);
}
#endif

#define rvm_compare_f32(a, b, comp) _mm256_cmp_ps(a, b, comp);

static inline rvecf rvm_inv_f32(rvecf a)
{
    return _mm256_rcp_ps(a);
}

static inline rvecf rvm_isqrt_f32(rvecf a)
{
    return _mm256_rsqrt_ps(a);
}

static inline rvecf rvm_sqrt_f32(rvecf a)
{
    return _mm256_sqrt_ps(a);
}

static inline rvecf rvecf_undefined(void)
{
    return _mm256_undefined_ps();
}

static inline rvecf rvm_broadcast_f32(const float *from)
{
    return _mm256_broadcast_ss(from);
}

static inline rvecf rvm_gather_f32(const float *from, rveci index)
{
    return _mm256_i32gather_ps(from, index, sizeof(float));
}

// 32-byte aligned
static inline rvecf rvm_load_f32(const float *from)
{
    return _mm256_load_ps(from);
}

// non-aligned
static inline rvecf rvm_loadu_f32(const float *from)
{
    return _mm256_loadu_ps(from);
}

static inline rvecf rvm_maskload_f32(const float *from, rveci mask)
{
    return _mm256_maskload_ps(from, mask);
}

static inline rvecf rvm_and_f32(rvecf a, rvecf b)
{
    return _mm256_and_ps(a, b);
}

static inline rvecf rvm_andnot_f32(rvecf a, rvecf b)
{
    return _mm256_andnot_ps(a, b);
}

static inline rvecf rvm_or_f32(rvecf a, rvecf b)
{
    return _mm256_or_ps(a, b);
}

static inline bool rvm_contains_f32(rvecf a, rvecf b)
{
    return _mm256_testc_ps(a, b);
}

static inline bool rvm_empty_f32(rvecf a, rvecf b)
{
    return _mm256_testz_ps(a, b);
}

static inline rvecf rvm_xor_f32(rvecf a, rvecf b)
{
    return _mm256_xor_ps(a, b);
}

static inline int rvm_make_mask_f32(rvecf a)
{
    return _mm256_movemask_ps(a);
}

static inline rvecf rvecf_f32(float a)
{
    return _mm256_set1_ps(a);
}

static inline rvecf rvecf_set_f32(float a, float b, float c, float d, float e, float f, float g, float h)
{
    return _mm256_setr_ps(a, b, c, d, e, f, g, h);
}

static inline rvecf rvecf_zero(void)
{
    return _mm256_setzero_ps();
}

static inline rvecf rvm_ceil_f32(rvecf a)
{
    return _mm256_ceil_ps(a);
}

static inline rvecf rvm_floor_f32(rvecf a)
{
    return _mm256_floor_ps(a);
}

static inline rvecf rvm_max_f32(rvecf a, rvecf b)
{
    return _mm256_max_ps(a, b);
}

static inline rvecf rvm_min_f32(rvecf a, rvecf b)
{
    return _mm256_min_ps(a, b);
}

#define rvm_round_f32(a, round) _mm256_round_ps(a, round);

static inline void rvm_maskstore_f32(float *to, rveci mask, rvecf a)
{
    _mm256_maskstore_ps(to, mask, a);
}

// 32-byte aligned
static inline void rvm_store_f32(float *to, rvecf a)
{
    _mm256_store_ps(to, a);
}

// non-aligned
static inline void rvm_storeu_f32(float *to, rvecf a)
{
    _mm256_storeu_ps(to, a);
}

// 32-byte aligned
static inline void rvm_stream_f32(void *to, rvecf a)
{
    _mm256_stream_ps(to, a);
}

#define rvm_blendi_f32(a, b, mask) _mm256_blend_ps(a, b, mask)

static inline rvecf rvm_blend_f32(rvecf a, rvecf b, rvecf mask)
{
    return _mm256_blendv_ps(a, b, mask);
}

static inline rvecf rvm_permute_f32(rvecf a, rveci mask)
{
    return _mm256_permutevar8x32_ps(a, mask);
}

#define rvm_shuffle_f32(a, b, mask) _mm256_shuffle_ps(a, b, mask)

static inline rvecf rvm_increment_f32(rvecf a)
{
    return _mm256_add_ps(a, _mm256_set1_ps(1.f));
}

#endif
#endif