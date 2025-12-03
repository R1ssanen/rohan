#include "rmath.h"

#include <stdbool.h>

float edge(float ax, float ay, float bx, float by, float cx, float cy)
{
    return (by - cy) * (ax - cx) + (cx - bx) * (ay - cy);
}

bool get_bary(float x0, float y0, float x1, float y1, float x2, float y2, bary *bary)
{
    bary->det = edge(x0, y0, x1, y1, x2, y2);
    const bool cull_backfaces = false;

    if (bary->det == 0.f)
        return false;
    else if ((bary->det < 0.f) && cull_backfaces)
        return false;

    bary->inv_det = 1.f / bary->det;
    bary->x0 = x0, bary->y0 = y0, bary->x1 = x1, bary->y1 = y1, bary->x2 = x2, bary->y2 = y2;

    bary->dx_b0 = -(y2 - y1) * bary->inv_det;
    bary->dy_b0 = (x2 - x1) * bary->inv_det;

    bary->dx_b1 = -(y0 - y2) * bary->inv_det;
    bary->dy_b1 = (x0 - x2) * bary->inv_det;

    bary->dx_b2 = -(y1 - y0) * bary->inv_det;
    bary->dy_b2 = (x1 - x0) * bary->inv_det;

    return true;
}

void get_coords(bary bary, float x, float y, float *b0, float *b1, float *b2)
{
    *b0 = edge(x, y, bary.x1, bary.y1, bary.x2, bary.y2) * bary.inv_det;
    *b1 = edge(bary.x0, bary.y0, x, y, bary.x2, bary.y2) * bary.inv_det;
    *b2 = edge(bary.x0, bary.y0, bary.x1, bary.y1, x, y) * bary.inv_det;
}
