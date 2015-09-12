#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "../mini_profiler_api.h"

MP_DEFINE_GLOBAL_VAR;
void sleep_portable(int milliseconds);

int main(void)
{
    int i, j;
    
    MP_OPEN();

    MP_ENTER("level1_1ms");
    sleep_portable(1); /* 1 ms */
    MP_LEAVE("level1_1ms");    
#if 1
    MP_ENTER("level1_103ms");
    sleep_portable(3); /* 3ms */
    for(i=0; i<5; i++)
    {
        MP_ENTER("level2_11ms");
        sleep_portable(1);
        for(j=0; j<10; j++)
        {
            MP_ENTER("level3_1ms");
            sleep_portable(1);
            MP_LEAVE("level3_1ms");
        }
        MP_LEAVE("level2_11ms");

        MP_ENTER("level2_9ms");
        sleep_portable(9);
        MP_LEAVE("level2_9ms");

    }
    MP_LEAVE("level1_103ms");
#endif
    MP_REPORT_AND_CLOSE();
#if defined(_MSC_VER)
    system("pause");
#endif
    return 0;
}

void sleep_portable(int milliseconds)
{
#if defined(__GNUC__)
#include <unistd.h>
    /* stable on X86/X64 Linux, but very un-stable on Pandaboard Linux */
    usleep(milliseconds * (1e3-60/milliseconds)); /* 60 us is cost by usleep() */
#elif defined(_MSC_VER)
#include "WinBase.h"
    Sleep(milliseconds);
#endif
}

