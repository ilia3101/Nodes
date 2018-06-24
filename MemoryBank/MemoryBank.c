#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "MemoryBank.h"

MemoryBank_t * new_MemoryBank(int UseMutex)
{
    MemoryBank_t * bank = malloc(sizeof(MemoryBank_t));

    /* Start with space for 16 memory blocks */
    int initial_size = 16;

    bank->num_memory_blocks = 0;
    bank->memory_blocks = malloc(initial_size * sizeof(void *));
    bank->array_size = initial_size;

    bank->use_mutex = UseMutex;
    if (UseMutex)
        pthread_mutex_init(&bank->lock, NULL);

    return bank;
}

/* Freed in backwards order 👌 */
void delete_MemoryBank(MemoryBank_t * Bank)
{
    for (int i=Bank->num_memory_blocks; i>0;) free(Bank->memory_blocks[--i]);
    // for (int i=0; i<Bank->num_memory_blocks;) free(Bank->memory_blocks[i++]);
    if (Bank->use_mutex) pthread_mutex_destroy(&Bank->lock);
    free(Bank->memory_blocks);
    free(Bank);
}

#define MemoryBankLock(BANK) \
    if ((BANK)->use_mutex) pthread_mutex_lock(&((BANK)->lock))
#define MemoryBankUnlock(BANK) \
    if ((BANK)->use_mutex) pthread_mutex_unlock(&((BANK)->lock))

void * mb_malloc(MemoryBank_t * Bank, size_t AllocSize)
{
    void * mem = malloc(AllocSize);

    if (Bank->num_memory_blocks >= Bank->array_size)
    {
        /* Reallocate to have enough memory (+8 extra so realloc less often) */
        Bank->array_size = Bank->num_memory_blocks + 8;
        Bank->memory_blocks = realloc( Bank->memory_blocks,
                                       Bank->array_size * sizeof(void *));
    }

    /* Record */
    Bank->memory_blocks[Bank->num_memory_blocks++] = mem;

    return mem;
}

void * MBMalloc(MemoryBank_t * Bank, size_t AllocSize)
{
    MemoryBankLock(Bank);
    void * mem = mb_malloc(Bank, AllocSize);
    MemoryBankUnlock(Bank);

    return mem;
}

void * MBZeroAlloc(MemoryBank_t * Bank, size_t AllocSize)
{
    MemoryBankLock(Bank);
    void * mem = mb_malloc(Bank, AllocSize);
    memset(mem, 0, AllocSize);
    MemoryBankUnlock(Bank);

    return mem;
}

void * MBRealloc(MemoryBank_t * Bank, void * Mem, size_t NewSize)
{
    MemoryBankLock(Bank);

    /* Find where the memory is in the memory bank */
    int index = 0;
    while (index < Bank->num_memory_blocks && Bank->memory_blocks[index] != Mem)
        ++index;

    if (index == Bank->num_memory_blocks)
    {
        /* Memory does not belong to the memory bank, maybe do something */
    }
    else
    {
        Mem = realloc(Mem, NewSize);
        Bank->memory_blocks[index] = Mem;
    }

    MemoryBankUnlock(Bank);

    return Mem;
}

void MBFree(MemoryBank_t * Bank, void * Mem)
{
    MemoryBankLock(Bank);

    /* Find where the memory is in the memory bank */
    int index = 0;
    while (index < Bank->num_memory_blocks && Bank->memory_blocks[index] != Mem)
        ++index;

    if (index != Bank->num_memory_blocks)
    {
        free(Mem);
        --Bank->num_memory_blocks;

        /* Move them all back one space to remove the old bit of memory */
        for (int i = index; i < Bank->num_memory_blocks; ++i)
            Bank->memory_blocks[i] = Bank->memory_blocks[i+1];

        /* Make the memory smaller if needed */
        if (Bank->num_memory_blocks < Bank->array_size-16)
        {
            Bank->array_size = Bank->num_memory_blocks + 8;
            Bank->memory_blocks = realloc( Bank->memory_blocks,
                                           Bank->array_size * sizeof(void *) );
        }
    }

    MemoryBankUnlock(Bank);
}