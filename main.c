/*
Write a multi-threaded C program that gives readers priority over writers
concerning a shared (global) variable.
Essentially, if any readers are waiting, then they have priority over writer threads --
writers can only write when there are no readers. This program should adhere to the
following constraints:

- Multiple readers/writers must be supported (5 of each is fine)
- Readers must read the shared variable X number of times
- Writers must write the shared variable X number of times
- Readers must print:
  - The value read
  - The number of readers present when value is read
- Writers must print:
  - The written value
  - The number of readers present were when value is written (should be 0)
- Before a reader/writer attempts to access the shared variable it should wait some random amount of time
  - Note: This will help ensure that reads and writes do not occur all at once
- Use pthreads, mutexes, and condition variables to synchronize access to the shared variable
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N_READERS 5 /* size of shared buffer */
#define N_WRITERS 5 /* size of shared buffer */

unsigned int shared_x = 0; /* shared variable x */
int waiting_readers = 0;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_reader = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_writer = PTHREAD_COND_INITIALIZER;

void *writer (void *param);
void *reader (void *param);

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
  // The value read
  // The number of readers present when value is read
}

void *writer_process (void *thread_argument) {
  // The written value
  // The number of readers present were when value is written (should be 0)
}
