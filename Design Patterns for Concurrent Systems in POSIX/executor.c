#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>

#include "executor.h"
#include "utils.h"

int ex_debug = 0;

/*
  Main function for threads executing callables.
  arg is of type main_pool_thread_params.
*/
void *pool_thread_main(void *arg);

/*
  Shutdown future helps shuting down threads blocked waiting for incoming
  callables. Use in the last questions.
*/
future_t shutdown_future;
void set_shutdown(future_t *future) { future->callable = NULL; };

int is_shutdown(future_t *future) {
  return ((future == NULL) || (future->callable == NULL));
};

/*
  Allocate and initialize executor. First, allocate and initialize a
  thread pool. Second, allocate and initialize a blocking queue to
  store pending callables.
*/
executor_t *executor_init(int core_pool_size, int max_pool_size,
                          long keep_alive_time, int callable_array_size) {
  executor_t *executor;
  executor = (executor_t *)malloc(sizeof(executor_t));

  executor->shutdown = 0;
  executor->keep_alive_time = keep_alive_time;
  executor->thread_pool = thread_pool_init(core_pool_size, max_pool_size);
  /*
    Create a protected buffer for futures. Use the implementation
    based on cond variables (first parameter sem_impl set to false).
  */
  executor->futures = protected_buffer_init(0, callable_array_size);
  set_shutdown(&shutdown_future);
  return executor;
}

/*
  Associate a thread from thread pool to a callable. Then invoke
  the callable. Otherwise, store it in the blocking queue.
*/
future_t *submit_callable(executor_t *executor, callable_t *callable) {
  future_t *future = (future_t *)malloc(sizeof(future_t));
  callable->executor = executor;
  future->callable = callable;
  future->completed = 0;

  /*
    Future must include synchronisation objects to block threads until the
    result of the callable computation becames available. See
    function get_callable_result.
  */

  /*
    Try to create a thread, but do not exceed core_pool_size
    (last parameter force set to false).
  */
  if (pool_thread_create(executor->thread_pool, pool_thread_main, future, 0))
    return future;

    /*
      When there are already enough created threads, queue the callable
      in the blocking queue.
    */

  /*
    When the queue is full, pop the first future from the queue and
    push the current one. We do that to preserve the queuing order.
  */
  future_t *first = protected_buffer_remove(executor->futures);
  if (first != NULL) {
    protected_buffer_add(executor->futures, future);
    future = first;
  }

  /*
    Try to create a thread, but allow to exceed core_pool_size (last
    parameter set to true).
  */

  /*
    We failed. The executor is overrun.
  */
  return NULL;
}

/*
  Get result from callable execution. Block if not available.
*/
void *get_callable_result(future_t *future) {
  void *result;

  /*
    Protect against concurrent accesses. Block until the callable has
    completed.
  */

  result = (void *)future->result;

  /*
    Unprotect against concurrent accesses
  */

  /*
    Do not bother to deallocate future
  */
  return result;
}

/*
  Define main procedure that executes callables. The arg parameter
  provides the first future object to be executed. Once it is
  executed, the main procedure picks a pending callable from the
  executor blocking queue.
*/
void *pool_thread_main(void *arg) {
  callable_t *callable;
  executor_t *executor;
  struct timespec ts_deadline;
  struct timeval tv_deadline;
  bool terminate = false;

  /*
   Get the status of the thread (core or temp) and the first future
  */
  pool_thread_main_params *extended_main_params =
      (pool_thread_main_params *)arg;
  int is_core = extended_main_params->is_core;
  future_t *future = (future_t *)extended_main_params->params;

  set_current_task_id(&extended_main_params->id);

  /*
    Compute the starting time for periodic threads
  */
  gettimeofday(&tv_deadline, NULL);
  TIMEVAL_TO_TIMESPEC(&tv_deadline, &ts_deadline);

  /*
    As long as the thread does not shutdown (terminate == true), it handles a
    future. It handles the first future passed in arg parameter. Once the
    callable has been executed possibly picks other ones.
  */
  while (!terminate) {
    callable = (callable_t *)future->callable;
    executor = (executor_t *)callable->executor;

    /*
      The thread is not periodic.
    */
    if (callable->period == 0) {
      future->result = callable->main(callable->params);

      /*
        As the callable has been executed, the future attribute completed and
        its synchronisation objects should be updated to resume threads waiting
        for the result.
      */

      /*
       For sanity reasons, initialize the next future to shutdown future in
       order for the system to "work" while not being fully implemented
      */
      future = &shutdown_future;

      /*
        Get the next future as the current one is not periodic. Two cases :
      */

      /*
       If the thread belongs to the core pool or if the executor does not
       deallocate pool threads (keep alive forever), wait for the next
       available future.
      */
      if (is_core || executor->keep_alive_time == FOREVER) {

        /*
          Check future is a shutdown future. If so, terminate current thread and
          add another shutdown future to the queue to unblock another thread
          and force it to terminate.
        */
        if (is_shutdown(future)) {
          pool_thread_terminate(executor->thread_pool);
          terminate = true;
          protected_buffer_add(executor->futures, (void *)&shutdown_future);
        }
      }
      /*
        The executor is configured to release a thread when it is
        idle for keep_alive_time milliseconds. This thread is not core
        since core threads are not deallocated.
      */
      else {
        /*
          Get a new future during at most keep_alive_time ms.
          Remember that POSIX delays are absolute delays.
        */
        /*
          There is no callable to handle after timeout, remove the current
          pool thread from the pool. If it is successful, terminate thread.
        */
        if (future == NULL) {
          pool_thread_terminate(executor->thread_pool);
          terminate = true;
        }
        /*
         If it is a shutdown future, terminate current thread and add another
         shutdown future to the queue to unblock another thread and force it to
         terminate. Then, terminate threat.
        */
        else if (is_shutdown(future)) {
          protected_buffer_add(executor->futures, (void *)&shutdown_future);
          pool_thread_terminate(executor->thread_pool);
          terminate = true;
        }
      }
    } else {
      future->result = callable->main(callable->params);

      /*
        Wait for the next release time. Implement withFixedRate semantics.
        Note that current ts_deadline represents the start time of the
        current job and the deadline of the previous job.
      */
      /*
        Loop as long as there is no shutdown.
      */
      if (get_shutdown(executor->thread_pool)) {
        pool_thread_terminate(executor->thread_pool);
        terminate = true;
      };
    }
  }
  mtxprintf(pt_debug, "terminated\n");
  return NULL;
}

/*
  Initiates an orderly shutdown in which previously submitted tasks are
  executed, but no new tasks will be accepted. Invocation has no additional
  effect if already shut down. This method waits for previously submitted tasks
  to complete execution.
*/
void executor_shutdown(executor_t *executor) {
  if (executor->shutdown)
    return;
  else
    executor->shutdown = 1;

  mtxprintf(ex_debug, "executor shutdown activated\n");
  thread_pool_shutdown(executor->thread_pool);
  /*
    Add in future queue a shutdown future to unblock blocked threads and force
    their termination.
  */
  /*
    Wait for all threads to be deallocated.
  */
  wait_thread_pool_empty(executor->thread_pool);
  mtxprintf(ex_debug, "executor shutdown terminated\n");
}
