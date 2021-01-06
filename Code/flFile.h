// Funclib File v1.0.0 (CORE)
// 
// This module contains wrapper functions for file manipulation,
// it also contains other useful filesystem functions.
// 
// Contributor(s): Flatmush



#ifndef FLFILE_H
#define FLFILE_H

#include "flGlobal.h"

#if FL_FILE != 0

#ifdef __cplusplus
extern "C" {
#endif

#define FILE_MODE_APPEND    (0)
#define FILE_MODE_READ      (1)
#define FILE_MODE_WRITE     (2)
#define FILE_MODE_BINARY    (4)
#define FILE_MODE_READWRITE (8)
#define FILE_MODE_READ_BINARY (FILE_MODE_READ | FILE_MODE_BINARY)
#define FILE_MODE_WRITE_BINARY (FILE_MODE_READ | FILE_MODE_BINARY)

#define FILE_TYPE_FILE      (0)
#define FILE_TYPE_MEMORY    (1)
#define FILE_TYPE_ZIP       (2)
#define FILE_TYPE_PBP       (3) // Compatible with PBP Plus files too.

#define FILE_STATE_NORMAL   (0)
#define FILE_STATE_EOF      (1)
#define FILE_STATE_ERROR    (2)

#define FILE_SEEK_SET    (SEEK_SET)
#define FILE_SEEK_CUR    (SEEK_CUR)
#define FILE_SEEK_END    (SEEK_END)

#define FILE_COPY_BLOCKSIZE 16384

typedef struct {
     char fseName[128];
} FileDirEntry;

typedef struct {
     char* filePath;
     u8*   fileData;
     int   filePointer;
     int   fileSize;
     u8    fileMode;
     u8    fileType;
     u8    fileState;
     u8    filePadding;
} File;

extern File* fileOpen(char* inPath, u8 inMode);
extern void  fileClose(File* inFile);
extern bool  fileCheckState(File* inFile);
extern void  fileSeek(File* inFile, long inSeek, int inMode);
extern int   fileRead(void* inDest, int inSize, File* inFile);
extern int   fileWrite(void* inSrc, int inSize, File* inFile);
extern int   filePuts(char* inString, File* inFile);
extern char* fileGets(char* inString, int inCount, File* inFile);
extern bool  fileEOF(File* inFile);
extern bool  fileError(File* inFile);

extern char  fileTypeGet(char* inPath);
extern bool  fileExists(char* inPath);
extern char* fileNameFromPath(char* inPath);
extern char* fileExtension(char* inPath);
extern char* filePathValidate(char* inPath);
extern bool  fileMkdir(char* inDir);
extern bool  fileDelete(char* inPath);
extern bool  fileCopy(char* inSrc, char* inDest);

#ifdef __cplusplus
}
#endif

#else

#include <stdio.h>
#include <pspkernel.h>

#define FILE_SEEK_SET    (SEEK_SET)
#define FILE_SEEK_CUR    (SEEK_CUR)
#define FILE_SEEK_END    (SEEK_END)

#define fileClose(inFile) fclose(inFile)
#define fileRead(inDest, inSize, inFile) fread(inDest, 1, inSize, inFile)
#define fileWrite(inSrc, inSize, inFile) fwrite(inSrc, 1, inSize, inFile)
#define filePuts(inString, inFile) fputs(inString, inFile)
#define fileGets(inString, inCount, inFile) fgets(inString, inCount, inFile)
#define fileSeek(inFile, inSeek, inMode) fseek(inFile, inSeek, inMode)
#define fileEOF(inFile) feof(inFile)
#define fileError(inFile) ferror(inFile)

#define fileDelete(inPath) sceIoRemove(inPath)

#endif

#endif
