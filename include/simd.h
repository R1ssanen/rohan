#ifndef ROHAN_SIMD_H_
#define ROHAN_SIMD_H_

#if defined(__AVX2__)
#include <immintrin.h>

#define ROHAN_SIMD_AVX2
#define ROHAN_LANE_WIDTH 8

extern __m256i prefix_masks[8];
extern __m256i postfix_masks[8];
extern __m256i infix_masks[64];
extern __m256 multiplier_masks[8];

void init_simd_tables(void);

#else
#error "AVX2 required"
#endif

#endif