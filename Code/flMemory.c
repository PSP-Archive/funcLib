#include "flGlobal.h"
#if FL_MEMORY != 0
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <pspkernel.h>

#if FL_INCLUDE_ALL_C == 0
#include "flMemory.h"
#include "flFile.h"

#if FL_DEBUG
#include "flDebug.h"
#endif
#endif

#if FL_MEMORY_PAGE
unsigned int memPageID = 0; // Current page ID.
#endif

#if FL_MEMORY_ERRORBYTES
MemoryTree* memErrorTree = NULL;
#endif

bool memInitialized = false;
bool memScratchpadUsed = false;

void memInit() {
     #if ((FL_MEMORY_PAGE) || (FL_MEMORY_SCRATCHPAD))
     FILE* tempFile;
     #endif
          
     #if FL_MEMORY_PAGE
     tempFile = fopen(MEMORY_PAGE_PATH, "w");
     if(!tempFile) {
          #if FL_DEBUG_ERROR
          debugError("Couldn't create blank memory page file at \"%s\".", MEMORY_PAGE_PATH);
          #endif
          return;
     }
     #if FL_DEBUG_STATUS
     debugStatusLog("Clearing page file.");
     #endif
     fclose(tempFile);
     #endif
     
     #if FL_MEMORY_SCRATCHPAD
     bool tempClear = true; u32 i;
     for(i = MEMORY_SCRATCHPAD_BASE; i < (MEMORY_SCRATCHPAD_BASE + MEMORY_SCRATCHPAD_SIZE); i++) {
          if(*((u8*)i)) {
               tempClear = false;
               break;
          }
     }
     if(!tempClear) {
          #if FL_DEBUG_STATUS
          debugStatusLog("Dumping scratchpad.");
          #endif
          u8 tempBuffer[16384];
          memCopy(tempBuffer, (void*)MEMORY_SCRATCHPAD_BASE, MEMORY_SCRATCHPAD_SIZE);
          tempFile = fopen("./scratchpad.dump", "w");
          if(tempFile) {
               fwrite(tempBuffer, 512, 32, tempFile);
               fclose(tempFile);
          }
     }
     #endif
     
     memInitialized = true;
}

void memTerm() {
     #if FL_MEMORY_PAGE
     #if FL_DEBUG_STATUS
     debugStatusLog("Deleting page file.");
     #endif
     fileDelete(FL_MEMORY_PAGE_PATH);
     #endif
     
     #if FL_MEMORY_SCRATCHPAD
     #if FL_DEBUG_STATUS
     debugStatusLog("Restoring scratchpad.");
     #endif
     u8 tempBuffer[16384];
     FILE* tempFile = fopen("./scratchpad.dump", "r");
     if(tempFile) {
          fread(tempBuffer, 4, 4096, tempFile);
          fclose(tempFile);
          fileDelete("./scratchpad.dump");
          memCopy((void*)MEMORY_SCRATCHPAD_BASE, tempBuffer, MEMORY_SCRATCHPAD_SIZE);
     } else 
          memClear((void*)MEMORY_SCRATCHPAD_BASE, MEMORY_SCRATCHPAD_SIZE);
     #endif
     
     memInitialized = false;
     #if FL_DEBUG_STATUS
     debugStatusLog("Memory terminated.");
     #endif
}

u32 memFreeSpace(u32 inAccuracy) {
     if(!inAccuracy)
          inAccuracy = 1;
         
     u32 tempBlockSize = (MEMORY_USER_SIZE >> 1);
     u32 tempFreeSpace = 0;
     void* tempPointers[8192];
     u32 tempPtr = 0;
     while((tempPtr < 8192) && (tempBlockSize > inAccuracy)) {
          tempPointers[tempPtr] = malloc(tempBlockSize);
          while(tempPointers[tempPtr]) {
               tempPtr++;
               tempFreeSpace += tempBlockSize;
               tempPointers[tempPtr] = malloc(tempBlockSize);
          }
          tempBlockSize >>= 1;
     }

     tempPtr = 0;
     while(tempPointers[tempPtr]) {
          free(tempPointers[tempPtr]);
          tempPtr++;
     }
    
     return tempFreeSpace;
}

u32 memFreeSpaceTotal() {
     u32 tempTotal = (memScratchpadUsed ? 0 : 16384);
     tempTotal += memFreeSpace(1);
     #if FL_MEMORY_VMEM
     tempTotal += vmemFreeSpace();
     #endif
     return tempTotal;
}

char* memString(u32 inBytes, bool inBinary) {
      u32 i = 0;
      float tempBytes = inBytes;

      if(inBinary) {
           while(inBytes >> 10) {
                i++;
                tempBytes /= 1024.0f;
                inBytes >>= 10;
           }
      } else {
           while(inBytes / 1000) {
                i++;
                tempBytes /= 1000.0f;
                inBytes /= 1000;
           }
      }

      char* tempOut = memAlign(FL_MEMORY_ALIGNMENT_DEFAULT, max(16, FL_MEMORY_ALIGNMENT_DEFAULT));
      sprintf(tempOut, "%4.2f ", tempBytes);

      if(i & 1) {
           if(i == 1)
                strcat(tempOut, "K");
           else
                strcat(tempOut, "G");
      } else
           strcat(tempOut, "M");
           
      if(i && inBinary)
           strcat(tempOut, "i");
      strcat(tempOut, "B");

      return tempOut;
}

