#ifndef ROHAN_RMATH_H_
#define ROHAN_RMATH_H_

#include <immintrin.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct bary bary;
struct bary
{
    float x0, y0, x1, y1, x2, y2;
    float dx_b0, dy_b0;
    float dx_b1, dy_b1;
    float dx_b2, dy_b2;
    float det, inv_det;
};

float edge(float ax, float ay, float bx, float by, float cx, float cy);

bool get_bary(float x0, float y0, float x1, float y1, float x2, float y2, bary *bary);

void get_coords(bary bary, float x, float y, float *b0, float *b1, float *b2);

static inline __m256i channels_to_rgba(__m256 rF, __m256 gF, __m256 bF)
{
    const __m256 scale = _mm256_set1_ps(255.0f);

    // Convert float to int32
    __m256i r32 = _mm256_cvtps_epi32(_mm256_mul_ps(rF, scale));
    __m256i g32 = _mm256_cvtps_epi32(_mm256_mul_ps(gF, scale));
    __m256i b32 = _mm256_cvtps_epi32(_mm256_mul_ps(bF, scale));

    // Narrow int32->int16
    __m128i r16 = _mm_packus_epi32(_mm256_castsi256_si128(r32), _mm256_extracti128_si256(r32, 1));
    __m128i g16 = _mm_packus_epi32(_mm256_castsi256_si128(g32), _mm256_extracti128_si256(g32, 1));
    __m128i b16 = _mm_packus_epi32(_mm256_castsi256_si128(b32), _mm256_extracti128_si256(b32, 1));

    // Narrow int16->int8 (low lane contains 8 values)
    __m128i r8 = _mm_packus_epi16(r16, r16);
    __m128i g8 = _mm_packus_epi16(g16, g16);
    __m128i b8 = _mm_packus_epi16(b16, b16);

    // Alpha = 255
    __m128i a8 = _mm_set1_epi32(~0);

    // SDL order: RGBA
    __m128i rg = _mm_unpacklo_epi8(r8, g8);
    __m128i ba = _mm_unpacklo_epi8(b8, a8);

    __m128i rgba_lo = _mm_unpacklo_epi16(rg, ba);
    __m128i rgba_hi = _mm_unpackhi_epi16(rg, ba);

    return _mm256_set_m128i(rgba_hi, rgba_lo);
}

#endif
