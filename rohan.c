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

    multiplier_masks[0] = _mm256_setr_ps(0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f);
    multiplier_masks[1] = _mm256_setr_ps(-1.f, 0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f);
    multiplier_masks[2] = _mm256_setr_ps(-2.f, -1.f, 0.f, 1.f, 2.f, 3.f, 4.f, 5.f);
    multiplier_masks[3] = _mm256_setr_ps(-3.f, -2.f, -1.f, 0.f, 1.f, 2.f, 3.f, 4.f);
    multiplier_masks[4] = _mm256_setr_ps(-4.f, -3.f, -2.f, -1.f, 0.f, 1.f, 2.f, 3.f);
    multiplier_masks[5] = _mm256_setr_ps(-5.f, -4.f, -3.f, -2.f, -1.f, 0.f, 1.f, 2.f);
    multiplier_masks[6] = _mm256_setr_ps(-6.f, -5.f, -4.f, -3.f, -2.f, -1.f, 0.f, 1.f);
    multiplier_masks[7] = _mm256_setr_ps(-7.f, -6.f, -5.f, -4.f, -3.f, -2.f, -1.f, 0.f);
}

static inline void advance(rohan_raster_state *restrict state, rohan_shader_object *restrict shader,
                           const __m256 *dattr_dx)
{
    shader->main(shader->instance, state);
    state->frag_x = _mm256_add_ps(state->frag_x, _mm256_set1_ps(8));
    state->offset += 8;

    for (int i = 0; i < shader->attribute_count; ++i)
    {
        shader->attributes[i] = _mm256_add_ps(shader->attributes[i], dattr_dx[i]);
    }
}

static void rasterize(rohan_shader_object *shader, int width, const float *restrict attr_0,
                      const __m256 *restrict dattr_dx, const __m256 *restrict dattr_dy, float x_left, float dx_dy_left,
                      float x_right, float dx_dy_right, int y_top, int y_bottom)
{
    rohan_raster_state state;
    __m256 seq = _mm256_setr_ps(0, 1, 2, 3, 4, 5, 6, 7);

    __m256 dattr_dx_step[ROHAN_MAX_ATTRIBUTES];
    __m256 attr_00[ROHAN_MAX_ATTRIBUTES];
    for (int i = 0; i < shader->attribute_count; ++i)
    {
        dattr_dx_step[i] = _mm256_mul_ps(dattr_dx[i], _mm256_set1_ps(8));
        attr_00[i] = _mm256_set1_ps(attr_0[i]);
    }

    size_t row = y_top * width;
    for (int y = y_top; y < y_bottom; ++y, row += width)
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

        state.offset = row + x0_aligned;
        state.frag_x = _mm256_add_ps(seq, _mm256_set1_ps(x0_aligned));
        state.frag_y = _mm256_set1_ps(y);

        __m256 multiplier_mask = multiplier_masks[header];
        for (int j = 0; j < shader->attribute_count; ++j)
        {
            shader->attributes[j] = _mm256_fmadd_ps(multiplier_mask, dattr_dx[j], attr_00[j]);
        }

        if (x0_aligned == x1_aligned)
        {
            state.mask = infix_masks[header][x1 - x0_aligned];
            shader->main(shader->instance, &state);
        }
        else
        {
            state.mask = prefix_masks[header];
            advance(&state, shader, dattr_dx_step);
            state.mask = _mm256_set1_epi32(-1);

            int steps = (x1_aligned - x0_aligned) >> 3;
            for (int j = 1; j < steps; ++j)
            {
                advance(&state, shader, dattr_dx_step);
            }

            state.mask = postfix_masks[x1 - x1_aligned];
            shader->main(shader->instance, &state);
        }

    interpolate:
        x_left += dx_dy_left;
        x_right += dx_dy_right;

        for (int j = 0; j < shader->attribute_count; ++j)
        {
            attr_00[j] = _mm256_add_ps(attr_00[j], dattr_dy[j]);
        }
    }
}

