#ifndef ROHAN_VECTOR_H_
#define ROHAN_VECTOR_H_

#include "cast.h"
#include "float.h"
#include "int.h"

typedef struct rvec2i rvec2i;
struct rvec2i
{
    rveci x, y;
};

static inline rvec2i rvvi_add(rvec2i a, rvec2i b)
{
    return (rvec2i){rvm_add_i32(a.x, b.x), rvm_add_i32(a.y, b.y)};
}

static inline rvec2i rvvi_sub(rvec2i a, rvec2i b)
{
    return (rvec2i){rvm_sub_i32(a.x, b.x), rvm_sub_i32(a.y, b.y)};
}

static inline rvec2i rvvi_mul(rvec2i a, rvec2i b)
{
    return (rvec2i){rvm_mul_i32(a.x, b.x), rvm_mul_i32(a.y, b.y)};
}

typedef struct rvec3i rvec3i;
struct rvec3i
{
    rveci x, y, z;
};

#endif
