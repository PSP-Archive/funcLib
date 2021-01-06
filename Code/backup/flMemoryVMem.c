#include "flGlobal.h"

#if FL_MEMORY_VMEM != 0
#include <string.h>
#include <stdio.h>
#include <pspkernel.h>

#if FL_INCLUDE_ALL_C == 0
#include "flMemory.h"

#if FL_DEBUG != 0
#include "flDebug.h"
#endif
#endif

u32   vmemStart = MEMORY_VMEM_BASE;
u32   vmemSize = MEMORY_VMEM_SIZE;
u32*  vmemBlockTable = NULL;
u32   vmemBlocks = 0;
u32   vmemFreeBlocks = 0;
bool  vmemInitialized = false;

VmemStrideBlock* vmemStrideBlocks = NULL;

void  vmemInit() {
     // Allocation table entries
     // 00000000 00000000 00000000 00000000
     // AA       FFFFFFFF FFFFFFFF FFFFFFFF - For AA type 0
     // AA       PPPPPPPP PPPPPPPP PPPPPPPP - For AA type 1
     // AA       SSSSSSSS SSSSSSSS SSSSSSSS - For AA type 2
     // AA       SSSSSSSS PPPPPPPP PPPPPPPP - For AA type 3
     // A = Allocation type (0 - FreeBlockStart, 1 - FreeMidBlock, 2 - AllocatedStartBlock, 3 - AllocatedMidBlock).
     // S = Allocation length in blocks.
     // P = Block number of start block.
     // F = Number of free blocks following.
     
     if(vmemInitialized) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to initialize vram, but it is already initialized.");
          #endif
          return;
     }
     
     while(vmemStart & (FL_MEMORY_VMEM_BLOCKSIZE - 1)) {
          vmemStart++;
          vmemSize--;
     }
     
     //vmemStart |= MEMORY_UNCACHED_OFFSET;
     
     vmemBlocks = (vmemSize / FL_MEMORY_VMEM_BLOCKSIZE);
     vmemBlockTable = memAlloc(vmemBlocks << 2);
     vmemFreeBlocks = vmemBlocks;
     
     vmemBlockTable[0] = vmemBlocks;
     unsigned int i;
     for(i = 1; i < vmemBlocks; i++) {
          vmemBlockTable[i] = 0x40000000;
     }
     
     vmemInitialized = true;
}

void vmemTerm() {
     if(!vmemInitialized) {
         #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to de-initialize vram, but it is not initialized.");
          #endif
          return;
     }
     vmemInitialized = false;
     memFree(vmemBlockTable);
     vmemBlocks = 0;
     vmemSize = MEMORY_VMEM_SIZE;
     vmemStart = MEMORY_VMEM_BASE;
}

void* vmemNormalPtr(void* inPtr) {
     inPtr = memCachedPtr(inPtr);
     if((u32)inPtr < MEMORY_VMEM_SIZE)
          inPtr = (void*)((u32)inPtr + MEMORY_VMEM_BASE);
     if(((u32)inPtr < MEMORY_VMEM_BASE) || ((u32)inPtr >= (MEMORY_VMEM_BASE + (MEMORY_VMEM_SIZE << 2))))
          return NULL;
     while((u32)inPtr >= (MEMORY_VMEM_BASE + MEMORY_VMEM_SIZE))
          inPtr = (void*)((u32)inPtr - MEMORY_VMEM_SIZE);
     return inPtr;
}

void* vmemAllocBuffer(u32 inWidth, u32 inHeight, u8 inBitDepth, u32 inStride) {
      void* tempOut;
      if(vmemInitialized) {
           #if FL_DEBUG_DEVWARNING != 0
           debugDevWarning("Allocating buffers after vmem is initialized is bad practice.");
           #endif
           tempOut = vmemAlloc((inWidth * inHeight * inBitDepth) >> 3);
      } else {
           if(vmemSize < ((inWidth * inHeight * inBitDepth) >> 3)) {
                #if DEBUG_ERROR != 0
                debugError("Error allocating vram buffer.\nProgram will probably now be unstable.");
                #endif
                return (void*)(MEMORY_VMEM_SIZE << 2);
           }
           vmemSize -= ((inWidth * inHeight * inBitDepth) >> 3);
           tempOut = (void*)vmemStart;
           vmemStart += ((inWidth * inHeight * inBitDepth) >> 3);
      }
      if(inStride) {
           u32 tempStride = (((inWidth - inStride) * inBitDepth) >> 3);
           void* tempStart = (void*)((u32)tempOut + tempStride);
           u32 tempBlockSize = ((inStride * inBitDepth) >> 3);
           u32 tempBlockCount = inHeight;
           vmemStrideBlockAdd(tempStart, tempStride, tempBlockSize, tempBlockCount);
      }
      return vmemRelativePtr(tempOut);
}

