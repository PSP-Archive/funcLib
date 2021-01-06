// Funclib Memory v1.0.0 (CORE)
// 
// This module contains functions for memory management.
// 
// Contributor(s): Flatmush, idea for memory allocation wrapper came from InsertWittyName.



#ifndef FLMEMORY_H
#define FLMEMORY_H

#include "flGlobal.h"
#include "flMemoryVMem.h"

#define MEMORY_UNCACHED_OFFSET 0x40000000

#define MEMORY_USER_BASE 0x08800000
#define MEMORY_USER_SIZE 0x01800000
#define MEMORY_KERNEL_BASE 0x08000000
#define MEMORY_KERNEL_SIZE 0x00800000
#define MEMORY_SCRATCHPAD_BASE 0x00010000
#define MEMORY_SCRATCHPAD_SIZE 0x00004000
#define MEMORY_PARTITION_KERNEL1_BASE 0x88000000
#define MEMORY_PARTITION_KERNEL1_SIZE 0x00300000
#define MEMORY_PARTITION_KERNEL2_BASE 0x88300000
#define MEMORY_PARTITION_KERNEL2_SIZE 0x00100000
#define MEMORY_PARTITION_KERNEL3_BASE 0x88400000
#define MEMORY_PARTITION_KERNEL3_SIZE 0x00400000

#if FL_MEMORY != 0

#ifdef __cplusplus
extern "C" {
#endif

#if FL_MEMORY_ERRORBYTES != 0
typedef struct {
     void* memtData;
     u32   memtSize;
     u32   memtAlignment;
     void* memtNext;
} MemoryTree;

extern MemoryTree* memErrorTree;
#endif

extern bool memScratchpadUsed;

extern void  memInit();
extern void  memTerm();



extern u32 memFreeSpace(u32 inAccuracy);



#if FL_DEBUG_CALLEDFROM != 0
extern void  memSetFrom(void* inPtr, int inVal, unsigned int inSize, const char* inFile, const char* inFunc, int inLine);
extern void  memClearFrom(void* inPtr, unsigned int inSize, const char* inFile, const char* inFunc, int inLine);
extern void  memCopyFrom(void* inDest, void* inSrc, unsigned int inSize, const char* inFile, const char* inFunc, int inLine);
#define memSet(inPtr, inVal, inSize) memSetFrom(inPtr, inVal, inSize, __FILE__, __FUNCTION__, __LINE__)
#define memClear(inPtr, inSize) memClearFrom(inPtr, inSize, __FILE__, __FUNCTION__, __LINE__)
#define memCopy(inDest, inSrc, inSize) memCopyFrom(inDest, inSrc, inSize, __FILE__, __FUNCTION__, __LINE__)
#else
extern void  memSet(void* inPtr, int inVal, unsigned int inSize);
extern void  memClear(void* inPtr, unsigned int inSize);
extern void  memCopy(void* inDest, void* inSrc, unsigned int inSize);
#endif



#if FL_DEBUG_CALLEDFROM != 0
extern void* memAllocFrom(int inSize, const char* inFile, const char* inFunc, int inLine);
extern void* memCallocFrom(int inSize0, int inSize1, const char* inFile, const char* inFunc, int inLine);
extern void* memQallocFrom(int inSize, const char* inFile, const char* inFunc, int inLine);
extern void* memAllocUncachedFrom(int inSize, const char* inFile, const char* inFunc, int inLine);
extern void* memReallocFrom(void* inData, int inSize, const char* inFile, const char* inFunc, int inLine);
extern void* memAlignFrom(int inBoundry, int inSize, const char* inFile, const char* inFunc, int inLine);
extern void  memFreeFrom(void* inData, const char* inFile, const char* inFunc, int inLine);
extern void* memUncachedPtrFrom(void* inPtr, const char* inFile, const char* inFunc, int inLine);
#define memAlloc(inSize) memAllocFrom(inSize, __FILE__, __FUNCTION__, __LINE__)
#define memCalloc(inSize0, inSize1) memCallocFrom(inSize0, inSize1, __FILE__, __FUNCTION__, __LINE__)
#define memQalloc(inSize) memQallocFrom(inSize, __FILE__, __FUNCTION__, __LINE__)
#define memAllocUncached(inSize) memAllocUncachedFrom(inSize0, inSize1, __FILE__, __FUNCTION__, __LINE__)
#define memRealloc(inData, inSize) memReallocFrom(inData, inSize, __FILE__, __FUNCTION__, __LINE__)
#define memAlign(inBoundry, inSize) memAlignFrom(inBoundry, inSize, __FILE__, __FUNCTION__, __LINE__)
#define memFree(inData) memFreeFrom(inData, __FILE__, __FUNCTION__, __LINE__)
#define memUncachedPtr(inPtr) memUncachedPtrFrom(inPtr, __FILE__, __FUNCTION__, __LINE__)
#else
extern void* memAlloc(int inSize);
extern void* memQalloc(int inSize);
extern void* memCalloc(int inSize0, int inSize1);
extern void* memAllocUncached(int inSize);
extern void* memRealloc(void* inData, int inSize);
extern void* memAlign(int inBoundry, int inSize);
extern void  memFree(void* inData);
extern void* memUncachedPtr(void* inPtr);
#endif


extern u32  memAddressAlignment(void* inPtr);
extern bool memAddressValid(void* inPtr);
extern bool memBlockValid(void* inPtr, u32 inSize);



#if FL_MEMORY_PAGE != 0
extern unsigned int memToPage(void* inData, unsigned int inSize);
extern void*        memFromPage(unsigned int inPageID);
#endif



#if FL_MEMORY_ERRORBYTES != 0
#if FL_DEBUG_CALLEDFROM != 0
extern bool memErrorBytesCheckFrom(void* inPtr, const char* inFile, const char* inFunc, int inLine);
extern void memtAddFrom(MemoryTree** inMemTree, void* inPtr, u32 inSize, u32 inAlignment, const char* inFile, const char* inFunc, int inLine);
extern void memtDelFrom(MemoryTree** inMemTree, void* inPtr, const char* inFile, const char* inFunc, int inLine);
extern void memtErrorBytesCheckFrom(MemoryTree** inMemTree, const char* inFile, const char* inFunc, int inLine);
#define memErrorBytesCheck(inPtr) memErrorBytesCheckFrom(inPtr, __FILE__, __FUNCTION__, __LINE__)
#define memtAdd(inMemTree, inPtr, inSize, inAlignment) memtAddFrom(inMemTree, inPtr, inSize, inAlignment, __FILE__, __FUNCTION__, __LINE__)
#define memtDel(inMemTree, inPtr) memtDelFrom(inMemTree, inPtr, __FILE__, __FUNCTION__, __LINE__)
#define memtErrorBytesCheck(inMemTree) memtErrorBytesCheckFrom(inMemTree, __FILE__, __FUNCTION__, __LINE__)
#else
extern bool memErrorBytesCheck(void* inPtr);
extern void memtAdd(MemoryTree** inMemTree, void* inPtr, u32 inSize, u32 inAlignment);
extern void memtDel(MemoryTree** inMemTree, void* inPtr);
extern void memtErrorBytesCheck(MemoryTree** inMemTree);
#endif
#endif

#ifdef __cplusplus
}
#endif

