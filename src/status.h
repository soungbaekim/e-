#include <pthread.h>
#include <fcntl.h>           /* For O_* constants */
#include <semaphore.h>
#include <stdbool.h>

struct status_elem {
  char *uuid;
  char *data;

  struct status_elem *next;

  pthread_mutex_t item_lock;
};


struct status_head {
  struct status_elem *start;

  //bool cleanup;
  //int ref_count;
  //sem_t *wait;

  pthread_mutex_t status_lock;
};


void status_init ();
struct status_elem *status_get (char *uuid);
//void status_inc ();
//void status_dec ();
void status_cleanup ();
