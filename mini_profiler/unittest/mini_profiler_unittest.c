#include <stdio.h>
#include <stdlib.h>
#include "../mp_internal_compiler_keywords.h"
#include "../mini_profiler_api.h"
#include "unittest_framework.h"

MP_DEFINE_GLOBAL_VAR;

static mp_bool test_mp_less_children_waste(void);
static mp_bool test_mp_enter_leave_1(void);         /* Add one node that not exist */
static mp_bool test_mp_enter_leave_2(void);         /* Add one node that already exist */
static mp_bool test_mp_report_and_close(void);      /* Use Valgrind to check memory leak. But report need test *manually* */


void mini_profiler_unittest(int *total_num, int *pass_num)
{
    printf("mini_profiler_unittest ...\n");
    test_one_case(test_mp_enter_leave_1,       "\ttest_mp_enter_leave_1",       total_num, pass_num);
    test_one_case(test_mp_enter_leave_2,       "\ttest_mp_enter_leave_2",       total_num, pass_num);
    test_one_case(test_mp_report_and_close,    "\ttest_mp_report_and_close ->[1]",    total_num, pass_num);
    printf("\t [1] This case need: \n\t   a) Use Valgrind to check memory leak; \n\t   b) Profiler report log needs verfication *manually*.\n");
    return ;
}

#if 0
/*
 * This function is a typical case for converting recursive tree algorithm to iteration version. 
 * Move it here since mini_profiler.h no more need it. But we may need to reference it in the future. 
 */
static mp_forceinline
mp_timer_t mp_less_children_waste(mp_node *parent)
{
#if 0
    /* recursive version
     * paste here as comments, since the interation version is hard to understand. 
     */
    mp_timer_t wt;
    int i=0;
    
    if(parent == NULL)
    {
        mp_timer_clear(&wt);
        return wt;
    }
    else if(parent->son_num < 1)
    {
		parent->actual_total = mp_timer_sub(parent->actual_total, parent->waste_total);
        return parent->waste_total;
    }
    else
    {
        mp_timer_clear(&wt);
        for(i=0; i<parent->son_num; i++)
        {
            wt = mp_timer_add(wt, mp_less_children_waste(parent->child[i]));
        }            
        parent->waste_total = mp_timer_add(parent->waste_total, wt);
        parent->actual_total = mp_timer_sub(parent->actual_total, parent->waste_total);
        return parent->waste_total;
    }
#else
    typedef struct snapshot_s
    {
        mp_node *mpn;
        int stage;
    }snapshot_t;
    
    mp_timer_t ret_val;
    int i=0;
    mp_internal_stack_t *stack = NULL;
    snapshot_t *curr_snapshot, *next_snapshot;
    
    curr_snapshot = malloc(sizeof(snapshot_t));
    curr_snapshot->mpn = parent;
    curr_snapshot->stage = 0;
    stack = mp_internal_stack_create();
    mp_internal_stack_push(stack, curr_snapshot);
    
    while(!mp_internal_stack_empty(stack))
    {
		curr_snapshot = (snapshot_t*)mp_internal_stack_top(stack);
        mp_internal_stack_pop(stack);
        
        switch(curr_snapshot->stage)
        {
        case 0:
            if(curr_snapshot->mpn == NULL)
            {
                mp_timer_clear(&ret_val);
                free(curr_snapshot);
                continue;
            }
            else if(curr_snapshot->mpn->son_num < 1)
            {
                ret_val = curr_snapshot->mpn->waste_total;
				curr_snapshot->mpn->actual_total = mp_timer_sub(curr_snapshot->mpn->actual_total, curr_snapshot->mpn->waste_total);
                free(curr_snapshot);
                continue;
            }
            else
            {
                curr_snapshot->stage = 1;
                mp_internal_stack_push(stack, curr_snapshot);
                for(i=curr_snapshot->mpn->son_num-1; i>=0; i--)
                {
					next_snapshot = (snapshot_t*)malloc(sizeof(snapshot_t));
                    next_snapshot->mpn = curr_snapshot->mpn->child[i];
                    next_snapshot->stage = 0;
                    mp_internal_stack_push(stack, next_snapshot);
                }
                continue;
            }
            break;
        case 1:
			mp_timer_clear(&ret_val);
			for(i=curr_snapshot->mpn->son_num-1; i>=0; i--)
			{
				ret_val = mp_timer_add(curr_snapshot->mpn->child[i]->waste_total, ret_val);
			}
			curr_snapshot->mpn->waste_total = mp_timer_add(curr_snapshot->mpn->waste_total, ret_val);
			curr_snapshot->mpn->actual_total = mp_timer_sub(curr_snapshot->mpn->actual_total, curr_snapshot->mpn->waste_total);
            free(curr_snapshot);
            continue;
            break;
        }
    }
	mp_internal_stack_destroy(stack);
    return ret_val;
#endif
}

/*
 * The input tree is (actual_total, waste_total):
 *        1000,1
 *      /   |    \
 * 200,2  300,3  400,4
 *        /  \     |
 *     50,5  60,6  70,7
 *
 * The output is:
 *        972,28
 *      /   |    \
 * 198,2  286,14  389,11
 *        /  \     |
 *     45,5  54,6  63,7
 */
static
void util_create_one_son(
        mp_node *p_parent
       ,int actual_total, int waste_total )
{
    char node_name[33] = {0};
    mp_node *p_curr = (mp_node*)malloc(sizeof(mp_node));
    sprintf(node_name,"name_%d",actual_total);
    mp_node_init(p_curr, node_name, p_parent);
    mp_timer_set(&p_curr->actual_total, actual_total);
    mp_timer_set(&p_curr->waste_total, waste_total);
    p_parent->child[p_parent->son_num] = p_curr;
    p_parent->son_num++;
}

