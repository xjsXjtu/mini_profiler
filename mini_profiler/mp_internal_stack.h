#ifndef MP_INTERNAL_STACK_H_
#define MP_INTERNAL_STACK_H_

/*
 * C program to implement a stack using linked list
 */

#include <stdio.h>
#include <stdlib.h>
#include "mp_internal_compiler_keywords.h"

typedef struct mp_internal_stack_node_s
{
    void* p_data;    /* use void* to simulate template in C++ */
    struct mp_internal_stack_node_s *p_next;
}mp_internal_stack_node_t;

typedef struct mp_internal_stack_s
{
    mp_internal_stack_node_t *top;
    int size;
}mp_internal_stack_t;

static mp_forceinline mp_internal_stack_t*  mp_internal_stack_create(void);
static mp_forceinline int                   mp_internal_stack_size(mp_internal_stack_t *stack);
static mp_forceinline void                  mp_internal_stack_push(mp_internal_stack_t *stack, void* p_data);
static mp_forceinline void*                 mp_internal_stack_top(mp_internal_stack_t *stack);
static mp_forceinline void                  mp_internal_stack_pop(mp_internal_stack_t *stack);
static mp_forceinline mp_bool               mp_internal_stack_empty(mp_internal_stack_t *stack);
static mp_forceinline void                  mp_internal_stack_destroy(mp_internal_stack_t *stack);


static mp_forceinline 
mp_internal_stack_t*  mp_internal_stack_create()
{
    mp_internal_stack_t *stack = (mp_internal_stack_t*)malloc(sizeof(mp_internal_stack_t));
    stack->size = 0;
    stack->top  = NULL;
    return stack;
}

static mp_forceinline
int mp_internal_stack_size(mp_internal_stack_t *stack)
{
    return stack->size;
}

static mp_forceinline 
void mp_internal_stack_push(mp_internal_stack_t *stack, void* p_data)
{
    mp_internal_stack_node_t *p_node = (mp_internal_stack_node_t*)malloc(sizeof(mp_internal_stack_node_t));
    p_node->p_data = p_data;
    
    if (mp_internal_stack_empty(stack))
    {
        p_node->p_next = NULL;
        stack->top = p_node;
    }
    else
    {
        p_node->p_next = stack->top;
        stack->top = p_node;
    }
    stack->size++;
}

static mp_forceinline 
void* mp_internal_stack_top(mp_internal_stack_t *stack)
{
    if(mp_internal_stack_empty(stack))
        return NULL;
    return stack->top->p_data;
}

static mp_forceinline 
void mp_internal_stack_pop(mp_internal_stack_t *stack)
{
    if(!mp_internal_stack_empty(stack))
    {
        mp_internal_stack_node_t *p_top = stack->top;
        stack->top = p_top-> p_next;
        stack->size--;
        free(p_top);
    }
}

static mp_forceinline 
mp_bool mp_internal_stack_empty(mp_internal_stack_t *stack)
{
    return stack->size <= 0;
}
 
static mp_forceinline 
void mp_internal_stack_destroy(mp_internal_stack_t *stack)
{
    while(!mp_internal_stack_empty(stack))
    {
        mp_internal_stack_node_t *p_top = stack->top;
        stack->top = p_top-> p_next;
        stack->size--;
        free(p_top);
    }
    free(stack);
}

#endif /* #ifndef MP_INTERNAL_STACK_H_ */
