#include "render.h"

#include <math.h>

#include "rmath.h"
#include "simd.h"

#define swapf(a, b)                                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        float tmp = a;                                                                                                 \
        a = b;                                                                                                         \
        b = tmp;                                                                                                       \
    } while (0)

typedef struct interpolant interpolant;
struct interpolant
{
    float p0;
    float dx;
    float dy;
};

static inline float step_x(interpolant interpolant, int i)
{
    return fmaf(interpolant.dx, i, interpolant.p0);
}

static inline float step_y(interpolant interpolant, int i)
{
    return fmaf(interpolant.dy, i, interpolant.p0);
}

static void rasterize_triangle(int *pixels, int w, bary bary, interpolant x_left, interpolant x_right, float y_top,
                               float y_bottom)
{

    __m256i draw_mask;
    size_t row = (int)y_top * w;

    float b0, b1, b2;
    get_coords(bary, x_left.p0, y_top, &b0, &b1, &b2);

    for (int i = 0; i < (int)y_bottom - (int)y_top; ++i, row += w)
    {
        int x0 = step_y(x_left, i), x1 = step_y(x_right, i);
        int x0_aligned = x0 & ~7, x1_aligned = x1 & ~7;
        int header = x0 - x0_aligned;

        float b0_start = fmaf(bary.dy_b0, i, b0);
        float b1_start = fmaf(bary.dy_b1, i, b1);
        float b2_start = fmaf(bary.dy_b2, i, b2);

        for (int x = x0; x <= x1; ++x)
        {
            // get_coords(bary, x, y_top + i, &b0_start, &b1_start, &b2_start);

            int red = 255.f * b0_start;
            int green = 255.f * b1_start;
            int blue = 255.f * b2_start;
            int alpha = 255;

            pixels[row + x] = (alpha << 24) | (blue << 16) | (green << 8) | red;

            b0_start += bary.dx_b0;
            b1_start += bary.dx_b1;
            b2_start += bary.dx_b2;
        }
        continue;

        __m256 multiplier_mask = multiplier_masks[header];
        __m256 b0_step = _mm256_set1_ps(bary.dx_b0 * 8.f);
        __m256 b1_step = _mm256_set1_ps(bary.dx_b1 * 8.f);
        __m256 b2_step = _mm256_set1_ps(bary.dx_b2 * 8.f);

        __m256 b0_1 = _mm256_fmadd_ps(_mm256_set1_ps(bary.dx_b0), multiplier_mask, _mm256_set1_ps(b0_start));
        __m256 b1_1 = _mm256_fmadd_ps(_mm256_set1_ps(bary.dx_b1), multiplier_mask, _mm256_set1_ps(b1_start));
        __m256 b2_1 = _mm256_fmadd_ps(_mm256_set1_ps(bary.dx_b2), multiplier_mask, _mm256_set1_ps(b2_start));

        if (x0_aligned == x1_aligned)
        {
            draw_mask = infix_masks[header * 8 + (x1 - x0_aligned)];
            _mm256_maskstore_epi32(pixels + row + x0_aligned, draw_mask, channels_to_rgba(b0_1, b1_1, b2_1));
            continue;
        }

        draw_mask = prefix_masks[header];
        _mm256_maskstore_epi32(pixels + row + x0_aligned, draw_mask, channels_to_rgba(b0_1, b1_1, b2_1));

        b0_1 = _mm256_add_ps(b0_1, b0_step);
        b1_1 = _mm256_add_ps(b1_1, b1_step);
        b2_1 = _mm256_add_ps(b2_1, b2_step);

        __m256i all = _mm256_set1_epi32(~0);
        draw_mask = all;

        for (int x = x0_aligned + 8; x < x1_aligned; x += 8)
        {
            _mm256_maskstore_epi32(pixels + row + x, draw_mask, channels_to_rgba(b0_1, b1_1, b2_1));

            b0_1 = _mm256_add_ps(b0_1, b0_step);
            b1_1 = _mm256_add_ps(b1_1, b1_step);
            b2_1 = _mm256_add_ps(b2_1, b2_step);
        }

        draw_mask = postfix_masks[(x1_aligned + 7) - x1];
        _mm256_maskstore_epi32(pixels + row + x1_aligned, draw_mask, channels_to_rgba(b0_1, b1_1, b2_1));
    }
}

void render_triangle(int *pixels, int w, float x0, float y0, float x1, float y1, float x2, float y2)
{
    bary bary;
    if (!get_bary(x0, y0, x1, y1, x2, y2, &bary))
        return;

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

    float inv_dy = 1.f / (y2 - y0);

    if ((int)y0 == (int)y1) // flat top
    {
        if (x0 > x1)
            swapf(x0, x1);

        interpolant x_left = {x0, 0.f, (x2 - x0) * inv_dy};
        interpolant x_right = {x1, 0.f, (x2 - x1) * inv_dy};

        rasterize_triangle(pixels, w, bary, x_left, x_right, y0, y2);
        return;
    }

    if ((int)y1 == (int)y2) // flat bottom
    {
        if (x1 > x2)
            swapf(x1, x2);

        interpolant x_left = {x0, 0.f, (x1 - x0) * inv_dy};
        interpolant x_right = {x0, 0.f, (x2 - x0) * inv_dy};

        rasterize_triangle(pixels, w, bary, x_left, x_right, y0, y2);
        return;
    }

    float dx_ac = (x2 - x0) * inv_dy;
    float dy_ab = y1 - y0;
    float x3 = fmaf(dx_ac, dy_ab, x0);

    interpolant x_top_a = {x0, 0.f, (x1 - x0) / dy_ab};
    interpolant x_top_b = {x0, 0.f, dx_ac};
    interpolant x_bottom_a = {x1, 0.f, (x2 - x1) / (y2 - y1)};
    interpolant x_bottom_b = {x3, 0.f, dx_ac};

    if (x1 < x3) // unified right side
    {
        rasterize_triangle(pixels, w, bary, x_top_a, x_top_b, y0, y1);
        rasterize_triangle(pixels, w, bary, x_bottom_a, x_bottom_b, y1, y2);
    }
    else // unified left side
    {
        rasterize_triangle(pixels, w, bary, x_top_b, x_top_a, y0, y1);
        rasterize_triangle(pixels, w, bary, x_bottom_b, x_bottom_a, y1, y2);
    }
}
