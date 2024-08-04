//Sample code for producer/consumer problem
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define BUFFER_SIZE 10
#define NUM_ITEMS 20

int buffer1[BUFFER_SIZE];
int buffer1_count = 0;
int buffer1_in = 0;
int buffer1_out = 0;
pthread_mutex_t buffer1_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t buffer1_full;
sem_t buffer1_empty;

int buffer2[BUFFER_SIZE];
int buffer2_count = 0;
int buffer2_in = 0;
int buffer2_out = 0;
pthread_mutex_t buffer2_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t buffer2_full;
sem_t buffer2_empty;

void produce(int* buffer, int* buffer_count, int* buffer_in, pthread_mutex_t* buffer_mutex, sem_t* buffer_full, sem_t* buffer_empty, int item) {
    sem_wait(buffer_empty);
    pthread_mutex_lock(buffer_mutex);
    
    buffer[*buffer_in] = item;
    *buffer_in = (*buffer_in + 1) % BUFFER_SIZE;
    (*buffer_count)++;
    
    pthread_mutex_unlock(buffer_mutex);
    sem_post(buffer_full);
}

int consume(int* buffer, int* buffer_count, int* buffer_out, pthread_mutex_t* buffer_mutex, sem_t* buffer_full, sem_t* buffer_empty) {
    sem_wait(buffer_full);
    pthread_mutex_lock(buffer_mutex);
    
    int item = buffer[*buffer_out];
    *buffer_out = (*buffer_out + 1) % BUFFER_SIZE;
    (*buffer_count)--;
    
    pthread_mutex_unlock(buffer_mutex);
    sem_post(buffer_empty);
    return item;
}

void* producer(void* arg) {
    int producer_id = *(int*)arg;
   // printf("The producer ID is %d \n",producer_id);
    for (int i = 0; i < NUM_ITEMS; i++) {
        // printf("The producer ID is %d \n",producer_id);
        int item = producer_id * 100 + i;
        produce(buffer1, &buffer1_count, &buffer1_in, &buffer1_mutex, &buffer1_full, &buffer1_empty, item);
        printf("Producer ID: %d, Produced: %d into Buffer 1\n", producer_id, item);
        usleep((rand() % 500) * 1000);
    }
    return NULL;
}

void* first_level_consumer_producer(void* arg) {
    int consumer_producer_id = *(int*)arg;
    //printf("first level consumer producerID: %d \n",consumer_producer_id);
    for (int i = 0; i < NUM_ITEMS; i++) {
        // printf("first level consumer producer ID: %d \n",consumer_producer_id);
        int item = consume(buffer1, &buffer1_count, &buffer1_out, &buffer1_mutex, &buffer1_full, &buffer1_empty);
        printf("Consumer ID: %d , Consumed: %d from Buffer 1\n", consumer_producer_id, item);
        int new_item = item * 10 + consumer_producer_id;
        produce(buffer2, &buffer2_count, &buffer2_in, &buffer2_mutex, &buffer2_full, &buffer2_empty, new_item);
        printf("Producer ID: %d , Produced: %d into Buffer 2\n", consumer_producer_id, new_item);
        usleep((rand() % 500) * 1000);
    }
    return NULL;
}

void* second_level_consumer(void* arg) {
    int consumer_id = *(int*)arg;
    //printf("Second level consumer ID: %d \n",consumer_id);
    for (int i = 0; i < NUM_ITEMS; i++) {
        // printf("Second level consumer ID: %d \n",consumer_id);
        int item = consume(buffer2, &buffer2_count, &buffer2_out, &buffer2_mutex, &buffer2_full, &buffer2_empty);
        printf("Consumer_ID: %d ,Consumed: %d from Buffer 2\n", consumer_id, item);
        usleep((rand() % 500) * 1000);
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    // Declare threads for all types of producers and consumers
  

    // Declare variables for 2 producers entity, 2 producer/consmuer entity and 2 consumers  
   
    // Initialize semaphores
    
    // create threads
    
    //Join threads
   
    //destroy threads
    
    printf("All producers and consumers have finished.\n");

    return 0;
}