#include "stdio.h"
#include "stdlib.h"

#if 0
#include "dlb_intrinsics.h"
#else
#include "../mini_profiler_api.h"
MP_DEFINE_GLOBAL_VAR;
#define DLB_INSTRUMENT_OPEN()           MP_OPEN()
#define DLB_INSTRUMENT_ENTER_PUNIT(a)   MP_ENTER(a)
#define DLB_INSTRUMENT_LEAVE_PUNIT(a)   MP_LEAVE(a)
#define DLB_INSTRUMENT_REPORT()         MP_REPORT_AND_CLOSE()
#define DLB_INSTRUMENT_CLOSE()  
#endif

void add_float(float *a, float *b, int len);
void mul_float(float *a, float *b, int len);
void add_double(double *a, double *b, int len);
void mul_double(double *a, double *b, int len);

int main(void)
{
    int len=10000;
    int i=0;
    int l=0, loops = 500;

    /* Setup Instrumentation */
    DLB_INSTRUMENT_OPEN();
    
    DLB_INSTRUMENT_ENTER_PUNIT("profiled_float");
    for(l=0; l<loops; l++)
    {
        float *a = NULL;
        float *b = NULL;

        DLB_INSTRUMENT_ENTER_PUNIT("malloc");
        a = malloc(sizeof(float) * len);
        b = malloc(sizeof(float) * len);
        DLB_INSTRUMENT_LEAVE_PUNIT("malloc");

        DLB_INSTRUMENT_ENTER_PUNIT("init");
        for(i=0; i<len; i++)
        {
            a[i] = (float)i;
            b[i] = (float)-2 * i;
        }
        DLB_INSTRUMENT_LEAVE_PUNIT("init");

        DLB_INSTRUMENT_ENTER_PUNIT("add_float");
        add_float(a, b, len);
        DLB_INSTRUMENT_LEAVE_PUNIT("add_float");

        DLB_INSTRUMENT_ENTER_PUNIT("mul_float");
        mul_float(a, b, len);
        DLB_INSTRUMENT_LEAVE_PUNIT("mul_float");
        free(a);
        free(b);
    }
    DLB_INSTRUMENT_LEAVE_PUNIT("profiled_float");

    DLB_INSTRUMENT_ENTER_PUNIT("profiled_double");
     for(l=0; l<loops; l++)
    {
        double *a = NULL;
        double *b = NULL;

        DLB_INSTRUMENT_ENTER_PUNIT("malloc");
        a = malloc(sizeof(double) * len);
        b = malloc(sizeof(double) * len);
        DLB_INSTRUMENT_LEAVE_PUNIT("malloc");

        DLB_INSTRUMENT_ENTER_PUNIT("init");
        for(i=0; i<len; i++)
        {
            a[i] = (float)i;
            b[i] = (float)-2 * i;
        }
        DLB_INSTRUMENT_LEAVE_PUNIT("init");

        DLB_INSTRUMENT_ENTER_PUNIT("add_double");
        add_double(a, b, len);
        DLB_INSTRUMENT_LEAVE_PUNIT("add_double");

        DLB_INSTRUMENT_ENTER_PUNIT("mul_double");
        mul_double(a, b, len);
        DLB_INSTRUMENT_LEAVE_PUNIT("mul_double");
        free(a);
        free(b);
    }
    DLB_INSTRUMENT_LEAVE_PUNIT("profiled_double");

    DLB_INSTRUMENT_REPORT();
    DLB_INSTRUMENT_CLOSE();
     
    return 0;
}

void add_float(float *a, float *b, int len)
{
    int i=0;
    for(i=0; i<len; i++)
    {
        a[i] = a[i] + b[i];
    }
    return;
}
void mul_float(float *a, float *b, int len)
{
    int i=0;
    for(i=0; i<len; i++)
    {
        a[i] = a[i] * b[i];
    }
    return;
}
void add_double(double *a, double *b, int len)
{
    int i=0;
    for(i=0; i<len; i++)
    {
        a[i] = a[i] + b[i];
    }
    return;
}
void mul_double(double *a, double *b, int len)
{
    int i=0;
    for(i=0; i<len; i++)
    {
        a[i] = a[i] * b[i];
    }
    return;
}

