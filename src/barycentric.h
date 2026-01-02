#ifndef ROHAN_BARYCENTRIC_H_
#define ROHAN_BARYCENTRIC_H_

#include <stdbool.h>
#include <stdint.h>

#include "simd/float.h"

typedef struct barycentrics barycentrics;
struct barycentrics
{
    rvecf dx_b0, dy_b0;
    rvecf dx_b1, dy_b1;
    rvecf dx_b2, dy_b2;
    float x0, y0;
};

bool get_barycentrics(float x0, float y0, float x1, float y1, float x2, float y2, bool cull_backfaces,
                      barycentrics *bary);

void get_weights(const barycentrics *bary, float x, float y, rvecf *w0, rvecf *w1, rvecf *w2);

#endif
