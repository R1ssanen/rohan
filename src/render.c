#include "render.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "barycentric.h"
#include "simd/rvm.h"

#define swapf(a, b)                                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        float tmp = a;                                                                                                 \
        a = b;                                                                                                         \
        b = tmp;                                                                                                       \
    } while (0)

static rveci prefix_masks[8];
static rveci postfix_masks[8];
static rveci infix_masks[8][8];
static rvecf multiplier_masks[8];

void rohan_initialize(void)
{
    prefix_masks[0] = rveci_i32(-1);
    prefix_masks[1] = rveci_set_i32(0, -1, -1, -1, -1, -1, -1, -1);
    prefix_masks[2] = rveci_set_i32(0, 0, -1, -1, -1, -1, -1, -1);
    prefix_masks[3] = rveci_set_i32(0, 0, 0, -1, -1, -1, -1, -1);
    prefix_masks[4] = rveci_set_i32(0, 0, 0, 0, -1, -1, -1, -1);
    prefix_masks[5] = rveci_set_i32(0, 0, 0, 0, 0, -1, -1, -1);
    prefix_masks[6] = rveci_set_i32(0, 0, 0, 0, 0, 0, -1, -1);
    prefix_masks[7] = rveci_set_i32(0, 0, 0, 0, 0, 0, 0, -1);

    postfix_masks[0] = rveci_i32(-1);
    postfix_masks[1] = rveci_set_i32(-1, -1, -1, -1, -1, -1, -1, 0);
    postfix_masks[2] = rveci_set_i32(-1, -1, -1, -1, -1, -1, 0, 0);
    postfix_masks[3] = rveci_set_i32(-1, -1, -1, -1, -1, 0, 0, 0);
    postfix_masks[4] = rveci_set_i32(-1, -1, -1, -1, 0, 0, 0, 0);
    postfix_masks[5] = rveci_set_i32(-1, -1, -1, 0, 0, 0, 0, 0);
    postfix_masks[6] = rveci_set_i32(-1, -1, 0, 0, 0, 0, 0, 0);
    postfix_masks[7] = rveci_set_i32(-1, 0, 0, 0, 0, 0, 0, 0);

    infix_masks[0][0] = rveci_set_i32(-1, 0, 0, 0, 0, 0, 0, 0);
    infix_masks[0][1] = rveci_set_i32(-1, -1, 0, 0, 0, 0, 0, 0);
    infix_masks[0][2] = rveci_set_i32(-1, -1, -1, 0, 0, 0, 0, 0);
    infix_masks[0][3] = rveci_set_i32(-1, -1, -1, -1, 0, 0, 0, 0);
    infix_masks[0][4] = rveci_set_i32(-1, -1, -1, -1, -1, 0, 0, 0);
    infix_masks[0][5] = rveci_set_i32(-1, -1, -1, -1, -1, -1, 0, 0);
    infix_masks[0][6] = rveci_set_i32(-1, -1, -1, -1, -1, -1, -1, 0);
    infix_masks[0][7] = rveci_set_i32(-1, -1, -1, -1, -1, -1, -1, -1);
    infix_masks[1][1] = rveci_set_i32(0, -1, 0, 0, 0, 0, 0, 0);
    infix_masks[1][2] = rveci_set_i32(0, -1, -1, 0, 0, 0, 0, 0);
    infix_masks[1][3] = rveci_set_i32(0, -1, -1, -1, 0, 0, 0, 0);
    infix_masks[1][4] = rveci_set_i32(0, -1, -1, -1, -1, 0, 0, 0);
    infix_masks[1][5] = rveci_set_i32(0, -1, -1, -1, -1, -1, 0, 0);
    infix_masks[1][6] = rveci_set_i32(0, -1, -1, -1, -1, -1, -1, 0);
    infix_masks[1][7] = rveci_set_i32(0, -1, -1, -1, -1, -1, -1, -1);
    infix_masks[2][2] = rveci_set_i32(0, 0, -1, 0, 0, 0, 0, 0);
    infix_masks[2][3] = rveci_set_i32(0, 0, -1, -1, 0, 0, 0, 0);
    infix_masks[2][4] = rveci_set_i32(0, 0, -1, -1, -1, 0, 0, 0);
    infix_masks[2][5] = rveci_set_i32(0, 0, -1, -1, -1, -1, 0, 0);
    infix_masks[2][6] = rveci_set_i32(0, 0, -1, -1, -1, -1, -1, 0);
    infix_masks[2][7] = rveci_set_i32(0, 0, -1, -1, -1, -1, -1, -1);
    infix_masks[3][3] = rveci_set_i32(0, 0, 0, -1, 0, 0, 0, 0);
    infix_masks[3][4] = rveci_set_i32(0, 0, 0, -1, -1, 0, 0, 0);
    infix_masks[3][5] = rveci_set_i32(0, 0, 0, -1, -1, -1, 0, 0);
    infix_masks[3][6] = rveci_set_i32(0, 0, 0, -1, -1, -1, -1, 0);
    infix_masks[3][7] = rveci_set_i32(0, 0, 0, -1, -1, -1, -1, -1);
    infix_masks[4][4] = rveci_set_i32(0, 0, 0, 0, -1, 0, 0, 0);
    infix_masks[4][5] = rveci_set_i32(0, 0, 0, 0, -1, -1, 0, 0);
    infix_masks[4][6] = rveci_set_i32(0, 0, 0, 0, -1, -1, -1, 0);
    infix_masks[4][7] = rveci_set_i32(0, 0, 0, 0, -1, -1, -1, -1);
    infix_masks[5][5] = rveci_set_i32(0, 0, 0, 0, 0, -1, 0, 0);
    infix_masks[5][6] = rveci_set_i32(0, 0, 0, 0, 0, -1, -1, 0);
    infix_masks[5][7] = rveci_set_i32(0, 0, 0, 0, 0, -1, -1, -1);
    infix_masks[6][6] = rveci_set_i32(0, 0, 0, 0, 0, 0, -1, 0);
    infix_masks[6][7] = rveci_set_i32(0, 0, 0, 0, 0, 0, -1, -1);
    infix_masks[7][7] = rveci_set_i32(0, 0, 0, 0, 0, 0, 0, -1);

    multiplier_masks[0] = rvecf_set_f32(0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f);
    multiplier_masks[1] = rvecf_set_f32(-1.f, 0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f);
    multiplier_masks[2] = rvecf_set_f32(-2.f, -1.f, 0.f, 1.f, 2.f, 3.f, 4.f, 5.f);
    multiplier_masks[3] = rvecf_set_f32(-3.f, -2.f, -1.f, 0.f, 1.f, 2.f, 3.f, 4.f);
    multiplier_masks[4] = rvecf_set_f32(-4.f, -3.f, -2.f, -1.f, 0.f, 1.f, 2.f, 3.f);
    multiplier_masks[5] = rvecf_set_f32(-5.f, -4.f, -3.f, -2.f, -1.f, 0.f, 1.f, 2.f);
    multiplier_masks[6] = rvecf_set_f32(-6.f, -5.f, -4.f, -3.f, -2.f, -1.f, 0.f, 1.f);
    multiplier_masks[7] = rvecf_set_f32(-7.f, -6.f, -5.f, -4.f, -3.f, -2.f, -1.f, 0.f);
}

