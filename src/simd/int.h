#ifndef ROHAN_SIMD_INT_H_
#define ROHAN_SIMD_INT_H_

#if !defined __AVX2__
#error "AVX2 instructions required."
#else

#include <immintrin.h>
#include <stdbool.h>

typedef __m256i rveci;

static inline rveci rvm_add_i64(rveci a, rveci b)
{
    return _mm256_add_epi64(a, b);
}

static inline rveci rvm_add_i32(rveci a, rveci b)
{
    return _mm256_add_epi32(a, b);
}

static inline rveci rvm_add_i8(rveci a, rveci b)
{
    return _mm256_add_epi8(a, b);
}

static inline rveci rvm_mul_i32(rveci a, rveci b)
{
    return _mm256_mullo_epi32(a, b);
}

static inline rveci rvm_sub_i64(rveci a, rveci b)
{
    return _mm256_sub_epi64(a, b);
}

static inline rveci rvm_sub_i32(rveci a, rveci b)
{
    return _mm256_sub_epi32(a, b);
}

static inline rveci rvm_sub_i8(rveci a, rveci b)
{
    return _mm256_sub_epi8(a, b);
}

static inline rveci rvm_equals_i64(rveci a, rveci b)
{
    return _mm256_cmpeq_epi64(a, b);
}

static inline rveci rvm_equals_i32(rveci a, rveci b)
{
    return _mm256_cmpeq_epi32(a, b);
}

static inline rveci rvm_equals_i8(rveci a, rveci b)
{
    return _mm256_cmpeq_epi8(a, b);
}

static inline rveci rvm_greater_i64(rveci a, rveci b)
{
    return _mm256_cmpgt_epi64(a, b);
}

static inline rveci rvm_greater_i32(rveci a, rveci b)
{
    return _mm256_cmpgt_epi32(a, b);
}

static inline rveci rvm_greater_i8(rveci a, rveci b)
{
    return _mm256_cmpgt_epi8(a, b);
}

static inline rveci rveci_undefined(void)
{
    return _mm256_undefined_si256();
}

#define rvm_gather_int(from, index, stride) _mm256_i32gather_epi32(from, index, stride);

// 32-byte aligned
static inline rveci rvm_load_int(const rveci *from)
{
    return _mm256_load_si256(from);
}

// non-aligned
static inline rveci rvm_loadu_int(const rveci *from)
{
    return _mm256_loadu_si256(from);
}

static inline rveci rvm_maskload_i64(const long long *from, rveci mask)
{
    return _mm256_maskload_epi64(from, mask);
}

static inline rveci rvm_maskload_i32(const int *from, rveci mask)
{
    return _mm256_maskload_epi32(from, mask);
}

// 32-byte aligned
static inline rveci rvm_streamload_int(const void *from)
{
    return _mm256_stream_load_si256(from);
}

static inline rveci rvm_and_int(rveci a, rveci b)
{
    return _mm256_and_si256(a, b);
}

static inline rveci rvm_andnot_int(rveci a, rveci b)
{
    return _mm256_andnot_si256(a, b);
}

static inline rveci rvm_or_int(rveci a, rveci b)
{
    return _mm256_or_si256(a, b);
}

static inline bool rvm_contains_int(rveci a, rveci b)
{
    return _mm256_testc_si256(a, b);
}

static inline bool rvm_empty_int(rveci a, rveci b)
{
    return _mm256_testz_si256(a, b);
}

static inline rveci rvm_xor_int(rveci a, rveci b)
{
    return _mm256_xor_si256(a, b);
}

static inline int rvm_make_mask_int(rveci a)
{
    return _mm256_movemask_epi8(a);
}

static inline rveci rvm_average_u8(rveci a, rveci b)
{
    return _mm256_avg_epu8(a, b);
}

static inline rveci rveci_i64(long long a)
{
    return _mm256_set1_epi64x(a);
}

static inline rveci rveci_i32(int a)
{
    return _mm256_set1_epi32(a);
}

static inline rveci rveci_i8(char a)
{
    return _mm256_set1_epi8(a);
}

static inline rveci rveci_set_i64(long long a, long long b, long long c, long long d)
{
    return _mm256_setr_epi64x(a, b, c, d);
}