#if FL_DEBUG_CALLEDFROM != 0
void* vmemAllocFrom(u32 inSize, const char* inFile, const char* inFunc, int inLine) {
#else
void* vmemAlloc(u32 inSize) {
#endif
     void* tempOut = vmemStrideAlloc(inSize);
     if(tempOut)
          return tempOut;

     if(inSize > (vmemFreeBlocks * FL_MEMORY_VMEM_BLOCKSIZE)) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("Error allocating video memory, not enough vmem free to attempt allocation.", inFile, inFunc, inLine);
          #else
          debugWarning("Error allocating video memory, not enough vmem free to attempt allocation.");
          #endif
          #endif
          return NULL;
     }
     
     while(inSize & (FL_MEMORY_VMEM_BLOCKSIZE - 1))
          inSize++;
     unsigned int tempBlocksNeeded = (inSize / FL_MEMORY_VMEM_BLOCKSIZE);
     int tempBestBlock = -1;
     unsigned int tempBestBlockSize = 0;
     unsigned int i = 0;
     while(i < vmemBlocks) {
          if(((vmemBlockTable[i] & 0xC0000000) >> 30) == 0) {
               if((vmemBlockTable[i] & 0x00FFFFFF) >= tempBlocksNeeded) {
                    if(((vmemBlockTable[i] & 0x00FFFFFF) < tempBestBlockSize) || (tempBestBlock < 0)){
                         tempBestBlock = i;
                         tempBestBlockSize = tempBlocksNeeded;
                         if((vmemBlockTable[i] & 0x00FFFFFF) == tempBlocksNeeded)
                              break;
                    }
               }
          }
          if(vmemBlockTable[i] & 0x40000000)
               i = (vmemBlockTable[i] & 0x00FFFFFF);
          else
               i += (vmemBlockTable[i] & 0x00FFFFFF);
     }
     if(tempBestBlock < 0) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("Error allocating video memory, memory is full/fragmented.", inFile, inFunc, inLine);
          #else
          debugWarning("Error allocating video memory, memory is full/fragmented.");
          #endif
          #endif
          return NULL;
     }
     if((vmemBlockTable[tempBestBlock] & 0x00FFFFFF) != tempBlocksNeeded) {
          vmemBlockTable[tempBestBlock + tempBlocksNeeded] = (vmemBlockTable[tempBestBlock] & 0x00FFFFFF) - tempBlocksNeeded;
          if(((vmemBlockTable[tempBestBlock] & 0x00FFFFFF) - tempBlocksNeeded) > 1) {
               for(i = (tempBestBlock + tempBlocksNeeded + 1); i < (tempBestBlock + (vmemBlockTable[tempBestBlock] & 0x00FFFFFF)); i++) {
                    vmemBlockTable[i] = (tempBestBlock + tempBlocksNeeded) | 0x40000000;
               }
          }
     }
     vmemBlockTable[tempBestBlock] = (tempBlocksNeeded | 0x80000000);
     if(tempBlocksNeeded > 1) {
          for(i = (tempBestBlock + 1); i < (tempBestBlock + tempBlocksNeeded); i++) {
               vmemBlockTable[i] = (tempBestBlock | 0xC0000000);
          }
     }
     tempOut = (void*)(vmemStart + (tempBestBlock * FL_MEMORY_VMEM_BLOCKSIZE));
     tempOut = vmemAbsolutePtr(tempOut);
     vmemFreeBlocks -= tempBlocksNeeded;
     
     return tempOut;
}

