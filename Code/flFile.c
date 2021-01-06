#include "flGlobal.h"
#if FL_FILE
#include <pspkernel.h>
#include <string.h>
#include <stdio.h>

#if !FL_INCLUDE_ALL_C
#include "flFile.h"
#include "flFilePbpPlus.h"
#include "flMemory.h"
#include "flDebug.h"
#endif

File* fileOpen(char* inPath, u8 inMode) {
     if(!inPath || !inPath[0] || (inMode > 15)) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to open file with invalid parameters (inPath: \"%s\", inMode: %i).", inPath, inMode);
          #endif
          return NULL;
     }

     File* tempOut = memAlloc(sizeof(File));
     if(!tempOut) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Couldn't create file struct while opening \"%s\".", inPath);
          #endif
          return NULL;
     }
     tempOut->fileMode = inMode;
     tempOut->fileState = FILE_STATE_NORMAL;
     tempOut->filePath = memAlloc(strlen(inPath) + 1);
     if(!tempOut->filePath) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Couldn't copy file string while opening \"%s\", probably out of memory.", inPath);
          #endif
          memFree(tempOut);
          return NULL;
     }
     strcpy(tempOut->filePath, inPath);
     tempOut->filePointer = 0;

     if(!strncmp(inPath, "mem:/", 5)) {
          tempOut->fileType = FILE_TYPE_MEMORY;
          int i;
          unsigned int tempDataPtr = 0;
          for(i = 5; (inPath[i] >= ASCII_0) && (inPath[i] <= ASCII_9); i++) {
               tempDataPtr *= 10;
               tempDataPtr += (inPath[i] - ASCII_0);
          }
          tempOut->fileData = (u8*)tempDataPtr;
          tempDataPtr = 0;
          for(i = 5; (inPath[i] >= ASCII_0) && (inPath[i] <= ASCII_9); i++) {
               tempDataPtr *= 10;
               tempDataPtr += (inPath[i] - ASCII_0);
          }
          tempOut->fileSize = tempDataPtr;
          return tempOut;
     }

     #if FL_FILE_PBPPLUS
     if(!strncmp(inPath, "pbp:/", 5)) {
          tempOut->fileType = FILE_TYPE_PBP;
          void* tempContext = filePbpPlusOpen("./EBOOT.PBP");
          if(!tempContext) {
               memFree(tempOut->filePath);
               memFree(tempOut);
               #if DEBUG_WARNING
               debugWarning("Error opening pbp file %s, couldn't create context.", inPath);
               #endif
               return NULL;
          }
          tempOut->fileData = (u8*)filePbpPlusFileRead(tempContext, &inPath[5]);
          if(!tempOut->fileData) {
               memFree(tempOut->filePath);
               memFree(tempOut);
               memFree(tempContext);
               #if DEBUG_WARNING
               debugWarning("Error opening pbp file %s, couldn't find file in archive.", inPath);
               #endif
               return NULL;
          }
          tempOut->fileSize = filePbpPlusFileSize(tempContext, &inPath[5]);

          memFree(tempContext);
          return tempOut;
     }
     #endif

     char tempMode[4];
     memClear(tempMode, 4);

     if(inMode & 2)
          tempMode[0] = 'w';
     else if(!(inMode & 3))
          tempMode[0] = 'a';
     else
          tempMode[0] = 'r';

     if(inMode & 4) {
          tempMode[1] = 'b';
          if(inMode & 8)
               tempMode[2] = '+';
     } else if(inMode & 8) {
          tempMode[1] = '+';
     }

     if(fileExists(inPath)) {
          SceIoStat tempStats;
          if(sceIoGetstat(inPath, &tempStats) != 0) {
               #if FL_DEBUG_WARNING != 0
               debugWarning("Couldn't get file stats for \"%s\".", inPath);
               #endif
               memFree(tempOut->filePath);
               memFree(tempOut);
               return NULL;
          }
          tempOut->fileSize = tempStats.st_size;
     } else {
          tempOut->fileSize = 0;
     }

     tempOut->fileData = (u8*)fopen(inPath, tempMode);
     if(!tempOut->fileData) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Couldn't open file \"%s\".", inPath);
          #endif
          memFree(tempOut->filePath);
          memFree(tempOut);
          return NULL;
     }
     tempOut->fileType = FILE_TYPE_FILE;

     return tempOut;
}

