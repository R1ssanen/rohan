#include "rohan.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define swap_f(a, b)                                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        float tmp = a;                                                                                                 \
        a = b;                                                                                                         \
        b = tmp;                                                                                                       \
    } while (0)

#define swap_fptr(a, b)                                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        const float *temp = a;                                                                                         \
        a = b;                                                                                                         \
        b = temp;                                                                                                      \
    } while (0)

static inline __m256 load_m256(const uint8_t *src)
{
    __m256 v;
    memcpy(&v, src, sizeof(__m256));
    return v;
}

static inline void store_m256(uint8_t *dest, __m256 v)
{
    memcpy(dest, &v, sizeof(__m256));
}

static __m256i prefix_masks[8];
static __m256i postfix_masks[8];
static __m256i infix_masks[8][8];
static __m256 multiplier_masks[8];

void rohan_init(void)
{
    prefix_masks[0] = _mm256_set1_epi32(-1);
    prefix_masks[1] = _mm256_setr_epi32(0, -1, -1, -1, -1, -1, -1, -1);
    prefix_masks[2] = _mm256_setr_epi32(0, 0, -1, -1, -1, -1, -1, -1);
    prefix_masks[3] = _mm256_setr_epi32(0, 0, 0, -1, -1, -1, -1, -1);
    prefix_masks[4] = _mm256_setr_epi32(0, 0, 0, 0, -1, -1, -1, -1);
    prefix_masks[5] = _mm256_setr_epi32(0, 0, 0, 0, 0, -1, -1, -1);
    prefix_masks[6] = _mm256_setr_epi32(0, 0, 0, 0, 0, 0, -1, -1);
    prefix_masks[7] = _mm256_setr_epi32(0, 0, 0, 0, 0, 0, 0, -1);

    postfix_masks[0] = _mm256_setr_epi32(-1, 0, 0, 0, 0, 0, 0, 0);
    postfix_masks[1] = _mm256_setr_epi32(-1, -1, 0, 0, 0, 0, 0, 0);
    postfix_masks[2] = _mm256_setr_epi32(-1, -1, -1, 0, 0, 0, 0, 0);
    postfix_masks[3] = _mm256_setr_epi32(-1, -1, -1, -1, 0, 0, 0, 0);
    postfix_masks[4] = _mm256_setr_epi32(-1, -1, -1, -1, -1, 0, 0, 0);
    postfix_masks[5] = _mm256_setr_epi32(-1, -1, -1, -1, -1, -1, 0, 0);
    postfix_masks[6] = _mm256_setr_epi32(-1, -1, -1, -1, -1, -1, -1, 0);
    postfix_masks[7] = _mm256_set1_epi32(-1);

    infix_masks[0][0] = _mm256_setr_epi32(-1, 0, 0, 0, 0, 0, 0, 0);
    infix_masks[0][1] = _mm256_setr_epi32(-1, -1, 0, 0, 0, 0, 0, 0);
    infix_masks[0][2] = _mm256_setr_epi32(-1, -1, -1, 0, 0, 0, 0, 0);
    infix_masks[0][3] = _mm256_setr_epi32(-1, -1, -1, -1, 0, 0, 0, 0);
    infix_masks[0][4] = _mm256_setr_epi32(-1, -1, -1, -1, -1, 0, 0, 0);
    infix_masks[0][5] = _mm256_setr_epi32(-1, -1, -1, -1, -1, -1, 0, 0);
    infix_masks[0][6] = _mm256_setr_epi32(-1, -1, -1, -1, -1, -1, -1, 0);
    infix_masks[0][7] = _mm256_setr_epi32(-1, -1, -1, -1, -1, -1, -1, -1);
    infix_masks[1][1] = _mm256_setr_epi32(0, -1, 0, 0, 0, 0, 0, 0);
    infix_masks[1][2] = _mm256_setr_epi32(0, -1, -1, 0, 0, 0, 0, 0);
    infix_masks[1][3] = _mm256_setr_epi32(0, -1, -1, -1, 0, 0, 0, 0);
    infix_masks[1][4] = _mm256_setr_epi32(0, -1, -1, -1, -1, 0, 0, 0);
    infix_masks[1][5] = _mm256_setr_epi32(0, -1, -1, -1, -1, -1, 0, 0);
    infix_masks[1][6] = _mm256_setr_epi32(0, -1, -1, -1, -1, -1, -1, 0);
    infix_masks[1][7] = _mm256_setr_epi32(0, -1, -1, -1, -1, -1, -1, -1);
    infix_masks[2][2] = _mm256_setr_epi32(0, 0, -1, 0, 0, 0, 0, 0);
    infix_masks[2][3] = _mm256_setr_epi32(0, 0, -1, -1, 0, 0, 0, 0);
    infix_masks[2][4] = _mm256_setr_epi32(0, 0, -1, -1, -1, 0, 0, 0);
    infix_masks[2][5] = _mm256_setr_epi32(0, 0, -1, -1, -1, -1, 0, 0);
    infix_masks[2][6] = _mm256_setr_epi32(0, 0, -1, -1, -1, -1, -1, 0);
    infix_masks[2][7] = _mm256_setr_epi32(0, 0, -1, -1, -1, -1, -1, -1);
    infix_masks[3][3] = _mm256_setr_epi32(0, 0, 0, -1, 0, 0, 0, 0);
    infix_masks[3][4] = _mm256_setr_epi32(0, 0, 0, -1, -1, 0, 0, 0);
    infix_masks[3][5] = _mm256_setr_epi32(0, 0, 0, -1, -1, -1, 0, 0);
    infix_masks[3][6] = _mm256_setr_epi32(0, 0, 0, -1, -1, -1, -1, 0);
    infix_masks[3][7] = _mm256_setr_epi32(0, 0, 0, -1, -1, -1, -1, -1);
    infix_masks[4][4] = _mm256_setr_epi32(0, 0, 0, 0, -1, 0, 0, 0);
    infix_masks[4][5] = _mm256_setr_epi32(0, 0, 0, 0, -1, -1, 0, 0);
    infix_masks[4][6] = _mm256_setr_epi32(0, 0, 0, 0, -1, -1, -1, 0);
    infix_masks[4][7] = _mm256_setr_epi32(0, 0, 0, 0, -1, -1, -1, -1);
    infix_masks[5][5] = _mm256_setr_epi32(0, 0, 0, 0, 0, -1, 0, 0);
    infix_masks[5][6] = _mm256_setr_epi32(0, 0, 0, 0, 0, -1, -1, 0);
    infix_masks[5][7] = _mm256_setr_epi32(0, 0, 0, 0, 0, -1, -1, -1);
    infix_masks[6][6] = _mm256_setr_epi32(0, 0, 0, 0, 0, 0, -1, 0);
    infix_masks[6][7] = _mm256_setr_epi32(0, 0, 0, 0, 0, 0, -1, -1);
    infix_masks[7][7] = _mm256_setr_epi32(0, 0, 0, 0, 0, 0, 0, -1);

    multiplier_masks[0] =
        _mm256_setr_ps(0.f, 1.f / 8.f, 2.f / 8.f, 3.f / 8.f, 4.f / 8.f, 5.f / 8.f, 6.f / 8.f, 7.f / 8.f);
    multiplier_masks[1] =
        _mm256_setr_ps(-1.f / 8.f, 0.f, 1.f / 8.f, 2.f / 8.f, 3.f / 8.f, 4.f / 8.f, 5.f / 8.f, 6.f / 8.f);
    multiplier_masks[2] =
        _mm256_setr_ps(-2.f / 8.f, -1.f / 8.f, 0.f, 1.f / 8.f, 2.f / 8.f, 3.f / 8.f, 4.f / 8.f, 5.f / 8.f);
    multiplier_masks[3] =
        _mm256_setr_ps(-3.f / 8.f, -2.f / 8.f, -1.f / 8.f, 0.f, 1.f / 8.f, 2.f / 8.f, 3.f / 8.f, 4.f / 8.f);
    multiplier_masks[4] =
        _mm256_setr_ps(-4.f / 8.f, -3.f / 8.f, -2.f / 8.f, -1.f / 8.f, 0.f, 1.f / 8.f, 2.f / 8.f, 3.f / 8.f);
    multiplier_masks[5] =
        _mm256_setr_ps(-5.f / 8.f, -4.f / 8.f, -3.f / 8.f, -2.f / 8.f, -1.f / 8.f, 0.f, 1.f / 8.f, 2.f / 8.f);
    multiplier_masks[6] =
        _mm256_setr_ps(-6.f / 8.f, -5.f / 8.f, -4.f / 8.f, -3.f / 8.f, -2.f / 8.f, -1.f / 8.f, 0.f, 1.f / 8.f);
    multiplier_masks[7] =
        _mm256_setr_ps(-7.f / 8.f, -6.f / 8.f, -5.f / 8.f, -4.f / 8.f, -3.f / 8.f, -2.f / 8.f, -1.f / 8.f, 0.f);

    // multiplier_masks[0] = _mm256_setr_ps(0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f);
    // multiplier_masks[1] = _mm256_setr_ps(-1.f, 0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f);
    // multiplier_masks[2] = _mm256_setr_ps(-2.f, -1.f, 0.f, 1.f, 2.f, 3.f, 4.f, 5.f);
    // multiplier_masks[3] = _mm256_setr_ps(-3.f, -2.f, -1.f, 0.f, 1.f, 2.f, 3.f, 4.f);
    // multiplier_masks[4] = _mm256_setr_ps(-4.f, -3.f, -2.f, -1.f, 0.f, 1.f, 2.f, 3.f);
    // multiplier_masks[5] = _mm256_setr_ps(-5.f, -4.f, -3.f, -2.f, -1.f, 0.f, 1.f, 2.f);
    // multiplier_masks[6] = _mm256_setr_ps(-6.f, -5.f, -4.f, -3.f, -2.f, -1.f, 0.f, 1.f);
    // multiplier_masks[7] = _mm256_setr_ps(-7.f, -6.f, -5.f, -4.f, -3.f, -2.f, -1.f, 0.f);
}

