#ifndef MP_INTERNAL_TIMER_X86_X64_H_
#define MP_INTERNAL_TIMER_X86_X64_H_

/*
 * 
 */
#if defined(__GNUC__)
/* ref: http://stackoverflow.com/questions/9887839/clock-cycle-count-wth-gcc */
#include <x86intrin.h>
#elif defined(_MSC_VER)
/* ref: https://msdn.microsoft.com/en-us/library/twchhe95%28v=vs.100%29.aspx */
#include <intrin.h>
#else
#error "Not implemented yet."
#endif

#define MP_TIMER_UNIT ("cycles")
#define MP_TIMER_MAX  (1.0 * 100 * 365 * 24 * 60 * 60 * (4 * 1e9)) /* 100 years on 4GHz CPU */
#define MP_TIMER_MOD  (0.0)

static mp_forceinline
void mp_internal_timer_start(void)
{}

static mp_forceinline
mp_timer_t mp_get_cur_time()
{
    return __rdtsc();
}

#endif /* #ifndef MP_INTERNAL_TIMER_X86_X64_H_ */