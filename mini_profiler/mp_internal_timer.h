#ifndef MP_INTERNAL_TIMER_H_
#define MP_INTERNAL_TIMER_H_

#include "mp_internal_compiler_keywords.h" /* mp_bool */
/*
 * Use doulbe as accumulate timer type
 *
 * ==> Why double type has enough precision?
 * Since floating point is a non-uniform sampling from real value. 
 * We need consider the precsion(sampling interval) for different value section.
 *
 * doulbe value is represented by (-1)^s * M * 2^E, according to IEEE standard. 
 *   Where: E is exponent, with 11-bit width and 1023 bias, so -1022 <= E <= +1023
 *          M is mantissa, with 52-bit width, implied leading 1. so M = 1.f(51)f(50)...f(0), 1 <= M < 2
 * 
 * If MP_TIMER_UNIT is "sec":
 *   - When the program cost ~1 second. 1 second = 1.xxx * 2^0  sec, so the sampling interval is 2^(-51) * 2^0 sec, more than needed. 
 *   - When the program cost ~1 year.   1 year   = 1.xxx * 2^25 sec, so the sampling interval is 2^(-51) * 2^25, still more than needed. 
 * If MP_TIMER_UNIT is "nsec":
 *   - When the program cost ~1 second. 1 second = 1.xxx * 2^30 nsec, so the sampling interval is 2^(-51) * 2^30 nsec, more than needed. 
 *   - When the program cost ~1 year.   1 year   = 1.xxx * 2^55 nsec, so the sampling interval is 2^(-51) * 2^55 = 16 nsec, still more than needed. 
 * If MP_TIMER_UNIT is "cycles" on 4GHz CPU:
 *   - When the program cost ~1 year.   1 year   = 1.xxx * 2^57 cycles, so the interval is 2^(-51) * 2^57 = 64 cycles, still more than needed. 
 */
typedef double mp_timer_t;

/*
 * Select one backend for mp_get_cur_time()
 */
#define MP_INTERNAL_TIMER_USING_X86_X64

#if defined(MP_INTERNAL_TIMER_USING_ARMV7)
#include "mp_internal_timer_armv7.h"
#elif defined(MP_INTERNAL_TIMER_USING_X86_X64)
#include "mp_internal_timer_x86_x64.h"
#elif defined(MP_INTERNAL_TIMER_USING_LINUX)
#include "mp_internal_timer_linux.h"
#elif defined(MP_INTERNAL_TIMER_USING_STANDARD_C_CLOCK)
#include "mp_internal_timer_standard_c_clock.h"
#else
#error "Please choose one timer."
#endif

static mp_forceinline 
void mp_timer_clear(mp_timer_t *t)
{
    *t = 0;
}

static mp_forceinline
void mp_timer_set(mp_timer_t *t, double a)
{
    *t = a;
}

static mp_forceinline 
mp_timer_t mp_timer_add(mp_timer_t a, mp_timer_t b)
{
    return a + b;
}

static mp_forceinline 
mp_timer_t mp_timer_sub(mp_timer_t a, mp_timer_t b)
{
    mp_timer_t out = a - b;
    if(out < 0)
        out += MP_TIMER_MOD;
    return out;
}

static mp_forceinline 
mp_bool mp_timer_larger_than(mp_timer_t a, mp_timer_t b)
{
    return a > b ? mp_true : mp_false;
}

static mp_forceinline 
mp_bool mp_timer_eq(mp_timer_t a, mp_timer_t b)
{
    return abs(a - b) < 1 ? mp_true : mp_false;
}

#endif /* #ifdef MP_INTERNAL_TIMER_H_ */