static inline void advance(rohan_raster_state *restrict state, rohan_shader_desc *restrict desc, size_t target_stride,
                           void *restrict instance, uint8_t *restrict attributes)
{
    desc->main(instance, state);
    state->pos.x = _mm256_add_epi32(state->pos.x, _mm256_set1_epi32(8));
    state->byte_offset += 8ull * target_stride;

    for (size_t i = 0; i < desc->attribute_count; ++i)
    {
        uint8_t *field = attributes + i * sizeof(__m256);
        __m256 attribute = load_m256(field);
        store_m256(field, _mm256_add_ps(attribute, desc->_attr_dx[i]));
    }
}

static void rasterize(rohan_shader_desc *restrict desc, void *restrict instance, size_t target_pitch,
                      size_t target_stride, float x_left, float dx_dy_left, float x_right, float dx_dy_right, int y_top,
                      int y_bottom)
{
    const __m256i seq = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
    const __m256i one = _mm256_set1_epi32(1);

    rohan_raster_state state = {.pos = {.y = _mm256_set1_epi32(y_top)}};
    uint8_t *attributes = (uint8_t *)instance + desc->attribute_offset;

    size_t row_byte_offset = y_top * target_pitch;
    for (int y = y_top; y < y_bottom; y += 1, row_byte_offset += target_pitch)
    {
        int x0 = ceilf(x_left);
        int x1 = floorf(x_right);
        if (x0 > x1)
        {
            goto interpolate;
        }

        int x0_aligned = x0 & ~7;
        int x1_aligned = x1 & ~7;
        int header = x0 - x0_aligned;

        state.byte_offset = row_byte_offset + x0_aligned * target_stride;
        state.pos.x = _mm256_add_epi32(_mm256_set1_epi32(x0_aligned), seq);
        state.pos.y = _mm256_add_epi32(state.pos.y, one);

        __m256 multiplier_mask = multiplier_masks[header];
        for (size_t i = 0; i < desc->attribute_count; ++i)
        {
            __m256 attribute = _mm256_fmadd_ps(multiplier_mask, desc->_attr_dx[i], desc->_attr_0[i]);
            store_m256(attributes + i * sizeof(__m256), attribute);
        }

        int aligned_diff = x1_aligned - x0_aligned;
        if (aligned_diff == 0)
        {
            state.mask = infix_masks[header][x1 - x0_aligned];
            desc->main(instance, &state);
        }
        else
        {
            state.mask = prefix_masks[header];
            advance(&state, desc, target_stride, instance, attributes);
            state.mask = _mm256_set1_epi32(-1);

            for (int i = 1; i < aligned_diff >> 3; ++i)
            {
                advance(&state, desc, target_stride, instance, attributes);
            }

            state.mask = postfix_masks[x1 - x1_aligned];
            desc->main(instance, &state);
        }

    interpolate:
        x_left += dx_dy_left;
        x_right += dx_dy_right;

        for (size_t i = 0; i < desc->attribute_count; ++i)
        {
            desc->_attr_0[i] = _mm256_add_ps(desc->_attr_0[i], desc->_attr_dy[i]);
        }
    }
}

