// Funclib Memory v1.0.0 (CORE)
// 
// This module contains functions for memory management.
// 
// Contributor(s): Flatmush, idea for memory allocation wrapper came from InsertWittyName.



#ifndef FLMEMORY_H
#define FLMEMORY_H

#include "flGlobal.h"
#include "flMemoryVMem.h"

#define MEMORY_UNCACHED_OFFSET 0x40000000 ///< The offset that is added in order to make a pointer uncached.

#define MEMORY_USER_BASE 0x08800000 ///< The start of the user memory.
#define MEMORY_USER_SIZE 0x01800000 ///< The length of the user memory.

#define MEMORY_KERNEL_BASE 0x08000000 ///< The start of the kernel memory.
#define MEMORY_KERNEL_SIZE 0x00800000 ///< The length of the kernel memory.

#define MEMORY_SCRATCHPAD_BASE 0x00010000 ///< The start of the scratchpad memory.
#define MEMORY_SCRATCHPAD_SIZE 0x00004000 ///< The length of the scratchpad memory.

#define MEMORY_PARTITION_KERNEL1_BASE 0x88000000 ///< The start of kernel partition 1.
#define MEMORY_PARTITION_KERNEL1_SIZE 0x00300000 ///< The size of kernel partition 1.
#define MEMORY_PARTITION_KERNEL2_BASE 0x88300000 ///< The start of kernel partition 2.
#define MEMORY_PARTITION_KERNEL2_SIZE 0x00100000 ///< The size of kernel partition 2.
#define MEMORY_PARTITION_KERNEL3_BASE 0x88400000 ///< The start of kernel partition 3.
#define MEMORY_PARTITION_KERNEL3_SIZE 0x00400000 ///< The size of kernel partition 3.

#if FL_MEMORY

