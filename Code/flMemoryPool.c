#include "flGlobal.h"

#if FL_MEMORY_POOL
#include <string.h>
#include <stdio.h>
#include <pspkernel.h>

#if !FL_INCLUDE_ALL_C
#include "flMemoryPool.h"
#include "flDebug.h"
#endif

MemoryPool* mempCreate(u32 inSize, u32 inType) {
     if(!inSize) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Cannot create memory pool.\nTrying to create a memory pool of size 0.");
          #endif
          return NULL;
     }
     if(!inType || (inType > 2)) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Cannot create memory pool.\nTrying to create a memory pool with an invalid type parameter.");
          #endif
          return NULL;
     }
     MemoryPool* tempOut = (MemoryPool*)memAlloc(inSize + sizeof(MemoryPool));
     if(!tempOut) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Cannot allocate memory pool.\nProbably not enough free memory.");
          #endif
          return NULL;
     }
     while(inSize & 15)
          inSize++;
     tempOut->mempData = (void*)((u32)tempOut + sizeof(MemoryPool));
     tempOut->mempSize = inSize;
     tempOut->mempType = inType;
     tempOut->mempAllocData = NULL;
     
     return tempOut;
}

void mempDestroy(MemoryPool* inMemPool) {
     if(!inMemPool) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to destroy NULL memory pool.");
          #endif
          return;
     }
     if(inMemPool->mempType == MEMORY_POOL_TYPE_TABLE)
          memFree(inMemPool->mempAllocData);
     memFree(inMemPool);
}



void* mempAlloc(MemoryPool* inMemPool, u32 inSize) {
     if(!inMemPool) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to allocate from NULL memory pool.");
          #endif
          return NULL;
     }
     if(!inSize) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to allocate 0 bytes from memory pool.");
          #endif
          return NULL;
     }
     if(inMemPool->mempType == MEMORY_POOL_TYPE_LINKLIST) {
          MemoryPoolHdrLL* tempHeader = NULL;
          MemoryPoolHdrLL* tempPrevHeader = NULL;
          if(!inMemPool->mempAllocData) {
               inMemPool->mempAllocData = inMemPool->mempData;
               tempHeader = (MemoryPoolHdrLL*)inMemPool->mempAllocData;
          } else {
               tempPrevHeader = (MemoryPoolHdrLL*)inMemPool->mempAllocData;
               while(tempPrevHeader->mempHLLNext != NULL)
                    tempPrevHeader = (MemoryPoolHdrLL*)tempPrevHeader->mempHLLNext;
               tempHeader = (MemoryPoolHdrLL*)((u32)tempPrevHeader + sizeof(MemoryPoolHdrLL) + tempPrevHeader->mempHLLSize);
               tempPrevHeader->mempHLLNext = (void*)tempHeader;
          }
          tempHeader->mempHLLData = (void*)((u32)tempHeader + sizeof(MemoryPoolHdrLL));
          tempHeader->mempHLLSize = inSize;
          tempHeader->mempHLLNext = NULL;
          tempHeader->mempHLLPrev = (void*)tempPrevHeader;
          
          return tempHeader->mempHLLData;
     }
     
     #if FL_DEBUG_WARNING != 0
     debugWarning("Trying to allocate from memory pool with an invalid type.");
     #endif
     return NULL;
}

void* mempCalloc(MemoryPool* inMemPool, u32 inSize0, u32 inSize1) {
     void* tempOut = mempAlloc(inMemPool, (inSize0 * inSize1));
     if(!tempOut)
          return NULL;
     memClear(tempOut, (inSize0 * inSize1));
     return tempOut;
}

void* mempRealloc(MemoryPool* inMemPool, void* inPtr, u32 inSize) {
     return NULL;
}

void* mempAlign(MemoryPool* inMemPool, u32 inBoundry, u32 inSize) {
     return NULL;
}

void mempFree(MemoryPool* inMemPool, void* inPtr) {

}

#endif
