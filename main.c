#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N_READERS 5 /* size of shared buffer */
#define N_WRITERS 5 /* size of shared buffer */
#define SLEEP_TIME 1000000 /* size of shared buffer */

unsigned int shared_x = 0; /* shared variable x */
int g_waiting_readers = 0;
int g_readers = 0;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_reader = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_writer = PTHREAD_COND_INITIALIZER;

void *writer_process (void *param);
void *reader_process (void *param);

int main(int argc, char *argv[]) {

  int i;

  int reader_numbers[N_READERS];
  int writer_numbers[N_WRITERS];

  pthread_t reader_thread_ids[N_READERS]; /* thread identifiers */
  pthread_t writer_thread_ids[N_WRITERS]; /* thread identifiers */

  // Start the readers
  for (i = 0; i < N_READERS; i++) {
    reader_numbers[i] = i;
    pthread_create(&reader_thread_ids[i], NULL, reader_process, &reader_numbers[i]);
  }

  // Start the writers
  for (i = 0; i < N_WRITERS; i++) {
    writer_numbers[i] = i;
    pthread_create(&writer_thread_ids[i], NULL, writer_process, &writer_numbers[i]);
  }

  // Wait for the readers to finish
  for (i = 0; i < N_READERS; i++) {
    pthread_join(reader_thread_ids[i], NULL);
  }

  // Wait for the writers to finish
  for (i = 0; i < N_WRITERS; i++) {
    pthread_join(writer_thread_ids[i], NULL);    
  }

  printf("Successfully completed.\n");
  return 0;
}

void *reader_process (void *thread_argument) {
  int id = *((int*)thread_argument);
  int i = 0;
  int num_readers = 0;

  for (i = 0; i < N_READERS; i++) {
    usleep(SLEEP_TIME); // Sleep 1 seconds

    // Critical section. Wait if writers are working.
    pthread_mutex_lock(&m);
      g_waiting_readers++;
      while (g_readers == -1) {
        pthread_cond_wait(&cond_reader, &m);
      }
      g_waiting_readers--;
      num_readers = ++g_readers;
    pthread_mutex_unlock(&m);

    // Read
    fprintf(stdout, "[r%d] reading %u  [readers: %2d]\n", id, shared_x, num_readers);

    // Send signal to writer if no readers are waiting
    pthread_mutex_lock(&m);
      g_readers--;
      if (g_readers == 0) {
        pthread_cond_signal(&cond_writer);
      }
    pthread_mutex_unlock(&m);
  }

  pthread_exit(0);
}

void *writer_process (void *thread_argument) {

  int id = *((int*)thread_argument);
  int i = 0;
  int num_readers = 0;

  for(i = 0; i < N_WRITERS; i++) {
    usleep(SLEEP_TIME); // Sleep 1 seconds

  // Critical section. Wait if readers are working.
    pthread_mutex_lock(&m);
      while (g_readers != 0) {
        pthread_cond_wait(&cond_writer, &m);
      }
      g_readers = -1;
      num_readers = g_readers;
    pthread_mutex_unlock(&m);

    // Write data
    fprintf(stdout, "[w%d] writing %u* [readers: %2d]\n", id, ++shared_x, num_readers);

    // Exit critical section. Broadcast or signal to cond_reader
    pthread_mutex_lock(&m);
      g_readers = 0;
      if (g_waiting_readers > 0) {
        pthread_cond_broadcast(&cond_reader);
      }
      else {
        pthread_cond_signal(&cond_writer);
      }
    pthread_mutex_unlock(&m);
  }

  pthread_exit(0);
}
