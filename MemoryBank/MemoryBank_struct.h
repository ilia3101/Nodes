#ifndef _MemoryBank_struct_h_
#define _MemoryBank_struct_h_

#include <pthread.h>

/* Everything in the memory bank object is private */
typedef struct {
    int use_mutex; /* Is the mutex being used */
    pthread_mutex_t lock;
    int num_memory_blocks; /* Number of memory blocks */
    int array_size; /* size of memory_blocks array (in elements) */
    void ** memory_blocks; /* All memory blocks */
} MemoryBank_t;

#endif