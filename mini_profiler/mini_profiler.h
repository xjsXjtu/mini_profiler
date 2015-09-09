#ifndef MINI_PROFILER
#define MINI_PROFILER

#ifdef  __cplusplus
extern "C" {
#endif

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "limits.h"
#include "assert.h"

#include "mp_internal_compiler_keywords.h"
#include "mp_internal_stack.h"
#include "mp_internal_timer.h"

#define MP_MAX_NODE_NAME_LEN (128)
#define MP_MAX_SON_NUM (128)
#define MP_MAX_DEPTH (32)

/*  Tree Node Arch                  Comments                            ==> Duaration Updates Related
 *   |
 *   | ----(1.0)              node_1: invoking mp_enter("node_1")       ==> clear p_curr->current_duaration, accumulate p_parent->current_duaration
 *   | ----(1.1)              node_1: exiting                           ==> update p_curr->start
 *   |
 *   | ----|
 *         |----(1_1.0)         node_1_1: invoking mp_enter("node_1_1") ==> clear p_curr->current_duaration, accumulate p_parent->current_duaration
 *         |----(1_1.1)         node_1_1: exiting                       ==> update p_curr->start
 *         |
 *         |----(1_1.2)         node_1_1: invoking mp_leave()           ==> accumulate p_curr->current_duaration, p_curr->total, p_parent->current_duaration
 *         |----(1_1.3)         node_1_1: exiting                       ==> update p_parent->start
 *         |
 *         |----(1_2.0)         node_1_2: invoking mp_enter("node_1_2")
 *         |----(1_2.1)         node_1_2: exiting
 *         |
 *         |----(1_2.2)         node_1_2: invoking mp_leave()
 *         |----(1_2.3)         node_1_2: exiting
 *   |---- |     
 *   |
 *   |----(1.2)               node_1: invoking mp_leave()               ==> accumulate p_curr->current_duaration, p_curr->total, p_parent->current_duaration
 *   |----(1.3)               node_1: exiting                           ==> update p_parent->start
 *
 */
typedef struct mp_node_s{
    char name[MP_MAX_NODE_NAME_LEN];            /* node name */
    struct mp_node_s *parent;                   /* parent node */
    struct mp_node_s *child[MP_MAX_SON_NUM];    /* children nodes */
    int son_num;                                /* children number */
    
    /* Node's beginning time stamp. 
     * It will be used to accumulate node's duaration time for current invoking.
     * Update when:
     *   - exiting self mp_enter(): eg. 1.1
     *   - exiting child's mp_leave(): eg. 1_1.3, 1_2.3
     */
    mp_timer_t start;
    /* Node's duaration time for current invoking. 
     * Will be set to 0 when:
     *   - in self mp_enter()
     * Will be accumulated:
     *   - For self duaration, when in a child's mp_enter() or in self mp_leave()
     *     current_duaration += current_timestamp - start
     *   - For Children's duaration, when in a child's mp_leave()
     *     parent->current_duaration += current_duaration.
     */
    mp_timer_t current_duaration;
    mp_timer_t duaration_min;
    mp_timer_t duaration_max;
    
    /* 
     * Number of times of invoking current node.
     * Update when:
     *   - In self mp_leave(): count++
     */
    int count;
    /* Node's total duaration across all inoking times. 
     * Accumulate when:
     *   - In self mp_leave(): total += current_duaration
     */
    mp_timer_t total;
}mp_node;

typedef struct mp_runtime_node_stack_s{
    mp_node *s[MP_MAX_DEPTH];
    int top;
}mp_runtime_node_stack;

extern mp_runtime_node_stack g_stack;

/* 
 * Prototype declarations 
 */
/* API functions */
static mp_forceinline void          mp_open(void);
static mp_forceinline void          mp_enter(char node_name[MP_MAX_NODE_NAME_LEN]);
static mp_forceinline void          mp_leave(void);
static mp_forceinline void          mp_report_and_close(void);
/* Internal functions */
static mp_forceinline void          mp_node_init(mp_node *mn, char name[MP_MAX_NODE_NAME_LEN], mp_node *parent);
static mp_forceinline mp_timer_t    mp_less_children_waste(mp_node *parent); /* recursive, cannot force inline yet. */
static mp_forceinline void          mp_print_tree(FILE *fp_log, mp_node *mn, int depth);
static mp_forceinline void          mp_free(mp_node *mn);


static mp_forceinline
void mp_node_init(mp_node *mn, char name[MP_MAX_NODE_NAME_LEN], mp_node *parent)
{
    strcpy(mn->name, name);
    mn->parent = parent;
    mn->son_num = 0;
    
    mp_timer_clear(&mn->start);
    mp_timer_clear(&mn->current_duaration);
    mp_timer_clear(&mn->duaration_max);
    mp_timer_set  (&mn->duaration_min, MP_TIMER_MAX);
    mp_timer_clear(&mn->total);
    mn->count = 0;
}

static mp_forceinline
void mp_open()
{
    mp_node *p_root = NULL;
    p_root = (mp_node*)malloc(sizeof(mp_node));
    mp_node_init(p_root, "Root", NULL);
    p_root->count++;

    /* push run-time stack */
    g_stack.s[0] = p_root;
    g_stack.top = 0;
    
    mp_internal_timer_start();
}

static mp_forceinline
void mp_enter(char node_name[MP_MAX_NODE_NAME_LEN])
{
    mp_timer_t entering_mp_enter_time = mp_get_cur_time();
    int i=0;
    mp_node *p_curr = NULL;
    mp_node *p_parent = g_stack.s[g_stack.top];
    
    /* if this node existed in his parent's children */
    assert(p_parent->son_num >= 0);
    for(i=0; i<p_parent->son_num; i++)
    {
        if(strcmp(node_name, p_parent->child[i]->name) == 0)
        {
            break;
        }
    }
    if(i == p_parent->son_num)
    {
        /* not existed in his parent's children yet */
        /* create this node, then add to his parent's children */
        p_curr = (mp_node*)malloc(sizeof(mp_node));
        mp_node_init(p_curr, node_name, p_parent);
        p_parent->child[p_parent->son_num] = p_curr;
        p_parent->son_num++;
    }
    else
    {
        /* already existed in his parent's children */
        p_curr = p_parent->child[i];
    }

    /* push to run-time stack */
    g_stack.top++;
    g_stack.s[g_stack.top] = p_curr;

    /* end mp_enter() */
    p_curr->count++;
    mp_timer_clear(&p_curr->current_duaration);
    p_parent->current_duaration = mp_timer_add(p_parent->current_duaration, mp_timer_sub(entering_mp_enter_time, p_parent->start));
    p_curr->start = mp_get_cur_time();
}

static mp_forceinline
void mp_leave()
{
    mp_timer_t entering_mp_leave_time = mp_get_cur_time();
    mp_node *p_curr = g_stack.s[g_stack.top];
    mp_node *p_parent = p_curr->parent;
    
    mp_timer_t curr_cost = mp_timer_sub(entering_mp_leave_time, p_curr->start);
    p_curr->current_duaration = mp_timer_add(p_curr->current_duaration, curr_cost);
    
    if(mp_true == mp_timer_larger_than(p_curr->current_duaration, p_curr->duaration_max))
    {
        mp_timer_set(&p_curr->duaration_max, p_curr->current_duaration);
    }
    if(mp_true == mp_timer_larger_than(p_curr->duaration_min, p_curr->current_duaration))
    {
        mp_timer_set(&p_curr->duaration_min, p_curr->current_duaration); 
    }
    p_curr->total = mp_timer_add(p_curr->total, p_curr->current_duaration);
    p_parent->current_duaration = mp_timer_add(p_parent->current_duaration, p_curr->current_duaration);
    
    /* Pop from run-time stack */
    /* Cannot free memory here, we need save the profiling data until the print them.
     * p_curr->parent->son_num--;
     * free(p_curr);
     * p_curr = NULL;
     */
    g_stack.top--;

    p_parent->start = mp_get_cur_time();
}

static mp_forceinline
void mp_print_tree(FILE *fp_log, mp_node *mn, int depth)
{
    int i=0;
    int *p_depth = NULL;
    mp_internal_stack_t *stack_node = NULL;
    mp_internal_stack_t *stack_depth = NULL;
    
    if(mn == NULL)
        return;

    stack_node = mp_internal_stack_create();
    stack_depth = mp_internal_stack_create();
    p_depth = (int*)malloc(sizeof(int));    *p_depth = depth;
    mp_internal_stack_push(stack_node, mn);
    mp_internal_stack_push(stack_depth, p_depth);
    
    while(!mp_internal_stack_empty(stack_node) && !mp_internal_stack_empty(stack_depth))
    {
        mp_node* mntmp = (mp_node*)mp_internal_stack_top(stack_node);
        int *p_depthtmp = (int*)mp_internal_stack_top(stack_depth);
        int depthtmp = *p_depthtmp;
        mp_internal_stack_pop(stack_node);
        mp_internal_stack_pop(stack_depth);
        free(p_depthtmp);
        
        /* Node Name */
        for(i=0; i<depthtmp; i++)
        {
            fprintf(fp_log, "|\t");
        }
        fprintf(fp_log, "+ Node Name        : %s\n", mntmp->name);
        
        /* Total */
        for(i=0; i<depthtmp; i++)
        {
            fprintf(fp_log, "|\t");
        }
        fprintf(fp_log, "| Total            : %.0f %s\n", mntmp->total, MP_TIMER_UNIT);
        
        /* Max */
        for(i=0; i<depthtmp; i++)
        {
            fprintf(fp_log, "|\t");
        }
        fprintf(fp_log, "| Max  : %.0f %s\n", mntmp->duaration_max, MP_TIMER_UNIT);
        
        /* Min */
        for(i=0; i<depthtmp; i++)
        {
            fprintf(fp_log, "|\t");
        }
        fprintf(fp_log, "| Min  : %.0f %s\n", mntmp->duaration_min, MP_TIMER_UNIT);
        
        /* Avg */
        for(i=0; i<depthtmp; i++)
        {
            fprintf(fp_log, "|\t");
        }
        fprintf(fp_log, "| Avg  : %.0f %s\n", mntmp->total / mntmp->count, MP_TIMER_UNIT);
        
        /* Number of Calls */
        for(i=0; i<depthtmp; i++)
        {
            fprintf(fp_log, "|\t");
        }
        fprintf(fp_log, "| Number of Calls             : %d\n", mntmp->count);
        
        /* last line */
        for(i=0; i<depthtmp; i++)
        {
            fprintf(fp_log, "|\t");
        }
        fprintf(fp_log, "|\n");
        
        /* print children */
        for(i=mntmp->son_num-1; i>=0; i--)
        {
            p_depth = (int*)malloc(sizeof(int));    *p_depth = depthtmp + 1;
            mp_internal_stack_push(stack_node, mntmp->child[i]);
            mp_internal_stack_push(stack_depth, p_depth);
        }
    }
    mp_internal_stack_destroy(stack_node);
    mp_internal_stack_destroy(stack_depth);
}

static mp_forceinline
void mp_free(mp_node *mn)
{
    int i=0;
    mp_internal_stack_t *stack = NULL;
    if(mn == NULL)
        return;

    stack = mp_internal_stack_create();
    mp_internal_stack_push(stack, (void*)mn);
    while(!mp_internal_stack_empty(stack))
    {
        mp_node *mn_temp = (mp_node*)mp_internal_stack_top(stack);
        mp_internal_stack_pop(stack);
        for(i=0; i<mn_temp->son_num; i++)
        {
            mp_internal_stack_push(stack, (void*)(mn_temp->child[i]));
        }
        free(mn_temp);
    }
    mp_internal_stack_destroy(stack);
}

static mp_forceinline
void mp_report_and_close()
{
    int i=0;
    mp_node *root = g_stack.s[0];
	FILE *fp_log = NULL;
    
#if 0
    /* all non-leaf node should less his children's waste time */
    mp_less_children_waste(root);
#endif
    /* print report */
    fp_log = fopen("profile_report.log", "w");
    if(fp_log == NULL)
    {
        printf("Error: Cannot open mp_profiler.log to write!!\n");
    }
    else
    {
        for(i=0; i<root->son_num; i++)
        {
            mp_print_tree(fp_log, root->child[i], 0);
        }
        fclose(fp_log);
    }
    
    /* free memory */
    mp_free(root);
}

#ifdef  __cplusplus
} /* extern "C" { */
#endif
#endif /* #ifndef MINI_PROFILER */
