 /*
 * Eluvio Coding Challenge
 * Soung Bae Kim
 * UC Berkeley EECS
 * Class of 2021
 */
 #include <stdio.h>
 #include <stddef.h>
 #include <stdlib.h>
 #include <string.h>

 #include <curl/curl.h>
 #include <unistd.h>
 #include <pthread.h>

 #include <fcntl.h>           /* For O_* constants */
 #include <semaphore.h>

 #include "status.h"


 #define UUID_LENGTH 64
 #define MAX_REQUESTS 5


 /* HTTPS Authorization Header */
 struct curl_slist *auth_list = NULL;
 /* URL */
 char *url = "https://challenges.qluv.io/items/";
 /* Semaphore to stay under MAX_REQUESTS */
 sem_t *sema;


 /* Callback fn to retrieve data from request
  * Receives item as userdata because of CURLOPT_WRITEDATA
  */
 static size_t
 write_callback (char *ptr, size_t size, size_t nmemb, void *userdata) {
   struct status_elem *item = (struct status_elem*) userdata;

   item->data = malloc (strlen (ptr) + 1);
   strcpy (item->data, ptr);

   return strlen (ptr);
 }

/* Makes the GET Request using libcurl */
 void execute_curl (struct status_elem *item) {
   printf ("execute_curl\n");

   /* Check semaphore for avaliability */
   int err = sem_wait (sema);
   if (err != 0) {
     printf ("sema wait failed ###########################\n");
   }

   printf ("form url\n");
   /* Form URL */
   char *formed_url = malloc (strlen (url) + strlen (item->uuid) + 1);
   strcpy (formed_url, url);
   strcat (formed_url, item->uuid);


   /* curl */
   CURL *handle;
   CURLcode res;

   /* Init curl handle */
   handle = curl_easy_init ();
   /* Verbose curl protocols */
   curl_easy_setopt (handle, CURLOPT_VERBOSE, (long) 1);
   /* Set Url */
   curl_easy_setopt (handle, CURLOPT_URL, formed_url);
   /* Set Authorization Header */
   curl_easy_setopt (handle, CURLOPT_HTTPHEADER, auth_list);
   /* Set GET */
   curl_easy_setopt (handle, CURLOPT_HTTPGET, (long) 1);
   /* Define our callback to get called when there's data to be written */
   curl_easy_setopt (handle, CURLOPT_WRITEFUNCTION, write_callback);
   /* Set a pointer to our struct to pass to the callback */
   curl_easy_setopt (handle, CURLOPT_WRITEDATA, item);



   /* Perform curl */
   res = curl_easy_perform (handle);
   /* Cleanup handle */
   curl_easy_cleanup (handle);
   /* Release sema */
   sem_post (sema);
   /* Free Memory */
   free (formed_url);
 }

 /* Function for every request thread */
 void *thread_start (void *arg) {
   char *uuid = (char *) arg;
   printf ("thread_start: %s\n", uuid);

   char *ret;
   struct status_elem *item = status_get (uuid);

   pthread_mutex_lock (&item->item_lock);
   if (item->data == NULL) {
     /* curl */
     execute_curl (item);
   }
   ret = item->data;
   pthread_mutex_unlock (&item->item_lock);

   /* Output the data */
   printf ("{UUID: %s -- DATA: %s}\n", uuid, ret);

   /* Thread Exit */
   //status_dec ();
   pthread_exit (NULL);
 }

/*
  Main keeps running to constantly take in more inputs to call the api with
  - Each call to the api creates a new thread
*/
int main (int argc, char **argv) {
  /* Initialize */
  printf ("Initializing...\n");
  /* Init status */
  status_init ();
  /* Init sema */
  sema = sem_open ("/request_sema", O_CREAT, 0644, MAX_REQUESTS);
  if (sema == SEM_FAILED) {
    printf ("sema init failed\n");
  }
  /* Init curl */
  curl_global_init (CURL_GLOBAL_ALL);
  /* HTTPS Authorization Header */
  auth_list = curl_slist_append (auth_list, "Authorization: YTZiNTQzODYtYmRjYS00ZWUxLWJjNjUtZmY2NDM2YmFjNTcx");


  /* Loop */
  pthread_t thread;

  char buf[UUID_LENGTH];
  char *cpy;

  while (scanf ("%s", buf) > 0) {
    /* For exiting */
    if (strcmp (buf, "exit") == 0) {
      goto exit;
    }

    /* Prepare string for new thread */
    cpy = malloc (strlen (buf) + 1);
    strcpy (cpy, buf);


    //status_inc ();

    /* Start new thread on thread_start() */
    pthread_create (&thread, NULL, &thread_start, cpy);
  }

exit:
  /* Exit */
  printf ("Exiting...\n");
  /* Cleanup status */
  status_cleanup ();
  /* Cleanup curl */
  curl_global_cleanup ();
  /* Close sema */
  sem_close (sema);

  return 0;
}
