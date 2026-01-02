#include "barycentric.h"

#include <math.h>
#include <stdbool.h>

#include "simd/float.h"

bool get_barycentrics(float x0, float y0, float x1, float y1, float x2, float y2, bool cull_backfaces,
                      barycentrics *bary)
{
    float det = fmaf(y1 - y2, x0 - x2, (x2 - x1) * (y0 - y2));

    if ((fabsf(det) < (0.5f * 2.f)) || ((det < 0.f) && cull_backfaces))
    {
        return false;
    }

    float inv_det = 1.f / det;
    bary->x0 = x0, bary->y0 = y0;

    bary->dx_b0 = rvecf_f32((y1 - y2) * inv_det);
    bary->dy_b0 = rvecf_f32((x2 - x1) * inv_det);
    bary->dx_b1 = rvecf_f32((y2 - y0) * inv_det);
    bary->dy_b1 = rvecf_f32((x0 - x2) * inv_det);
    bary->dx_b2 = rvecf_f32((y0 - y1) * inv_det);
    bary->dy_b2 = rvecf_f32((x1 - x0) * inv_det);

    return true;
}

void get_weights(const barycentrics *bary, float x, float y, rvecf *w0, rvecf *w1, rvecf *w2)
{
    rvecf dx = rvecf_f32(x - bary->x0), dy = rvecf_f32(y - bary->y0);
    *w1 = rvm_fma_f32(bary->dx_b1, dx, rvm_mul_f32(bary->dy_b1, dy));
    *w2 = rvm_fma_f32(bary->dx_b2, dx, rvm_mul_f32(bary->dy_b2, dy));
    *w0 = rvm_sub_f32(rvm_sub_f32(rvecf_f32(1.f), *w1), *w2);
}