#if FL_DEBUG_CALLEDFROM
void memSetByteFrom(void* inPtr, int inVal, unsigned int inSize, const char* inFile, const char* inFunc, int inLine) {
#else
void memSetByte(void* inPtr, int inVal, unsigned int inSize) {
#endif
     if(!inPtr || !inSize) {
          #if FL_DEBUG_WARNING
          char tempString[256];
          tempString[0] = 0;
          if(!inPtr)
               strcat(tempString, "memSetByte failed, pointer is NULL.\n");
          if(!inSize)
               strcat(tempString, "Trying to memSetByte 0 bytes of memory.\n");

          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, tempString);
          #else
          debugWarning(tempString);
          #endif
          #endif
          return;
     }
     if(!memBlockValid(inPtr, inSize)) {
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memSetByte) Trying to memSetByte an invalid memory block.");
          #else
          debugWarning("Trying to memSetByte an invalid memory block.");
          #endif
          #endif
          return;
     }
     
     inSize += (u32)inPtr;
     if((inSize & 3) || ((u32)inPtr & 3)) {
          if((inSize & 1) || ((u32)inPtr & 1)) {
               u8 tempVal = ((inVal & 0xFF) ^ (inVal < 0 ? 0x80 : 0x00));
               u8* tempPtr = (u8*)inPtr;
               while((u32)tempPtr < inSize)
                    *(tempPtr++) = tempVal;
          } else {
               u16 tempVal = ((inVal & 0xFF) ^ (inVal < 0 ? 0x80 : 0x00));
               tempVal |= (tempVal << 8);
               u16* tempPtr = (u16*)inPtr;
               while((u32)tempPtr < inSize)
                    *(tempPtr++) = tempVal;
          }
     } else {
          u32 tempVal = ((inVal & 0xFF) ^ (inVal < 0 ? 0x80 : 0x00));
          tempVal |= (tempVal << 8);
          tempVal |= (tempVal << 16);
          u32* tempPtr = (u32*)inPtr;
          while((u32)tempPtr < inSize)
               *(tempPtr++) = tempVal;
     }
     
     #if FL_MEMORY_ERRORBYTES
     #if FL_DEBUG_CALLEDFROM
     memtErrorBytesCheckFrom(&memErrorTree, inFile, inFunc, inLine);
     #else
     memtErrorBytesCheck(&memErrorTree);
     #endif
     #endif
}

#if FL_DEBUG_CALLEDFROM
void memSetShortFrom(void* inPtr, int inVal, unsigned int inSize, const char* inFile, const char* inFunc, int inLine) {
#else
void memSetShort(void* inPtr, int inVal, unsigned int inSize) {
#endif
     if(!inPtr || !inSize) {
          #if FL_DEBUG_WARNING
          char tempString[256];
          tempString[0] = 0;
          if(!inPtr)
               strcat(tempString, "memSetShort failed, pointer is NULL.\n");
          if(!inSize)
               strcat(tempString, "Trying to memSetShort 0 bytes of memory.\n");

          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, tempString);
          #else
          debugWarning(tempString);
          #endif
          #endif
          return;
     }
     if(!memBlockValid(inPtr, inSize)) {
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memSetShort) Trying to memSetShort an invalid memory block.");
          #else
          debugWarning("Trying to memSetShort an invalid memory block.");
          #endif
          #endif
          return;
     }
     
     inSize += (u32)inPtr;
     if((inSize & 3) || ((u32)inPtr & 3)) {
          if((inSize & 1) || ((u32)inPtr & 1)) {
               u16 tempVal = ((inVal & 0xFF) ^ (inVal < 0 ? 0x8000 : 0x0000));
               u8* tempPtr = (u8*)inPtr;
               if(inSize & 1)
                    tempVal = ((tempVal & 0x00FF) << 8) | ((tempVal & 0xFF00) >> 8);
               while((u32)tempPtr < inSize) {
                    *tempPtr = ((u32)tempPtr ? (tempVal & 0x00FF) : ((tempVal & 0xFF00) >> 8));
                    tempPtr++;
               }
          } else {
               u16 tempVal = ((inVal & 0xFF) ^ (inVal < 0 ? 0x8000 : 0x0000));
               u16* tempPtr = (u16*)inPtr;
               while((u32)tempPtr < inSize)
                    *(tempPtr++) = tempVal;
          }
     } else {
          u32 tempVal = ((inVal & 0xFF) ^ (inVal < 0 ? 0x8000 : 0x0000));
          tempVal |= (tempVal << 16);
          u32* tempPtr = (u32*)inPtr;
          while((u32)tempPtr < inSize)
               *(tempPtr++) = tempVal;
     }
     
     #if FL_MEMORY_ERRORBYTES
     #if FL_DEBUG_CALLEDFROM
     memtErrorBytesCheckFrom(&memErrorTree, inFile, inFunc, inLine);
     #else
     memtErrorBytesCheck(&memErrorTree);
     #endif
     #endif
}

