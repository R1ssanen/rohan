#ifndef ROHAN_SHADER_H_
#define ROHAN_SHADER_H_

#if defined __cplusplus
extern "C"
{
#endif

#include <stddef.h>

#include "render.h"
#include "simd/float.h"
#include "type.h"

#ifndef ROHAN_MAX_ATTRIBUTES
#define ROHAN_MAX_ATTRIBUTES 8
#endif

    typedef rohan_raster_state *ROHAN_RESTRICT rohan_raster_state_rp;
    typedef void *ROHAN_RESTRICT rohan_value_rp;

    typedef void (*rohan_shader_program_fn)(rohan_value_rp, const rohan_raster_state_rp);
    typedef void (*rohan_shader_uniform_fn)(rohan_value_rp, int, const rohan_value_rp, size_t);
    typedef struct rohan_shader_object (*rohan_shader_instance_fn)(void);
    typedef void (*rohan_shader_destroy_fn)(struct rohan_shader_object *);

    typedef struct rohan_shader_object rohan_shader_object;
    struct rohan_shader_object
    {
        rohan_shader_program_fn main;
        rohan_shader_uniform_fn set_uniform;
        rohan_shader_destroy_fn destroy;
        void *instance;
        rvecf *attributes;
        int attribute_count;
    };

#if defined __cplusplus
}
#endif
#endif