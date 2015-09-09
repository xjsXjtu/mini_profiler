#ifndef UNITTEST_FRAMEWORK_H_
#define UNITTEST_FRAMEWORK_H_
#include <stdio.h>
#include <stdlib.h>
#include "../mp_internal_compiler_keywords.h"

typedef mp_bool (*unittest_function)(void);
void test_one_case(unittest_function f, char* fname, int *total_num, int *pass_num);

#endif /* #ifndef UNITTEST_FRAMEWORK_H_ */