#if FL_DEBUG_CALLEDFROM
void memClearFrom(void* inPtr, unsigned int inSize, const char* inFile, const char* inFunc, int inLine) {
#else
void memClear(void* inPtr, unsigned int inSize) {
#endif
     if(!inPtr || !inSize) {
          #if FL_DEBUG_WARNING
          char tempString[256];
          tempString[0] = 0;
          if(!inPtr)
               strcat(tempString, "memClear failed, pointer is NULL.\n");
          if(!inSize)
               strcat(tempString, "Trying to memClear 0 bytes of memory.\n");

          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, tempString);
          #else
          debugWarning(tempString);
          #endif
          #endif
          return;
     }
     if(!memBlockValid(inPtr, inSize)) {
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memClear) Trying to memClear an invalid memory block.");
          #else
          debugWarning("Trying to memClear an invalid memory block.");
          #endif
          #endif
          return;
     }
     
     inSize += (u32)inPtr;
     if((inSize & 31) || ((u32)inPtr & 31)) {
          if((inSize & 15) || ((u32)inPtr & 15)) {
               if((inSize & 7) || ((u32)inPtr & 7)) {
                    if((inSize & 3) || ((u32)inPtr & 3)) {
                         if((inSize & 3) || ((u32)inPtr & 3)) {
                              u8* tempPtr = (u8*)inPtr;
                              while((u32)tempPtr < inSize)
                                   *(tempPtr++) = 0x00;
                         } else {
                              u16* tempPtr = (u16*)inPtr;
                              while((u32)tempPtr < inSize)
                                   *(tempPtr++) = 0x0000;
                         }
                    } else {
                         u32* tempPtr = (u32*)inPtr;
                         while((u32)tempPtr < inSize)
                              *(tempPtr++) = 0x00000000;
                    }
               } else {
                    u64 tempVal = 0x0000000000000000;
                    u64* tempPtr = (u64*)inPtr;
                    while((u32)tempPtr < inSize)
                         *(tempPtr++) = tempVal;
               }
          } else {
               u128 tempVal;
               tempVal.val[0] = 0x00000000; tempVal.val[1] = 0x00000000; tempVal.val[2] = 0x00000000; tempVal.val[3] = 0x00000000;
               u128* tempPtr = (u128*)inPtr;
               while((u32)tempPtr < inSize)
                    *(tempPtr++) = tempVal;
          }
     } else {
          u256 tempVal;
          tempVal.val[0] = 0x00000000; tempVal.val[1] = 0x00000000; tempVal.val[2] = 0x00000000; tempVal.val[3] = 0x00000000;
          tempVal.val[4] = 0x00000000; tempVal.val[5] = 0x00000000; tempVal.val[6] = 0x00000000; tempVal.val[7] = 0x00000000;
          u256* tempPtr = (u256*)inPtr;
          while((u32)tempPtr < inSize)
               *(tempPtr++) = tempVal;
     }
     
     #if FL_MEMORY_ERRORBYTES
     #if FL_DEBUG_CALLEDFROM
     memtErrorBytesCheckFrom(&memErrorTree, inFile, inFunc, inLine);
     #else
     memtErrorBytesCheck(&memErrorTree);
     #endif
     #endif
}

