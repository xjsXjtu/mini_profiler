#ifndef MP_INTERNAL_TIMER_LINUX_H_
#define MP_INTERNAL_TIMER_LINUX_H_

/* 
 * Use clock_gettime() from Linux system call
 *   - This method can handle OS delay functions. 
 *     eg. when OS delay 1 second, clock() can include this 1 second, as well as the cost of invoking delay function itself.
 *   - For armcc, please add "-Llibrt.a" to your linker flag.  
 */
#include <time.h>
#define MP_TIMER_UNIT ("nsec")
#define MP_TIMER_MAX  (1.0 * 100 * 365 * 24 * 60 * 60 * 1e9) /* 100 years */
#define MP_TIMER_MOD  (0.0)

static mp_forceinline
void mp_internal_timer_start(void)
{}

static mp_forceinline
mp_timer_t mp_get_cur_time()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000000000.0 + t.tv_nsec;
}

#endif /* #ifndef MP_INTERNAL_TIMER_LINUX_H_ */