void fileClose(File* inFile) {
     if(!inFile) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to close NULL file.");
          #endif
          return;
     }
     if(inFile->fileType == FILE_TYPE_FILE)
          fclose((FILE*)inFile->fileData);

     #if FL_FILE_PBPPLUS
     if((inFile->fileType == FILE_TYPE_PBP) && inFile->fileData) {
          if(inFile->fileMode & FILE_MODE_WRITE) {
               void* tempContext = filePbpPlusOpen("./EBOOT.PBP");
               if(tempContext) {
                    filePbpPlusFileWrite(tempContext, &inFile->filePath[5], (char*)inFile->fileData);
                    memFree(tempContext);
               }
          }
          memFree(inFile->fileData);
     }
     #endif
     memFree(inFile->filePath);
     memFree(inFile);
     return;
}

bool fileCheckState(File* inFile) {
     if(!inFile) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to check the state of a NULL file.");
          #endif
          return false;
     }
     if(inFile->fileType == FILE_TYPE_FILE) {
          if(feof((FILE*)inFile->fileData))
               inFile->fileState |= FILE_STATE_EOF;
          else
               inFile->fileState &= ~FILE_STATE_EOF;
          if(ferror((FILE*)inFile->fileData))
               inFile->fileState |= FILE_STATE_ERROR;
          else
               inFile->fileState &= ~FILE_STATE_ERROR;
     } else if((inFile->fileType == FILE_TYPE_MEMORY) || (inFile->fileType == FILE_TYPE_PBP)) {
          if(inFile->filePointer >= (inFile->fileSize - 1))
               inFile->fileState |= FILE_STATE_EOF;
          else
               inFile->fileState &= ~FILE_STATE_EOF;
          if(inFile->filePointer < 0)
               inFile->fileState |= FILE_STATE_ERROR;
          else
               inFile->fileState &= ~FILE_STATE_ERROR;
     }
     if(inFile->fileState != FILE_STATE_NORMAL)
          return false;
     return true;
}

void fileSeek(File* inFile, long inSeek, int inMode) {
     if(!inFile || ((inMode != FILE_SEEK_SET) && (inMode != FILE_SEEK_CUR) && (inMode != FILE_SEEK_END))) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to fseek a NULL file, or fseeking using a bad seek mode.");
          #endif
          return;
     }

     if(inFile->fileType == FILE_TYPE_FILE) {
          fseek((FILE*)inFile->fileData, inSeek, inMode);
     } else if((inFile->fileType == FILE_TYPE_MEMORY) || (inFile->fileType == FILE_TYPE_PBP)) {
          if(inMode == FILE_SEEK_SET)
               inFile->filePointer = inSeek;
          else if(inMode == FILE_SEEK_CUR)
               inFile->filePointer += inSeek;
          else
               inFile->filePointer = (inFile->fileSize + inSeek);
     }
}

int fileRead(void* inDest, int inSize, File* inFile) {
     if(!fileCheckState(inFile))
          return -1;
     if(inFile->fileType == FILE_TYPE_FILE) {
          inSize = fread(inDest, 1, inSize, (FILE*)inFile->fileData);
     } else if((inFile->fileType == FILE_TYPE_MEMORY) || (inFile->fileType == FILE_TYPE_PBP)) {
          if((inFile->filePointer + inSize) > inFile->fileSize)
               inSize = (inFile->fileSize - (inFile->filePointer + inSize));
          memCopy(inDest, &inFile->fileData[inFile->filePointer], inSize);
     } else {
          #if FL_DEBUG_ERROR != 0
          debugError("Trying to read from corrupted file context.");
          #endif
          return -1;
     }
     inFile->filePointer += inSize;
     return inSize;
}

