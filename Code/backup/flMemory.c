#include "flGlobal.h"
#if FL_MEMORY != 0
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <pspkernel.h>

#if FL_INCLUDE_ALL_C == 0
#include "flMemory.h"
#include "flFile.h"

#if FL_DEBUG != 0
#include "flDebug.h"
#endif
#endif

#if FL_MEMORY_PAGE != 0
unsigned int memPageID = 0; // Current page ID.
#endif

#if FL_MEMORY_ERRORBYTES != 0
MemoryTree* memErrorTree = NULL;
#endif

bool memScratchpadUsed = false;

void memInit() {
     #if FL_MEMORY_PAGE != 0
     FILE* tempFile = fopen(MEMORY_PAGE_PATH, "w");
     if(!tempFile) {
          #if FL_DEBUG_ERROR != 0
          debugError("Couldn't create blank memory page file.");
          #endif
          return;
     }
     fclose(tempFile);
     #endif
}

void memTerm() {
     #if FL_MEMORY_PAGE != 0
     #if FL_FILE != 0
     fileDelete(FL_MEMORY_PAGE_PATH);
     #else
     sceIoRemove(FL_MEMORY_PAGE_PATH);
     #endif
     #endif
}

u32 memFreeSpace(u32 inAccuracy) {
    if(!inAccuracy)
         inAccuracy = 1;

    u32 tempBlockSize = (MEMORY_USER_SIZE >> 1);
    u32 tempTests;
    
    for(tempTests = 0; tempBlockSize > inAccuracy; tempTests++)
         tempBlockSize >>= 1;
    tempBlockSize = (MEMORY_USER_SIZE >> 1);

    void* tempPointers[tempTests];
    
    u32 i; 
    u32 tempSpace = 0;
    for(i = 0; i < tempTests; i++) {
         tempPointers[i] = malloc(tempBlockSize);
         tempSpace += (tempPointers[i] ? tempBlockSize : 0);
         tempBlockSize >>= 1;
    }
    
    for(i = 0; i < tempTests; i++) {
         if(tempPointers[i])
              free(tempPointers[i]);
    }
    
    return tempSpace;
}

#if FL_DEBUG_CALLEDFROM != 0
inline void memSetFrom(void* inPtr, int inVal, unsigned int inSize, const char* inFile, const char* inFunc, int inLine) {
#else
inline void memSet(void* inPtr, int inVal, unsigned int inSize) {
#endif
     if(!inPtr || !inSize) {
          #if FL_DEBUG_WARNING != 0
          char tempString[256];
          if(!inPtr)
               sprintf(tempString, "memSet failed, pointer is NULL.\n");
          if(!inSize)
               sprintf(tempString, "%sTrying to memSet 0 bytes of memory.\n", tempString);

          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom(tempString, inFile, inFunc, inLine);
          #else
          debugWarning(tempString);
          #endif
          #endif
          return;
     }
     if(!memBlockValid(inPtr, inSize)) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("(memSet) Trying to memSet an invalid memory block.", inFile, inFunc, inLine);
          #else
          debugWarning("Trying to memSet an invalid memory block.");
          #endif
          #endif
          return;
     }
     
     inSize += (u32)inPtr;
     if(!(inSize & 4) && !((u32)inPtr & 4)) {
          u32 tempVal = (inVal & 0xFF);
          tempVal += (tempVal << 8);
          tempVal += (tempVal << 16);
          u32* tempPtr = (u32*)inPtr;
          while((u32)tempPtr < inSize)
               *(tempPtr++) = tempVal;
     } else {
          u8 tempVal = (inVal & 0xFF);
          u8* tempPtr = (u8*)inPtr;
          while((u32)tempPtr < inSize)
               *(tempPtr++) = tempVal;
     }
     
     #if FL_MEMORY_ERRORBYTES != 0
     #if FL_DEBUG_CALLEDFROM != 0
     memtErrorBytesCheckFrom(&memErrorTree, inFile, inFunc, inLine);
     #else
     memtErrorBytesCheck(&memErrorTree);
     #endif
     #endif
}

