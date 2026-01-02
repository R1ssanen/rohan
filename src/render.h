#ifndef ROHAN_RENDER_H_
#define ROHAN_RENDER_H_

#include <stddef.h>

#include "simd/rvm.h"

typedef enum rohan_render_mode
{
    ROHAN_LINES,
    ROHAN_TRIANGLES
} rohan_render_mode;

typedef struct rohan_raster_state rohan_raster_state;
struct rohan_raster_state
{
    struct
    {
        rvecf x, y, z;
    } weight;
    struct
    {
        rvecf x, y;
    } pos;
    rveci mask;
    size_t index; // cluster begin
};

typedef rohan_raster_state *restrict rohan_raster_state_rp;
typedef void *restrict rohan_value_rp;

typedef void (*rohan_shader_program_fn)(rohan_value_rp, const rohan_raster_state_rp);
typedef void (*rohan_shader_uniform_fn)(rohan_value_rp, int, const rohan_value_rp, size_t);
typedef void *(*rohan_shader_instance_fn)(void);

typedef struct rohan_shader_object rohan_shader_object;
struct rohan_shader_object
{
    rohan_shader_program_fn shader;
    void *instance;
    int max_w;
};

void rohan_initialize(void);

void rohan_render(rohan_shader_object *shader, const float *vertices, const int *indices, size_t index_count,
                  rohan_render_mode mode);

#endif