#if FL_DEBUG_CALLEDFROM
void memCopyFrom(void* inDest, void* inSrc, unsigned int inSize, const char* inFile, const char* inFunc, int inLine) {
#else
void memCopy(void* inDest, void* inSrc, unsigned int inSize) {
#endif
     if(!inDest || !inSrc || !inSize) {
          #if FL_DEBUG_WARNING
          char tempString[256];
          tempString[0] = 0;
          if(!inDest)
               strcat(tempString, "Trying to memCopy to a NULL destination.\n");
          if(!inSrc)
               strcat(tempString, "Trying to memCopy from a NULL source.\n");
          if(!inSize)
               strcat(tempString, "Trying to memCopy 0 bytes of memory.\n");
          
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, tempString);
          #else
          debugWarning(tempString);
          #endif
          #endif
          return;
     }
     
     if((((u32)inDest + inSize) < (u32)inDest) || (((u32)inSrc + inSize) < (u32)inSrc)) {
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memCopy) Unsigned integer overflow while copying data.");
          #else
          debugWarning("Unsigned integer overflow while copying data.");
          #endif
          #endif
          return;
     }
     
     // Handle overlaps
     if((((u32)inSrc + inSize) > (u32)inDest) && (((u32)inSrc + inSize) <= ((u32)inDest + inSize))) {
          u32 tempSize = (((u32)inSrc + inSize) - (u32)inDest);
          void* tempBuffer = memQalloc(tempSize);
          memCopy(tempBuffer, (void*)((u32)inSrc + (inSize - tempSize)), tempSize);
          memCopy((void*)((u32)inDest + tempSize), inSrc, (inSize - tempSize));
          memCopy(inDest, tempBuffer, tempSize);
          memFree(tempBuffer);
          return;
     }
     
     if(!memBlockValid(inSrc, inSize) || !memBlockValid(inDest, inSize)) {
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memCopy) Trying to memCopy to/from an invalid memory block (0x%08X-0x%08X <- 0x%08X-0x%08X).", (unsigned int)inDest, ((unsigned int)inDest + inSize), (unsigned int)inSrc, ((unsigned int)inSrc + inSize));
          #else
          debugWarning("Trying to memCopy to/from an invalid memory block (0x%08X-0x%08X <- 0x%08X-0x%08X).", (unsigned int)inDest, ((unsigned int)inDest + inSize), (unsigned int)inSrc, ((unsigned int)inSrc + inSize));
          #endif
          #endif
          return;
     }
     
     inSize += (u32)inDest;
     if((inSize & 31) || ((u32)inDest & 31) || ((u32)inSrc & 31)) {
          if((inSize & 15) || ((u32)inDest & 15) || ((u32)inSrc & 15)) {
               if((inSize & 7) || ((u32)inDest & 7) || ((u32)inSrc & 7)) {
                    if((inSize & 3) || ((u32)inDest & 3) || ((u32)inSrc & 3)) {
                         if((inSize & 1) || ((u32)inDest & 1) || ((u32)inSrc & 1)) {
                              u8* tempDest = (u8*)inDest;
                              u8* tempSrc = (u8*)inSrc;
                              while((u32)tempDest < inSize)
                                   *(tempDest++) = *(tempSrc++);
                         } else {
                              u16* tempDest = (u16*)inDest;
                              u16* tempSrc = (u16*)inSrc;
                              while((u32)tempDest < inSize)
                                   *(tempDest++) = *(tempSrc++);
                         }
                    } else {
                         u32* tempDest = (u32*)inDest;
                         u32* tempSrc = (u32*)inSrc;
                         while((u32)tempDest < inSize)
                              *(tempDest++) = *(tempSrc++);
                    }
               } else {
                    u64* tempDest = (u64*)inDest;
                    u64* tempSrc = (u64*)inSrc;
                    while((u32)tempDest < inSize)
                         *(tempDest++) = *(tempSrc++);
               }
          } else {
               u128* tempDest = (u128*)inDest;
               u128* tempSrc = (u128*)inSrc;
               while((u32)tempDest < inSize)
                    *(tempDest++) = *(tempSrc++);
          }
     } else {
          u256* tempDest = (u256*)inDest;
          u256* tempSrc = (u256*)inSrc;
          while((u32)tempDest < inSize)
               *(tempDest++) = *(tempSrc++);
     }

     #if FL_MEMORY_ERRORBYTES
     #if FL_DEBUG_CALLEDFROM
     memtErrorBytesCheckFrom(&memErrorTree, inFile, inFunc, inLine);
     #else
     memtErrorBytesCheck(&memErrorTree);
     #endif
     #endif
}

#if FL_DEBUG_CALLEDFROM
int memCompareFrom(void* inSrc0, void* inSrc1, unsigned int inSize, const char* inFile, const char* inFunc, int inLine) {
#else
int memCompare(void* inSrc0, void* inSrc1, unsigned int inSize) {
#endif
     if(!inSrc0 || !inSrc1 || !inSize) {
          #if FL_DEBUG_WARNING
          char tempString[256];
          tempString[0] = 0;
          if(!inSrc0 || !inSrc1)
               strcat(tempString, "Trying to memCompare to a NULL region.\n");
          if(!inSize)
               strcat(tempString, "Trying to memCompare 0 bytes of memory.\n");
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, tempString);
          #else
          debugWarning(tempString);
          #endif
          #endif
          return -1;
     }
     
     if((((u32)inSrc0 + inSize) < (u32)inSrc1) || (((u32)inSrc0 + inSize) < (u32)inSrc1)) {
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memCompare) Unsigned integer overflow while comparing data.");
          #else
          debugWarning("Unsigned integer overflow while comparing data.");
          #endif
          #endif
          return -1;
     }
     
     if(!memBlockValid(inSrc0, inSize) || !memBlockValid(inSrc1, inSize)) {
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memCompare) Trying to memCompare to/from an invalid memory block (0x%08X-0x%08X <- 0x%08X-0x%08X).", (unsigned int)inSrc0, ((unsigned int)inSrc0 + inSize), (unsigned int)inSrc1, ((unsigned int)inSrc1 + inSize));
          #else
          debugWarning("Trying to memCompare to/from an invalid memory block (0x%08X-0x%08X <- 0x%08X-0x%08X).", (unsigned int)inSrc0, ((unsigned int)inSrc0 + inSize), (unsigned int)inSrc1, ((unsigned int)inSrc1 + inSize));
          #endif
          #endif
          return -1;
     }
     
     int tempOut = memcmp(inSrc0, inSrc1, inSize);

     #if FL_MEMORY_ERRORBYTES
     #if FL_DEBUG_CALLEDFROM
     memtErrorBytesCheckFrom(&memErrorTree, inFile, inFunc, inLine);
     #else
     memtErrorBytesCheck(&memErrorTree);
     #endif
     #endif
     
     return tempOut;
}