static
mp_bool test_mp_less_children_waste()
{
    mp_node *p_root = (mp_node*)malloc(sizeof(mp_node));
    mp_node *p_parent = NULL, *p_curr = NULL;
	mp_bool out = mp_false;
	mp_timer_t actual_totle, waste_totle;
    
    mp_node_init(p_root, "Root", NULL);
    mp_timer_set(&p_root->actual_total, 1000);
    mp_timer_set(&p_root->waste_total, 1);
    
    util_create_one_son(p_root, 200, 2);
    util_create_one_son(p_root, 300, 3);
    util_create_one_son(p_root, 400, 4);
    
    p_parent = p_root->child[1];
    util_create_one_son(p_parent, 50, 5);
    util_create_one_son(p_parent, 60, 6);
    p_parent = p_root->child[2];
    util_create_one_son(p_parent, 70, 7);
    
    mp_less_children_waste(p_root);
    
    if(  (p_curr = p_root,					   mp_timer_set(&actual_totle, 972), mp_timer_set(&waste_totle, 28), (mp_timer_eq(p_curr->actual_total, actual_totle) && mp_timer_eq(p_curr->waste_total, waste_totle)))
      && (p_curr = p_root->child[0],		   mp_timer_set(&actual_totle, 198), mp_timer_set(&waste_totle, 2),  (mp_timer_eq(p_curr->actual_total, actual_totle) && mp_timer_eq(p_curr->waste_total, waste_totle)))
      && (p_curr = p_root->child[1],		   mp_timer_set(&actual_totle, 286), mp_timer_set(&waste_totle, 14), (mp_timer_eq(p_curr->actual_total, actual_totle) && mp_timer_eq(p_curr->waste_total, waste_totle)))
      && (p_curr = p_root->child[2],		   mp_timer_set(&actual_totle, 389), mp_timer_set(&waste_totle, 11), (mp_timer_eq(p_curr->actual_total, actual_totle) && mp_timer_eq(p_curr->waste_total, waste_totle)))
      && (p_curr = p_root->child[1]->child[0], mp_timer_set(&actual_totle, 45),  mp_timer_set(&waste_totle, 5),  (mp_timer_eq(p_curr->actual_total, actual_totle) && mp_timer_eq(p_curr->waste_total, waste_totle)))
      && (p_curr = p_root->child[1]->child[1], mp_timer_set(&actual_totle, 54),  mp_timer_set(&waste_totle, 6),  (mp_timer_eq(p_curr->actual_total, actual_totle) && mp_timer_eq(p_curr->waste_total, waste_totle)))
      && (p_curr = p_root->child[2]->child[0], mp_timer_set(&actual_totle, 63),  mp_timer_set(&waste_totle, 7),  (mp_timer_eq(p_curr->actual_total, actual_totle) && mp_timer_eq(p_curr->waste_total, waste_totle)))
       )
    {
        out = mp_true;
    }
	mp_free(p_root);
    return out;
}
#endif

/* add one node that not exist */
static mp_bool test_mp_enter_leave_1(void)
{
    int i=0;
    mp_bool out = mp_false;
    mp_open();
    mp_enter("Level_1");
    mp_enter("Level_2");
    if(  g_stack.top == 2 
      && g_stack.s[1]->son_num == 1
      && g_stack.s[1]->child[0] == g_stack.s[2]
      && strcmp(g_stack.s[1]->name, "Level_1") == 0
      && g_stack.s[2]->parent == g_stack.s[1]
      && g_stack.s[2]->son_num == 0
      && strcmp(g_stack.s[2]->name, "Level_2") == 0
      )
    {
        out = mp_true;
    }
    
    for(i=0; i<1024; i++);
    
    mp_leave();
    if(  out == mp_true
      && g_stack.top == 1 
      && g_stack.s[1]->son_num == 0
      && g_stack.s[1]->child[0] == NULL
      && strcmp(g_stack.s[1]->name, "Level_1") == 0
      )
    {
        out = mp_true;
    }
    mp_leave();
    
    mp_report_and_close();
    return out;
}

/* add one node that already exist */
static mp_bool test_mp_enter_leave_2(void)
{
    int i=0, j=0;
    mp_bool out = mp_false;
    mp_open();
    mp_enter("Level_1");
    
    mp_enter("Level_2_1");
    for(i=0; i<1024; i++);
    mp_leave();
    
    mp_enter("Level_2_2");
    for(i=0; i<1024; i++);
    mp_leave();
    
    mp_enter("Level_2_1");
    if(  g_stack.top == 2 
      && g_stack.s[1]->son_num == 2
      && g_stack.s[1]->child[0] == g_stack.s[2]
      && strcmp(g_stack.s[1]->name, "Level_1") == 0
      && g_stack.s[2]->parent == g_stack.s[1]
      && g_stack.s[2]->son_num == 0
      && strcmp(g_stack.s[2]->name, "Level_2_1") == 0
      )
    {
        out = mp_true;
    }
    for(i=0; i<1024; i++);
    mp_leave();
    if(  out == mp_true
      && g_stack.top == 1 
      && g_stack.s[1]->son_num == 2
      && g_stack.s[1]->child[0] == NULL
      && strcmp(g_stack.s[1]->name, "Level_1") == 0
      )
    {
        out = mp_true;
    }
    
    mp_leave();
    mp_report_and_close();
    return out;
}

/*
 * Use Valgrind to check memory leak. 
 * But report need test *manually* 
 */
static mp_bool test_mp_report_and_close(void)
{
    int i=0;
    mp_bool out = mp_true;
    mp_open();
    mp_enter("Level_1");
    mp_enter("Level_2");
    for(i=0; i<1024; i++);
    mp_leave();
    mp_leave();
    mp_report_and_close();
    return out;
}
       