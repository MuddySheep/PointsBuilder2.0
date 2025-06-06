#ifndef PB_SIMD_H
#define PB_SIMD_H

#if defined(__AVX512F__)
#define PB_AVX512 1
#elif defined(__AVX2__)
#define PB_AVX2 1
#endif

#if defined(PB_AVX512) || defined(PB_AVX2)
#define PB_SIMD_ENABLED 1
#else
#define PB_SIMD_ENABLED 0
#endif

#endif // PB_SIMD_H
