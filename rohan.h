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

#ifndef ROHAN_MAX_ATTRIBUTES
#define ROHAN_MAX_ATTRIBUTES 8
#endif

#include <immintrin.h>
#include <stddef.h>

    typedef struct rohan_raster_state rohan_raster_state;
    struct rohan_raster_state
    {
        struct
        {
            __m256 x;
            __m256 y;
        } pos;

        __m256i mask;
        size_t byte_offset; // cluster begin
        int primitive_id;
    };

    typedef struct rohan_shader_object rohan_shader_object;
    struct rohan_shader_object
    {
        void *instance;
        void (*main)(void *rohan_restrict instance, const rohan_raster_state *rohan_restrict state);
        void (*set_uniform)(void *rohan_restrict instance, int index, const void *rohan_restrict value,
                            size_t value_size);
        void (*destroy)(rohan_shader_object *rohan_restrict self);
        __m256 *attributes;
        int attribute_count;
        int target_pitch;
        int target_stride;
    };

    enum rohan_render_mode
    {
        ROHAN_LINE,
        ROHAN_LINE_STRIP,
        ROHAN_LINE_LOOP,
        ROHAN_TRIANGLE,
        ROHAN_TRIANGLE_FAN,
        ROHAN_QUAD,
    };

    ROHAN_API void rohan_init(void);

    ROHAN_API void rohan_render(rohan_shader_object *rohan_restrict shader, const float *rohan_restrict vertices,
                                const int *rohan_restrict indices, size_t index_count, enum rohan_render_mode mode);

#ifdef __cplusplus
}
#endif
#endif