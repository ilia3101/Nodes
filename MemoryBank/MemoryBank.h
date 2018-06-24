/* A perfect, thread safe and uncrashable memory keeping system, as long as you
 * use it's functions to work with the memory it provides. */

#ifndef _MemoryBank_h_
#define _MemoryBank_h_

#include "MemoryBank_struct.h"


/* Creat a new memory bank, UseMutex is a true/false (for thread safe) */
MemoryBank_t * new_MemoryBank(int UseMutex);

/* No more memory leaks 💓💓💓️ */
void delete_MemoryBank(MemoryBank_t * Bank);

/* Allocate some memory with a record of it in the bank */
void * MBMalloc(MemoryBank_t * Bank, size_t AllocSize);

/* Allocate + fill with 0s, calloc equivalent but no stupid size1 and size2 */
void * MBZeroAlloc(MemoryBank_t * Bank, size_t AllocSize);

/* Memory must belong to the memory bank */
void * MBRealloc(MemoryBank_t * Bank, void * Mem, size_t NewSize);

/* Free a memory created by the memory bank */
void MBFree(MemoryBank_t * Bank, void * Mem);



/************************* END, private functions now *************************/

void * mb_malloc(MemoryBank_t * Bank, size_t AllocSize);


#endif