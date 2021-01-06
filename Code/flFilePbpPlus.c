#include "flGlobal.h"
#if FL_FILE_PBPPLUS
#include <stdio.h>
#include <string.h>

#if !FL_INCLUDE_ALL_C
#include "flFilePbpPlus.h"
#include "flMemory.h"
#include "flDebug.h"
#endif

typedef struct {
     char         pbpPlusIdentifier[4];
     unsigned int pbpPlusVersion;
     unsigned int pbpPlusEntries;
     unsigned int pbpPlusReserved;
} pbpPlusHeader;

typedef struct {
     char         pbpPlusDataName[64];
     unsigned int pbpPlusDataOffset;
     unsigned int pbpPlusDataLength;
     unsigned int pbpPlusDataReserved[2];
} pbpPlusDataHeader;

typedef struct {
     unsigned int       pbpPlusFilesCount;
     pbpPlusDataHeader* pbpPlusFiles;
     char               pbpPlusPath[120];
} pbpPlusContext;

void* filePbpPlusOpen(char* inPBP) {
     FILE* tempFile = fopen(inPBP, "rb");
     if(!tempFile)
          return NULL;
     fseek(tempFile, 40, SEEK_SET);
     
     pbpPlusHeader tempHeader;
     fread(tempHeader.pbpPlusIdentifier, 1, 4, tempFile);
     fread(&tempHeader.pbpPlusVersion, 1, 4, tempFile);
     fread(&tempHeader.pbpPlusEntries, 1, 4, tempFile);
     fread(&tempHeader.pbpPlusReserved, 1, 4, tempFile);
     if(strncmp(tempHeader.pbpPlusIdentifier, "PBPP", 4) || tempHeader.pbpPlusVersion || !tempHeader.pbpPlusEntries) {
          fclose(tempFile);
          return NULL;
     }
     
     pbpPlusContext* tempOut = memAlloc(sizeof(pbpPlusContext));
     if(!tempOut) {
          fclose(tempFile);
          return NULL;
     }     
     tempOut->pbpPlusFilesCount = tempHeader.pbpPlusEntries;
     tempOut->pbpPlusFiles = memAlloc(tempHeader.pbpPlusEntries * sizeof(pbpPlusDataHeader));
     strcpy(tempOut->pbpPlusPath, inPBP);
     if(!tempOut->pbpPlusFiles) {
          memFree(tempOut);
          fclose(tempFile);
          return NULL;
     }
     
     int i;
     for(i = 0; i < tempHeader.pbpPlusEntries; i++) {
          fread(tempOut->pbpPlusFiles[i].pbpPlusDataName, 1, 64, tempFile);
          fread(&tempOut->pbpPlusFiles[i].pbpPlusDataOffset, 1, 4, tempFile);
          fread(&tempOut->pbpPlusFiles[i].pbpPlusDataLength, 1, 4, tempFile);
          fread(tempOut->pbpPlusFiles[i].pbpPlusDataReserved, 1, 8, tempFile);
     }
     
     fclose(tempFile);
     return (void*)tempOut;
}

int filePbpPlusFileSize(void* inContext, char* inFile) {
     if(!inContext || !inFile || !inFile[0])
          return -1;
     pbpPlusContext* tempContext = (pbpPlusContext*)inContext;
     int i;
     for(i = 0; i < tempContext->pbpPlusFilesCount; i++) {
          if(!strcmp(tempContext->pbpPlusFiles[i].pbpPlusDataName, inFile))
               return tempContext->pbpPlusFiles[i].pbpPlusDataLength;
     }
     return -1;
}

int filePbpPlusFileOffset(void* inContext, char* inFile) {
     if(!inContext || !inFile || !inFile[0])
          return -1;
     pbpPlusContext* tempContext = (pbpPlusContext*)inContext;
     int i;
     for(i = 0; i < tempContext->pbpPlusFilesCount; i++) {
          if(!strcmp(tempContext->pbpPlusFiles[i].pbpPlusDataName, inFile))
               return tempContext->pbpPlusFiles[i].pbpPlusDataOffset;
     }
     return -1;
}

char* filePbpPlusFileRead(void* inContext, char* inFile) {
     pbpPlusContext* tempContext = (pbpPlusContext*)inContext;
     pbpPlusDataHeader* tempPlusHeader = NULL;
     int i;
     for(i = 0; i < tempContext->pbpPlusFilesCount; i++) {
          if(!strcmp(tempContext->pbpPlusFiles[i].pbpPlusDataName, inFile))
               tempPlusHeader = &tempContext->pbpPlusFiles[i];
     }
     
     if(!inFile || !inFile[0] || !tempPlusHeader || (tempPlusHeader->pbpPlusDataLength < 0) || !tempContext->pbpPlusPath || !tempContext->pbpPlusPath[0])
          return NULL;
     FILE* tempFile = fopen(tempContext->pbpPlusPath, "rb");
     if(!tempFile)
          return NULL;
     char* tempOut = memAlloc(tempPlusHeader->pbpPlusDataLength);
     if(!tempOut) {
          fclose(tempFile);
          return NULL;
     }
     fseek(tempFile, tempPlusHeader->pbpPlusDataOffset, SEEK_SET);
     if(fread(tempOut, 1, tempPlusHeader->pbpPlusDataLength, tempFile) != tempPlusHeader->pbpPlusDataLength) {
          memFree(tempOut);
          fclose(tempFile);
          return NULL;
     }
     fclose(tempFile);
     return tempOut;
}

void filePbpPlusFileWrite(void* inContext, char* inFile, char* inData) {
     if(!inContext || !inFile || !inFile[0] || !inData)
          return;
     pbpPlusContext* tempContext = (pbpPlusContext*)inContext;
     pbpPlusDataHeader* tempPlusHeader = NULL;
     int i;
     for(i = 0; i < tempContext->pbpPlusFilesCount; i++) {
          if(!strcmp(tempContext->pbpPlusFiles[i].pbpPlusDataName, inFile))
               tempPlusHeader = &tempContext->pbpPlusFiles[i];
     }
     
     if(!tempPlusHeader || (tempPlusHeader->pbpPlusDataLength < 0) || !tempContext->pbpPlusPath || !tempContext->pbpPlusPath[0])
          return;
     FILE* tempFile = fopen(tempContext->pbpPlusPath, "r+");
     if(!tempFile)
          return;
     fseek(tempFile, tempPlusHeader->pbpPlusDataOffset, SEEK_SET);
     fwrite(inData, 1, tempPlusHeader->pbpPlusDataLength, tempFile);
     fclose(tempFile);
}

#endif
