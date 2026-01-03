#ifndef ROHAN_RENDER_H_
#define ROHAN_RENDER_H_

#include <stddef.h>

#include "simd/rvm.h"

#if defined _WIN32 || defined __CYGWIN__
#ifdef ROHAN_BUILD
#define ROHAN_API __declspec(dllexport)
#else
#define ROHAN_API __declspec(dllimport)
#endif
#elif defined __GNUC__ || defined __clang__
#define ROHAN_API __attribute__((visibility("default")))
#else
#define ROHAN_API
#endif

struct rohan_shader_object;

enum
{
    ROHAN_LINE,
    ROHAN_LINE_STRIP,
    ROHAN_LINE_LOOP,
    ROHAN_TRIANGLE,
    ROHAN_TRIANGLE_FAN,
    ROHAN_QUAD,
};

typedef struct rohan_raster_state rohan_raster_state;
struct rohan_raster_state
{
    rvecf frag_x, frag_y;
    rveci mask;
    size_t offset; // cluster begin
    size_t primitive_id;
};

void rohan_initialize(void);

void rohan_render(struct rohan_shader_object *restrict shader, int width, const float *restrict vertices,
                  const int *restrict indices, size_t index_count, int mode);

#endif