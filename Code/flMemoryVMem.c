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

#if FL_MEMORY_VMEM_CACHE
void* vmemCache = (void*)((MEMORY_VMEM_BASE + MEMORY_VMEM_SIZE) - FL_MEMORY_VMEM_CACHE);
#endif

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
     
     #if FL_MEMORY_VMEM_CACHE
     vmemCache = (void*)memUncachedPtr(vmemAbsolutePtr(vmemAllocBuffer(FL_MEMORY_VMEM_CACHE, 1, 8, 0)));
     sceKernelDcacheWritebackInvalidateRange(vmemCache, FL_MEMORY_VMEM_CACHE);
     #endif
     
     while(vmemStart & (FL_MEMORY_VMEM_BLOCKSIZE - 1)) {
          vmemStart++;
          vmemSize--;
     }
     
     vmemBlocks = (vmemSize / FL_MEMORY_VMEM_BLOCKSIZE);
     vmemBlockTable = memAlloc(vmemBlocks << 2);
     vmemFreeBlocks = vmemBlocks;
     
     vmemBlockTable[0] = (vmemBlocks & 0x00FFFFFF);
     
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
     vmemStrideBlockDelRecursive(vmemStrideBlocks);
}

void* vmemNormalPtr(void* inPtr) {
     inPtr = memCachedPtr(inPtr);
     
     // The following assumes that any pointer passed in that is under MEMORY_VMEM_BASE
     // is a relative pointer, however assumptions are bad.
     //if((u32)inPtr < MEMORY_VMEM_SIZE)
     //   inPtr = (void*)((u32)inPtr + MEMORY_VMEM_BASE);*/
     
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

     if(!vmemInitialized) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom(inFile, inFunc, inLine, "Trying to allocate vram before vram is initialized.");
          #else
          debugWarning("Trying to allocate vram before vram is initialized.");
          #endif
          #endif
          return NULL;
     }

     if(inSize > (vmemFreeBlocks * FL_MEMORY_VMEM_BLOCKSIZE)) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom(inFile, inFunc, inLine, "Error allocating video memory, not enough vmem free to attempt allocation.");
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
     
     // Find best memory chunk to allocate.
     while(i < vmemBlocks) {
          if(((vmemBlockTable[i] & 0xC0000000) == 0) && ((vmemBlockTable[i] & 0x00FFFFFF) >= tempBlocksNeeded)) {
               if(((vmemBlockTable[i] & 0x00FFFFFF) < tempBestBlockSize) || (tempBestBlock < 0)){
                    tempBestBlock = i;
                    tempBestBlockSize = (vmemBlockTable[i] & 0x00FFFFFF);
                    if(tempBestBlockSize == tempBlocksNeeded)
                         break;
               }
          }
          if(vmemBlockTable[i] & 0x40000000) {
               #if FL_DEBUG_WARNING != 0
               #if FL_DEBUG_CALLEDFROM != 0
               debugWarningFrom(inFile, inFunc, inLine, "Error while walking vmem, block table corrupt.");
               #else
               debugWarning("Error while walking vmem, block table corrupt.");
               #endif
               #endif
               return NULL;
          } else
               i += (vmemBlockTable[i] & 0x00FFFFFF);
     }
     
     // If no suitable memory chunk was found.
     if(tempBestBlock < 0) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom(inFile, inFunc, inLine, "Error allocating video memory, memory is fragmented.");
          #else
          debugWarning("Error allocating video memory, memory is fragmented.");
          #endif
          #endif
          return NULL;
     }

     // Split block between allocation and remaining free memory.
     vmemBlockTable[tempBestBlock] = ((tempBlocksNeeded & 0x00FFFFFF) | 0x80000000);
     if(tempBlocksNeeded > 1)
          vmemBlockTable[tempBestBlock + tempBlocksNeeded - 1] = ((tempBestBlock & 0x00FFFFFF) | 0xC0000000);
     if(tempBestBlockSize != tempBlocksNeeded) {
          vmemBlockTable[tempBestBlock + tempBlocksNeeded] = ((tempBestBlockSize - tempBlocksNeeded) & 0x00FFFFFF);
          if((tempBestBlockSize - tempBlocksNeeded) > 1)
               vmemBlockTable[tempBestBlock + tempBestBlockSize - 1] = (((tempBestBlock + tempBlocksNeeded) & 0x00FFFFFF) | 0x40000000);
     }
    
     // Calculate pointer value
     tempOut = (void*)(vmemStart + (tempBestBlock * FL_MEMORY_VMEM_BLOCKSIZE));
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
     debugDevWarningFrom(inFile, inFunc, inLine, "VRam reallocation not yet quickly implemented, reverting to vmemAlloc->memCopy->vmemFree combo.");
     #else
     debugDevWarning("VRam reallocation not yet quickly implemented, reverting to vmemAlloc->memCopy->vmemFree combo.");
     #endif
     #endif
     void* tempOut = vmemAlloc(inSize);
     if(!tempOut) {
          #if FL_DEBUG_WARNING != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugWarningFrom(inFile, inFunc, inLine, "VRam reallocation failed, memory is full/fragmented.");
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
          debugErrorFrom(inFile, inFunc, inLine, "Trying to realloc invalid vram pointer, out of range.");
          #else
          debugError("Trying to realloc invalid vram pointer, out of range.");
          #endif
          #endif
          return NULL;
     }
     if((vmemBlockTable[tempStartBlock] & 0xC0000000) != 0x80000000) {
          #if FL_DEBUG_ERROR != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugErrorFrom(inFile, inFunc, inLine, "Trying to realloc invalid vram pointer, middle of block.");
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
     if(!vmemInitialized)
          return;

     if(vmemStrideFree(inData))
          return;

     inData = vmemNormalPtr(inData);

     if(!inData || (((u32)inData - vmemStart) % FL_MEMORY_VMEM_BLOCKSIZE) || (((u32)inData - vmemStart) >= vmemSize) || ((u32)inData < vmemStart)) {
          #if FL_DEBUG_ERROR != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugErrorFrom(inFile, inFunc, inLine, "Trying to free invalid vram pointer, out of range.");
          #else
          debugError("Trying to free invalid vram pointer, out of range.");
          #endif
          #endif
          return;
     }

     u32 tempStartBlock = (((u32)inData - vmemStart) / FL_MEMORY_VMEM_BLOCKSIZE);
     u32 tempLength = (vmemBlockTable[tempStartBlock] & 0x00FFFFFF);
     if((vmemBlockTable[tempStartBlock] & 0xC0000000) != 0x80000000) {
          #if FL_DEBUG_ERROR != 0
          #if FL_DEBUG_CALLEDFROM != 0
          debugErrorFrom(inFile, inFunc, inLine, "Trying to free invalid vram pointer, middle of block or not allocated.");
          #else
          debugError("Trying to free invalid vram pointer, middle of block or not allocated.");
          #endif
          #endif
          return;
     }
     
     vmemFreeBlocks += tempLength;

     // If there is a free block after it then join onto that.
     if((tempStartBlock + tempLength) < vmemBlocks) {
          if((vmemBlockTable[tempStartBlock + tempLength] & 0xC0000000) == 0x00000000) {
               tempLength += (vmemBlockTable[tempStartBlock + tempLength] & 0x00FFFFFF);
          }
     }
     
     // If there is a free block before it then join onto that.
     if(tempStartBlock > 0) {
          if((vmemBlockTable[tempStartBlock - 1] & 0xC0000000) == 0x00000000) {
               tempStartBlock--;
               tempLength++;
          } else if((vmemBlockTable[tempStartBlock - 1] & 0xC0000000) == 0x40000000) {
               tempStartBlock = (vmemBlockTable[tempStartBlock - 1] & 0x00FFFFFF);
               tempLength += (vmemBlockTable[tempStartBlock] & 0x00FFFFFF);
          }
     }
     
     // Create full length free block.
     vmemBlockTable[tempStartBlock] = tempLength;
     vmemBlockTable[tempStartBlock + tempLength - 1] = (tempStartBlock | 0x40000000);
     
     return;
}

