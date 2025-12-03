#ifndef ROHAN_FRAME_H_
#define ROHAN_FRAME_H_

typedef struct RFrame RFrame;

RFrame *rohan_create_frame(int w, int h);

void rohan_free_frame(RFrame *frame);

void *rohan_obtain_buffer(RFrame *frame);

#endif