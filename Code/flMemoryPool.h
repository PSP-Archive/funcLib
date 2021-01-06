// Funclib Memory Pool v1.0.0
// 
// This module contains functions for memory pooling.
// 
// Contributor(s): Flatmush.



#ifndef FLMEMORYPOOL_H
#define FLMEMORYPOOL_H

#include "flGlobal.h"
#include "flMemory.h"
#include "flMemoryVMem.h"

#define MEMORY_POOL_TYPE_NONE     0
#define MEMORY_POOL_TYPE_TABLE    1
#define MEMORY_POOL_TYPE_LINKLIST 2

#if FL_MEMORY_POOL != 0

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
     void* mempData;
     u32   mempSize;
     u32   mempType;
     void* mempAllocData;
} MemoryPool;

typedef struct {
     void* mempHLLData;
     u32   mempHLLSize;
     void* mempHLLNext;
     void* mempHLLPrev;
} MemoryPoolHdrLL;

extern MemoryPool* mempCreate(u32 inSize, u32 inType);
extern void        mempDestroy(MemoryPool* inMemPool);

extern void*       mempAlloc(MemoryPool* inMemPool, u32 inSize);
extern void*       mempCalloc(MemoryPool* inMemPool, u32 inSize0, u32 inSize1);
extern void*       mempRealloc(MemoryPool* inMemPool, void* inPtr, u32 inSize);
extern void*       mempAlign(MemoryPool* inMemPool, u32 inBoundry, u32 inSize);
extern void        mempFree(MemoryPool* inMemPool, void* inPtr);

#ifdef __cplusplus
}
#endif

#endif

#endif