static inline void advance(rohan_raster_state *restrict state, rohan_shader_object *restrict shader, rvecf dx_b0,
                           rvecf dx_b1, rvecf dx_b2)
{
    shader->shader(shader->instance, state);
    state->index += 8;
    state->pos.x = rvm_add_f32(state->pos.x, rvecf_f32(8));
    state->weight.x = rvm_add_f32(state->weight.x, dx_b0);
    state->weight.y = rvm_add_f32(state->weight.y, dx_b1);
    state->weight.z = rvm_add_f32(state->weight.z, dx_b2);
}

static void rasterize_triangle(rohan_shader_object *shader, const barycentrics *bary, float x_left, float dxdy_x_left,
                               float x_right, float dxdy_x_right, float y_top, float y_bottom)
{
    rohan_raster_state state;
    state.pos.y = rvecf_f32(y_top);
    size_t row = (int)y_top * shader->max_w;

    rvecf b0_0, b1_0, b2_0;
    get_weights(bary, x_left, y_top, &b0_0, &b1_0, &b2_0);

    rvecf dx_b0 = bary->dx_b0;
    rvecf dx_b1 = bary->dx_b1;
    rvecf dx_b2 = bary->dx_b2;

    rvecf dy_b0 = rvm_fma_f32(bary->dx_b0, rvecf_f32(dxdy_x_left), bary->dy_b0);
    rvecf dy_b1 = rvm_fma_f32(bary->dx_b1, rvecf_f32(dxdy_x_left), bary->dy_b1);
    rvecf dy_b2 = rvm_fma_f32(bary->dx_b2, rvecf_f32(dxdy_x_left), bary->dy_b2);

    rvecf eight = rvecf_f32(8.f);
    rvecf dx_b0_cluster = rvm_mul_f32(dx_b0, eight);
    rvecf dx_b1_cluster = rvm_mul_f32(dx_b1, eight);
    rvecf dx_b2_cluster = rvm_mul_f32(dx_b2, eight);

    rvecf seq = rvecf_set_f32(0, 1, 2, 3, 4, 5, 6, 7);
    rvecf lines = rvecf_f32(0.f);

    for (int i = 0; i < (int)y_bottom - (int)y_top;
         ++i, row += shader->max_w, state.pos.y = rvm_increment_f32(state.pos.y), lines = rvm_increment_f32(lines))
    {
        int x0 = floorf(fmaf(dxdy_x_left, i, x_left));
        int x1 = ceilf(fmaf(dxdy_x_right, i, x_right)) - 1.f;
        int x0_aligned = x0 & ~7;
        int x1_aligned = x1 & ~7;
        int header = x0 - x0_aligned;

        state.index = row + x0_aligned;
        state.pos.x = rvm_add_f32(seq, rvecf_f32(x0_aligned));

        rvecf multiplier_mask = multiplier_masks[header];
        state.weight.x = rvm_fma_f32(dx_b0, multiplier_mask, rvm_fma_f32(dy_b0, lines, b0_0));
        state.weight.y = rvm_fma_f32(dx_b1, multiplier_mask, rvm_fma_f32(dy_b1, lines, b1_0));
        state.weight.z = rvm_fma_f32(dx_b2, multiplier_mask, rvm_fma_f32(dy_b2, lines, b2_0));

        if (x0_aligned == x1_aligned)
        {
            state.mask = infix_masks[header][x1 - x0_aligned];
            shader->shader(shader->instance, &state);
        }
        else
        {
            state.mask = prefix_masks[header];
            advance(&state, shader, dx_b0_cluster, dx_b1_cluster, dx_b2_cluster);
            state.mask = rveci_i32(-1);

            for (int j = 1; j < (x1_aligned - x0_aligned) >> 3; ++j)
            {
                advance(&state, shader, dx_b0_cluster, dx_b1_cluster, dx_b2_cluster);
            }

            state.mask = postfix_masks[(x1_aligned + 7) - x1];
            shader->shader(shader->instance, &state);
        }
    }
}