#ifdef __cplusplus
extern "C" {
#endif

#if FL_MEMORY_ERRORBYTES
typedef struct {
     void* memtData;
     u32   memtSize;
     u32   memtAlignment;
     void* memtNext;
} MemoryTree;

extern MemoryTree* memErrorTree;
#endif

/**
  * A boolean to represent whether the memory system has been initialized or not,
  * this is set by memInit() and memTerm().
  * @see memInit(), memTerm()
  */
extern bool  memInitialized;

/**
  * A boolean to represent whether the scratchpad is currently in use or not,
  * this allows the fastest possible allocation/free'ing of memory for temp data.
  * @see memQalloc()
  */
extern bool  memScratchpadUsed;


/**
  * Called by flInitialize() to initialize the memory system.
  * @see flInitialize(), memTerm()
  */
extern void  memInit();

/**
  * Called by flTerminate() to deinitialize the memory system.
  * @see flTerminate(), memInit()
  */
extern void  memTerm();



/**
  * Returns a close estimate of the amount of free space.
  * @param inAccuracy The amount of bytes that the returned answer should be accurate to.
  * @return The amount of free space accurate to inAccuracy bytes.
  * @note Since malloc allocates 16 byte aligned data, this is the closest accuracy you will get, so there's not much point in setting inAccuracy to below 16.
  */
extern u32   memFreeSpace(u32 inAccuracy);

/**
  * Returns a close estimate of the total amount of free space available to the program on the whole system,
  * this includes video memory, scratchpad and any other memory funcLib can get it's hands on.
  * @return The amount of free space available on the whole system, to the highest accuracy possible.
  */
extern u32   memFreeSpaceTotal();

/**
  * Converts an amount of bytes into a more humanly readable string to be printed in a log or to the screen.
  * @param inBytes The amount of bytes.
  * @param inBinary If set to true then it will return binary results where there are 1024 bytes in a Kilobyte (and append e.g. "KiB"), if not then a KiloByte will be considered 1000 bytes (and e.g "KB" will be printed).
  * @return A string representing the bytes in SI units, (e.g 1024 bytes = "1 KiB" or "1.02 KB").
  * @warning This returns an allocated string that must be free'd after use.
  * @see memFreeSpace(), memFreeSpaceTotal(), memFree()
  */
extern char* memString(u32 inBytes, bool inBinary);



#if FL_DEBUG_CALLEDFROM
extern void  memSetByteFrom(void* inPtr, int inVal, unsigned int inSize, const char* inFile, const char* inFunc, int inLine);
extern void  memSetShortFrom(void* inPtr, int inVal, unsigned int inSize, const char* inFile, const char* inFunc, int inLine);
extern void  memClearFrom(void* inPtr, unsigned int inSize, const char* inFile, const char* inFunc, int inLine);
extern void  memCopyFrom(void* inDest, void* inSrc, unsigned int inSize, const char* inFile, const char* inFunc, int inLine);
extern int   memCompareFrom(void* inSrc0, void* inSrc1, unsigned int inSize, const char* inFile, const char* inFunc, int inLine);
#define memSetByte(inPtr, inVal, inSize) memSetByteFrom(inPtr, inVal, inSize, __FILE__, __FUNCTION__, __LINE__)
#define memSetShort(inPtr, inVal, inSize) memSetShortFrom(inPtr, inVal, inSize, __FILE__, __FUNCTION__, __LINE__)
#define memClear(inPtr, inSize) memClearFrom(inPtr, inSize, __FILE__, __FUNCTION__, __LINE__)
#define memCopy(inDest, inSrc, inSize) memCopyFrom(inDest, inSrc, inSize, __FILE__, __FUNCTION__, __LINE__)
#define memCompare(inSrc0, inSrc1, inSize) memCompareFrom(inSrc0, inSrc1, inSize, __FILE__, __FUNCTION__, __LINE__)
#else
extern void  memSetByte(void* inPtr, int inVal, unsigned int inSize);
extern void  memSetShort(void* inPtr, int inVal, unsigned int inSize);
extern void  memClear(void* inPtr, unsigned int inSize);
extern void  memCopy(void* inDest, void* inSrc, unsigned int inSize);
extern int   memCompare(void* inSrc0, void* inSrc1, unsigned int inSize);
#endif



#if FL_DEBUG_CALLEDFROM
/**
  * This functions is identical to memAlloc() except that it takes extra parameters to define where it was called from,
  * @param inSize The size of the block to be allocated.
  * @param inFile The source file that the original call came from.
  * @param inFunc The function that the original call originated from.
  * @param inLine The line on which the original call was on.
  * @return A pointer to the allocated memory block.
  * @see memAlloc(), memAlloc
  */
extern void* memAllocFrom(int inSize, const char* inFile, const char* inFunc, int inLine);
extern void* memCallocFrom(int inSize0, int inSize1, const char* inFile, const char* inFunc, int inLine);
extern void* memQallocFrom(int inSize, const char* inFile, const char* inFunc, int inLine);
extern void* memAllocUncachedFrom(int inSize, const char* inFile, const char* inFunc, int inLine);
extern void* memReallocFrom(void* inData, int inSize, const char* inFile, const char* inFunc, int inLine);
extern void* memAlignFrom(int inBoundry, int inSize, const char* inFile, const char* inFunc, int inLine);
extern void  memFreeFrom(void* inData, const char* inFile, const char* inFunc, int inLine);
extern void* memUncachedPtrFrom(void* inPtr, const char* inFile, const char* inFunc, int inLine);
/**
  * This is a wrapper for malloc, it allows for extra options such as higher default alignment,
  * memory integrity checks and overflowing into areas other than the heap.
  * @param inSize Size of the block to be allocated.
  * @return Pointer to the allocated block or NULL on failiure.
  * @note This acts in exactly the same way as malloc but complies with the funclib options,
  *       this macro version of the function is used when FL_DEBUG_CALLEDFROM is defined to reserve compatability
  *       and trace where an error was called from without lots of modifications.
  * @see FL_MEMORY_ALIGNMENT_DEFAULT, FL_MEMORY_ERRORBYTES, FL_MEMORY_VMEM_OVERFLOW, FL_MEMORY_VMEM_STIDE_FILL
  */
#define memCalloc(inSize0, inSize1) memCallocFrom(inSize0, inSize1, __FILE__, __FUNCTION__, __LINE__)
#define memQalloc(inSize) memQallocFrom(inSize, __FILE__, __FUNCTION__, __LINE__)
#define memAllocUncached(inSize) memAllocUncachedFrom(inSize, __FILE__, __FUNCTION__, __LINE__)
#define memRealloc(inData, inSize) memReallocFrom(inData, inSize, __FILE__, __FUNCTION__, __LINE__)
#define memAlign(inBoundry, inSize) memAlignFrom(inBoundry, inSize, __FILE__, __FUNCTION__, __LINE__)
#define memFree(inData) memFreeFrom(inData, __FILE__, __FUNCTION__, __LINE__)
#define memUncachedPtr(inPtr) memUncachedPtrFrom(inPtr, __FILE__, __FUNCTION__, __LINE__)
#else
/**
  * This is a wrapper for malloc, it allows for extra options such as higher default alignment,
  * memory integrity checks and overflowing into areas other than the heap.
  * @param inSize Size of the block to be allocated.
  * @return Pointer to the allocated block or NULL on failiure.
  * @note This acts in exactly the same way as malloc but complies with the funclib options.
  * @see FL_MEMORY_ALIGNMENT_DEFAULT, FL_MEMORY_ERRORBYTES, FL_MEMORY_VMEM_OVERFLOW, FL_MEMORY_VMEM_STIDE_FILL
  */
extern void* memQalloc(int inSize);
extern void* memCalloc(int inSize0, int inSize1);
extern void* memAllocUncached(int inSize);
extern void* memRealloc(void* inData, int inSize);
extern void* memAlign(int inBoundry, int inSize);
extern void  memFree(void* inData);
extern void* memUncachedPtr(void* inPtr);
#endif
#define memAlloc(inSize) memAlign(FL_MEMORY_ALIGNMENT_DEFAULT, inSize)


/**
  * Calculates the largest binary increment that a pointer is aligned to (e.g 16, 32, 64, 128, etc.).
  * @param inPtr The pointer whose alignment will be calculated.
  * @return The mamximum alignment value.
  */
extern u32  memAddressAlignment(void* inPtr);

/**
  * Checks if a pointer resides within valid memory blocks on the terget platform.
  * @param inPtr The pointer whose validity will be checked.
  * @return True if the pointer is valid.
  */
extern bool memAddressValid(void* inPtr);

/**
  * Checks if a memory block resides within valid memory blocks on the terget platform.
  * @param inPtr The pointer to the start of the memory block whose validity will be checked.
  * @param inSize The length of the memory block whose validity will be checked.
  * @return True if the pointer is valid.
  */
extern bool memBlockValid(void* inPtr, u32 inSize);



#if FL_MEMORY_PAGE
extern unsigned int memToPage(void* inData, unsigned int inSize);
extern void*        memFromPage(unsigned int inPageID);
#endif



#if FL_MEMORY_ERRORBYTES
#if FL_DEBUG_CALLEDFROM
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
#define memAlloc(inSize) memalign(FL_MEMORY_ALIGNMENT_DEFAULT, (inSize)) ///< This allows malloc to be used instead of memAlloc calls when FL_MEMORY is 0.
#define memAlign(inBoundry, inSize) memalign(max(FL_MEMORY_ALIGNMENT_DEFAULT, (inBoundry)), (inSize))
#define memRealloc(inData, inSize) realloc((inData), (inSize))
#define memCalloc(inSize0, inSize1) memAlloc((inSize0) * (inSize1))
#define memQalloc(inSize) memAlloc(inSize)
#define memFree(inData) free(inData)
#define memCopy(inDest, inSrc, inSize) memmove((inDest), (inSrc), (inSize))
#define memClear(inPtr, inSize) memset(inPtr, 0x00, inSize)
#define memSetByte(inPtr, inVal, inSize) memset(inPtr, inVal, inSize)
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