#if FL_DEBUG_CALLEDFROM != 0
inline void memClearFrom(void* inPtr, unsigned int inSize, const char* inFile, const char* inFunc, int inLine) {
#else
inline void memClear(void* inPtr, unsigned int inSize) {
#endif
     if(!inPtr || !inSize) {
          #if FL_DEBUG_WARNING != 0
          char tempString[256];
          if(!inPtr)
               sprintf(tempString, "memClear failed, pointer is NULL.\n");
          if(!inSize)
               sprintf(tempString, "%sTrying to memClear 0 bytes of memory.\n", tempString);

          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom(tempString, inFile, inFunc, inLine);
          #else
          debugWarning(tempString);
          #endif
          #endif
          return;
     }
     if(!memBlockValid(inPtr, inSize)) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("(memClear) Trying to memClear an invalid memory block.", inFile, inFunc, inLine);
          #else
          debugWarning("Trying to memClear an invalid memory block.");
          #endif
          #endif
          return;
     }
     
     inSize += (u32)inPtr;
     if(!(inSize & 4) && !((u32)inPtr & 4)) {
          u32* tempPtr = (u32*)inPtr;
          while((u32)tempPtr < inSize)
               *(tempPtr++) = 0x00000000;
     } else {
          u8* tempPtr = (u8*)inPtr;
          while((u32)tempPtr < inSize)
               *(tempPtr++) = 0x00;
     }
     
     #if FL_MEMORY_ERRORBYTES != 0
     #if FL_DEBUG_CALLEDFROM != 0
     memtErrorBytesCheckFrom(&memErrorTree, inFile, inFunc, inLine);
     #else
     memtErrorBytesCheck(&memErrorTree);
     #endif
     #endif
}

#if FL_DEBUG_CALLEDFROM != 0
inline void memCopyFrom(void* inDest, void* inSrc, unsigned int inSize, const char* inFile, const char* inFunc, int inLine) {
#else
inline void memCopy(void* inDest, void* inSrc, unsigned int inSize) {
#endif
     if(!inDest || !inSrc || !inSize) {
          #if FL_DEBUG_WARNING != 0
          char tempString[256];
          tempString[0] = 0;
          if(!inDest)
               strcat(tempString, "Trying to memCopy to a NULL destination.\n");
          if(!inSrc)
               strcat(tempString, "Trying to memCopy from a NULL source.\n");
          if(!inSize)
               strcat(tempString, "Trying to memCopy 0 bytes of memory.\n");
          
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom(tempString, inFile, inFunc, inLine);
          #else
          debugWarning(tempString);
          #endif
          #endif
          return;
     }
     
     if((((u32)inDest + inSize) < (u32)inDest) || (((u32)inSrc + inSize) < (u32)inSrc)) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("(memCopy) Unsigned integer overflow while copying data.", inFile, inFunc, inLine);
          #else
          debugWarning("Unsigned integer overflow while copying data.");
          #endif
          #endif
          return;
     }
     
     if(!memBlockValid(inSrc, inSize) || !memBlockValid(inDest, inSize)) {
          char tempString[256];
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          sprintf(tempString, "(memCopy) Trying to memCopy to/from an invalid memory block (0x%08X-0x%08X <- 0x%08X-0x%08X).", (unsigned int)inDest, ((unsigned int)inDest + inSize), (unsigned int)inSrc, ((unsigned int)inSrc + inSize));
          debugWarningFrom(tempString, inFile, inFunc, inLine);
          #else
          sprintf(tempString, "Trying to memCopy to/from an invalid memory block (0x%08X-0x%08X <- 0x%08X-0x%08X).", (unsigned int)inDest, ((unsigned int)inDest + inSize), (unsigned int)inSrc, ((unsigned int)inSrc + inSize));
          debugWarning("Trying to memCopy to/from an invalid memory block.");
          #endif
          #endif
          return;
     }
     
     inSize += (u32)inDest;
     if(!(inSize & 3) && !((u32)inDest & 3) && !((u32)inSrc & 3)) {
          u32* tempDest = (u32*)inDest;
          u32* tempSrc = (u32*)inSrc;
          while((u32)tempDest < inSize)
               *(tempDest++) = *(tempSrc++);
     } else {
          u8* tempDest = (u8*)inDest;
          u8* tempSrc = (u8*)inSrc;
          while((u32)tempDest < inSize)
               *(tempDest++) = *(tempSrc++);
     }

     #if FL_MEMORY_ERRORBYTES != 0
     #if FL_DEBUG_CALLEDFROM != 0
     memtErrorBytesCheckFrom(&memErrorTree, inFile, inFunc, inLine);
     #else
     memtErrorBytesCheck(&memErrorTree);
     #endif
     #endif
}


#if FL_DEBUG_CALLEDFROM != 0
void* memAllocFrom(int inSize, const char* inFile, const char* inFunc, int inLine) {
     return memAlignFrom(FL_MEMORY_ALIGNMENT_DEFAULT, inSize, inFile, inFunc, inLine);
}
#else
void* memAlloc(int inSize) {
     return memAlign(FL_MEMORY_ALIGNMENT_DEFAULT, inSize);
}
#endif