#if FL_DEBUG_CALLEDFROM != 0
void* vmemCallocFrom(u32 inSize0, u32 inSize1, const char* inFile, const char* inFunc, int inLine) {
#else     
void* vmemCalloc(u32 inSize0, u32 inSize1) {
#endif
     void* tempOut = vmemStrideCalloc(inSize0, inSize1);
     if(tempOut)
          return tempOut;
          
     #if FL_DEBUG_CALLEDFROM != 0     
     tempOut = vmemAllocFrom(inSize0 * inSize1, inFile, inFunc, inLine);
     #else
     tempOut = vmemAlloc(inSize0 * inSize1);
     #endif
     
     if(!tempOut)
          return NULL;
     memClear(tempOut, (inSize0 * inSize1));
     return tempOut;
}

#if FL_DEBUG_CALLEDFROM != 0
void* vmemReallocFrom(void* inData, u32 inSize, const char* inFile, const char* inFunc, int inLine) {
#else
void* vmemRealloc(void* inData, u32 inSize) {
#endif
     #if FL_DEBUG_DEVWARNING != 0
     #if FL_DEBUG_CALLEDFROM != 0
     debugDevWarningFrom("VRam reallocation not yet quickly implemented, reverting to vmemAlloc->memCopy->vmemFree combo.", inFile, inFunc, inLine);
     #else
     debugDevWarning("VRam reallocation not yet quickly implemented, reverting to vmemAlloc->memCopy->vmemFree combo.");
     #endif
     #endif
     void* tempOut = vmemAlloc(inSize);
     if(!tempOut) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom("VRam reallocation failed, memory is full/fragmented.", inFile, inFunc, inLine);
          #else
          debugWarning("VRam reallocation failed, memory is full/fragmented.");
          #endif
          #endif
          return NULL;
     }

     unsigned int tempStartBlock = ((((u32)memCachedPtr(vmemRelativePtr(inData))) - vmemStart) / FL_MEMORY_VMEM_BLOCKSIZE);
     if((((u32)memCachedPtr(vmemRelativePtr(inData))) < vmemStart) || (tempStartBlock >= vmemBlocks)) {
          #if FL_DEBUG_ERROR != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugErrorFrom("Trying to realloc invalid vram pointer, out of range.", inFile, inFunc, inLine);
          #else
          debugError("Trying to realloc invalid vram pointer, out of range.");
          #endif
          #endif
          return NULL;
     }
     if((vmemBlockTable[tempStartBlock] & 0xC0000000) != 0x80000000) {
          #if FL_DEBUG_ERROR != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugErrorFrom("Trying to realloc invalid vram pointer, middle of block.", inFile, inFunc, inLine);
          #else
          debugError("Trying to realloc invalid vram pointer, middle of block.");
          #endif
          #endif
          return NULL;
     }
     int tempLength = (vmemBlockTable[tempStartBlock] & 0x00FFFFFF);
     
     memCopy(tempOut, inData, (tempLength * FL_MEMORY_VMEM_BLOCKSIZE));
     
     vmemFree(inData);
     return tempOut;
}

#if FL_DEBUG_CALLEDFROM != 0
void vmemFreeFrom(void* inData, const char* inFile, const char* inFunc, int inLine) {
#else
void vmemFree(void* inData) {
#endif
     if(vmemStrideFree(inData))
          return;

     inData = vmemNormalPtr(inData);

     unsigned int tempStartBlock = ((((u32)vmemRelativePtr(inData)) - vmemStart) / FL_MEMORY_VMEM_BLOCKSIZE);
     if(!inData || (tempStartBlock >= vmemBlocks) || (((u32)vmemRelativePtr(inData)) < vmemStart)) {
          #if FL_DEBUG_ERROR != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugErrorFrom("Trying to free invalid vram pointer, out of range.", inFile, inFunc, inLine);
          #else
          debugError("Trying to free invalid vram pointer, out of range.");
          #endif
          #endif
          return;
     }
     if((vmemBlockTable[tempStartBlock] & 0xC0000000) != 0x80000000) {
          #if FL_DEBUG_ERROR != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugErrorFrom("Trying to free invalid vram pointer, middle of block.", inFile, inFunc, inLine);
          #else
          debugError("Trying to free invalid vram pointer, middle of block.");
          #endif
          #endif
          return;
     }
     unsigned int tempLength = (vmemBlockTable[tempStartBlock] & 0x00FFFFFF);
     vmemFreeBlocks += tempLength;
     
     u32 i;
     
     // Free block by clearing the MSB.
     for(i = tempStartBlock; i < (tempStartBlock + tempLength); i++) {
          vmemBlockTable[i] &= ~0x80000000;
     }
     
     // Join following free blocks to current free block.
     i = tempStartBlock;
     while(!(vmemBlockTable[i] & 0x80000000) && (i < (vmemSize / FL_MEMORY_VMEM_BLOCKSIZE))) {
          if((vmemBlockTable[i] & 0xC0000000) == 0) {
               tempLength += (vmemBlockTable[i] & 0x00FFFFFF);
               i += (vmemBlockTable[i] & 0x00FFFFFF);
          } else {
               tempLength++;
               i++;
          }
     }
    
     // Join previous free blocks to current free block.
     while((tempStartBlock > 0) && !(vmemBlockTable[tempStartBlock - 1] & 0x8000000)) {
          if((vmemBlockTable[tempStartBlock - 1] & 0xC000000) == 0) {
               tempStartBlock--;
               tempLength++;
          } else {
               tempStartBlock = (vmemBlockTable[tempStartBlock - 1] & 0x00FFFFFF);
               tempLength += vmemBlockTable[tempStartBlock];
          }
     }
     
     // Create full length free block.
     vmemBlockTable[tempStartBlock] = tempLength;
     if(tempLength > 1) {
          for(i = (tempStartBlock + 1); i < tempLength; i++) {
                vmemBlockTable[i] = tempStartBlock | 0x40000000;
          }
     }
     return;
}

inline u32 vmemFreeSpace() {
     return (vmemFreeBlocks * FL_MEMORY_VMEM_BLOCKSIZE);
}

u32 vmemLargestFreeBlock() {
     unsigned int i = 0;
     unsigned int tempOut = 0;
     while(i < vmemBlocks) {
          if(((vmemBlockTable[i] & 0xC0000000) >> 30) == 0) {
               if((vmemBlockTable[i] & 0x00FFFFFF) > tempOut)
                    tempOut = (vmemBlockTable[i] & 0x00FFFFFF);
          }
          if(vmemBlockTable[i] & 0x40000000)
               i = (vmemBlockTable[i] & 0x00FFFFFF);
          else
               i += (vmemBlockTable[i] & 0x00FFFFFF);
     }
     return (tempOut * FL_MEMORY_VMEM_BLOCKSIZE);
}

float vmemFragmentation() {
     unsigned int i = 0;
     unsigned int tempFragments = 0;
     while(i < vmemBlocks) {
          if(((vmemBlockTable[i] & 0xC0000000) >> 30) == 0) {
               tempFragments++;
          }
          if(vmemBlockTable[i] & 0x40000000)
               i = (vmemBlockTable[i] & 0x00FFFFFF);
          else
               i += (vmemBlockTable[i] & 0x00FFFFFF);
     }
     float tempOut = ((float)vmemFreeSpace() / (float)tempFragments);
     return (tempOut * FL_MEMORY_VMEM_BLOCKSIZE);
}

void vmemStrideBlockAdd(void* inStart, u32 inStride, u32 inBlockSize, u32 inBlockCount) {
     if(!inStride || !inBlockSize || !inBlockCount) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to create stride block with bad parameters.");
          #endif
          return;
     }
     inStart = vmemNormalPtr(inStart);
     if(!memBlockValid(inStart, (((inBlockSize + inStride) * inBlockCount) - inStride))) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Cannot create stride block from an invalid pointer or memory block.");
          #endif
          return;
     }

     VmemStrideBlock* tempOut = (VmemStrideBlock*)memAlloc(sizeof(VmemStrideBlock) + inBlockCount);
     if(!tempOut) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Failed to allocate node to add vmem stride block.");
          #endif
          return;
     }

     tempOut->vmsbStart = inStart;
     tempOut->vmsbStride = inStride;
     tempOut->vmsbBlockSize = inBlockSize;
     tempOut->vmsbBlockCount = inBlockCount;
     tempOut->vmsbBlockAlign = min(memAddressAlignment(inStart), memAddressAlignment((void*)(inStride + inBlockSize)));
     
     tempOut->vmsbBlockTable = (u8*)((u32)tempOut + sizeof(VmemStrideBlock));
     memClear(tempOut->vmsbBlockTable, tempOut->vmsbBlockCount);
     tempOut->vmsbLast = NULL;
     tempOut->vmsbNext = vmemStrideBlocks;
     if(tempOut->vmsbNext)
          ((VmemStrideBlock*)(tempOut->vmsbNext))->vmsbLast = tempOut;
     vmemStrideBlocks = tempOut;
}

