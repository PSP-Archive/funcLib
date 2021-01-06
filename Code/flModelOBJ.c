#include "flGlobal.h"
#if FL_MODEL_OBJ != 0
#include <stdlib.h>
#include <stdio.h>
#include <psprtc.h>

#if FL_INCLUDE_ALL_C == 0
#include "flModelOBJ.h"
#include "flModel.h"
#include "flMemory.h"
#include "flMath.h"
#include "flColor.h"
#include "flFile.h"
#include "flString.h"

#include "flTexture.h"

#if FL_DEBUG != 0
#include "flDebug.h"
#endif
#endif

bool mdl3dStatLoadOBJ_faceRead(u8* inFaceStr, Model3dStaticFace* inFacePtr, Model3dStatic* inModel, Texture* inCurTexture, int inVertPtr, int inTexVertPtr, int inNormPtr) {
     u8* tempLinePtr = inFaceStr;
     inFacePtr->mdlfVertCount = 0;
     while((tempLinePtr[0] != '\n') && (tempLinePtr[0] != 0)) {
          while((tempLinePtr[0] == ' ') || (tempLinePtr[0] == ASCII_TAB))
               tempLinePtr++;
          if((tempLinePtr[0] == '\n') || (tempLinePtr[0] == 0) || (tempLinePtr[0] == '\r'))
               break;
          inFacePtr->mdlfVertCount++;
          while((tempLinePtr[0] != ' ') && (tempLinePtr[0] != ASCII_TAB) && (tempLinePtr[0] != '\n') && (tempLinePtr[0] != 0))
               tempLinePtr++;
     }
     u8* tempBlockAlloc = memAlloc(sizeof(vect3f*) * inFacePtr->mdlfVertCount * 3);
     if(!tempBlockAlloc) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Couldn't read OBJ face data.\nProbably out of memory.");
          #endif
          return false;
     }
     memClear(tempBlockAlloc, (sizeof(vect3f*) * inFacePtr->mdlfVertCount * 3));
     inFacePtr->mdlfVerts = (vect3f**)&tempBlockAlloc[0]; //(vect3f**)tempBlockAlloc;
     inFacePtr->mdlfTexVerts = (vect2f**)&tempBlockAlloc[sizeof(vect3f*) * inFacePtr->mdlfVertCount]; //(vect2f**)((u32)tempBlockAlloc + (sizeof(vect3f*) * inFacePtr->mdlfVertCount));
     inFacePtr->mdlfNormals = (vect3f**)&tempBlockAlloc[(sizeof(vect3f*) << 1) * inFacePtr->mdlfVertCount]; //(vect3f**)((u32)tempBlockAlloc + ((sizeof(vect3f*) << 1) * inFacePtr->mdlfVertCount));
     tempLinePtr = inFaceStr;
     int i, tempIndex;
     for(i = (inFacePtr->mdlfVertCount - 1); i >= 0 ; i--) {
          while((tempLinePtr[0] == ' ') || (tempLinePtr[0] == ASCII_TAB) || (tempLinePtr[0] == '\r'))
               tempLinePtr++;
          if(tempLinePtr[0] == 0)
               return false;
          tempIndex = strToInt(tempLinePtr);
          if(tempLinePtr[0] == '-') {
               tempIndex += inVertPtr;
               tempLinePtr++;
          } else if(tempLinePtr[0] == '+') {
               tempIndex += (inVertPtr - 1);
               tempLinePtr++;
          } else {
               tempIndex -= 1;
          }
          if(tempIndex < 0) {
               #if FL_DEBUG_WARNING != 0
               debugWarning("Invalid obj file, contains reference to vertex < 1.");
               #endif
               return false;
          }
          inFacePtr->mdlfVerts[i] = &inModel->mdlVerts[tempIndex];
          while((tempLinePtr[0] >= '0') && (tempLinePtr[0] <= '9'))
               tempLinePtr++;
          if(tempLinePtr[0] == '/') {
               tempLinePtr++;
               if(tempLinePtr[0] != '/') {
                    tempIndex = strToInt(tempLinePtr);
                    if(tempLinePtr[0] == '-') {
                         tempIndex += inTexVertPtr;
                         tempLinePtr++;
                    } else if(tempLinePtr[0] == '+') {
                         tempIndex += (inTexVertPtr - 1);
                         tempLinePtr++;
                    } else {
                         tempIndex -= 1;
                    }
                    if(tempIndex < 0) {
                         #if FL_DEBUG_WARNING != 0
                         debugWarning("Invalid obj file, contains reference to texture vert < 1.");
                         #endif
                         return false;
                    }
                    inFacePtr->mdlfTexVerts[i] = &inModel->mdlTexVerts[tempIndex];
                    while((tempLinePtr[0] >= '0') && (tempLinePtr[0] <= '9'))
                         tempLinePtr++;
               } else { 
                    inFacePtr->mdlfTexVerts[i] = NULL;
               }
               if(tempLinePtr[0] == '/') {
                    tempLinePtr++;
                    tempIndex = strToInt(tempLinePtr);
                    if(tempLinePtr[0] == '-') {
                         tempIndex += inNormPtr;
                         tempLinePtr++;
                    } else if(tempLinePtr[0] == '+') {
                         tempIndex += (inNormPtr - 1);
                         tempLinePtr++;
                    } else {
                         tempIndex -= 1;
                    }
                    if(tempIndex < 0) {
                         #if FL_DEBUG_WARNING != 0
                         debugWarning("Invalid obj file, contains reference to normal < 1.");
                         #endif
                         return false;
                    }
                    inFacePtr->mdlfNormals[i] = &inModel->mdlNormals[tempIndex];
                    while((tempLinePtr[0] >= '0') && (tempLinePtr[0] <= '9'))
                         tempLinePtr++;
                    while(tempLinePtr[0] == '/') {
                         if((tempLinePtr[0] == '-') || (tempLinePtr[0] == '+'))
                              tempLinePtr++;                         
                         while((tempLinePtr[0] >= '0') && (tempLinePtr[0] <= '9'))
                              tempLinePtr++;
                    }
               } else {
                    inFacePtr->mdlfNormals[i] = NULL;
               }
          } else {
               inFacePtr->mdlfTexVerts[i] = NULL;
               inFacePtr->mdlfNormals[i] = NULL;
          }
     }
     inFacePtr->mdlfTexture = inCurTexture;
     return true;
}