u32 vmemFreeSpace() {
     return ((vmemFreeBlocks * FL_MEMORY_VMEM_BLOCKSIZE) + vmemStrideFreeSpace());
}

u32 vmemLargestFreeBlock() {
     unsigned int i = 0;
     unsigned int tempOut = 0;
     while(i < vmemBlocks) {
          if((vmemBlockTable[i] & 0xC0000000) == 0) {
               if((vmemBlockTable[i] & 0x00FFFFFF) > tempOut)
                    tempOut = (vmemBlockTable[i] & 0x00FFFFFF);
          }
          if(vmemBlockTable[i] & 0x40000000) {
               #if FL_DEBUG_WARNING != 0
               debugWarning("Error while walking vmem, block table corruption.");
               #endif
               return 0;
          } else
               i += (vmemBlockTable[i] & 0x00FFFFFF);
     }
     return (tempOut * FL_MEMORY_VMEM_BLOCKSIZE);
}

float vmemFragmentation() {
     if(!vmemInitialized)
          return -1.0f;
     unsigned int i = 0;
     unsigned int tempFragments = 0;
     while(i < vmemBlocks) {
          if((vmemBlockTable[i] & 0xC0000000) == 0)
               tempFragments++;
          if(vmemBlockTable[i] & 0x40000000) {
               #if FL_DEBUG_WARNING != 0
               debugWarning("Error while walking vmem, block table corruption.");
               #endif
               return -1.0f;
          } else
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

void vmemStrideBlockDel(VmemStrideBlock* inBlock) {
     if(!inBlock)
          return;
     if(!inBlock->vmsbLast)
          vmemStrideBlocks = (VmemStrideBlock*)inBlock->vmsbNext;
     else
          inBlock->vmsbLast = inBlock->vmsbNext;
     memFree(inBlock);
}

void vmemStrideBlockDelRecursive(VmemStrideBlock* inBlock) {
     if(!inBlock)
          return;
     if(inBlock->vmsbLast) {
          vmemStrideBlockDelRecursive((VmemStrideBlock*)inBlock->vmsbLast);
          return;
     }
     if(inBlock->vmsbNext)
          vmemStrideBlockDelRecursive((VmemStrideBlock*)inBlock->vmsbNext);
     memFree(inBlock);
}

void* vmemStrideAlign(u32 inAlign, u32 inSize) {
     if(!vmemStrideBlocks || !inSize)
          return NULL;
     if(!inAlign)
          inAlign = FL_MEMORY_ALIGNMENT_DEFAULT;

     VmemStrideBlock* tempBlock = vmemStrideBlocks;
     
     VmemStrideBlock* tempBestBlock = NULL;
     u32              tempBestIndex = 0;
     
     u32 i;
     while(flRunning) {
          if((tempBlock->vmsbBlockAlign >= inAlign) && (tempBlock->vmsbBlockSize >= inSize)) {
               if(!tempBestBlock || (tempBlock->vmsbBlockSize < tempBestBlock->vmsbBlockSize)) {
                    for(i = 0; i < tempBlock->vmsbBlockCount; i++) {
                         if(!tempBlock->vmsbBlockTable[i]) {
                              tempBestBlock = tempBlock;
                              tempBestIndex = i;
                              break;
                         }
                    }
               }
          }
          if(!tempBlock->vmsbNext)
               break;
          tempBlock = ((VmemStrideBlock*)(tempBlock->vmsbNext));
     }
 
     if(!tempBestBlock)
          return NULL;
 
     tempBestBlock->vmsbBlockTable[tempBestIndex] = 1;
     return (void*)((u32)tempBestBlock->vmsbStart + (tempBestIndex * (tempBestBlock->vmsbBlockSize + tempBestBlock->vmsbStride)));
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
               #if FL_DEBUG_WARNING != 0
               debugWarning("Trying to free an invalid stride block");
               #endif
               return true;
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

u32 vmemStrideFreeSpace() {
     if(!vmemStrideBlocks)
          return 0;

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

     return (tempSize - tempUsed);
}

#endif