#if FL_DEBUG_CALLEDFROM != 0
void* memQallocFrom(int inSize, const char* inFile, const char* inFunc, int inLine) {
#else
void* memQalloc(int inSize) {
#endif
     if((inSize <= MEMORY_SCRATCHPAD_SIZE) && !memScratchpadUsed) {
          memScratchpadUsed = true;
          return (void*)MEMORY_SCRATCHPAD_BASE;
     }
     
     void* tempOut;
     
     #if FL_MEMORY_VMEM != 0
     #if FL_DEBUG_CALLEDFROM != 0
     tempOut = vmemAllocFrom(inSize, inFile, inFunc, inLine);
     #else
     tempOut = vmemAlloc(inSize);
     #endif
     if(tempOut)
          return tempOut;
     #endif
     
     #if FL_DEBUG_CALLEDFROM != 0
     tempOut = memAllocFrom(inSize, inFile, inFunc, inLine);
     #else
     tempOut = memAlloc(inSize);
     #endif
     
     return tempOut;
}

#if FL_DEBUG_CALLEDFROM != 0
void* memCallocFrom(int inSize0, int inSize1, const char* inFile, const char* inFunc, int inLine) {
     void* tempOut = memAllocFrom((inSize0 * inSize1), inFile, inFunc, inLine);
#else
void* memCalloc(int inSize0, int inSize1) {
     void* tempOut = memAlloc(inSize0 * inSize1);
#endif
     if(!tempOut)
          return NULL;
     #if FL_DEBUG_CALLEDFROM != 0
     memClearFrom(tempOut, (inSize0 * inSize1), inFile, inFunc, inLine);
     #else
     memClear(tempOut, (inSize0 * inSize1));
     #endif
     return tempOut;
}

#if FL_DEBUG_CALLEDFROM != 0
void* memAllocUncachedFrom(int inSize, const char* inFile, const char* inFunc, int inLine) {
#else
void* memAllocUncached(int inSize) {
#endif
     void* tempOut = memAlign(max(64, FL_MEMORY_ALIGNMENT_DEFAULT), inSize);
     if(!tempOut) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("(memAllocUncached) Error allocating uncached memory, memory is probably full/fragmented.", inFile, inFunc, inLine);
          #else
          debugWarning("Error allocating uncached memory, memory is probably full/fragmented.");
          #endif
          #endif
          return NULL;
     }
     #if FL_MEMORY_ERRORBYTES != 0
     sceKernelDcacheWritebackInvalidateRange((void*)((unsigned int)tempOut - max(64, FL_MEMORY_ALIGNMENT_DEFAULT)), (inSize + (max(64, FL_MEMORY_ALIGNMENT_DEFAULT) << 1)));
     #else
     sceKernelDcacheWritebackInvalidateRange(tempOut, inSize);
     #endif
     tempOut = memUncachedPtr(tempOut);
     return tempOut;
}

#if FL_DEBUG_CALLEDFROM != 0
void* memReallocFrom(void* inData, int inSize, const char* inFile, const char* inFunc, int inLine) {
#else
void* memRealloc(void* inData, int inSize) {
#endif
     #if FL_MEMORY_VMEM != 0
     if(vmemPointer(inData))
          return vmemRealloc(inData, inSize);
     #endif
     if(!inSize) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("(memRealloc) Trying to re-allocate to 0 bytes.", inFile, inFunc, inLine);
          #else
          debugWarning("Trying to re-allocate to 0 bytes.");
          #endif
          #endif
          return NULL;
     }
     #if FL_MEMORY_ERRORBYTES != 0
     if(!memErrorBytesCheck(inData)) {
          #if FL_DEBUG_ERROR != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugErrorFrom("Error re-allocating memory, original data was corrupt.", inFile, inFunc, inLine);
          #else
          debugError("Error re-allocating memory, original data was corrupt.");
          #endif
          #endif
          return NULL;
     }
     u32* tempData = (u32*)((unsigned int)inData - 16);
     u32 tempBoundry = tempData[0];
     while(inSize & (tempBoundry - 1))
          inSize++;
     inData = (void*)((unsigned int)inData - tempBoundry);
     void* tempOut = realloc(inData, (inSize + (tempBoundry << 1)));
     if(!tempOut) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("(memRealloc) Error re-allocating memory, memory is probably full/fragmented.", inFile, inFunc, inLine);
          #else
          debugWarning("Error re-allocating memory, memory is probably full/fragmented.");
          #endif
          #endif
          return NULL;
     }
     
     #if FL_DEBUG_CALLEDFROM != 0
     memtAddFrom(&memErrorTree, tempOut, inSize, tempBoundry, inFile, inFunc, inLine);
     #else
     memtAdd(&memErrorTree, tempOut, inSize, tempBoundry);
     #endif
     
     tempOut = (void*)((unsigned int)tempOut + tempBoundry);
     u32* tempPattern = (u32*)((unsigned int)tempOut - 16);
     tempPattern[0] = tempBoundry;
     tempPattern[1] = inSize;
     tempPattern[2] = 0xFEEBDAED;
     tempPattern[3] = 0x76543210;
     tempPattern = (u32*)((unsigned int)tempOut + inSize);
     tempPattern[0] = 0xDEADBEEF;
     tempPattern[1] = 0x01234567;
     tempPattern[2] = tempBoundry;
     tempPattern[3] = inSize;
     
     #if FL_DEBUG_CALLEDFROM != 0
     memtErrorBytesCheckFrom(&memErrorTree, inFile, inFunc, inLine);
     #else
     memtErrorBytesCheck(&memErrorTree);
     #endif
     #else
     if((unsigned int)inData & MEMORY_UNCACHED_OFFSET) {
          while(inSize & 63)
               inSize++;
     } else {
          while(inSize & 15)
               inSize++;
     }
     void* tempOut = realloc(inData, inSize);
     #if FL_DEBUG_WARNING != 0
     if(!tempOut) {
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("(memRealloc) Error re-allocating memory, memory is probably full/fragmented.", inFile, inFunc, inLine);
          #else
          debugWarning("Error re-allocating memory, memory is probably full/fragmented.");
          #endif
     }
     #endif
     #endif
     if((unsigned int)inData & MEMORY_UNCACHED_OFFSET)
          tempOut = memUncachedPtr(tempOut);
     return tempOut;
}

#if FL_DEBUG_CALLEDFROM != 0
void* memAlignFrom(int inBoundry, int inSize, const char* inFile, const char* inFunc, int inLine) {
#else
void* memAlign(int inBoundry, int inSize) {
#endif
     if(!inSize) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("(memAlign) Trying to allocate aligned data of 0 bytes.", inFile, inFunc, inLine);
          #else
          debugWarning("Trying to allocate aligned data of 0 bytes.");
          #endif
          #endif
          return NULL;
     }
     if(!inBoundry) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("(memAlign) Trying to align to 0 byte boundry.\nReverting to default allocation alignment.", inFile, inFunc, inLine);
          #else
          debugWarning("Trying to align to 0 byte boundry.\nReverting to default allocation alignment.");
          #endif
          #endif
     }
     
     if(inBoundry < FL_MEMORY_ALIGNMENT_DEFAULT)
          inBoundry = FL_MEMORY_ALIGNMENT_DEFAULT;

     int i, tempBoundZeroCnt = 0, tempBoundMaxPower = 0;
     for(i = 0; i < 31; i++) {
          if(inBoundry & (1 << i)) {
               tempBoundZeroCnt++;
               tempBoundMaxPower = i;
          }
     }
     if(tempBoundZeroCnt > 1) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("(memAlign) Trying to align to a non-binary boundry.\nReverting to next valid boundry.", inFile, inFunc, inLine);
          #else
          debugWarning("Trying to align to a non-binary boundry.\nReverting to next valid boundry.");
          #endif
          #endif
          inBoundry = (1 << (tempBoundMaxPower + 1));
     }
     
     while(inSize & (inBoundry - 1))
          inSize++;

     void* tempOut;

     #if FL_MEMORY_VMEM_STRIDE_FILL != 0
     tempOut = vmemStrideAlign(inBoundry, inSize);
     if(tempOut)
          return tempOut;
     #endif          

     #if FL_MEMORY_ERRORBYTES != 0
     tempOut = memAlign(inBoundry, (inSize + (inBoundry << 1)));
     if(!tempOut) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("(memAlign) Error allocating aligned memory, memory is probably full/fragmented.", inFile, inFunc, inLine);
          #else
          debugWarning("Error allocating aligned memory, memory is probably full/fragmented.");
          #endif
          #endif
          return NULL;
     }
     #if FL_DEBUG_CALLEDFROM != 0
     memtAddFrom(&memErrorTree, tempOut, inSize, inBoundry, inFile, inFunc, inLine);
     #else
     memtAdd(&memErrorTree, tempOut, inSize, inBoundry);
     #endif
     
     tempOut = (void*)((unsigned int)tempOut + inBoundry);
     u32* tempPattern = (u32*)((unsigned int)tempOut - 16);
     tempPattern[0] = inBoundry;
     tempPattern[1] = inSize;
     tempPattern[2] = 0xFEEBDAED;
     tempPattern[3] = 0x76543210;
     tempPattern = (u32*)((unsigned int)tempOut + inSize);
     tempPattern[0] = 0xDEADBEEF;
     tempPattern[1] = 0x01234567;
     tempPattern[2] = inBoundry;
     tempPattern[3] = inSize;
     
     #if FL_DEBUG_CALLEDFROM != 0
     memtErrorBytesCheckFrom(&memErrorTree, inFile, inFunc, inLine);
     #else
     memtErrorBytesCheck(&memErrorTree);
     #endif
     #else
     tempOut = memalign(inBoundry, inSize);
     #if FL_DEBUG_WARNING != 0
     if(!tempOut) {
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("(memAlign) Error allocating aligned memory, memory is probably full/fragmented.", inFile, inFunc, inLine);
          #else
          debugWarning("Error allocating aligned memory, memory is probably full/fragmented.");
          #endif
     }
     #endif
     #endif
     return tempOut;
}

