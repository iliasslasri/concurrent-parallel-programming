#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>

#include "protected_buffer.h"

extern int pt_debug;

typedef void *(*main_func_t)(void *);

typedef struct {
  int core_pool_size;
  int max_pool_size;
  int size;
  int shutdown;
} thread_pool_t;

// Create a thread pool. This pool must be protected against
// concurrent accesses.
thread_pool_t *thread_pool_init(int core_pool_size, int max_pool_size);

// Decrease threads pool size and broadcast update.
void pool_thread_terminate(thread_pool_t *thread_pool);

// pool_thread_main_params extends main original params.
// params is the parameter for the original main procedure.
// is_core is an extra parameter to indicate to a pool thread whether
// it belongs to the core thread pool or not.
typedef struct {
  int is_core;
  void *params;
  int id;
} pool_thread_main_params;

#define NOTHREAD -1  // Thread not created
#define CORETHREAD 1 // Thread created as core thread
#define TEMPTHREAD 0 // Thread created but not as core thread

// If the current thread pool size is not greater than core_pool_size,
// create a new thread. If it is and force is true, create a new
// thread as well. When a thread is created, increment the current
// thread pool size. Use main as a thread main procedure. Return
// thread status (NOTHREAD, CORETHREAD, TEMPTHREAD)
int pool_thread_create(thread_pool_t *thread_pool, main_func_t main,
                       void *main_params, int force);

// Shutdown
void thread_pool_shutdown(thread_pool_t *thread_pool);

// Getter
int get_shutdown(thread_pool_t *thread_pool);

// Decrease thread number and broadcast update. Return whether thread
// was actually removed (size > core_pool_size)
int pool_thread_remove(thread_pool_t *thread_pool);

// Wait for pool to be empty.
void wait_thread_pool_empty(thread_pool_t *thread_pool);
#endif
