#include "simd.h"

__m256i prefix_masks[8];
__m256i postfix_masks[8];
__m256i infix_masks[64];
__m256 multiplier_masks[8];

static void init_coverage_tables(void)
{
    prefix_masks[0] = _mm256_set1_epi32(~0);
    prefix_masks[1] = _mm256_setr_epi32(0, ~0, ~0, ~0, ~0, ~0, ~0, ~0);
    prefix_masks[2] = _mm256_setr_epi32(0, 0, ~0, ~0, ~0, ~0, ~0, ~0);
    prefix_masks[3] = _mm256_setr_epi32(0, 0, 0, ~0, ~0, ~0, ~0, ~0);
    prefix_masks[4] = _mm256_setr_epi32(0, 0, 0, 0, ~0, ~0, ~0, ~0);
    prefix_masks[5] = _mm256_setr_epi32(0, 0, 0, 0, 0, ~0, ~0, ~0);
    prefix_masks[6] = _mm256_setr_epi32(0, 0, 0, 0, 0, 0, ~0, ~0);
    prefix_masks[7] = _mm256_setr_epi32(0, 0, 0, 0, 0, 0, 0, ~0);

    postfix_masks[0] = _mm256_set1_epi32(~0);
    postfix_masks[1] = _mm256_setr_epi32(~0, ~0, ~0, ~0, ~0, ~0, ~0, 0);
    postfix_masks[2] = _mm256_setr_epi32(~0, ~0, ~0, ~0, ~0, ~0, 0, 0);
    postfix_masks[3] = _mm256_setr_epi32(~0, ~0, ~0, ~0, ~0, 0, 0, 0);
    postfix_masks[4] = _mm256_setr_epi32(~0, ~0, ~0, ~0, 0, 0, 0, 0);
    postfix_masks[5] = _mm256_setr_epi32(~0, ~0, ~0, 0, 0, 0, 0, 0);
    postfix_masks[6] = _mm256_setr_epi32(~0, ~0, 0, 0, 0, 0, 0, 0);
    postfix_masks[7] = _mm256_setr_epi32(~0, 0, 0, 0, 0, 0, 0, 0);

    infix_masks[0] = _mm256_setr_epi32(~0, 0, 0, 0, 0, 0, 0, 0);
    infix_masks[1] = _mm256_setr_epi32(~0, ~0, 0, 0, 0, 0, 0, 0);
    infix_masks[2] = _mm256_setr_epi32(~0, ~0, ~0, 0, 0, 0, 0, 0);
    infix_masks[3] = _mm256_setr_epi32(~0, ~0, ~0, ~0, 0, 0, 0, 0);
    infix_masks[4] = _mm256_setr_epi32(~0, ~0, ~0, ~0, ~0, 0, 0, 0);
    infix_masks[5] = _mm256_setr_epi32(~0, ~0, ~0, ~0, ~0, ~0, 0, 0);
    infix_masks[6] = _mm256_setr_epi32(~0, ~0, ~0, ~0, ~0, ~0, ~0, 0);
    infix_masks[7] = _mm256_setr_epi32(~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0);

    infix_masks[9] = _mm256_setr_epi32(0, ~0, 0, 0, 0, 0, 0, 0);
    infix_masks[10] = _mm256_setr_epi32(0, ~0, ~0, 0, 0, 0, 0, 0);
    infix_masks[11] = _mm256_setr_epi32(0, ~0, ~0, ~0, 0, 0, 0, 0);
    infix_masks[12] = _mm256_setr_epi32(0, ~0, ~0, ~0, ~0, 0, 0, 0);
    infix_masks[13] = _mm256_setr_epi32(0, ~0, ~0, ~0, ~0, ~0, 0, 0);
    infix_masks[14] = _mm256_setr_epi32(0, ~0, ~0, ~0, ~0, ~0, ~0, 0);
    infix_masks[15] = _mm256_setr_epi32(0, ~0, ~0, ~0, ~0, ~0, ~0, ~0);

    infix_masks[18] = _mm256_setr_epi32(0, 0, ~0, 0, 0, 0, 0, 0);
    infix_masks[19] = _mm256_setr_epi32(0, 0, ~0, ~0, 0, 0, 0, 0);
    infix_masks[20] = _mm256_setr_epi32(0, 0, ~0, ~0, ~0, 0, 0, 0);
    infix_masks[21] = _mm256_setr_epi32(0, 0, ~0, ~0, ~0, ~0, 0, 0);
    infix_masks[22] = _mm256_setr_epi32(0, 0, ~0, ~0, ~0, ~0, ~0, 0);
    infix_masks[23] = _mm256_setr_epi32(0, 0, ~0, ~0, ~0, ~0, ~0, ~0);

    infix_masks[27] = _mm256_setr_epi32(0, 0, 0, ~0, 0, 0, 0, 0);
    infix_masks[28] = _mm256_setr_epi32(0, 0, 0, ~0, ~0, 0, 0, 0);
    infix_masks[29] = _mm256_setr_epi32(0, 0, 0, ~0, ~0, ~0, 0, 0);
    infix_masks[30] = _mm256_setr_epi32(0, 0, 0, ~0, ~0, ~0, ~0, 0);
    infix_masks[31] = _mm256_setr_epi32(0, 0, 0, ~0, ~0, ~0, ~0, ~0);

    infix_masks[36] = _mm256_setr_epi32(0, 0, 0, 0, ~0, 0, 0, 0);
    infix_masks[37] = _mm256_setr_epi32(0, 0, 0, 0, ~0, ~0, 0, 0);
    infix_masks[38] = _mm256_setr_epi32(0, 0, 0, 0, ~0, ~0, ~0, 0);
    infix_masks[39] = _mm256_setr_epi32(0, 0, 0, 0, ~0, ~0, ~0, ~0);

    infix_masks[45] = _mm256_setr_epi32(0, 0, 0, 0, 0, ~0, 0, 0);
    infix_masks[46] = _mm256_setr_epi32(0, 0, 0, 0, 0, ~0, ~0, 0);
    infix_masks[47] = _mm256_setr_epi32(0, 0, 0, 0, 0, ~0, ~0, ~0);

    infix_masks[54] = _mm256_setr_epi32(0, 0, 0, 0, 0, 0, ~0, 0);
    infix_masks[55] = _mm256_setr_epi32(0, 0, 0, 0, 0, 0, ~0, ~0);

    infix_masks[63] = _mm256_setr_epi32(0, 0, 0, 0, 0, 0, 0, ~0);
}

static void init_multiplier_table(void)
{
    multiplier_masks[0] = _mm256_setr_ps(0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f);
    multiplier_masks[1] = _mm256_setr_ps(-1.f, 0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f);
    multiplier_masks[2] = _mm256_setr_ps(-2.f, -1.f, 0.f, 1.f, 2.f, 3.f, 4.f, 5.f);
    multiplier_masks[3] = _mm256_setr_ps(-3.f, -2.f, -1.f, 0.f, 1.f, 2.f, 3.f, 4.f);
    multiplier_masks[4] = _mm256_setr_ps(-4.f, -3.f, -2.f, -1.f, 0.f, 1.f, 2.f, 3.f);
    multiplier_masks[5] = _mm256_setr_ps(-5.f, -4.f, -3.f, -2.f, -1.f, 0.f, 1.f, 2.f);
    multiplier_masks[6] = _mm256_setr_ps(-6.f, -5.f, -4.f, -3.f, -2.f, -1.f, 0.f, 1.f);
    multiplier_masks[7] = _mm256_setr_ps(-7.f, -6.f, -5.f, -4.f, -3.f, -2.f, -1.f, 0.f);
}

void init_simd_tables(void)
{
    init_coverage_tables();
    init_multiplier_table();
}
