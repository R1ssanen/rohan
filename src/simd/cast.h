#ifndef ROHAN_CAST_H_
#define ROHAN_CAST_H_

#if defined __cplusplus
extern "C"
{
#endif

#include "float.h"
#include "int.h"

    static inline rveci rvecf_as_int(rvecf a)
    {
        return _mm256_castps_si256(a);
    }

    static inline float rvecf_0(rvecf a)
    {
        return _mm256_cvtss_f32(a);
    }

    static inline rveci rvecf_to_int(rvecf a)
    {
        return _mm256_cvttps_epi32(a);
    }

    static inline rvecf rveci_as_float(rveci a)
    {
        return _mm256_castsi256_ps(a);
    }

    static inline int rveci_0(rveci a)
    {
        return _mm256_cvtsi256_si32(a);
    }

    static inline rvecf rveci_to_float(rveci a)
    {
        return _mm256_cvtepi32_ps(a);
    }

#if defined __cplusplus
}
#endif
#endif
