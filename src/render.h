#ifndef ROHAN_RENDER_H_
#define ROHAN_RENDER_H_

#if defined __cplusplus
extern "C"
{
#endif

#include <stddef.h>

#include "simd/rvm.h"
#include "type.h"

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

    ROHAN_API void rohan_initialize(void);

    ROHAN_API void rohan_render_triangle_raw(rohan_shader_object *shader, int width, float x0, float y0, float x1,
                                             float y1, float x2, float y2, const float *ROHAN_RESTRICT attr_0,
                                             const float *ROHAN_RESTRICT attr_1, const float *ROHAN_RESTRICT attr_2);

    ROHAN_API void rohan_render(struct rohan_shader_object *ROHAN_RESTRICT shader, int width,
                                const float *ROHAN_RESTRICT vertices, const int *ROHAN_RESTRICT indices,
                                size_t index_count, int mode);

#if defined __cplusplus
}
#endif
#endif