#if FL_DEBUG_CALLEDFROM
void* memQallocFrom(int inSize, const char* inFile, const char* inFunc, int inLine) {
#else
void* memQalloc(int inSize) {
#endif
     #if FL_MEMORY_SCRATCHPAD     
     if(memInitialized) {
          if((inSize <= MEMORY_SCRATCHPAD_SIZE) && !memScratchpadUsed) {
               memScratchpadUsed = true;
               return (void*)MEMORY_SCRATCHPAD_BASE;
          }
     }
     #endif
     
     void* tempOut;
     
     #if FL_MEMORY_VMEM
     if(vmemInitialized) {
          tempOut = vmemAlloc(inSize);
          if(tempOut)
               return tempOut;
     }
     #endif
     
     tempOut = memAlloc(inSize);
     
     #if FL_DEBUG_WARNING
     if(!tempOut) {
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "Error allocating memory using memQalloc.\nMemory full/fragmented.");
          #else
          debugWarningFrom("Error allocating memory using memQalloc.\nMemory full/fragmented.");
          #endif
     }
     #endif
     
     return tempOut;
}

#if FL_DEBUG_CALLEDFROM
void* memCallocFrom(int inSize0, int inSize1, const char* inFile, const char* inFunc, int inLine) {
     void* tempOut = memAlignFrom(FL_MEMORY_ALIGNMENT_DEFAULT, (inSize0 * inSize1), inFile, inFunc, inLine);
#else
void* memCalloc(int inSize0, int inSize1) {
     void* tempOut = memAlign(FL_MEMORY_ALIGNMENT_DEFAULT, (inSize0 * inSize1));
#endif
     if(!tempOut)
          return NULL;
     #if FL_DEBUG_CALLEDFROM
     memClearFrom(tempOut, (inSize0 * inSize1), inFile, inFunc, inLine);
     #else
     memClear(tempOut, (inSize0 * inSize1));
     #endif
     return tempOut;
}

#if FL_DEBUG_CALLEDFROM
void* memAllocUncachedFrom(int inSize, const char* inFile, const char* inFunc, int inLine) {
#else
void* memAllocUncached(int inSize) {
#endif
     u32 tempAlignment = max(64, FL_MEMORY_ALIGNMENT_DEFAULT);
     void* tempOut = memAlign(tempAlignment, inSize);
     if(!tempOut) {
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memAllocUncached) Error allocating uncached memory, memory is probably full/fragmented.");
          #else
          debugWarning("Error allocating uncached memory, memory is probably full/fragmented.");
          #endif
          #endif
          return NULL;
     }
     #if FL_MEMORY_ERRORBYTES
     if((inSize + (tempAlignment << 1)) >= 16384)
          sceKernelDcacheWritebackInvalidateAll();
     else
          sceKernelDcacheWritebackInvalidateRange((void*)((unsigned int)tempOut - tempAlignment), (inSize + (tempAlignment << 1)));
     #else
     if(inSize >= 16384)
          sceKernelDcacheWritebackInvalidateAll();
     else
          sceKernelDcacheWritebackInvalidateRange(tempOut, inSize);
     #endif
     tempOut = memUncachedPtr(tempOut);
     return tempOut;
}

#if FL_DEBUG_CALLEDFROM
void* memReallocFrom(void* inData, int inSize, const char* inFile, const char* inFunc, int inLine) {
#else
void* memRealloc(void* inData, int inSize) {
#endif
     #if FL_MEMORY_VMEM
     if(vmemPointer(inData))
          return vmemRealloc(inData, inSize);
     #endif
     if(!inSize) {
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memRealloc) Trying to re-allocate to 0 bytes.");
          #else
          debugWarning("Trying to re-allocate to 0 bytes.");
          #endif
          #endif
          return NULL;
     }
     #if FL_MEMORY_ERRORBYTES
     if(!memErrorBytesCheck(inData)) {
          #if FL_DEBUG_ERROR
          #if FL_DEBUG_CALLEDFROM
          debugErrorFrom(inFile, inFunc, inLine, "Error re-allocating memory, original data was corrupt.");
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
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memRealloc) Error re-allocating memory, memory is probably full/fragmented.");
          #else
          debugWarning("Error re-allocating memory, memory is probably full/fragmented.");
          #endif
          #endif
          return NULL;
     }
     
     #if FL_DEBUG_CALLEDFROM
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
     
     #if FL_DEBUG_CALLEDFROM
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
     #if FL_DEBUG_WARNING
     if(!tempOut) {
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memRealloc) Error re-allocating memory, memory is probably full/fragmented.");
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

#if FL_DEBUG_CALLEDFROM
void* memAlignFrom(int inBoundry, int inSize, const char* inFile, const char* inFunc, int inLine) {
#else
void* memAlign(int inBoundry, int inSize) {
#endif
     if(!inSize) {
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memAlign) Trying to allocate aligned data of 0 bytes.");
          #else
          debugWarning("Trying to allocate aligned data of 0 bytes.");
          #endif
          #endif
          return NULL;
     }
     if(!inBoundry) {
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memAlign) Trying to align to 0 byte boundry.\nReverting to default allocation alignment.");
          #else
          debugWarning("Trying to align to 0 byte boundry.\nReverting to default allocation alignment.");
          #endif
          #endif
     }
     
     if(inBoundry < FL_MEMORY_ALIGNMENT_DEFAULT)
          inBoundry = FL_MEMORY_ALIGNMENT_DEFAULT;

     int i, tempBoundZeroCnt = 0, tempBoundMaxPower = 0;
     for(i = 0; i < (sizeof(void*) << 3); i++) {
          if(inBoundry & (1 << i)) {
               tempBoundZeroCnt++;
               tempBoundMaxPower = i;
          }
     }
     if(tempBoundZeroCnt > 1) {
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memAlign) Trying to align to a non-binary boundry.\nReverting to next valid boundry.");
          #else
          debugWarning("Trying to align to a non-binary boundry.\nReverting to next valid boundry.");
          #endif
          #endif
          inBoundry = (1 << (tempBoundMaxPower + 1));
     }
     
     while(inSize & (inBoundry - 1))
          inSize++;

     void* tempOut;

     #if FL_MEMORY_VMEM_STRIDE_FILL
     tempOut = vmemStrideAlign(inBoundry, inSize);
     if(tempOut)
          return tempOut;
     #endif          

     #if FL_MEMORY_ERRORBYTES
     tempOut = memalign(inBoundry, (inSize + (inBoundry << 1)));
     if(!tempOut) {
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memAlign) Error allocating aligned memory, memory is probably full/fragmented.");
          #else
          debugWarning("Error allocating aligned memory, memory is probably full/fragmented.");
          #endif
          #endif
          return NULL;
     }
     #if FL_DEBUG_CALLEDFROM
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
     
     #if FL_DEBUG_CALLEDFROM
     memtErrorBytesCheckFrom(&memErrorTree, inFile, inFunc, inLine);
     #else
     memtErrorBytesCheck(&memErrorTree);
     #endif
     #else
     tempOut = memalign(inBoundry, inSize);
     #if FL_DEBUG_WARNING
     if(!tempOut) {
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memAlign) Error allocating aligned memory, memory is probably full/fragmented.");
          #else
          debugWarning("Error allocating aligned memory, memory is probably full/fragmented.");
          #endif
     }
     #endif
     #endif
     
     #if FL_MEMORY_VMEM_OVERFLOW
     if(!tempOut && (inBoundry < FL_MEMORY_VMEM_BLOCKSIZE))
          tempOut = vmemAlloc(inSize);
     #endif
     
     if(!tempOut && !memScratchpadUsed) {
          memScratchpadUsed = true;
          tempOut = (void*)MEMORY_SCRATCHPAD_BASE;
     }
     
     return tempOut;
}