int fileWrite(void* inSrc, int inSize, File* inFile) {
     if(!fileCheckState(inFile))
          return -1;
     if(inFile->fileType == FILE_TYPE_FILE) {
          inSize = fwrite(inSrc, 1, inSize, (FILE*)inFile->fileData);
          if((inFile->filePointer + inSize) > inFile->fileSize)
               inFile->fileSize = (inFile->filePointer + inSize);
     } else if((inFile->fileType == FILE_TYPE_MEMORY) || (inFile->fileType == FILE_TYPE_PBP)) {
          if((inFile->filePointer + inSize) > inFile->fileSize)
               inSize = (inFile->fileSize - (inFile->filePointer + inSize));
          memCopy(&inFile->fileData[inFile->filePointer], inSrc, inSize);
     } else {
          #if FL_DEBUG_ERROR != 0
          debugError("Trying to read from corrupted file context.");
          #endif
          return -1;
     }
     inFile->filePointer += inSize;
     return inSize;
}

int filePuts(char* inString, File* inFile) {
     if(!inString || !inString[0]) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying print a NULL string to file.");
          #endif
          return -1;
     }
     return fileWrite(inString, strlen(inString), inFile);
}

char* fileGets(char* inString, int inCount, File* inFile) {
     int i;
     for(i = 0; i < (inCount - 1); i++) {
          if(fileRead(&inString[i], 1, inFile) != 1)
               return NULL;
          if(inString[i] == 0)
               return inString;
          if(inString[i] == '\n') {
               i++;
               break;
          }
     }
     inString[i] = 0;
     return inString;
}

bool fileEOF(File* inFile) {
     fileCheckState(inFile);
     return (inFile->fileState & FILE_STATE_EOF);
}

bool fileError(File* inFile) {
     fileCheckState(inFile);
     return (inFile->fileState & FILE_STATE_ERROR);
}

char fileTypeGet(char* inPath) {
     if(!inPath || !inPath[0]) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to get the type of a file with NULL path.");
          #endif
          return -1;
     }

     SceIoStat tempStats;

     if(inPath[strlen(inPath) - 1] == '/') {
          inPath[strlen(inPath) - 1] = 0;
          sceIoGetstat(inPath, &tempStats);
          inPath[strlen(inPath) - 1] = '/';
          if(tempStats.st_mode & FIO_S_IFDIR)
               return 0;
          return -1;
     }
     if(!strcmp(&inPath[strlen(inPath) - 3], "/..") || !strcmp(inPath, ".."))
          return 2;
     if(!strcmp(&inPath[strlen(inPath) - 2], "/.") || !strcmp(inPath, "."))
          return 3;

     sceIoGetstat(inPath, &tempStats);

     if(tempStats.st_mode & FIO_S_IFDIR) {
          return 0;
     } else {
          if(fileExists(inPath))
               return 1;
          return -1;
     }
}

bool fileExists(char* inPath) {
     if(!inPath || !inPath[0])
          return false;

     if(!strncmp(inPath, "mem:/", 5)) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to check if memory address exists \"%s\".", inPath);
          #endif
          return true;
     }

     FILE* tempFile = fopen(inPath, "rb");
     if(tempFile) {
          fclose(tempFile);
          return true;
     }
     return false;
}

char* fileNameFromPath(char* inPath) {
     if(!inPath || !inPath[0]) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to get filename from a NULL string.");
          #endif
          return NULL;
     }
     char* tempStrPtr = inPath;
     int   tempStrLen = 0;
     while(tempStrPtr[0]) {
          tempStrLen++;
          if((tempStrPtr[0] == '/') || (tempStrPtr[0] == '\\'))
               tempStrLen = 0;
          tempStrPtr++;
     }

     if(!tempStrLen)
          return NULL;
     tempStrLen++;
     char* tempOut = (char*)memAlloc(tempStrLen);
     memCopy(tempOut, &inPath[strlen(inPath) - (tempStrLen - 1)], tempStrLen);

     return tempOut;
}

