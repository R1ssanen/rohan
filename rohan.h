#ifndef ROHAN_H_
#define ROHAN_H_

#ifndef __AVX2__
#error "librohan: AVX2 instructions required."
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef ROHAN_BUILD
#define ROHAN_API __declspec(dllexport)
#else
#define ROHAN_API __declspec(dllimport)
#endif
#elif defined(__GNUC__) || defined(__clang__)
#define ROHAN_API __attribute__((visibility("default")))
#else
#define ROHAN_API
#endif

#ifdef __cplusplus
extern "C"
{
#define rohan_restrict
#else
#include <stdbool.h>
#define rohan_restrict restrict
#endif

#include <immintrin.h>
#include <stddef.h>
#include <stdint.h>

    typedef struct rohan_raster_state rohan_raster_state;
    struct rohan_raster_state
    {
        struct
        {
            __m256i x, y;
        } pos;

        __m256i mask;
        size_t byte_offset;
    };

    typedef struct rohan_shader_desc rohan_shader_desc;
    struct rohan_shader_desc
    {
        const char *name;
        void (*main)(void *rohan_restrict instance, const rohan_raster_state *rohan_restrict state);
        void (*destroy)(rohan_shader_desc *rohan_restrict self);
        __m256 *_attr_dx, *_attr_dy, *_attr_0, *_attr_01;
        size_t attribute_offset;
        size_t attribute_count;
        size_t instance_size;
        size_t uniform_offsets[];
    };

    typedef rohan_shader_desc *(*rohan_get_description_fn)(void);

    ROHAN_API void rohan_init(void);

    ROHAN_API void rohan_rasterize(rohan_shader_desc *rohan_restrict desc, void *rohan_restrict instance,
                                   size_t target_pitch, size_t target_stride, const float *rohan_restrict vertex0,
                                   const float *rohan_restrict vertex1, const float *rohan_restrict vertex2);

#ifdef __cplusplus
}
#endif
#endif