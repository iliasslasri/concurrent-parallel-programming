
#include "circular_buffer.h"
#include "protected_buffer.h"
#include "utils.h"
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Initialise the protected buffer structure above.
protected_buffer_t *cond_protected_buffer_init(int length) {
  protected_buffer_t *b;
  b = (protected_buffer_t *)malloc(sizeof(protected_buffer_t));
  b->buffer = circular_buffer_init(length);
  
  // Initialize the mutexes
  b->mutex_prod = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(b->mutex_prod, NULL);
  b->mutex_cons = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(b->mutex_cons, NULL);

  // Initialize the condition variables
  b->full = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
  pthread_cond_init(b->full, NULL);
  b->empty = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
  pthread_cond_init(b->empty, NULL);

  return b;
}

// Extract an element from buffer. If the attempted operation is
// not possible immedidately, the method call blocks until it is.
void *cond_protected_buffer_get(protected_buffer_t *b) {
  void *d;

  // Enter mutual exclusion
  pthread_mutex_lock(b->mutex_cons);
  // Wait until there is a full slot to get data from the unprotected
  // circular buffer (circular_buffer_get).
  if(circular_buffer_size(b->buffer) == 0) {
    pthread_cond_wait(b->empty, b->mutex_cons);
  }
  // Signal or broadcast that an empty slot is available in the
  // unprotected circular buffer (if needed)
  if (circular_buffer_size(b->buffer) == b->buffer->max_size) {
    d = circular_buffer_get(b->buffer);
    pthread_cond_signal(b->full);
  } else {
    d = circular_buffer_get(b->buffer);
  }

  if (d == NULL)
    mtxprintf(pb_debug, "get (B) - data=NULL\n");
  else
    mtxprintf(pb_debug, "get (B) - data=%d\n", *(int *)d);

  // Leave mutual exclusion
  pthread_mutex_unlock(b->mutex_cons);
  return d;
}

// Insert an element into buffer. If the attempted operation is
// not possible immedidately, the method call blocks until it is.
void cond_protected_buffer_put(protected_buffer_t *b, void *d) {

  // Enter mutual exclusion
  pthread_mutex_lock(b->mutex_prod);
  // Wait until there is an empty slot to put data in the unprotected
  // circular buffer (circular_buffer_put).
  if(circular_buffer_size(b->buffer) == b->buffer->max_size) {
    pthread_cond_wait(b->full, b->mutex_prod);
  }
  // Signal or broadcast that a full slot is available in the
  // unprotected circular buffer (if needed)
  if(circular_buffer_size(b->buffer) == 0) {
    circular_buffer_put(b->buffer, d);
    pthread_cond_signal(b->empty);
  } else {
    circular_buffer_put(b->buffer, d);
  }
  if (d == NULL)
    mtxprintf(pb_debug, "put (B) - data=NULL\n");
  else
    mtxprintf(pb_debug, "put (B) - data=%d\n", *(int *)d);

    // Leave mutual exclusion
  pthread_mutex_unlock(b->mutex_prod);
}

// Extract an element from buffer. If the attempted operation is not
// possible immedidately, return NULL. Otherwise, return the element.
void *cond_protected_buffer_remove(protected_buffer_t *b) {
  void *d;

  // Enter mutual exclusion
  pthread_mutex_lock(b->mutex_cons);
  // Signal or broadcast that an empty slot is available in the
  // unprotected circular buffer (if needed)

  d = circular_buffer_get(b->buffer);
  if (d == NULL)
    mtxprintf(pb_debug, "remove (U) - data=NULL\n");
  else
    mtxprintf(pb_debug, "remove (U) - data=%d\n", *(int *)d);

  // Leave mutual exclusion
  pthread_mutex_unlock(b->mutex_cons);
  return d;
}

// Insert an element into buffer. If the attempted operation is
// not possible immedidately, return 0. Otherwise, return 1.
int cond_protected_buffer_add(protected_buffer_t *b, void *d) {
  int done;

  // Enter mutual exclusion
  pthread_mutex_lock(b->mutex_prod);
  // Signal or broadcast that a full slot is available in the
  // unprotected circular buffer (if needed)

  done = circular_buffer_put(b->buffer, d);
  if (!done)
    d = NULL;

  if (d == NULL)
    mtxprintf(pb_debug, "add (U) - data=NULL\n");
  else
    mtxprintf(pb_debug, "add (U) - data=%d\n", *(int *)d);
  pthread_mutex_unlock(b->mutex_prod);
  // Leave mutual exclusion
  return done;
}

// Extract an element from buffer. If the attempted operation is not
// possible immedidately, the method call blocks until it is, but
// waits no longer than the given timeout. Return the element if
// successful. Otherwise, return NULL.
void *cond_protected_buffer_poll(protected_buffer_t *b,
                                 struct timespec *abstime) {
  void *d = NULL;
  int rc = 0;

  // Enter mutual exclusion
  pthread_mutex_lock(b->mutex_cons);
  // Wait until there is a full slot to get data from the unprotected
  // circular buffer  (circular_buffer_get) but waits no longer than
  // the given timeout.
  if (circular_buffer_size(b->buffer) == 0) 
    pthread_cond_timedwait(b->empty, b->mutex_cons, abstime);

  // Signal or broadcast that an empty slot is available in the
  // unprotected circular buffer (if needed)
  if (circular_buffer_size(b->buffer) == b->buffer->max_size) {
    d = circular_buffer_get(b->buffer);
    pthread_cond_signal(b->full);
  } else {
    d = circular_buffer_get(b->buffer);
  }

  if (d == NULL)
    mtxprintf(pb_debug, "poll (T) - data=NULL\n");
  else
    mtxprintf(pb_debug, "poll (T) - data=%d\n", *(int *)d);

    // Leave mutual exclusion
  pthread_mutex_unlock(b->mutex_cons);
  return d;
}

// Insert an element into buffer. If the attempted operation is not
// possible immedidately, the method call blocks until it is, but
// waits no longer than the given timeout. Return 0 if not
// successful. Otherwise, return 1.
int cond_protected_buffer_offer(protected_buffer_t *b, void *d,
                                struct timespec *abstime) {
  int rc = 0;
  int done = 0;

  // Enter mutual exclusion
  pthread_mutex_lock(b->mutex_prod);

  // Wait until there is an empty slot to put data in the unprotected
  // circular buffer (circular_buffer_put) but waits no longer than
  // the given timeout.
  if (circular_buffer_size(b->buffer) == b->buffer->max_size) 
    pthread_cond_timedwait(b->full, b->mutex_prod, abstime);

  // Signal or broadcast that a full slot is available in the
  // unprotected circular buffer (if needed)
  if (circular_buffer_size(b->buffer) == 0) {
    done = circular_buffer_put(b->buffer, d);
    pthread_cond_signal(b->empty);
  } else {
    done = circular_buffer_put(b->buffer, d);
  }
  
  if (!done)
    d = NULL;

  if (d == NULL)
    mtxprintf(pb_debug, "offer (T) - data=NULL\n");
  else
    mtxprintf(pb_debug, "offer (T) - data=%d\n", *(int *)d);

    // Leave mutual exclusion
  pthread_mutex_unlock(b->mutex_prod);
  return done;
}
