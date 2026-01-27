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
#include <stdint.h>

    typedef struct rohan_raster_state rohan_raster_state;
    struct rohan_raster_state
    {
        struct
        {
            __m256 x, y;
        } pos;

        __m256i mask;
        size_t byte_offset; // cluster begin
        uint32_t primitive_id;
    };

    typedef struct rohan_shader_spec rohan_shader_spec;
    struct rohan_shader_spec
    {
        const char *name;
        void (*main)(void *rohan_restrict instance, const rohan_raster_state *rohan_restrict state);
        void (*destroy)(rohan_shader_spec *rohan_restrict self);
        size_t attribute_offset;
        size_t attribute_count;
        size_t instance_size;
        size_t uniform_offsets[];
    };

    typedef rohan_shader_spec *(*rohan_get_specification_fn)(void);

    enum rohan_render_mode
    {
        ROHAN_LINE,
        ROHAN_LINE_STRIP,
        ROHAN_LINE_LOOP,
        ROHAN_TRIANGLE,
        ROHAN_TRIANGLE_FAN,
        ROHAN_QUAD
    };

    ROHAN_API void rohan_init(void);

    ROHAN_API void rohan_render(const rohan_shader_spec *rohan_restrict spec, void *rohan_restrict instance,
                                size_t target_pitch, size_t target_stride, const float *rohan_restrict vertices,
                                const uint32_t *rohan_restrict indices, size_t index_count,
                                enum rohan_render_mode mode);

#ifdef __cplusplus
}
#endif
#endif