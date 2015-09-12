#include <stdio.h>
#include <stdlib.h>

#include "unittest_framework.h"
#include "mp_internal_stack_unittest.h"
#include "mini_profiler_unittest.h"

int main()
{
    int total_num = 0, pass_num = 0;
    mp_internal_stack_unittest(&total_num, &pass_num);
    mini_profiler_unittest(&total_num, &pass_num);
    printf("Unittest passed: %d/%d\n", pass_num, total_num);

#if defined(_MSC_VER)
    system("pause");
#endif
    return 0;
}    

void test_one_case(unittest_function f, char* fname, int *total_num, int *pass_num)
{
    printf("%s ... ", fname);
    if(f())
    {
        printf("PASS\n");
        *pass_num += 1;
    }
    else
    {
        printf("FAIL\n");
    }
    *total_num += 1;
}