void* vmemStrideAlign(u32 inAlign, u32 inSize) {
     if(!vmemStrideBlocks || !inSize)
          return NULL;
     if(!inAlign)
          inAlign = FL_MEMORY_ALIGNMENT_DEFAULT;

     VmemStrideBlock* tempBlock = vmemStrideBlocks;
     
     u32 i;
     while(flRunning) {
          if((tempBlock->vmsbBlockAlign >= inAlign) && (tempBlock->vmsbBlockSize >= inSize)) {
               for(i = 0; i < tempBlock->vmsbBlockCount; i++) {
                    if(!tempBlock->vmsbBlockTable[i]) {
                         tempBlock->vmsbBlockTable[i] = 1;
                         return (void*)((u32)tempBlock->vmsbStart + (i * (tempBlock->vmsbBlockSize + tempBlock->vmsbStride)));
                    }
               }
          }
          if(!tempBlock->vmsbNext)
               break;
          tempBlock = ((VmemStrideBlock*)(tempBlock->vmsbNext));
     }
     return NULL;
}

void* vmemStrideAlloc(u32 inSize) {
     return vmemStrideAlign(FL_MEMORY_ALIGNMENT_DEFAULT, inSize);
}

void* vmemStrideCalloc(u32 inSize0, u32 inSize1) {
     void* tempOut = vmemStrideAlloc(inSize0 * inSize1);
     memClear(tempOut, (inSize0 * inSize1));
     return tempOut;
}

