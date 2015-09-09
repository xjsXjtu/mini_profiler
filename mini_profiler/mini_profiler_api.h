#ifndef MINI_PROFILER_API_H_
#define MINI_PROFILER_API_H_

#include "mini_profiler.h"

/* 
 * The only one global variable. 
 * Need to declare before use. 
 * It's to save the run-time path from root to current node along side the node tree
 */
#define MP_DEFINE_GLOBAL_VAR   mp_runtime_node_stack g_stack;

#define MP_OPEN()               mp_open();

#define MP_ENTER(node_name)     mp_enter(node_name);

#define MP_LEAVE(node_name)     mp_leave();

#define MP_REPORT_AND_CLOSE()   mp_report_and_close();
            
#endif /* #ifndef MINI_PROFILER_API_H_ */