static void render_triangle(rohan_shader_object *shader, int width, float x0, float y0, float x1, float y1, float x2,
                            float y2, const float *restrict attr_0, const float *restrict attr_1,
                            const float *restrict attr_2)
{
    y0 = floorf(y0);
    y1 = floorf(y1);
    y2 = floorf(y2);

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
    __m256 dattr_dx[ROHAN_MAX_ATTRIBUTES];
    __m256 dattr_dy[ROHAN_MAX_ATTRIBUTES];

    if ((int)y0 == (int)y1) // flat top
    {
        float dxdy_left = (x2 - x0) * inv_dy_ac;
        float dxdy_right = (x2 - x1) * inv_dy_ac;
        float inv_dx_ab = 1.f / (x1 - x0);

        if (x0 < x1)
        {
            for (int i = 0; i < shader->attribute_count; ++i)
            {
                dattr_dx[i] = _mm256_set1_ps((attr_1[i] - attr_0[i]) * inv_dx_ab);
                dattr_dy[i] = _mm256_set1_ps((attr_2[i] - attr_0[i]) * inv_dy_ac);
            }

            rasterize(shader, width, attr_0, dattr_dx, dattr_dy, x0, dxdy_left, x1, dxdy_right, y0, y2);
        }
        else
        {
            float inv_dy_bc = 1.f / (y2 - y1);
            for (int i = 0; i < shader->attribute_count; ++i)
            {
                dattr_dx[i] = _mm256_set1_ps((attr_0[i] - attr_1[i]) * (-inv_dx_ab));
                dattr_dy[i] = _mm256_set1_ps((attr_2[i] - attr_1[i]) * inv_dy_bc);
            }

            rasterize(shader, width, attr_1, dattr_dx, dattr_dy, x1, dxdy_right, x0, dxdy_left, y0, y2);
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
            for (int i = 0; i < shader->attribute_count; ++i)
            {
                dattr_dx[i] = _mm256_set1_ps((attr_2[i] - attr_1[i]) * inv_dx_bc);
                dattr_dy[i] = _mm256_set1_ps((attr_1[i] - attr_0[i]) * inv_dy_ab);
            }

            rasterize(shader, width, attr_0, dattr_dx, dattr_dy, x0, dxdy_left, x0, dxdy_right, y0, y2);
        }
        else
        {
            for (int i = 0; i < shader->attribute_count; ++i)
            {
                dattr_dx[i] = _mm256_set1_ps((attr_1[i] - attr_2[i]) * (-inv_dx_bc));
                dattr_dy[i] = _mm256_set1_ps((attr_2[i] - attr_0[i]) * inv_dy_ac);
            }

            rasterize(shader, width, attr_0, dattr_dx, dattr_dy, x0, dxdy_right, x0, dxdy_left, y0, y2);
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

        float attr_3[ROHAN_MAX_ATTRIBUTES];
        for (int i = 0; i < shader->attribute_count; ++i)
        {
            attr_3[i] = fmaf((attr_2[i] - attr_0[i]) * inv_dy_ac, dy_ab, attr_0[i]);
        }

        if (x1 < x3)
        {
            for (int i = 0; i < shader->attribute_count; ++i)
            {
                dattr_dx[i] = _mm256_set1_ps((attr_3[i] - attr_1[i]) * inv_dx_bd);
                dattr_dy[i] = _mm256_set1_ps((attr_1[i] - attr_0[i]) * inv_dy_ab);
            }

            rasterize(shader, width, attr_0, dattr_dx, dattr_dy, x0, dxdy_top, x0, dxdy_ac, y0, y1);

            for (int i = 0; i < shader->attribute_count; ++i)
            {
                dattr_dy[i] = _mm256_set1_ps((attr_2[i] - attr_1[i]) * inv_dy_bc);
            }

            rasterize(shader, width, attr_1, dattr_dx, dattr_dy, x1, dxdy_bottom, x3, dxdy_ac, y1, y2);
        }
        else
        {
            for (int i = 0; i < shader->attribute_count; ++i)
            {
                dattr_dx[i] = _mm256_set1_ps((attr_1[i] - attr_3[i]) * (-inv_dx_bd));
                dattr_dy[i] = _mm256_set1_ps((attr_3[i] - attr_0[i]) * inv_dy_ab);
            }

            rasterize(shader, width, attr_0, dattr_dx, dattr_dy, x0, dxdy_ac, x0, dxdy_top, y0, y1);
            rasterize(shader, width, attr_3, dattr_dx, dattr_dy, x3, dxdy_ac, x1, dxdy_bottom, y1, y2);
        }
    }
}

static void render_line(rohan_shader_object *shader, int width, float x0, float y0, float x1, float y1,
                        const float *restrict attr_0, const float *restrict attr_1)
{
    return;
}

void rohan_render(rohan_shader_object *restrict shader, int width, const float *restrict vertices,
                  const int *restrict indices, size_t index_count, rohan_render_mode mode)
{
    int attribs = 2 + shader->attribute_count;
    const float *v0, *v1, *v2, *v3;

    switch (mode)
    {
    case ROHAN_LINE:
        for (size_t i = 0; i < index_count; i += 2)
        {
            v0 = vertices + indices[i] * attribs;
            v1 = vertices + indices[i + 1] * attribs;
            render_line(shader, width, v0[0], v0[1], v1[0], v1[1], v0 + 2, v1 + 2);
        }
        break;

    case ROHAN_LINE_STRIP:
        for (size_t i = 0; i < index_count - 1; i += 1)
        {
            v0 = vertices + indices[i] * attribs;
            v1 = vertices + indices[i + 1] * attribs;
            render_line(shader, width, v0[0], v0[1], v1[0], v1[1], v0 + 2, v1 + 2);
        }
        break;

    case ROHAN_LINE_LOOP:
        for (size_t i = 0; i < index_count - 1; i += 1)
        {
            v0 = vertices + indices[i] * attribs;
            v1 = vertices + indices[i + 1] * attribs;
            render_line(shader, width, v0[0], v0[1], v1[0], v1[1], v0 + 2, v1 + 2);
        }
        v0 = vertices + indices[index_count - 1] * attribs;
        v1 = vertices + indices[0] * attribs;
        render_line(shader, width, v0[0], v0[1], v1[0], v1[1], v0 + 2, v1 + 2);
        break;

    case ROHAN_TRIANGLE:
        for (size_t i = 0; i < index_count; i += 3)
        {
            v0 = vertices + indices[i] * attribs;
            v1 = vertices + indices[i + 1] * attribs;
            v2 = vertices + indices[i + 2] * attribs;
            render_triangle(shader, width, v0[0], v0[1], v1[0], v1[1], v2[0], v2[1], v0 + 2, v1 + 2, v2 + 2);
        }
        break;

    case ROHAN_TRIANGLE_FAN:
        v0 = vertices + indices[0] * attribs;
        for (size_t i = 1; i < index_count - 1; i += 1)
        {
            v1 = vertices + indices[i] * attribs;
            v2 = vertices + indices[i + 1] * attribs;
            render_triangle(shader, width, v0[0], v0[1], v1[0], v1[1], v2[0], v2[1], v0 + 2, v1 + 2, v2 + 2);
        }
        break;

    case ROHAN_QUAD:
        for (size_t i = 0; i < index_count; i += 4)
        {
            v0 = vertices + indices[i] * attribs;
            v1 = vertices + indices[i + 1] * attribs;
            v2 = vertices + indices[i + 2] * attribs;
            v3 = vertices + indices[i + 3] * attribs;
            render_triangle(shader, width, v0[0], v0[1], v1[0], v1[1], v2[0], v2[1], v0 + 2, v1 + 2, v2 + 2);
            render_triangle(shader, width, v0[0], v0[1], v2[0], v2[1], v3[0], v3[1], v0 + 2, v2 + 2, v3 + 2);
        }
        break;

    default:
        break;
    }
}
