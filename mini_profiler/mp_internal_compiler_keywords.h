#ifndef MP_INTERNAL_FORCEINLINE_H_
#define MP_INTERNAL_FORCEINLINE_H_

/* 
 * Define bool
 */
#ifndef mp_bool
typedef int mp_bool;
#define mp_true (1)
#define mp_false (0)
#endif /* #ifndef mp_bool */

/* 
 * Define force inline
 * ----
 * All functions should be inline, 
 * No matter what debug or release
 * No matter how complicated the function implementation is.
 * That's because:
 *     to keep easy to integrate, 
 *     we declare and define all the functions in this header file.
 */

#if defined(__GNUC__)
/* Reference: https://gcc.gnu.org/onlinedocs/gcc/Inline.html */
#define mp_forceinline inline __attribute__((always_inline)) /* GCC specific */
#elif defined(_MSC_VER)
/* Reference: https://en.wikipedia.org/wiki/Inline_function#Microsoft_Visual_C.2B.2B_specific */
#define mp_forceinline __forceinline /* VC++ specific */
#else
#warning "force inline not defined, may introduce link errors."
#define mp_forceinline inline
#endif

#endif /* #ifndef MP_INTERNAL_FORCEINLINE_H_ */