bool vmemStrideFree(void* inData) {
     if(!vmemStrideBlocks)
          return false;

     inData = vmemNormalPtr(inData);
     if(!inData)
          return false;
     
     VmemStrideBlock* tempBlock = vmemStrideBlocks;
     u32 i;
     while(flRunning) {
          if(((u32)inData >= (u32)tempBlock->vmsbStart) && ((u32)inData < ((u32)tempBlock->vmsbStart + (((tempBlock->vmsbBlockSize + tempBlock->vmsbStride) * tempBlock->vmsbBlockCount) - tempBlock->vmsbStride)))) {
               for(i = 0; i < tempBlock->vmsbBlockCount; i++) {
                    if(((u32)tempBlock->vmsbStart + (i * (tempBlock->vmsbStride + tempBlock->vmsbBlockSize))) == (u32)inData) {
                         if(!tempBlock->vmsbBlockTable[i])
                              return false;
                         tempBlock->vmsbBlockTable[i] = 0;
                         return true;
                    }
               }
          }
          if(!tempBlock->vmsbNext)
               break;
          tempBlock = ((VmemStrideBlock*)(tempBlock->vmsbNext));
     }
     return false;
}

float vmemStrideUsage() {
     if(!vmemStrideBlocks)
          return 0.0f;

     VmemStrideBlock* tempBlock = vmemStrideBlocks;

     u32 tempSize = 0;
     u32 tempUsed = 0;
     u32 i;

     while(flRunning) {
          tempSize += (tempBlock->vmsbBlockSize * tempBlock->vmsbBlockCount);
          
          for(i = 0; i < tempBlock->vmsbBlockCount; i++) {
               if(tempBlock->vmsbBlockTable[i])
                    tempUsed += tempBlock->vmsbBlockSize;
          }
          
          if(!tempBlock->vmsbNext)
               break;

          tempBlock = ((VmemStrideBlock*)(tempBlock->vmsbNext));
     }
     
     if(!tempSize)
          return 0.0f;
     return ((float)tempUsed / (float)tempSize);
}

#endif