Model3dStatic* mdl3dStatLoadOBJ(char* inPath) {
     if(!inPath || !fileExists(inPath)) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Cannot open OBJ file.");
          #endif
          return NULL;
     }
     
     #if FL_FILE != 0
     File* tempFile = fileOpen(inPath, FILE_MODE_READ);
     #else
     FILE* tempFile = fopen(inPath, "r");
     #endif
     
     Model3dStatic* tempOut = memAlloc(sizeof(Model3dStatic));
     if(!tempOut) {
          fileClose(tempFile);
          #if FL_DEBUG_WARNING != 0
          debugWarning("Couldn't create model struct.\nProbably out of memory.");
          #endif
          return NULL;
     }
     tempOut->mdlOptimized = false;
     tempOut->mdlTexVertCount = 0;
     tempOut->mdlVertCount = 0;
     tempOut->mdlTextureCount = 0;
     tempOut->mdlTexVertCount = 0;
     tempOut->mdlNormalCount = 0;
     tempOut->mdlFaceCount = 0;
     
     u8 tempLine[256];
     u8* tempLinePtr;
     
     while(fileGets((char*)tempLine, 256, tempFile)) {
          tempLinePtr = tempLine;
          while((tempLinePtr[0] == ' ') || (tempLinePtr[0] == ASCII_TAB))
               tempLinePtr++;
          if(strncmp((char*)tempLinePtr, "vt", 2) == 0)
               tempOut->mdlTexVertCount++;
          else if(strncmp((char*)tempLinePtr, "vn", 2) == 0)
               tempOut->mdlNormalCount++;
          else if(tempLinePtr[0] == 'v')
               tempOut->mdlVertCount++;
          else if(tempLinePtr[0] == 'f')
               tempOut->mdlFaceCount++;
          else if(strncmp((char*)tempLinePtr, "usemtl", 6) == 0)
               tempOut->mdlTextureCount++;
     }
     
     u32 tempSize = (sizeof(vect3f) * tempOut->mdlVertCount); 
     tempSize += (sizeof(vect2f) * tempOut->mdlTexVertCount);
     tempSize += (sizeof(vect3f) * tempOut->mdlNormalCount);
     tempSize += (sizeof(Model3dStaticFace) * tempOut->mdlFaceCount);
     tempSize += (sizeof(Texture*) * tempOut->mdlTextureCount);
     
     u8* tempAllocBlock = memAlloc(tempSize);
     if(!tempAllocBlock) {
          fileClose(tempFile);
          mdl3dStatFree(tempOut);
          #if FL_DEBUG_WARNING != 0
          debugWarning("Couldn't allocate model data.\nProbably out of memory.");
          #endif
          return NULL;
     }
     
     u32 tempOffset = 0;
     tempOut->mdlVerts = (vect3f*)&tempAllocBlock[tempOffset]; //(vect3f*)(tempAllocBlock);
     tempOffset += (sizeof(vect3f) * tempOut->mdlVertCount);
     tempOut->mdlTexVerts = (vect2f*)&tempAllocBlock[tempOffset]; //(vect2f*)((u32)(tempAllocBlock) + tempOffset);
     tempOffset += (sizeof(vect2f) * tempOut->mdlTexVertCount);
     tempOut->mdlNormals = (vect3f*)&tempAllocBlock[tempOffset]; //(vect3f*)((u32)(tempAllocBlock) + tempOffset);
     tempOffset += (sizeof(vect3f) * tempOut->mdlNormalCount);
     tempOut->mdlFaces = (Model3dStaticFace*)&tempAllocBlock[tempOffset]; //(Model3dStaticFace*)((u32)(tempAllocBlock) + tempOffset);
     tempOffset += (sizeof(Model3dStaticFace) * tempOut->mdlFaceCount);     
     tempOut->mdlTextures = (Texture**)&tempAllocBlock[tempOffset]; //(Texture**)((u32)(tempAllocBlock) + tempOffset);
     
     fileSeek(tempFile, 0, FILE_SEEK_SET);
     
     u32 tempVertPtr = 0;
     u32 tempTexVertPtr = 0;
     u32 tempNormPtr = 0;
     u32 tempFacePtr = 0;
     u32 tempTexPtr = 0;
     
     Texture* tempCurTexture = NULL;
     
     int tempLineNum = 0;
     while(fileGets((char*)tempLine, 256, tempFile)) {
          tempLineNum++;
          tempLinePtr = tempLine;
          while((tempLinePtr[0] == ' ') || (tempLinePtr[0] == ASCII_TAB))
               tempLinePtr++;
          if(strncmp((char*)tempLinePtr, "vt", 2) == 0) {
               tempLinePtr += 2;
               tempOut->mdlTexVerts[tempTexVertPtr] = strToVect2f(tempLinePtr);
               tempTexVertPtr++;
          } else if(strncmp((char*)tempLinePtr, "vn", 2) == 0) {
               tempLinePtr += 2;
               tempOut->mdlNormals[tempNormPtr] = strToVect3f(tempLinePtr);
               tempNormPtr++;
          } else if(tempLinePtr[0] == 'v') {
               tempLinePtr++;
               tempOut->mdlVerts[tempVertPtr] = strToVect3f(tempLinePtr);
               tempVertPtr++;
          } else if(tempLinePtr[0] == 'f') {
               tempLinePtr++;
               if(!mdl3dStatLoadOBJ_faceRead(tempLinePtr, &tempOut->mdlFaces[tempFacePtr],tempOut, tempCurTexture, (int)tempVertPtr, (int)tempTexVertPtr, (int)tempNormPtr)) {
                    fileClose(tempFile);
                    mdl3dStatFree(tempOut);
                    #if FL_DEBUG_WARNING != 0
                    char tempString[256];
                    sprintf(tempString, "Error reading face data on line %i while loading OBJ.", tempLineNum);
                    debugWarning(tempString);
                    #endif
                    return NULL;
               }
               tempFacePtr++;
          } else if(strncmp((char*)tempLinePtr, "usemtl", 6) == 0) {
               tempLinePtr += 6;
               while((tempLinePtr[0] == ' ') || (tempLinePtr[0] == ASCII_TAB))
                    tempLinePtr++;
               tempLine[strlen((char*)tempLine) - 1] = 0;
               tempOut->mdlTextures[tempTexPtr] = texLoad((char*)tempLinePtr);
               if(!tempOut->mdlTextures[tempTexPtr]) {
                    fileClose(tempFile);
                    mdl3dStatFree(tempOut);
                    #if FL_DEBUG_WARNING != 0
                    char tempString[256];
                    sprintf(tempString, "Error reading texture data on line %i while loading OBJ.", tempLineNum);
                    debugWarning(tempString);
                    #endif
                    return NULL;
               }
               tempLine[strlen((char*)tempLine) - 1] = '\n';
               tempCurTexture = tempOut->mdlTextures[tempTexPtr];
               tempTexPtr++;
          }
     }

     return tempOut;
}

