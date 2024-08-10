#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define BUFFER_SIZE 10  // Define the size of the buffers
#define NUM_ITEMS 20    // Define the number of items each producer will produce

int buffer1[BUFFER_SIZE];  // Buffer 1
int buffer1_count = 0;     // Count of items in buffer 1
int buffer1_in = 0;        // Next write position in buffer 1
int buffer1_out = 0;       // Next read position in buffer 1
pthread_mutex_t buffer1_mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex for buffer 1
sem_t buffer1_full;        // Semaphore to track full slots in buffer 1
sem_t buffer1_empty;       // Semaphore to track empty slots in buffer 1

int buffer2[BUFFER_SIZE];  // Buffer 2
int buffer2_count = 0;     // Count of items in buffer 2
int buffer2_in = 0;        // Next write position in buffer 2
int buffer2_out = 0;       // Next read position in buffer 2
pthread_mutex_t buffer2_mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex for buffer 2
sem_t buffer2_full;        // Semaphore to track full slots in buffer 2
sem_t buffer2_empty;       // Semaphore to track empty slots in buffer 2

// Function to produce an item into a buffer
void produce(int* buffer, int* buffer_count, int* buffer_in, pthread_mutex_t* buffer_mutex, sem_t* buffer_full, sem_t* buffer_empty, int item) {
    sem_wait(buffer_empty);  // Wait if the buffer is full
    pthread_mutex_lock(buffer_mutex);  // Lock the buffer
    
    // Add the item to the buffer and update the index
    buffer[*buffer_in] = item;
    *buffer_in = (*buffer_in + 1) % BUFFER_SIZE;
    (*buffer_count)++;
    
    pthread_mutex_unlock(buffer_mutex);  // Unlock the buffer
    sem_post(buffer_full);  // Signal that the buffer now has a new item
}

// Function to consume an item from a buffer
int consume(int* buffer, int* buffer_count, int* buffer_out, pthread_mutex_t* buffer_mutex, sem_t* buffer_full, sem_t* buffer_empty) {
    sem_wait(buffer_full);  // Wait if the buffer is empty
    pthread_mutex_lock(buffer_mutex);  // Lock the buffer
    
    // Remove the item from the buffer and update the index
    int item = buffer[*buffer_out];
    *buffer_out = (*buffer_out + 1) % BUFFER_SIZE;
    (*buffer_count)--;
    
    pthread_mutex_unlock(buffer_mutex);  // Unlock the buffer
    sem_post(buffer_empty);  // Signal that the buffer now has a free slot
    return item;
}

// Producer thread function
void* producer(void* arg) {
    int producer_id = *(int*)arg;  // Get the producer ID
    
    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = producer_id * 100 + i;  // Generate an item
        produce(buffer1, &buffer1_count, &buffer1_in, &buffer1_mutex, &buffer1_full, &buffer1_empty, item);  // Produce the item into buffer 1
        printf("Producer ID: %d, Produced: %d into Buffer 1\n", producer_id, item);
        usleep((rand() % 500) * 1000);  // Sleep for a random time
    }
    return NULL;
}

// First-level consumer and producer thread function
void* first_level_consumer_producer(void* arg) {
    int consumer_producer_id = *(int*)arg;  // Get the ID
    
    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = consume(buffer1, &buffer1_count, &buffer1_out, &buffer1_mutex, &buffer1_full, &buffer1_empty);  // Consume an item from buffer 1
        printf("Consumer ID: %d , Consumed: %d from Buffer 1\n", consumer_producer_id, item);
        int new_item = item * 10 + consumer_producer_id;  // Process the item and produce a new item
        produce(buffer2, &buffer2_count, &buffer2_in, &buffer2_mutex, &buffer2_full, &buffer2_empty, new_item);  // Produce the new item into buffer 2
        printf("Producer ID: %d , Produced: %d into Buffer 2\n", consumer_producer_id, new_item);
        usleep((rand() % 500) * 1000);  // Sleep for a random time
    }
    return NULL;
}

// Second-level consumer thread function
void* second_level_consumer(void* arg) {
    int consumer_id = *(int*)arg;  // Get the consumer ID
    
    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = consume(buffer2, &buffer2_count, &buffer2_out, &buffer2_mutex, &buffer2_full, &buffer2_empty);  // Consume an item from buffer 2
        printf("Consumer_ID: %d ,Consumed: %d from Buffer 2\n", consumer_id, item);
        usleep((rand() % 500) * 1000);  // Sleep for a random time
    }
    return NULL;
}

int main() {
    srand(time(NULL));  // Seed the random number generator
    
    pthread_t producer_threads[2];  // Declare threads for 2 producers
    pthread_t consumer_producer_threads[2];  // Declare threads for 2 first-level consumer/producers
    pthread_t consumer_threads[2];  // Declare threads for 2 second-level consumers
    
    int ids[2] = {1, 2};  // IDs for the threads
    
    // Initialize semaphores
    sem_init(&buffer1_full, 0, 0);  // Initialize buffer 1 full semaphore to 0
    sem_init(&buffer1_empty, 0, BUFFER_SIZE);  // Initialize buffer 1 empty semaphore to BUFFER_SIZE
    sem_init(&buffer2_full, 0, 0);  // Initialize buffer 2 full semaphore to 0
    sem_init(&buffer2_empty, 0, BUFFER_SIZE);  // Initialize buffer 2 empty semaphore to BUFFER_SIZE
    
    // Create producer threads
    for (int i = 0; i < 2; i++) {
        pthread_create(&producer_threads[i], NULL, producer, &ids[i]);
    }
    
    // Create first-level consumer/producer threads
    for (int i = 0; i < 2; i++) {
        pthread_create(&consumer_producer_threads[i], NULL, first_level_consumer_producer, &ids[i]);
    }
    
    // Create second-level consumer threads
    for (int i = 0; i < 2; i++) {
        pthread_create(&consumer_threads[i], NULL, second_level_consumer, &ids[i]);
    }
    
    // Join producer threads
    for (int i = 0; i < 2; i++) {
        pthread_join(producer_threads[i], NULL);
    }
    
    // Join first-level consumer/producer threads
    for (int i = 0; i < 2; i++) {
        pthread_join(consumer_producer_threads[i], NULL);
    }
    
    // Join second-level consumer threads
    for (int i = 0; i < 2; i++) {
        pthread_join(consumer_threads[i], NULL);
    }
    
    // Destroy semaphores
    sem_destroy(&buffer1_full);
    sem_destroy(&buffer1_empty);
    sem_destroy(&buffer2_full);
    sem_destroy(&buffer2_empty);
    
    printf("All producers and consumers have finished.\n");

    return 0;
}