void render_triangle(rohan_shader_object *shader, float x0, float y0, float x1, float y1, float x2, float y2)
{
    barycentrics bary;
    if (!get_barycentrics(x0, y0, x1, y1, x2, y2, true, &bary))
    {
        return;
    }

    if (y0 > y1)
    {
        swapf(x0, x1);
        swapf(y0, y1);
    }
    if (y1 > y2)
    {
        swapf(x1, x2);
        swapf(y1, y2);
    }
    if (y0 > y1)
    {
        swapf(x0, x1);
        swapf(y0, y1);
    }

    float inv_dy_ac = 1.f / (y2 - y0);

    if ((int)y0 == (int)y1) // flat top
    {
        float dxdy_left = (x2 - x0) * inv_dy_ac;
        float dxdy_right = (x2 - x1) * inv_dy_ac;

        if (x0 < x1)
        {
            rasterize_triangle(shader, &bary, x0, dxdy_left, x1, dxdy_right, y0, y2);
        }
        else
        {
            rasterize_triangle(shader, &bary, x1, dxdy_right, x0, dxdy_left, y0, y2);
        }
    }

    else if ((int)y1 == (int)y2) // flat bottom
    {
        float dxdy_left = (x1 - x0) * inv_dy_ac;
        float dxdy_right = (x2 - x0) * inv_dy_ac;

        if (x1 < x2)
        {
            rasterize_triangle(shader, &bary, x0, dxdy_left, x0, dxdy_right, y0, y2);
        }
        else
        {
            rasterize_triangle(shader, &bary, x0, dxdy_right, x0, dxdy_left, y0, y2);
        }
    }

    else
    {
        float dy_ab = y1 - y0;
        float dxdy_ac = (x2 - x0) * inv_dy_ac;
        float x3 = fmaf(dxdy_ac, dy_ab, x0);

        float dxdy_top = (x1 - x0) / dy_ab;
        float dxdy_bottom = (x2 - x1) / (y2 - y1);

        if (x1 < x3)
        {
            rasterize_triangle(shader, &bary, x0, dxdy_top, x0, dxdy_ac, y0, y1);
            rasterize_triangle(shader, &bary, x1, dxdy_bottom, x3, dxdy_ac, y1, y2);
        }
        else
        {
            rasterize_triangle(shader, &bary, x0, dxdy_ac, x0, dxdy_top, y0, y1);
            rasterize_triangle(shader, &bary, x3, dxdy_ac, x1, dxdy_bottom, y1, y2);
        }
    }
}

