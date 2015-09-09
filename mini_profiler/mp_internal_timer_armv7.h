#ifndef MP_INTERNAL_TIMER_ARMV7_H_
#define MP_INTERNAL_TIMER_ARMV7_H_

#define MP_TIMER_UNIT ("cycles")
#define MP_TIMER_MAX  (1.0 * 100 * 365 * 24 * 60 * 60 * (1  * 1e9)) /* 100 years on 1GHz CPU */
#define MP_TIMER_MOD  (64.0 * (((uint64_t)1)<<32))

#if defined(__ARMCC_VERSION)
register unsigned int PMCR       __asm("cp15:0:c9:c12:0");
register unsigned int PMCNTENSET __asm("cp15:0:c9:c12:1");
register unsigned int PMCNTENCLR __asm("cp15:0:c9:c12:2");
register unsigned int PMCCNTR    __asm("cp15:0:c9:c13:0");

static mp_forceinline
void mp_internal_timer_armv7_stop(void)
{
    PMCNTENCLR = 0x80000000;    /* Disable the cycle counter */
}

static mp_forceinline
void mp_internal_timer_start(void)
{
    mp_internal_timer_armv7_stop();
    PMCR |= 0xF;                /* Set the D, C, P, E flag bits */
    PMCNTENSET = 0x80000000;    /* Enable the cycle counter */
}

static mp_forceinline 
mp_timer_t mp_internal_timer_armv7_read(void)
{
    return PMCCNTR;             /* Read the cycle counter. */
}

static mp_forceinline
mp_timer_t mp_get_cur_time()
{
    /* 
     * Overflow will be handled in mp_timer_sub()
     */
    return 64 * mp_internal_timer_armv7_read();
}

#else
#error "Not implemented yet."
#endif

#endif /* #ifndef MP_INTERNAL_TIMER_ARMV7_H_ */