bool mdl3dStatSaveOBJ(char* inPath, Model3dStatic* inModel) {
     #if FL_FILE != 0
     File* tempFile = fileOpen(inPath, FILE_MODE_WRITE);
     #else
     FILE* tempFile = fopen(inPath, "w");
     #endif
     
     if(!tempFile) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Couldn't open obj file for saving.");
          #endif
          return false;
     }
     
     char tempString[256];
     int i, j, k;
     pspTime tempTime;
     sceRtcGetCurrentClockLocalTime(&tempTime);
     
     #if FL_DEBUG_DATEFORMAT_AMERICAN != 0
     sprintf(tempString, "# Created by funcLib on %02i/%02i/%04i at %02i:%02i:%02i\n", tempTime.month, tempTime.day, tempTime.year, tempTime.hour, tempTime.minutes, tempTime.seconds);
     #else
     sprintf(tempString, "# Created by funcLib on %02i/%02i/%04i at %02i:%02i:%02i\n", tempTime.day, tempTime.month, tempTime.year, tempTime.hour, tempTime.minutes, tempTime.seconds);
     #endif
     filePuts(tempString, tempFile);
     sprintf(tempString, "# Verts: %i\n", (int)inModel->mdlVertCount);
     filePuts(tempString, tempFile);
     sprintf(tempString, "# TexCoords: %i\n", (int)inModel->mdlTexVertCount);
     filePuts(tempString, tempFile);
     sprintf(tempString, "# Normals: %i\n", (int)inModel->mdlNormalCount);
     filePuts(tempString, tempFile);
     sprintf(tempString, "# Materials: %i\n", (int)inModel->mdlTextureCount);
     filePuts(tempString, tempFile);
     sprintf(tempString, "# Faces: %i\n", (int)inModel->mdlFaceCount);
     filePuts(tempString, tempFile);
     j = 0;
     for(i = 0; i < inModel->mdlFaceCount; i++)
          j += (inModel->mdlFaces[i].mdlfVertCount - 2);
     sprintf(tempString, "# Triangles: %i\n", j);
     filePuts(tempString, tempFile);

     sprintf(tempString, "\n# Verteces\n");
     filePuts(tempString, tempFile);
     for(i = 0; i < inModel->mdlVertCount; i++) {
          sprintf(tempString, "v %f %f %f\n", inModel->mdlVerts[i].x, inModel->mdlVerts[i].y, inModel->mdlVerts[i].z);
          filePuts(tempString, tempFile);
     }
     
     sprintf(tempString, "\n# TexCoords\n");
     filePuts(tempString, tempFile);
     for(i = 0; i < inModel->mdlTexVertCount; i++) {
          sprintf(tempString, "vt %f %f\n", inModel->mdlTexVerts[i].x, inModel->mdlTexVerts[i].y);
          filePuts(tempString, tempFile);
     }
     
     sprintf(tempString, "\n# Normals\n");
     filePuts(tempString, tempFile);
     for(i = 0; i < inModel->mdlNormalCount; i++) {
          sprintf(tempString, "vn %f %f %f\n", inModel->mdlNormals[i].x, inModel->mdlNormals[i].y, inModel->mdlNormals[i].z);
          filePuts(tempString, tempFile);
     }
     
     sprintf(tempString, "\n# Faces\n");
     filePuts(tempString, tempFile);
     char tempStringAdd[256];
     u32 tempCurTexture = 0;
     for(i = 0; i < inModel->mdlFaceCount; i++) {
          if(inModel->mdlTextureCount > 0) {
               if(!tempCurTexture || (tempCurTexture != (unsigned int)inModel->mdlFaces[i].mdlfTexture)) {
                    sprintf(tempString, "usemtl %s\n", texPath(inModel->mdlFaces[i].mdlfTexture));
                    tempCurTexture = (unsigned int)inModel->mdlFaces[i].mdlfTexture;
                    filePuts(tempString, tempFile);
               }
          }
          tempString[0] = 'f';
          tempString[1] = 0;
          for(j = (inModel->mdlFaces[i].mdlfVertCount - 1); j >= 0 ; j--) {
               for(k = 0; k < inModel->mdlVertCount; k++) {
                    if(vect3f_Cmp(*inModel->mdlFaces[i].mdlfVerts[j], inModel->mdlVerts[k])) {
                         sprintf(tempStringAdd, " %i", (k + 1));
                         strcat(tempString, tempStringAdd);
                         break;
                    }
               }
               if(inModel->mdlFaces[i].mdlfTexVerts[j]) {
                    for(k = 0; k < inModel->mdlTexVertCount; k++) {
                         if(vect2f_Cmp(*inModel->mdlFaces[i].mdlfTexVerts[j], inModel->mdlTexVerts[k])) {
                              sprintf(tempStringAdd, "/%i", (k + 1));
                              strcat(tempString, tempStringAdd);
                              break;
                         }
                    }
               }
               if(inModel->mdlFaces[i].mdlfNormals[j]) {
                    if(!inModel->mdlFaces[i].mdlfTexVerts[j]) {
                         sprintf(tempStringAdd, "/");
                         strcat(tempString, tempStringAdd);
                    }
                    for(k = 0; k < inModel->mdlNormalCount; k++) {
                         if(vect2f_Cmp(*inModel->mdlFaces[i].mdlfNormals[j], inModel->mdlNormals[k])) {
                              sprintf(tempStringAdd, "/%i", (k + 1));
                              strcat(tempString, tempStringAdd);
                              break;
                         }
                    }
               }
          }
          filePuts(tempString, tempFile);
          filePuts("\n", tempFile);
     }
     
     sprintf(tempString, "\n# End of OBJ\n");
     filePuts(tempString, tempFile);
     
     fileClose(tempFile);
     
     return true;
}

#endif