/*
void render_line(int *pixels, int w, int x0, int y0, int x1, int y1, int color)
{
    int dx = x1 - x0;
    if (dx == 0)
    { // vertical

        int miny, maxy;
        if (y0 > y1)
        {
            miny = y1;
            maxy = y0;
        }
        else
        {
            miny = y0;
            maxy = y1;
        }

        size_t index = miny * w + x0;
        for (int y = miny; y < maxy; ++y)
        {
            pixels[index] = color;
            index += w;
        }
        return;
    }

    int dy = y1 - y0;
    if (dy == 0)
    { // horizontal
        int minx = x0 > x1 ? x1 : x0;
        size_t begin = y0 * w + minx;
        memset(pixels + begin, abs(dx), color);
        return;
    }

    if (abs(dx) > abs(dy))
    { // gentle slope < 45deg
        if (x0 > x1)
        {
            swapi(x0, x1);
            y0 = y1;
        }

        float slope = (float)dy / (float)dx;
        float y = y0;

        for (int x = x0; x < x1; ++x, y += slope)
            pixels[(int)y * w + x] = color;
    }

    else
    { // steep slope >= 45deg
        if (y0 > y1)
        {
            swapi(y0, y1);
            x0 = x1;
        }

        float slope = (float)dx / (float)dy;
        size_t row = y0 * w;
        float x = x0;

        for (int y = y0; y < y1; ++y, x += slope)
        {
            pixels[row + (int)x] = color;
            row += w;
        }
    }
}
*/

void rohan_render(rohan_shader_object *shader, const float *vertices, const int *indices, size_t index_count,
                  rohan_render_mode __UNUSED_PARAM(mode))
{
    const float N = 16.f;
    const float inv_N = 1.f / N;

    for (size_t i = 0; i < index_count; i += 3)
    {
        const float *v0 = vertices + indices[i + 0] * 2;
        const float *v1 = vertices + indices[i + 1] * 2;
        const float *v2 = vertices + indices[i + 2] * 2;

        render_triangle(shader, nearbyintf(v0[0] * N) * inv_N, nearbyintf(v0[1] * N) * inv_N,
                        nearbyintf(v1[0] * N) * inv_N, nearbyintf(v1[1] * N) * inv_N, nearbyintf(v2[0] * N) * inv_N,
                        nearbyintf(v2[1] * N) * inv_N);
    }
}