#if FL_DEBUG_CALLEDFROM
void memFreeFrom(void* inData, const char* inFile, const char* inFunc, int inLine) {
#else
void memFree(void* inData) {
#endif
     if(!inData) {
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memFree) Trying to free NULL pointer.");
          #else
          debugWarning("Trying to free NULL pointer.");
          #endif
          #endif
          return;
     }
     if(!memAddressValid(inData)) {
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memFree) Trying to free invalid pointer (0x%08X).", (unsigned int)inData);
          #else
          debugWarning("Trying to free invalid pointer (0x%08X).", (unsigned int)inData);
          #endif
          #endif
          return;
     }
     if(((u32)inData >= MEMORY_SCRATCHPAD_BASE) && ((u32)inData < (MEMORY_SCRATCHPAD_BASE + MEMORY_SCRATCHPAD_SIZE))) {
          memScratchpadUsed = false;
          return;
     }
     #if FL_MEMORY_VMEM
     if(vmemPointer(inData)) {
          #if FL_DEBUG_CALLEDFROM
          return vmemFreeFrom(inData, inFile, inFunc, inLine);
          #else
          return vmemFree(inData);
          #endif
     }
     #endif
     #if FL_MEMORY_ERRORBYTES
     if(!memErrorBytesCheck(inData)) {
          #if FL_DEBUG_ERROR
          #if FL_DEBUG_CALLEDFROM
          debugErrorFrom(inFile, inFunc, inLine, "(memFree) Cannot free pointer, data corrupt.");
          #else
          debugError("Cannot free pointer, data corrupt.");
          #endif
          #endif
          return;
     }
     u32* tempData = (u32*)((unsigned int)inData - 16);
     inData = (void*)((unsigned int)inData - tempData[0]);
     if(!inData) {
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memFree) Trying to free NULL pointer.");
          #else
          debugWarning("Trying to free NULL pointer.");
          #endif
          #endif
          return;
     }
     #endif
     #if FL_MEMORY_ERRORBYTES
     #if FL_DEBUG_CALLEDFROM
     memtDelFrom(&memErrorTree, inData, inFile, inFunc, inLine);
     memtErrorBytesCheckFrom(&memErrorTree, inFile, inFunc, inLine);
     #else
     memtDel(&memErrorTree, inData);
     memtErrorBytesCheck(&memErrorTree);
     #endif
     #endif
     free(inData);
}

