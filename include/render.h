#ifndef ROHAN_RENDER_H_
#define ROHAN_RENDER_H_

void render_triangle(int *pixels, int w, float x0, float y0, float x1, float y1,
                     float x2, float y2);

void render_line(int *pixels, int w, int x0, int y0, int x1, int y1, int color);

#endif