char* fileExtension(char* inPath) {
     if(!inPath || !inPath[0]) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to get file extension from a NULL string.");
          #endif
          return NULL;
     }
     int tempLen = 0;
     char* tempStrPtr = &inPath[strlen(inPath) - 1];
     while(tempStrPtr[0] != '.') {
          tempLen++;
          tempStrPtr--;
     }
     if(!tempLen)
          return NULL;
     char* tempOut = (char*)memAlloc(tempLen + 1);
     if(!tempOut) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Cannot allocate file extension string.\nProbably out of memory.");
          #endif
          return NULL;
     }
     int i;
     for(i = 1; i <= tempLen; i++) {
          if((tempStrPtr[i] == '\r') ||(tempStrPtr[i] == '\n') || (tempStrPtr[i] == ' '))
               tempOut[i - 1] = 0;
          else
               tempOut[i - 1] = tempStrPtr[i];
     }
     tempOut[tempLen] = 0;
     return tempOut;
}

char* filePathValidate(char* inPath) {
     if(!inPath || !inPath[0])
          return NULL;
     u32 i;
     for(i = 0; i < strlen(inPath); i++) {
          if((inPath[i] == '\n') || (inPath[i] == '\r'))
               inPath[i] = 0;
     }
     return inPath;
}

bool fileMkdir(char* inDir) {
     if(!inDir || !inDir[0]) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Cannot create a folder with a NULL name.");
          #endif
          return false;
     }
     sceIoMkdir(inDir, 0777);
     return true;
}

bool fileDelete(char* inPath) {
     if(!inPath || !inPath[0]) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Cannot delete a file with NULL path.");
          #endif
          return false;
     }

     if(!strncmp(inPath, "mem:/", 5)) {
          int i;
          unsigned int tempDataPtr = 0;
          for(i = 5; (inPath[i] >= ASCII_0) && (inPath[i] <= ASCII_9); i++) {
               tempDataPtr *= 10;
               tempDataPtr += (inPath[i] - ASCII_0);
          }
          memFree((void*)tempDataPtr);
          return true;
     }

     int tempfileTypeGet = fileTypeGet(inPath);
     if(tempfileTypeGet) {
          if(tempfileTypeGet == 1)
               sceIoRemove(inPath);
          return true;
     }

     #if FL_DEBUG_WARNING != 0
     debugWarning("Recursive deletion and folder deletion are not yet supported.");
     #endif
     return false;
}

bool fileCopy(char* inSrc, char* inDest) {
     if(!inSrc || !inDest || !inSrc[0] || !inDest[0]) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to copy to/from a null file path.");
          #endif
          return false;
     }

     SceUID tempSrc = sceIoOpen(inSrc, PSP_O_RDONLY, 0777);
     if(tempSrc <= 0) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Can't open source file \"%s\" for copying.", inSrc);
          #endif
          return false;
     }

     SceUID tempDest = sceIoOpen(inDest, PSP_O_WRONLY | PSP_O_CREAT, 0777);
     if(tempDest <= 0) {
          sceIoClose(tempSrc);
          #if FL_DEBUG_WARNING != 0
          debugWarning("Can't open destination file \"%s\" for copying.", inDest);
          #endif
          return false;
     }

     char* tempData = memQalloc(FILE_COPY_BLOCKSIZE);
     int tempRead = sceIoRead(tempSrc, tempData, FILE_COPY_BLOCKSIZE);
     int tempWritten;

     while(tempRead > 0) {
          tempWritten = sceIoWrite(tempDest, tempData, tempRead);
          if(tempWritten != tempRead) {
               sceIoClose(tempDest);
               sceIoClose(tempSrc);
               sceIoRemove(inDest);
               #if FL_DEBUG_WARNING != 0
               debugWarning("Error writing while copying file \"%s\" to \"%s\".", inSrc, inDest);
               #endif
               return false;
          }
          tempRead = sceIoRead(tempSrc, tempData, FILE_COPY_BLOCKSIZE);
     }

     sceIoClose(tempDest);
     sceIoClose(tempSrc);

     return true;
}

#endif
