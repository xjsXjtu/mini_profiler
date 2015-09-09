#include <stdio.h>
#include <stdlib.h>
#include "unittest_framework.h"
#include "../mp_internal_stack.h"

static mp_bool test_mp_internal_stack_create(void);
static mp_bool test_mp_internal_stack_size(void);
static mp_bool test_mp_internal_stack_push_top_pop(void);
static mp_bool test_mp_internal_stack_empty(void);
static mp_bool test_mp_internal_stack_destroy(void); /* there's another method to test memory leak by Valgrind, don't know how to test here yet */

void mp_internal_stack_unittest(int *total_num, int *pass_num)
{
    printf("mp_internal_stack_unittest ...\n");
    test_one_case(test_mp_internal_stack_create, "\ttest_mp_internal_stack_create", total_num, pass_num);
    test_one_case(test_mp_internal_stack_size,   "\ttest_mp_internal_stack_size", total_num, pass_num);
    test_one_case(test_mp_internal_stack_push_top_pop, "\ttest_mp_internal_stack_push_top_pop", total_num, pass_num);
    test_one_case(test_mp_internal_stack_empty, "\ttest_mp_internal_stack_empty", total_num, pass_num);
    test_one_case(test_mp_internal_stack_destroy, "\ttest_mp_internal_stack_destroy ->[1]", total_num, pass_num);
    printf("\t [1] This case need to Use Valgrind to check memory leak.\n");
    
    return ;
}

static
mp_bool test_mp_internal_stack_create()
{
    mp_internal_stack_t* stack = mp_internal_stack_create();
    mp_bool out = mp_false;
    if(stack != NULL && stack->size == 0 && stack->top == NULL)
    {
        out = mp_true;
    }
    else
    {
        out = mp_false;
    }
    mp_internal_stack_destroy(stack);
    return out;
}

static
mp_bool test_mp_internal_stack_size()
{
    mp_internal_stack_t* stack = mp_internal_stack_create();
    mp_bool out = mp_false;
    if(mp_internal_stack_size(stack) != 0)
    {
       out = mp_false;
    }
    else
    {
        int *p_data = (int*)malloc(sizeof(int));
        *p_data = 1;
        mp_internal_stack_push(stack, p_data);
        if(mp_internal_stack_size(stack) != 1)
        {
           out = mp_false;
        }
        else
        {
            out = mp_true;
        }
    }
	while(!mp_internal_stack_empty(stack))
	{
		int *p_data = (int*)mp_internal_stack_top(stack);
		mp_internal_stack_pop(stack);
		free(p_data);
	}
    mp_internal_stack_destroy(stack);
    return out;
}

static
mp_bool test_mp_internal_stack_push_top_pop()
{
    mp_internal_stack_t* stack = mp_internal_stack_create();
    mp_bool out = mp_false;
    int *p_data = NULL;
    
    p_data = (int*)malloc(sizeof(int));    *p_data = 1;
    mp_internal_stack_push(stack, p_data);
    p_data = (int*)malloc(sizeof(int));    *p_data = 2;
    mp_internal_stack_push(stack, p_data);

	p_data = (int*)mp_internal_stack_top(stack);
	mp_internal_stack_pop(stack);
	free(p_data);
    
	p_data = (int*)malloc(sizeof(int));    *p_data = 3;
    mp_internal_stack_push(stack, p_data);
    
	p_data = (int*)mp_internal_stack_top(stack);
	if( *p_data == 3
        && (mp_internal_stack_pop(stack), free(p_data), *(int*)(mp_internal_stack_top(stack)) == 1)
       )
    {
        out = mp_true;
    }
    else
    {
        out = mp_false;
    }
	while(!mp_internal_stack_empty(stack))
	{
		p_data = (int*)mp_internal_stack_top(stack);
		mp_internal_stack_pop(stack);
		free(p_data);
	}
    mp_internal_stack_destroy(stack);
    return out;
}

static
mp_bool test_mp_internal_stack_empty()
{
    mp_internal_stack_t* stack = mp_internal_stack_create();
    mp_bool out = mp_false;
    if(mp_internal_stack_empty(stack) == mp_false)
    {
       out = mp_false;
    }
    else
    {
        int *p_data = (int*)malloc(sizeof(int));
        *p_data = 1;
        mp_internal_stack_push(stack, p_data);
        if(mp_internal_stack_empty(stack) == mp_true)
        {
           out = mp_false;
        }
        else
        {
            out = mp_true;
        }
    }
	while(!mp_internal_stack_empty(stack))
	{
		int *p_data = (int*)mp_internal_stack_top(stack);
		mp_internal_stack_pop(stack);
		free(p_data);
	}
    mp_internal_stack_destroy(stack);
    return out;
}

/* don't know how to test yet */
static
mp_bool test_mp_internal_stack_destroy()
{
	mp_internal_stack_t* stack = mp_internal_stack_create();
    int *p_data = NULL;
    
    p_data = (int*)malloc(sizeof(int));    *p_data = 1;
    mp_internal_stack_push(stack, p_data);
    p_data = (int*)malloc(sizeof(int));    *p_data = 2;
    mp_internal_stack_push(stack, p_data);

	while(!mp_internal_stack_empty(stack))
	{
		p_data = (int*)mp_internal_stack_top(stack);
		mp_internal_stack_pop(stack);
		free(p_data);
	}
    mp_internal_stack_destroy(stack);

    return mp_true;
}


