#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>

#include "thread_pool.h"
#include "utils.h"

int pt_debug = 0;

// Create a thread pool. This pool must be protected against
// concurrent accesses.
thread_pool_t *thread_pool_init(int core_pool_size, int max_pool_size) {
  thread_pool_t *thread_pool;

  thread_pool = (thread_pool_t *)malloc(sizeof(thread_pool_t));
  thread_pool->shutdown = 0;
  thread_pool->core_pool_size = core_pool_size;
  thread_pool->max_pool_size = max_pool_size;
  thread_pool->size = 0;
  return thread_pool;
}

// If the current thread pool size is not greater than core_pool_size,
// create a new thread. If it is and force is true, create a new
// thread as well. If a thread is created, increment the current
// thread pool size. Use main as a thread main procedure. Return
// thread status (NOTHREAD, PERMANENT, TEMPORARY)

int pool_thread_create(thread_pool_t *thread_pool, main_func_t main,
                       void *main_params, int force) {
  pool_thread_main_params *extended_main_params;
  int status = NOTHREAD;
  pthread_t thread;
  char *task_name;

  if (thread_pool->shutdown) return 0;

  /*
    Protect structure against concurrent accesses
  */

  if (thread_pool->size < thread_pool->core_pool_size) {
    // Always create a thread as long as there are less then
    // core_pool_size threads created.

    status = CORETHREAD;
    thread_pool->size++;
    extended_main_params =
        (pool_thread_main_params *)malloc(sizeof(pool_thread_main_params));
    extended_main_params->is_core = status;
    extended_main_params->params = main_params;
    extended_main_params->id = thread_pool->size;

    // Create a thread to execute the main function passed as parameters

    asprintf(&task_name, "core %02d", extended_main_params->id);
    set_task_name(extended_main_params->id, task_name);
  } else if ((force) && (thread_pool->size < thread_pool->max_pool_size)) {
    status = TEMPTHREAD;
    thread_pool->size++;
    extended_main_params =
        (pool_thread_main_params *)malloc(sizeof(pool_thread_main_params));
    extended_main_params->is_core = status;
    extended_main_params->params = main_params;
    extended_main_params->id = thread_pool->size;
    asprintf(&task_name, "temp %02d", extended_main_params->id);
    set_task_name(extended_main_params->id, task_name);
  }

  // Do not protect the structure against concurrent accesses anymore

  if (status != NOTHREAD)
    mtxprintf(pt_debug, "thread created\n");
  return (status != NOTHREAD);
}

void thread_pool_shutdown(thread_pool_t *thread_pool) {
  thread_pool->shutdown = 1;
}

/*
  Decrease thread pool size and broadcast update.
*/
void pool_thread_terminate(thread_pool_t *thread_pool) {
  /*
    Protect against concurrent accesses. Broadcast the update.
  */
}

/*
  Wait for pool size to be empty.
*/
void wait_thread_pool_empty(thread_pool_t *thread_pool) {
  /*
    Wait for thread pool size to be equal zero. Protect section against
    concurrent access
  */
  if (thread_pool->size != 0)
    mtxprintf(pt_debug, "pool not empty, exit process\n");
}

int get_shutdown(thread_pool_t *thread_pool) {
  return thread_pool->shutdown; 
}
