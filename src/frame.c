#include "frame.h"

#include <immintrin.h>
#include <stdlib.h>

struct RFrame
{
    unsigned *pixels;
    int w;
    int h;
};

RFrame *rohan_create_frame(int w, int h)
{
    RFrame *frame = malloc(sizeof(RFrame));
    frame->pixels = _mm_malloc(w * h * sizeof(unsigned), 32); // calloc(w * h, sizeof(unsigned));
    return frame;
}

void rohan_free_frame(RFrame *frame)
{
    _mm_free(frame->pixels);
    free(frame);
}

void *rohan_obtain_buffer(RFrame *frame)
{
    return frame->pixels;
}