#if FL_DEBUG_CALLEDFROM != 0
void memFreeFrom(void* inData, const char* inFile, const char* inFunc, int inLine) {
#else
void memFree(void* inData) {
#endif
     if(!inData) {
          #if FL_DEBUG != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("(memFree) Trying to free NULL pointer.", inFile, inFunc, inLine);
          #else
          debugWarning("Trying to free NULL pointer.");
          #endif
          #endif
          return;
     }
     if(!memAddressValid(inData)) {
          char tempString[256];
          #if FL_DEBUG != 0
          #if FL_DEBUG_CALLEDFROM != 0
          sprintf(tempString, "(memFree) Trying to free invalid pointer (0x%08X).", (unsigned int)inData);
          debugWarningFrom(tempString, inFile, inFunc, inLine);
          #else
          sprintf(tempString, "Trying to free invalid pointer (0x%08X).", (unsigned int)inData);
          debugWarning(tempString);
          #endif
          #endif
          return;
     }
     if(((u32)inData >= MEMORY_SCRATCHPAD_BASE) && ((u32)inData < (MEMORY_SCRATCHPAD_BASE + MEMORY_SCRATCHPAD_SIZE))) {
          memScratchpadUsed = false;
          return;
     }
     #if FL_MEMORY_VMEM != 0
     if(vmemPointer(inData)) {
          #if FL_DEBUG_CALLEDFROM != 0
          return vmemFreeFrom(inData, inFile, inFunc, inLine);
          #else
          return vmemFree(inData);
          #endif
     }
     #endif
     #if FL_MEMORY_ERRORBYTES != 0
     if(!memErrorBytesCheck(inData)) {
          #if FL_DEBUG_ERROR != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugErrorFrom("(memFree) Cannot free pointer, data corrupt.", inFile, inFunc, inLine);
          #else
          debugError("Cannot free pointer, data corrupt.");
          #endif
          #endif
          return;
     }
     u32* tempData = (u32*)((unsigned int)inData - 16);
     inData = (void*)((unsigned int)inData - tempData[0]);
     if(!inData) {
          #if FL_DEBUG != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("(memFree) Trying to free NULL pointer.", inFile, inFunc, inLine);
          #else
          debugWarning("Trying to free NULL pointer.");
          #endif
          #endif
          return;
     }
     #endif
     #if FL_MEMORY_ERRORBYTES != 0
     #if FL_DEBUG_CALLEDFROM != 0
     memtDelFrom(&memErrorTree, inData, inFile, inFunc, inLine);
     memtErrorBytesCheckFrom(&memErrorTree, inFile, inFunc, inLine);
     #else
     memtDel(&memErrorTree, inData);
     memtErrorBytesCheck(&memErrorTree);
     #endif
     #endif
     free(inData);
}

#if FL_DEBUG_CALLEDFROM != 0
void* memUncachedPtrFrom(void* inPtr, const char* inFile, const char* inFunc, int inLine) {
#else
void* memUncachedPtr(void* inPtr) {
#endif
     if(!inPtr) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("(memUncachedPtr) Trying to create uncached null pointer.", inFile, inFunc, inLine);
          #else
          debugWarning("Trying to create uncached null pointer.");
          #endif
          #endif
          return NULL;
     }
     inPtr = (void*)((unsigned int)inPtr | MEMORY_UNCACHED_OFFSET);
     return inPtr;
}

u32  memAddressAlignment(void* inPtr) {
     u32 i;
     u32 tempBits = (sizeof(void*) << 3);
     for(i = 0; i < tempBits; i++) {
          if((u32)inPtr & (1 << i))
               return (1 << i);
     }
     return (1 << (tempBits - 1));
}

bool memAddressValid(void* inPtr) {
     u32 tempPtr = (u32)memCachedPtr(inPtr);
     if((tempPtr >= MEMORY_USER_BASE) && (tempPtr < (MEMORY_USER_BASE + MEMORY_USER_SIZE)))
          return true;
     if((tempPtr >= MEMORY_SCRATCHPAD_BASE) && (tempPtr < (MEMORY_SCRATCHPAD_BASE + MEMORY_SCRATCHPAD_SIZE)))
          return true;
     if((tempPtr >= MEMORY_VMEM_BASE) && (tempPtr < (MEMORY_VMEM_BASE + (MEMORY_VMEM_SIZE << 2))))
          return true;
     return false;
}

bool memBlockValid(void* inPtr, u32 inSize) {
     u32 tempPtr[2];
     tempPtr[0] = (u32)memCachedPtr(inPtr);
     tempPtr[1] = ((tempPtr[0] + inSize) - 1);
     if((tempPtr[0] >= MEMORY_USER_BASE) && (tempPtr[0] < (MEMORY_USER_BASE + MEMORY_USER_SIZE))) {
          if((tempPtr[1] >= MEMORY_USER_BASE) && (tempPtr[1] < (MEMORY_USER_BASE + MEMORY_USER_SIZE)))
               return true;
          return false;
     }
     if((tempPtr[0] >= MEMORY_SCRATCHPAD_BASE) && (tempPtr[0] < (MEMORY_SCRATCHPAD_BASE + MEMORY_SCRATCHPAD_SIZE))) {
          if((tempPtr[1] >= MEMORY_SCRATCHPAD_BASE) && (tempPtr[1] < (MEMORY_SCRATCHPAD_BASE + MEMORY_SCRATCHPAD_SIZE)))
               return true;
          return false;
     }
     if((tempPtr[0] >= MEMORY_VMEM_BASE) && (tempPtr[0] < (MEMORY_VMEM_BASE + (MEMORY_VMEM_SIZE << 2)))) {
          if((tempPtr[1] >= MEMORY_VMEM_BASE) && (tempPtr[1] < (MEMORY_VMEM_BASE + (MEMORY_VMEM_SIZE << 2)))) {
               if((tempPtr[0] & (MEMORY_VMEM_SIZE - 1)) < (tempPtr[1] & (MEMORY_VMEM_SIZE - 1)))
                    return true;
               return false;
          }
          return false;
     }
     return false;
}

#if FL_MEMORY_PAGE != 0
unsigned int memToPage(void* inData, unsigned int inSize) {
     if(!inData) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to page NULL pointer.");
          #endif
          return 0;
     }
     FILE* tempFile = fopen(MEMORY_PAGE_PATH, "ab");
     if(!tempFile) {
          #if FL_DEBUG_ERROR != 0
          debugError("Cannot open page file, to offload page.");
          #endif
          return 0;
     }
     unsigned int tempID = memPageID;
     memPageID++;
     fwrite(&tempID, 1, 4, tempFile);
     fwrite(&inSize, 1, 4, tempFile);
     fwrite(inData, 1, inSize, tempFile);
     fclose(tempFile);
     memFree(inData);
     return tempID;
}