#if FL_DEBUG_CALLEDFROM
void* memUncachedPtrFrom(void* inPtr, const char* inFile, const char* inFunc, int inLine) {
#else
void* memUncachedPtr(void* inPtr) {
#endif
     if(!inPtr) {
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memUncachedPtr) Trying to create uncached null pointer.");
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

#if FL_MEMORY_PAGE
unsigned int memToPage(void* inData, unsigned int inSize) {
     if(!inData) {
          #if FL_DEBUG_WARNING
          debugWarning("Trying to page NULL pointer.");
          #endif
          return 0;
     }
     if(!memBlockValid(inData, inSize)) {
          #if FL_DEBUG_WARNING
          debugWarning("Trying to page invalid memory block [%0x08X -> %0x08X].", (unsigned int)inData, ((unsigned int)inData + inSize));
          #endif
          return 0;
     }
     if(((unsigned int)inData >= MEMORY_SCRATCHPAD_BASE) && (inSize < MEMORY_SCRATCHPAD_SIZE)) {
          #if FL_DEBUG_WARNING
          debugWarning("Cannot page scratchpad data directly.");
          #endif
          return 0;
     }

     FILE* tempFile = fopen(MEMORY_PAGE_PATH, "ab");
     if(!tempFile) {
          #if FL_DEBUG_ERROR
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
          #if FL_DEBUG_WARNING
          debugWarning("Trying to retrieve page 0 (Error page).");
          #endif
          return NULL;
     }
     FILE* tempFile = fopen(MEMORY_PAGE_PATH, "rb");
     if(!tempFile) {
          #if FL_DEBUG_ERROR
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
                    #if FL_DEBUG_ERROR
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
     #if FL_DEBUG_ERROR
     debugError("Page with matching PageID not found within page file.");
     #endif
     return NULL;
}
#endif

#if FL_MEMORY_ERRORBYTES
#if FL_DEBUG_CALLEDFROM
bool memErrorBytesCheckFrom(void* inPtr, const char* inFile, const char* inFunc, int inLine) {
#else
bool memErrorBytesCheck(void* inPtr) {
#endif
     u32 tempBoundry;
     u32 tempSize;
     u32* tempPtr = (u32*)((unsigned int)inPtr - 16);
     tempBoundry = tempPtr[0];
     tempSize = tempPtr[1];
     if((tempSize & (tempBoundry - 1)) || ((u32)inPtr & (tempBoundry - 1))) {
          #if FL_DEBUG_ERROR
          #if FL_DEBUG_CALLEDFROM
          debugErrorFrom(inFile, inFunc, inLine, "(memErrorBytesCheck) Memory integrity error, alignment doesn't match description.");
          #else
          debugError("Memory integrity error, alignment doesn't match description.");
          #endif
          #endif
          return false;
     }
     if((tempPtr[2] != 0xFEEBDAED) || (tempPtr[3] != 0x76543210)) {
          #if FL_DEBUG_ERROR
          #if FL_DEBUG_CALLEDFROM
          debugErrorFrom(inFile, inFunc, inLine, "(memErrorBytesCheck) Memory integrity error, underflow.");
          #else
          debugError("Memory integrity error, underflow.");
          #endif
          #endif
          return false;
     }
     tempPtr = (u32*)((unsigned int)inPtr + tempSize);
     if((tempPtr[0] != 0xDEADBEEF) || (tempPtr[1]!= 0x01234567) || (tempPtr[2] != tempBoundry) || (tempPtr[3] != tempSize)) {
          #if FL_DEBUG_ERROR
          #if FL_DEBUG_CALLEDFROM
          debugErrorFrom(inFile, inFunc, inLine, "(memErrorBytesCheck) Memory integrity error, probably overflow, small chance of underflow.");
          #else
          debugError("Memory integrity error, probably overflow, small chance of underflow.");
          #endif
          #endif
          return false;
     }
     return true;
}

#if FL_DEBUG_CALLEDFROM
void memtAddFrom(MemoryTree** inMemTree, void* inPtr, u32 inSize, u32 inAlignment, const char* inFile, const char* inFunc, int inLine) {
#else
void memtAdd(MemoryTree** inMemTree, void* inPtr, u32 inSize, u32 inAlignment) {
#endif
     if(!inPtr || !inSize || !inAlignment) {
          #if FL_DEBUG_ERROR
          #if FL_DEBUG_CALLEDFROM
          debugErrorFrom(inFile, inFunc, inLine, "(memAdd) Trying to add a node with invalid properties to a memory tree.");
          #else
          debugError("Trying to add a node with invalid properties to a memory tree.");
          #endif
          #endif
          return;
     }
     
     if(!memBlockValid(inPtr, (inSize + (inAlignment << 1)))) {
          #if FL_DEBUG_ERROR
          #if FL_DEBUG_CALLEDFROM
          debugErrorFrom(inFile, inFunc, inLine, "(memAdd) Trying to add a node that points to an invalid memory block to a memory tree.");
          #else
          debugError("Trying to add a node that points to an invalid memory block to a memory tree.");
          #endif
          #endif
          return;
     }
     
     if((inSize & (inAlignment - 1)) || ((u32)inPtr & (inAlignment - 1))) {
          #if FL_DEBUG_ERROR
          #if FL_DEBUG_CALLEDFROM
          debugErrorFrom(inFile, inFunc, inLine, "(memAdd) Trying to add a node that breaks it's own alignment to a memory tree.");
          #else
          debugError("Trying to add a node that breaks it's own alignment to a memory tree.");
          #endif
          #endif
          return;
     }
     
     if(!inMemTree) {
          #if FL_DEBUG_ERROR
          #if FL_DEBUG_CALLEDFROM
          debugErrorFrom(inFile, inFunc, inLine, "(memAdd) Trying to add a node to a NULL memory tree.");
          #else
          debugError("Trying to add a node to a NULL memory tree.");
          #endif
          #endif
          return;
     }
     
     MemoryTree* tempOut = malloc(sizeof(MemoryTree));
     if(!tempOut) {
          #if FL_DEBUG_ERROR
          #if FL_DEBUG_CALLEDFROM
          debugErrorFrom(inFile, inFunc, inLine, "(memAdd) Couldn't allocate node for memory tree.\nProbably out of memory.");
          #else
          debugError("Couldn't allocate node for memory tree.\nProbably out of memory.");
          #endif
          #endif
          return;
     }
     
     tempOut->memtData = inPtr;
     tempOut->memtSize = inSize;
     tempOut->memtAlignment = inAlignment;
     tempOut->memtNext = (*inMemTree ? *inMemTree : NULL);
     *inMemTree = tempOut;
}

#if FL_DEBUG_CALLEDFROM
void memtDelFrom(MemoryTree** inMemTree, void* inPtr, const char* inFile, const char* inFunc, int inLine) {
#else
void memtDel(MemoryTree** inMemTree, void* inPtr) {
#endif
     if(!inMemTree || !*inMemTree) {
          #if FL_DEBUG_ERROR
          #if FL_DEBUG_CALLEDFROM
          debugErrorFrom(inFile, inFunc, inLine, "(memDel) Trying to delete a node from a NULL memory tree.");
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
               #if FL_DEBUG_ERROR
               #if FL_DEBUG_CALLEDFROM
               debugErrorFrom(inFile, inFunc, inLine, "(memDel) Pointer not found within memory tree.");
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

#if FL_DEBUG_CALLEDFROM
void memtErrorBytesCheckFrom(MemoryTree** inMemTree, const char* inFile, const char* inFunc, int inLine) {
#else
void memtErrorBytesCheck(MemoryTree** inMemTree) {
#endif
     if(!inMemTree) {
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memtErrorBytesCheck) Trying to error check a NULL memory tree.");
          #else
          debugWarning("Trying to error check a NULL memory tree.");
          #endif
          #endif
          return;
     }
     if(!*inMemTree) {
          #if FL_DEBUG_WARNING
          #if FL_DEBUG_CALLEDFROM
          debugWarningFrom(inFile, inFunc, inLine, "(memtErrorBytesCheck) Trying to error check an empty memory tree.");
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
               #if FL_DEBUG_ERROR
               #if FL_DEBUG_CALLEDFROM
               debugErrorFrom(inFile, inFunc, inLine, "(memtErrorBytesCheck) Memory tree data corrupt.\nPoints to invalid memory block.");
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
               #if FL_DEBUG_ERROR
               #if FL_DEBUG_CALLEDFROM
               debugErrorFrom(inFile, inFunc, inLine, "(memtErrorBytesCheck) Memory tree data corrupt.\nBad alignment value.");
               #else
               debugError("Memory tree data corrupt.\nBad alignment value.");
               #endif
               #endif
               return;
          }
          
          if((u32)tempNode->memtData & (tempNode->memtAlignment - 1)) {
               #if FL_DEBUG_ERROR
               #if FL_DEBUG_CALLEDFROM
               debugErrorFrom(inFile, inFunc, inLine, "(memtErrorBytesCheck) Memory tree data corrupt.\nData is not aligned as described in memory tree.");
               #else
               debugError("Memory tree data corrupt.\nData is not aligned as described in memory tree.");
               #endif
               #endif
               return;
          }
          if((u32)tempNode->memtSize & (tempNode->memtAlignment - 1)) {
               #if FL_DEBUG_ERROR
               #if FL_DEBUG_CALLEDFROM
               debugErrorFrom(inFile, inFunc, inLine, "(memtErrorBytesCheck) Memory tree data corrupt.\nSize is not aligned as described in memory tree.");
               #else
               debugError("Memory tree data corrupt.\nSize is not aligned as described in memory tree.");
               #endif
               #endif
               return;
          }
          
          if(tempNode->memtNext && !memBlockValid(tempNode->memtNext, sizeof(MemoryTree))) {
               #if FL_DEBUG_ERROR
               #if FL_DEBUG_CALLEDFROM
               debugErrorFrom(inFile, inFunc, inLine, "(memtErrorBytesCheck) Memory tree data corrupt.\nPoints to invalid next node.");
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
               #if FL_DEBUG_ERROR
               #if FL_DEBUG_CALLEDFROM
               debugErrorFrom(inFile, inFunc, inLine, "(memtErrorBytesCheck) Memory integrity error, underflow.");
               #else
               debugError("Memory integrity error, underflow.");
               #endif
               #endif
               return;
          }
          tempPtr = (u32*)(((unsigned int)tempNode->memtData + tempNode->memtAlignment) + tempSize);
          if((tempPtr[0] != 0xDEADBEEF) || (tempPtr[1] != 0x01234567) || (tempPtr[2] != tempBoundry) || (tempPtr[3] != tempSize)) {
               #if FL_DEBUG_ERROR
               #if FL_DEBUG_CALLEDFROM
               debugErrorFrom(inFile, inFunc, inLine, "(memtErrorBytesCheck) Memory integrity error, overflow.");
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