static inline rveci rveci_set_i32(int a, int b, int c, int d, int e, int f, int g, int h)
{
    return _mm256_setr_epi32(a, b, c, d, e, f, g, h);
}

static inline rveci rveci_zero(void)
{
    return _mm256_setzero_si256();
}

#define rvm_lshift_int(a, count) _mm256_slli_si256(a, count);

static inline rveci rvm_lshift_i64(rveci a, int count)
{
    return _mm256_slli_epi64(a, count);
}

static inline rveci rvm_lshift_i32(rveci a, int count)
{
    return _mm256_slli_epi32(a, count);
}

#define rvm_rshift_uint(a, count) _mm256_srli_si256(a, count);

static inline rveci rvm_rshift_u64(rveci a, int count)
{
    return _mm256_srli_epi64(a, count);
}

static inline rveci rvm_rshift_u32(rveci a, int count)
{
    return _mm256_srli_epi32(a, count);
}

static inline rveci rvm_rshift_i64(rveci a, int count)
{
    return _mm256_srai_epi64(a, count);
}

static inline rveci rvm_rshift_i32(rveci a, int count)
{
    return _mm256_srai_epi32(a, count);
}

static inline rveci rvm_abs_i32(rveci a)
{
    return _mm256_abs_epi32(a);
}

static inline rveci rvm_abs_i8(rveci a)
{
    return _mm256_abs_epi8(a);
}

static inline rveci rvm_max_u32(rveci a, rveci b)
{
    return _mm256_max_epu32(a, b);
}

static inline rveci rvm_max_u8(rveci a, rveci b)
{
    return _mm256_max_epu8(a, b);
}

static inline rveci rvm_max_i32(rveci a, rveci b)
{
    return _mm256_max_epi32(a, b);
}

static inline rveci rvm_max_i8(rveci a, rveci b)
{
    return _mm256_max_epi8(a, b);
}

static inline rveci rvm_min_u32(rveci a, rveci b)
{
    return _mm256_min_epu32(a, b);
}

static inline rveci rvm_min_u8(rveci a, rveci b)
{
    return _mm256_min_epu8(a, b);
}

static inline rveci rvm_min_i32(rveci a, rveci b)
{
    return _mm256_min_epi32(a, b);
}

static inline rveci rvm_min_i8(rveci a, rveci b)
{
    return _mm256_min_epi8(a, b);
}

static inline void rvm_maskstore_i64(long long *to, rveci mask, rveci a)
{
    _mm256_maskstore_epi64(to, mask, a);
}

static inline void rvm_maskstore_i32(int *to, rveci mask, rveci a)
{
    _mm256_maskstore_epi32(to, mask, a);
}

// 32-byte aligned
static inline void rvm_store_int(void *to, rveci a)
{
    _mm256_store_epi32(to, a);
}

// non-aligned
static inline void rvm_storeu_int(void *to, rveci a)
{
    _mm256_storeu_epi32(to, a);
}

// 32-byte aligned
static inline void rvm_stream_int(void *to, rveci a)
{
    _mm256_stream_si256(to, a);
}

#define rvm_blend_i32(a, b, mask) _mm256_blend_epi32(a, b, mask);

static inline rveci rvm_blend_i8(rveci a, rveci b, rveci mask)
{
    return _mm256_blendv_epi8(a, b, mask);
}

#define rvm_extract_i64(a, index) _mm256_extract_epi64(a, index);

#define rvm_extract_i32(a, index) _mm256_extract_epi32(a, index);

#define rvm_extract_i8(a, index) _mm256_extract_epi8(a, index);

#define rvm_insert_i64(a, i, index) _mm256_insert_epi64(a, i, index);

#define rvm_insert_i32(a, i, index) _mm256_insert_epi32(a, i, index);

#define rvm_insert_i8(a, i, index) _mm256_insert_epi8(a, i, index);

static inline rveci rvm_permute_i32(rveci a, rveci mask)
{
    return _mm256_permutevar8x32_epi32(a, mask);
}

#define rvm_shuffle_i32(a, mask) _mm256_shuffle_epi32(a, mask)

#define rvm_shuffle_i8(a, mask) _mm256_shuffle_epi8(a, mask)

#endif
#endif