void* memFromPage(unsigned int inPageID) {
     if(!inPageID) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to retrieve page 0 (Error page).");
          #endif
          return NULL;
     }
     FILE* tempFile = fopen(MEMORY_PAGE_PATH, "rb");
     if(!tempFile) {
          #if FL_DEBUG_ERROR != 0
          debugError("Cannot open page file, to retrieve paged data.");
          #endif
          return NULL;
     }
     unsigned int tempID = 0;
     unsigned int tempSize = 0;
     void* tempOut = NULL;
     while(!feof(tempFile)) {
          fread(&tempID, 1, 4, tempFile);
          fread(&tempSize, 1, 4, tempFile);
          if(tempID == inPageID) {
               tempOut = memAlloc(tempSize);
               if(!tempOut) {
                    #if FL_DEBUG_ERROR != 0
                    debugError("Cannot load page file, memory allocation failed.");
                    #endif
                    return NULL;
               }
               fread(tempOut, 1, tempSize, tempFile);
               return tempOut;
          } else {
               fseek(tempFile, tempSize, SEEK_CUR);
          }
     }
     #if FL_DEBUG_ERROR != 0
     debugError("Page with matching PageID not found within page file.");
     #endif
     return NULL;
}
#endif

#if FL_MEMORY_ERRORBYTES != 0
#if FL_DEBUG_CALLEDFROM != 0
bool memErrorBytesCheckFrom(void* inPtr, const char* inFile, const char* inFunc, int inLine) {
#else
bool memErrorBytesCheck(void* inPtr) {
#endif
     u32 tempBoundry;
     u32 tempSize;
     u32* tempPtr = (u32*)((unsigned int)inPtr - 16);
     tempBoundry = tempPtr[0];
     tempSize = tempPtr[1];
     if((tempPtr[2] != 0xFEEBDAED) || (tempPtr[3] != 0x76543210)) {
          #if FL_DEBUG_ERROR != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugErrorFrom("(memErrorBytesCheck) Memory integrity error, underflow.", inFile, inFunc, inLine);
          #else
          debugError("Memory integrity error, underflow.");
          #endif
          #endif
          return false;
     }
     tempPtr = (u32*)((unsigned int)inPtr + tempSize);
     if((tempPtr[0] != 0xDEADBEEF) || (tempPtr[1] != 0x01234567) || (tempPtr[2] != tempBoundry) || (tempPtr[3] != tempSize)) {
          #if FL_DEBUG_ERROR != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugErrorFrom("(memErrorBytesCheck) Memory integrity error, probably overflow, small chance of underflow.", inFile, inFunc, inLine);
          #else
          debugError("Memory integrity error, probably overflow, small chance of underflow.");
          #endif
          #endif
          return false;
     }
     return true;
}

#if FL_DEBUG_CALLEDFROM != 0
void memtAddFrom(MemoryTree** inMemTree, void* inPtr, u32 inSize, u32 inAlignment, const char* inFile, const char* inFunc, int inLine) {
#else
void memtAdd(MemoryTree** inMemTree, void* inPtr, u32 inSize, u32 inAlignment) {
#endif
     if(!inPtr || !inSize || !inAlignment) {
          #if FL_DEBUG_ERROR != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugErrorFrom("(memAdd) Trying to add a node with invalid properties to a memory tree.", inFile, inFunc, inLine);
          #else
          debugError("Trying to add a node with invalid properties to a memory tree.");
          #endif
          #endif
          return;
     }
     if(!inMemTree) {
          #if FL_DEBUG_ERROR != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugErrorFrom("(memAdd) Trying to add a node to a NULL memory tree.", inFile, inFunc, inLine);
          #else
          debugError("Trying to add a node to a NULL memory tree.");
          #endif
          #endif
          return;
     }
     
     MemoryTree* tempOut = malloc(sizeof(MemoryTree));
     if(!tempOut) {
          #if FL_DEBUG_ERROR != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugErrorFrom("(memAdd) Couldn't allocate node for memory tree.\nProbably out of memory.", inFile, inFunc, inLine);
          #else
          debugError("Couldn't allocate node for memory tree.\nProbably out of memory.");
          #endif
          #endif
          return;
     }
     tempOut->memtData = inPtr;
     tempOut->memtSize = inSize;
     tempOut->memtAlignment = inAlignment;
     tempOut->memtNext = NULL;
     
     if(*inMemTree) {
          MemoryTree* tempLastNode = *inMemTree;
          while(tempLastNode->memtNext)
               tempLastNode = (MemoryTree*)tempLastNode->memtNext;
          tempLastNode->memtNext = tempOut;
     } else {
          *inMemTree = tempOut;
     }
}

#if FL_DEBUG_CALLEDFROM != 0
void memtDelFrom(MemoryTree** inMemTree, void* inPtr, const char* inFile, const char* inFunc, int inLine) {
#else
void memtDel(MemoryTree** inMemTree, void* inPtr) {
#endif
     if(!inMemTree || !*inMemTree) {
          #if FL_DEBUG_ERROR != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugErrorFrom("(memDel) Trying to delete a node from a NULL memory tree.", inFile, inFunc, inLine);
          #else
          debugError("Trying to delete a node from a NULL memory tree.");
          #endif
          #endif
          return;
     }

     MemoryTree* tempDelNode = NULL;     
     if((*inMemTree)->memtData == inPtr) {
          tempDelNode = *inMemTree;
          *inMemTree = (MemoryTree*)tempDelNode->memtNext;
          free(tempDelNode);
          return;
     }
     
     MemoryTree* tempLastNode = *inMemTree;
     while(flRunning) {
          if(!tempLastNode->memtNext) {
               #if FL_DEBUG_ERROR != 0
               #if FL_DEBUG_CALLEDFROM != 0
               debugErrorFrom("(memDel) Pointer not found within memory tree.", inFile, inFunc, inLine);
               #else
               debugError("Pointer not found within memory tree.");
               #endif
               #endif
               return;
          }
          tempDelNode = (MemoryTree*)tempLastNode->memtNext;
          if(tempDelNode->memtData == inPtr)
               break;
          tempLastNode = tempDelNode;
     }
     tempLastNode->memtNext = (MemoryTree*)tempDelNode->memtNext;
     free(tempDelNode);
}

#if FL_DEBUG_CALLEDFROM != 0
void memtErrorBytesCheckFrom(MemoryTree** inMemTree, const char* inFile, const char* inFunc, int inLine) {
#else
void memtErrorBytesCheck(MemoryTree** inMemTree) {
#endif
     if(!inMemTree) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("(memtErrorBytesCheck) Trying to error check a NULL memory tree.", inFile, inFunc, inLine);
          #else
          debugWarning("Trying to error check a NULL memory tree.");
          #endif
          #endif
          return;
     }
     if(!*inMemTree) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("(memtErrorBytesCheck) Trying to error check an empty memory tree.", inFile, inFunc, inLine);
          #else
          debugWarning("Trying to error check an empty memory tree.");
          #endif
          #endif
          return;
     }
     MemoryTree* tempNode = *inMemTree;
     u32* tempPtr;
     u32 tempSize;
     u32 tempBoundry;
     while(tempNode) {
          if(!memBlockValid(tempNode->memtData, (tempNode->memtSize + (tempNode->memtAlignment << 1)))) {
               #if FL_DEBUG_ERROR != 0
               #if FL_DEBUG_CALLEDFROM != 0
               debugErrorFrom("(memtErrorBytesCheck) Memory tree data corrupt.\nPoints to invalid memory block.", inFile, inFunc, inLine);
               #else
               debugError("Memory tree data corrupt.\nPoints to invalid memory block.");
               #endif
               #endif
               return;
          }
          
          int i, j = 0;
          for(i = 0; i < 32; i++) {
               if(tempNode->memtAlignment & (1 << i))
                   j++;
          }
          if(j != 1) {
               #if FL_DEBUG_ERROR != 0
               #if FL_DEBUG_CALLEDFROM != 0
               debugErrorFrom("(memtErrorBytesCheck) Memory tree data corrupt.\nBad alignment value.", inFile, inFunc, inLine);
               #else
               debugError("Memory tree data corrupt.\nBad alignment value.");
               #endif
               #endif
               return;
          }
          
          if((u32)tempNode->memtData & (tempNode->memtAlignment - 1)) {
               #if FL_DEBUG_ERROR != 0
               #if FL_DEBUG_CALLEDFROM != 0
               debugErrorFrom("(memtErrorBytesCheck) Memory tree data corrupt.\nData is not aligned as described in memory tree.", inFile, inFunc, inLine);
               #else
               debugError("Memory tree data corrupt.\nData is not aligned as described in memory tree.");
               #endif
               #endif
               return;
          }
          if((u32)tempNode->memtSize & (tempNode->memtAlignment - 1)) {
               #if FL_DEBUG_ERROR != 0
               #if FL_DEBUG_CALLEDFROM != 0
               debugErrorFrom("(memtErrorBytesCheck) Memory tree data corrupt.\nSize is not aligned as described in memory tree.", inFile, inFunc, inLine);
               #else
               debugError("Memory tree data corrupt.\nSize is not aligned as described in memory tree.");
               #endif
               #endif
               return;
          }
          
          if(!memBlockValid(tempNode->memtNext, sizeof(MemoryTree))) {
               #if FL_DEBUG_ERROR != 0
               #if FL_DEBUG_CALLEDFROM != 0
               debugErrorFrom("(memtErrorBytesCheck) Memory tree data corrupt.\nPoints to invalid next node.", inFile, inFunc, inLine);
               #else
               debugError("Memory tree data corrupt.\nPoints to invalid next node.");
               #endif
               #endif
               return;
          }
          
          tempPtr = (u32*)tempNode->memtData;
          tempSize = tempNode->memtSize;
          tempBoundry = tempNode->memtAlignment;
          if((tempPtr[0] != tempBoundry) || (tempPtr[1] != tempSize) || (tempPtr[2] != 0xFEEBDAED) || (tempPtr[3] != 0x76543210)) {
               #if FL_DEBUG_ERROR != 0
               #if FL_DEBUG_CALLEDFROM != 0
               debugErrorFrom("(memtErrorBytesCheck) Memory integrity error, underflow.", inFile, inFunc, inLine);
               #else
               debugError("Memory integrity error, underflow.");
               #endif
               #endif
               return;
          }
          tempPtr = (u32*)(((unsigned int)tempNode->memtData + tempNode->memtAlignment) + tempSize);
          if((tempPtr[0] != 0xDEADBEEF) || (tempPtr[1] != 0x01234567) || (tempPtr[2] != tempBoundry) || (tempPtr[3] != tempSize)) {
               #if FL_DEBUG_ERROR != 0
               #if FL_DEBUG_CALLEDFROM != 0
               debugErrorFrom("(memtErrorBytesCheck) Memory integrity error, overflow.", inFile, inFunc, inLine);
               #else
               debugError("Memory integrity error, overflow.");
               #endif
               #endif
               return;
          }
          tempNode = (MemoryTree*)tempNode->memtNext;
     }
}
#endif

#endif
