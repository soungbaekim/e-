#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "status.h"

#ifdef DEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif


struct status_head *status;

/* Initializing status - should be called once in the begining */
void status_init () {
  DEBUG_PRINT (("initializing status...\n"));
  /* Allocate memory for status head */
  status = malloc (sizeof (struct status_head));
  if (status == NULL) {
    printf ("malloc failed\n");
    return;
  }

  /* Initialize status_lock */
  int err = pthread_mutex_init (&status->status_lock, NULL);
  if (err != 0) {
    printf ("status_lock init failed\n");
    return;
  }

  status->start = NULL;
  status->ref_count = 0;
  status->cleanup = false;

  /* Init sema */
  status->wait = sem_open ("wait_sema", O_CREAT, 0644, 0);
  if (status->wait == SEM_FAILED) {
    printf ("wait_sema init failed\n");
  }

  DEBUG_PRINT (("... complete\n"));
}

/* Return the corresponding item struct for the given uuid */
struct status_elem *status_get (char *uuid) {
  DEBUG_PRINT (("status_get: %s\n", uuid));
  /* Acquire lock for search */
  pthread_mutex_lock (&status->status_lock);

  struct status_elem *cur = status->start;
  struct status_elem *prev = NULL;
  /* Iterate through the linked list to find item */
  while (cur != NULL) {
    prev = cur;

    if (strcmp (cur->uuid, uuid) == 0) {
      DEBUG_PRINT (("found: %s\n", uuid));
      goto done;
    }
    cur = cur->next;
  }
  DEBUG_PRINT (("not found: %s\n", uuid));
  /* Add a new elem for this uuid */
  cur = malloc (sizeof (struct status_elem));
  pthread_mutex_init (&cur->item_lock, NULL);

  cur->uuid = malloc (strlen(uuid)+1);
  strcpy (cur->uuid, uuid);

  cur->data = NULL;
  cur->next = NULL;

  /* Add to list */
  if (prev == NULL) {
    status->start = cur;
  } else {
    prev->next = cur;
  }

  done:
    /* Release lock */
    pthread_mutex_unlock (&status->status_lock);
    return cur;
}

/* Increments the ref_count by one */
void status_inc () {
  pthread_mutex_lock (&status->status_lock);
  status->ref_count++;
  DEBUG_PRINT (("inc: %d\n", status->ref_count));
  pthread_mutex_unlock (&status->status_lock);
}

/* Decrements the ref_count by one */
void status_dec () {
  pthread_mutex_lock (&status->status_lock);
  status->ref_count--;
  DEBUG_PRINT (("dec: %d\n", status->ref_count));
  if (status->ref_count == 0 && status->cleanup) {
    sem_post (status->wait);
  }
  pthread_mutex_unlock (&status->status_lock);
}

/* Free and destory allocated memory */
void status_cleanup () {
  DEBUG_PRINT (("cleaning status...\n"));
  /* Wait for all processes */
  pthread_mutex_lock (&status->status_lock);
  DEBUG_PRINT (("status_cleanup: %d\n", status->ref_count));
  if (status->ref_count != 0) {
    DEBUG_PRINT (("status waiting...\n"));
    status->cleanup = true;
    pthread_mutex_unlock (&status->status_lock);
    sem_wait (status->wait);
    DEBUG_PRINT (("status continuing...\n"));
  }

  /* Free each elem */
  struct status_elem *cur = status->start;
  struct status_elem *next;

  while (cur != NULL) {
    next = cur->next;
    pthread_mutex_destroy (&cur->item_lock);
    free (cur->uuid);
    free (cur->data);
    free (cur);
    cur = next;
  }

  /* Free head */
  pthread_mutex_destroy (&status->status_lock);
  sem_close (status->wait);
  free (status);
  DEBUG_PRINT (("... complete\n"));
}