#else

#include <malloc.h>
#define memAlloc(inSize) malloc(inSize)
#define memAlign(inBoundry, inSize) memalign(inBoundry, inSize)
#define memRealloc(inData, inSize) realloc(inData, inSize)
#define memCalloc(inSize0, inSize1) calloc(inSize0, inSize1)
#define memQalloc(inSize) memAlloc(inSize)
#define memFree(inData) free(inData)
#define memCopy(inDest, inSrc, inSize) memcpy(inDest, inSrc, inSize)
#define memClear(inPtr, inSize) memset(inPtr, 0x00, inSize)
#define memSet(inPtr, inVal, inSize) memset(inPtr, inVal, inSize)
#define memUncachedPtr(inPtr) ((void*)((unsigned int)(inPtr) | MEMORY_UNCACHED_OFFSET))

//#define memBlockValid(inPtr) ...TODO
//#define memBlockValid(inPtr, inSize) (memAddressValid(inPtr) && memAddressValid((void*)((u32)(inPtr) + (inSize))))

#endif

#define memCachedPtr(inPtr) ((void*)((unsigned int)(inPtr) & ~MEMORY_UNCACHED_OFFSET))
#define memIsUncachedPtr(inPtr) (((unsigned int)(inPtr) & MEMORY_UNCACHED_OFFSET) != 0)
#define memIsCachedPtr(inPtr) (((unsigned int)(inPtr) & MEMORY_UNCACHED_OFFSET) == 0)
#define memCachedPointer(inPtr) memCachedPtr(inPtr);
#define memUncachedPointer(inPtr) memUncachedPtr(inPtr);
#define memIsUncachedPointer(inPtr) memIsUncachedPtr(inPtr)
#define memIsCachedPointer(inPtr) memIsCachedPtr(inPtr)

#endif
