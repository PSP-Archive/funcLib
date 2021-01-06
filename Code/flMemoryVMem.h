// Funclib Memory VMem v1.0.0
// 
// This module contains functions for video memory management.
// 
// Contributor(s): VRAM code was written from scratch by Flatmush but based off Raphaels vram manager idea.



#ifndef FLMEMORYVMEM_H
#define FLMEMORYVMEM_H

#include "flGlobal.h"
#include "flMemory.h"

#define MEMORY_VMEM_BASE       0x04000000
#define MEMORY_VMEM_SIZE       0x00200000

#if FL_MEMORY_VMEM != 0

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
     void* vmsbStart;
     u32   vmsbStride;
     
     u32   vmsbBlockSize;
     u32   vmsbBlockCount;
     u32   vmsbBlockAlign;
     u8*   vmsbBlockTable;
     
     void* vmsbLast;     
     void* vmsbNext;
} VmemStrideBlock;

extern u32   vmemStart;
extern u32   vmemSize;
extern u32*  vmemBlockTable;
extern u32   vmemBlocks;
extern u32   vmemFreeBlocks;
extern bool  vmemInitialized;

extern VmemStrideBlock* vmemStrideBlocks;

extern void  vmemInit();
extern void  vmemTerm();

extern void* vmemNormalPtr(void* inPtr);
#define vmemNormalPointer(inPtr) vmemNormalPtr(inPtr)

extern u32   vmemFreeSpace();
extern u32   vmemLargestFreeBlock();
extern float vmemFragmentation();

extern void* vmemAllocBuffer(u32 inWidth, u32 inHeight, u8 inBitDepth, u32 inStride);
#if (FL_DEBUG != 0) && (FL_DEBUG_CALLEDFROM != 0)
extern void* vmemAllocFrom(u32 inSize, const char* inFile, const char* inFunc, int inLine);
extern void* vmemCallocFrom(u32 inSize0, u32 inSize1, const char* inFile, const char* inFunc, int inLine);
extern void* vmemReallocFrom(void* inData, u32 inSize, const char* inFile, const char* inFunc, int inLine);
extern void  vmemFreeFrom(void* inData, const char* inFile, const char* inFunc, int inLine);
#define vmemAlloc(inSize) vmemAllocFrom(inSize, __FILE__, __FUNCTION__, __LINE__)
#define vmemCalloc(inSize0, inSize1) vmemCallocFrom(inSize0, inSize1, __FILE__, __FUNCTION__, __LINE__)
#define vmemRealloc(inData, inSize) vmemReallocFrom(inData, inSize, __FILE__, __FUNCTION__, __LINE__)
#define vmemFree(inData) vmemFreeFrom(inData, __FILE__, __FUNCTION__, __LINE__)
#else
extern void* vmemAlloc(u32 inSize);
extern void* vmemCalloc(u32 inSize0, u32 inSize1);
extern void* vmemRealloc(void* inData, u32 inSize);
extern void  vmemFree(void* inData);
#endif

extern void  vmemStrideBlockAdd(void* inStart, u32 inStride, u32 inBlockSize, u32 inBlockCount);
extern void  vmemStrideBlockDel(VmemStrideBlock* inBlock);
extern void  vmemStrideBlockDelRecursive(VmemStrideBlock* inBlock);

extern void* vmemStrideAlign(u32 inAlign, u32 inSize);
extern void* vmemStrideAlloc(u32 inSize);
extern void* vmemStrideCalloc(u32 inSize0, u32 inSize1);
extern bool  vmemStrideFree(void* inData);
extern float vmemStrideUsage();
extern u32   vmemStrideFreeSpace();

#ifdef __cplusplus
}
#endif

#endif

#define vmemAbsolutePtr(inPtr) ((void*)((unsigned int)(inPtr) | MEMORY_VMEM_BASE))
#define vmemRelativePtr(inPtr) ((void*)((unsigned int)(inPtr) & ~MEMORY_VMEM_BASE))
#define vmemPtr(inPtr) (((unsigned int)memCachedPtr(inPtr) >= MEMORY_VMEM_BASE) && ((unsigned int)memCachedPtr(inPtr) < (MEMORY_VMEM_BASE + (MEMORY_VMEM_SIZE << 2))))
#define vmemAbsPtr(inPtr) vmemAbsolutePtr(inPtr)
#define vmemRelPtr(inPtr) vmemRelativePtr(inPtr)
#define vmemAbsolutePointer(inPtr) vmemAbsolutePtr(inPtr)
#define vmemRelativePointer(inPtr) vmemRelativePtr(inPtr)
#define vmemPointer(inPtr) vmemPtr(inPtr)
#define vmemAlign(inBoundry, inSize) vmemAlloc(inSize)

#endif
