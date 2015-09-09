#ifndef MP_INTERNAL_TIMER_STANDARD_C_CLOCK_H_
#define MP_INTERNAL_TIMER_STANDARD_C_CLOCK_H_

/* 
 * Use clock() from C standard libary
 *   - This method cannot handle OS delay functions. 
 *     eg. when OS delay 1 second, clock() cannot include this 1 second, but only the cost of invoking delay function itself.  
 */
#include <time.h>
#define MP_TIMER_UNIT ("usec")
#define MP_TIMER_MAX  (1.0 * 100 * 365 * 24 * 60 * 60 * 1e6) /* 100 years */
#define MP_TIMER_MOD  (0.0)

static mp_forceinline
void mp_internal_timer_start(void)
{}

static mp_forceinline
mp_timer_t mp_get_cur_time()
{
    clock_t t = clock();
    return t * 1.0 * 1e6 / CLOCKS_PER_SEC;
}

#endif /* #ifndef MP_INTERNAL_TIMER_STANDARD_C_CLOCK_H_ */