void rohan_rasterize(rohan_shader_desc *restrict desc, void *restrict instance, size_t target_pitch,
                     size_t target_stride, const float *rohan_restrict vertex0, const float *rohan_restrict vertex1,
                     const float *rohan_restrict vertex2)
{
    float x0 = vertex0[0], x1 = vertex1[0], x2 = vertex2[0];
    float y0 = floorf(vertex0[1]), y1 = floorf(vertex1[1]), y2 = floorf(vertex2[1]);
    const float *attr_0 = vertex0 + 2, *attr_1 = vertex1 + 2, *attr_2 = vertex2 + 2;

    if (y0 > y1)
    {
        swap_f(x0, x1);
        swap_f(y0, y1);
        swap_fptr(attr_0, attr_1);
    }
    if (y1 > y2)
    {
        swap_f(x1, x2);
        swap_f(y1, y2);
        swap_fptr(attr_1, attr_2);
    }
    if (y0 > y1)
    {
        swap_f(x0, x1);
        swap_f(y0, y1);
        swap_fptr(attr_0, attr_1);
    }

    float inv_dy_ac = 1.f / (y2 - y0);

    if ((int)y0 == (int)y1) // flat top
    {
        float dxdy_left = (x2 - x0) * inv_dy_ac;
        float dxdy_right = (x2 - x1) * inv_dy_ac;
        float inv_dx_ab = 1.f / (x1 - x0);

        if (x0 < x1)
        {
            for (size_t i = 0; i < desc->attribute_count; ++i)
            {
                desc->_attr_dx[i] = _mm256_set1_ps((attr_1[i] - attr_0[i]) * inv_dx_ab * 8.f);
                desc->_attr_dy[i] = _mm256_set1_ps((attr_2[i] - attr_0[i]) * inv_dy_ac);
                desc->_attr_0[i] = _mm256_set1_ps(attr_0[i]);
            }

            rasterize(desc, instance, target_pitch, target_stride, x0, dxdy_left, x1, dxdy_right, y0, y2);
        }
        else
        {
            float inv_dy_bc = 1.f / (y2 - y1);
            for (size_t i = 0; i < desc->attribute_count; ++i)
            {
                desc->_attr_dx[i] = _mm256_set1_ps((attr_0[i] - attr_1[i]) * -inv_dx_ab * 8.f);
                desc->_attr_dy[i] = _mm256_set1_ps((attr_2[i] - attr_1[i]) * inv_dy_bc);
                desc->_attr_0[i] = _mm256_set1_ps(attr_1[i]);
            }

            rasterize(desc, instance, target_pitch, target_stride, x1, dxdy_right, x0, dxdy_left, y0, y2);
        }
    }

    else if ((int)y1 == (int)y2) // flat bottom
    {
        float dxdy_left = (x1 - x0) * inv_dy_ac;
        float dxdy_right = (x2 - x0) * inv_dy_ac;
        float inv_dx_bc = 1.f / (x2 - x1);

        if (x1 < x2)
        {
            float inv_dy_ab = 1.f / (y1 - y0);

            for (size_t i = 0; i < desc->attribute_count; ++i)
            {
                desc->_attr_dx[i] = _mm256_set1_ps((attr_2[i] - attr_1[i]) * inv_dx_bc * 8.f);
                desc->_attr_dy[i] = _mm256_set1_ps((attr_1[i] - attr_0[i]) * inv_dy_ab);
                desc->_attr_0[i] = _mm256_set1_ps(attr_0[i]);
            }

            rasterize(desc, instance, target_pitch, target_stride, x0, dxdy_left, x0, dxdy_right, y0, y2);
        }
        else
        {
            for (size_t i = 0; i < desc->attribute_count; ++i)
            {
                desc->_attr_dx[i] = _mm256_set1_ps((attr_1[i] - attr_2[i]) * -inv_dx_bc * 8.f);
                desc->_attr_dy[i] = _mm256_set1_ps((attr_2[i] - attr_0[i]) * inv_dy_ac);
                desc->_attr_0[i] = _mm256_set1_ps(attr_0[i]);
            }

            rasterize(desc, instance, target_pitch, target_stride, x0, dxdy_right, x0, dxdy_left, y0, y2);
        }
    }

    else
    {
        float dy_ab = y1 - y0;
        float dxdy_ac = (x2 - x0) * inv_dy_ac;
        float x3 = fmaf(dxdy_ac, dy_ab, x0);

        float dxdy_top = (x1 - x0) / dy_ab;
        float dxdy_bottom = (x2 - x1) / (y2 - y1);

        float inv_dx_bd = 1.f / (x3 - x1);
        float inv_dy_ab = 1.f / (y1 - y0);
        float inv_dy_bc = 1.f / (y2 - y1);

        if (x1 < x3) // long side right
        {
            for (size_t i = 0; i < desc->attribute_count; ++i)
            {
                float attr_3 = fmaf((attr_2[i] - attr_0[i]) * inv_dy_ac, dy_ab, attr_0[i]);

                desc->_attr_dx[i] = _mm256_set1_ps((attr_3 - attr_1[i]) * inv_dx_bd * 8.f);
                desc->_attr_dy[i] = _mm256_set1_ps((attr_1[i] - attr_0[i]) * inv_dy_ab);
                desc->_attr_0[i] = _mm256_set1_ps(attr_0[i]);
            }

            rasterize(desc, instance, target_pitch, target_stride, x0, dxdy_top, x0, dxdy_ac, y0, y1);

            for (size_t i = 0; i < desc->attribute_count; ++i)
            {
                desc->_attr_dy[i] = _mm256_set1_ps((attr_2[i] - attr_1[i]) * inv_dy_bc);
                desc->_attr_0[i] = _mm256_set1_ps(attr_1[i]);
            }

            rasterize(desc, instance, target_pitch, target_stride, x1, dxdy_bottom, x3, dxdy_ac, y1, y2);
        }
        else // long side left
        {
            for (size_t i = 0; i < desc->attribute_count; ++i)
            {
                float attr_3 = fmaf((attr_2[i] - attr_0[i]) * inv_dy_ac, dy_ab, attr_0[i]);

                desc->_attr_dx[i] = _mm256_set1_ps((attr_1[i] - attr_3) * -inv_dx_bd * 8.f);
                desc->_attr_dy[i] = _mm256_set1_ps((attr_3 - attr_0[i]) * inv_dy_ab);

                desc->_attr_0[i] = _mm256_set1_ps(attr_0[i]);
                desc->_attr_01[i] = _mm256_set1_ps(attr_3);
            }

            rasterize(desc, instance, target_pitch, target_stride, x0, dxdy_ac, x0, dxdy_top, y0, y1);

            __m256 *tmp = desc->_attr_0;
            desc->_attr_0 = desc->_attr_01;
            desc->_attr_01 = tmp;

            rasterize(desc, instance, target_pitch, target_stride, x3, dxdy_ac, x1, dxdy_bottom, y1, y2);
        }
    }
}
