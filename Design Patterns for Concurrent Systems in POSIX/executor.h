#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <pthread.h>

#include "protected_buffer.h"
#include "thread_pool.h"

#define FOREVER -1

extern int ex_debug;

struct _executor_t;

typedef struct {
  void *params;
  main_func_t main;
  long period;
  struct _executor_t *executor;
} callable_t;

typedef struct {
  int completed;
  callable_t *callable;
  void *result;
  pthread_mutex_t *mutex; /* mutex to protect from concurrent access */
  pthread_cond_t *cond; /* cond var to signal task completed */
} future_t;

extern future_t shutdown_future;

typedef struct _executor_t {
  thread_pool_t *thread_pool;
  long keep_alive_time;
  protected_buffer_t *futures;
  int shutdown;
} executor_t;

// Allocate and initialize executor. Allocate and initialize a thread
// pool. Allocate and initialize a blocking queue to store unhandled
// callables.
executor_t *executor_init(int core_pool_size, int max_pool_size,
                          long keep_alive_time, int callable_array_size);

// Associate a thread from thread pool to callable. Then invoke
// callable. Otherwise, store it in the blocking queue.
future_t *submit_callable(executor_t *executor, callable_t *callable);

// Get result from callable execution. Block if not available.
void *get_callable_result(future_t *future);

// Wait for pool threads to be completed
void executor_shutdown(executor_t *executor);
#endif
