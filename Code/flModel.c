#include "flGlobal.h"
#if FL_MODEL != 0
#include <stdlib.h>
#include <stdio.h>
#include <pspgu.h>
#include <pspgum.h>

#if FL_INCLUDE_ALL_C == 0
#include "flModel.h"
#include "flMemory.h"
#include "flMath.h"
#include "flColor.h"
#include "flFile.h"

#include "flTexture.h"
#include "flGraphics.h"

#if FL_MODEL_OBJ != 0
#include "flModelOBJ.h"
#endif

#if FL_MODEL_MD2 != 0
#include "flModelMD2.h"
#endif

#if FL_DEBUG != 0
#include "flDebug.h"
#endif
#endif

// Prototypes
void mdl3dStatDrawTextured(Model3dStatic* inModel);

void mdlInit() {
     // Nothing to initialize (yet!)
}

Model3dStatic* mdl3dStatLoad(char* inPath) {
     if(!inPath || (inPath[0] == 0)) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Invalid model file path.");
          #endif
          return NULL;
     }
     char* tempExt = fileExtension(inPath);
     if(!tempExt) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Cannot determine file extension for model.");
          #endif
          return NULL;
     }
     strLCase((u8*)tempExt);
     
     Model3dStatic* tempOut = NULL;
     if(!strcmp(tempExt, "obj")) {
          #if FL_MODEL_OBJ != 0
          tempOut = mdl3dStatLoadOBJ(inPath);
          #else
          #if FL_DEBUG_DEVWARNING != 0
          debugDevWarning("OBJ models not supported in this build\nSet FL_MODEL_OBJ to non-zero to enable them.");
          #endif
          #endif
     }

     memFree(tempExt);

     if(!tempOut) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Cannot open this file type as model.");
          #endif     
          return NULL;
     }
     
     mdl3dStatOptimize(tempOut);
     return tempOut;
}

bool mdl3dStatSave(char* inPath, Model3dStatic* inModel) {
     if(!inPath || (inPath[0] == 0)) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Invalid model file path.");
          #endif
          return false;
     }
     
     if(!inModel) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Cannot save NULL model.");
          #endif
          return false;
     }
     
     mdl3dStatOptimize(inModel);
     
     char* tempExt = fileExtension(inPath);
     if(!tempExt) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Cannot determine file extension for model.");
          #endif
          return false;
     }
     strLCase((u8*)tempExt);
     
     bool tempOut = false;
     if(!strcmp(tempExt, "obj")) {
          #if FL_MODEL_OBJ != 0
          tempOut = mdl3dStatSaveOBJ(inPath, inModel);
          #else
          #if FL_DEBUG_DEVWARNING != 0
          debugDevWarning("OBJ models not supported in this build\nSet FL_MODEL_OBJ to non-zero to enable them.");
          #endif
          #endif
     }

     memFree(tempExt);

     if(!tempOut) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Cannot save this model type.");
          #endif
          return false;
     }
     
     return tempOut;
}

void mdl3dStatDrawTextured(Model3dStatic* inModel) {
     int i, j;     
     vertTfVf* tempVerts;
     int tempVertCount = 0;
     Texture* tempCurTex = NULL;
     if(graphicsWireframe) {
          for(i = 0; i < inModel->mdlFaceCount; i++)
               tempVertCount += inModel->mdlFaces[i].mdlfVertCount;
          tempVerts = sceGuGetMemory(sizeof(vertTfVf) * tempVertCount);
          int tempVertIndex = 0;
          for(i = 0; i < inModel->mdlFaceCount; i++) {
               if(!tempCurTex || (inModel->mdlFaces[i].mdlfTexture != tempCurTex)) {
                    texBind(inModel->mdlFaces[i].mdlfTexture);
                    sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
                    if(inModel->mdlFaces[i].mdlfTexture->texMipMaps)
                         sceGuTexFilter(GU_LINEAR_MIPMAP_LINEAR, GU_LINEAR_MIPMAP_LINEAR);
                    else
                         sceGuTexFilter(GU_LINEAR, GU_LINEAR);
                    tempCurTex = inModel->mdlFaces[i].mdlfTexture;
               }
               for(j = 0; j < inModel->mdlFaces[i].mdlfVertCount; j++) {
                    tempVerts[tempVertIndex].vertU = inModel->mdlFaces[i].mdlfTexVerts[j]->x;
                    tempVerts[tempVertIndex].vertV = inModel->mdlFaces[i].mdlfTexVerts[j]->y;
                    tempVerts[tempVertIndex].vertX = inModel->mdlFaces[i].mdlfVerts[j]->x;
                    tempVerts[tempVertIndex].vertY = inModel->mdlFaces[i].mdlfVerts[j]->y;
                    tempVerts[tempVertIndex].vertZ = inModel->mdlFaces[i].mdlfVerts[j]->z;
                    tempVertIndex++;
          
                    if((j + 1) >= inModel->mdlFaces[i].mdlfVertCount) {
                         tempVerts[tempVertIndex].vertU = inModel->mdlFaces[i].mdlfTexVerts[0]->x;
                         tempVerts[tempVertIndex].vertV = inModel->mdlFaces[i].mdlfTexVerts[0]->y;
                         tempVerts[tempVertIndex].vertX = inModel->mdlFaces[i].mdlfVerts[0]->x;
                         tempVerts[tempVertIndex].vertY = inModel->mdlFaces[i].mdlfVerts[0]->y;
                         tempVerts[tempVertIndex].vertZ = inModel->mdlFaces[i].mdlfVerts[0]->z;
                    } else {
                         tempVerts[tempVertIndex].vertU = inModel->mdlFaces[i].mdlfTexVerts[j + 1]->x;
                         tempVerts[tempVertIndex].vertV = inModel->mdlFaces[i].mdlfTexVerts[j + 1]->y;
                         tempVerts[tempVertIndex].vertX = inModel->mdlFaces[i].mdlfVerts[j + 1]->x;
                         tempVerts[tempVertIndex].vertY = inModel->mdlFaces[i].mdlfVerts[j + 1]->y;
                         tempVerts[tempVertIndex].vertZ = inModel->mdlFaces[i].mdlfVerts[j + 1]->z;
                    }
                    tempVertIndex++;
               }
          }
          sceGumDrawArray(GU_LINES, GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D, tempVertCount, 0, tempVerts);
     } else {
          for(i = 0; i < inModel->mdlFaceCount; i++)
               tempVertCount += (inModel->mdlFaces[i].mdlfVertCount - 2);
          tempVertCount += (tempVertCount << 1);
          tempVerts = sceGuGetMemory(sizeof(vertTfVf) * tempVertCount);
          int tempVertIndex = 0;
          for(i = 0; i < inModel->mdlFaceCount; i++) {
               if(!tempCurTex || (inModel->mdlFaces[i].mdlfTexture != tempCurTex)) {
                    texBind(inModel->mdlFaces[i].mdlfTexture);
                    sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
                    if(inModel->mdlFaces[i].mdlfTexture->texMipMaps)
                         sceGuTexFilter(GU_LINEAR_MIPMAP_LINEAR, GU_LINEAR_MIPMAP_LINEAR);
                    else
                         sceGuTexFilter(GU_LINEAR, GU_LINEAR);
                    tempCurTex = inModel->mdlFaces[i].mdlfTexture;
               }
               for(j = 0; j < (inModel->mdlFaces[i].mdlfVertCount - 2); j++) {
                    tempVerts[tempVertIndex].vertU = inModel->mdlFaces[i].mdlfTexVerts[0]->x;
                    tempVerts[tempVertIndex].vertV = inModel->mdlFaces[i].mdlfTexVerts[0]->y;
                    tempVerts[tempVertIndex].vertX = inModel->mdlFaces[i].mdlfVerts[0]->x;
                    tempVerts[tempVertIndex].vertY = inModel->mdlFaces[i].mdlfVerts[0]->y;
                    tempVerts[tempVertIndex].vertZ = inModel->mdlFaces[i].mdlfVerts[0]->z;
                    tempVertIndex++;
          
                    tempVerts[tempVertIndex].vertU = inModel->mdlFaces[i].mdlfTexVerts[j + 1]->x;
                    tempVerts[tempVertIndex].vertV = inModel->mdlFaces[i].mdlfTexVerts[j + 1]->y;
                    tempVerts[tempVertIndex].vertX = inModel->mdlFaces[i].mdlfVerts[j + 1]->x;
                    tempVerts[tempVertIndex].vertY = inModel->mdlFaces[i].mdlfVerts[j + 1]->y;
                    tempVerts[tempVertIndex].vertZ = inModel->mdlFaces[i].mdlfVerts[j + 1]->z;
                    tempVertIndex++;

                    tempVerts[tempVertIndex].vertU = inModel->mdlFaces[i].mdlfTexVerts[j + 2]->x;
                    tempVerts[tempVertIndex].vertV = inModel->mdlFaces[i].mdlfTexVerts[j + 2]->y;
                    tempVerts[tempVertIndex].vertX = inModel->mdlFaces[i].mdlfVerts[j + 2]->x;
                    tempVerts[tempVertIndex].vertY = inModel->mdlFaces[i].mdlfVerts[j + 2]->y;
                    tempVerts[tempVertIndex].vertZ = inModel->mdlFaces[i].mdlfVerts[j + 2]->z;
                    tempVertIndex++;
               }
          }
          sceGumDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D, tempVertCount, 0, tempVerts);
     }
}

void mdl3dStatDraw(Model3dStatic* inModel) {
     if(!inModel) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to draw NULL model.");
          #endif
          return;
     }
     if(!inModel->mdlVerts) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to draw model with NULL data.");
          #endif
          return;
     }

     if(inModel->mdlTextureCount > 0) {
          mdl3dStatDrawTextured(inModel);
          return;
     }

     sceGuDisable(GU_TEXTURE_2D);

     int i, j;     
     vertVf* tempVerts;
     int tempVertCount = 0;
     if(graphicsWireframe) {
          for(i = 0; i < inModel->mdlFaceCount; i++)
               tempVertCount += inModel->mdlFaces[i].mdlfVertCount;
          tempVerts = sceGuGetMemory(sizeof(vertVf) * tempVertCount);
          int tempVertIndex = 0;
          for(i = 0; i < inModel->mdlFaceCount; i++) {
               for(j = 0; j < inModel->mdlFaces[i].mdlfVertCount; j++) {
                    tempVerts[tempVertIndex].vertX = inModel->mdlFaces[i].mdlfVerts[j]->x;
                    tempVerts[tempVertIndex].vertY = inModel->mdlFaces[i].mdlfVerts[j]->y;
                    tempVerts[tempVertIndex].vertZ = inModel->mdlFaces[i].mdlfVerts[j]->z;
                    tempVertIndex++;
          
                    if((j + 1) >= inModel->mdlFaces[i].mdlfVertCount) {
                         tempVerts[tempVertIndex].vertX = inModel->mdlFaces[i].mdlfVerts[0]->x;
                         tempVerts[tempVertIndex].vertY = inModel->mdlFaces[i].mdlfVerts[0]->y;
                         tempVerts[tempVertIndex].vertZ = inModel->mdlFaces[i].mdlfVerts[0]->z;
                    } else {
                         tempVerts[tempVertIndex].vertX = inModel->mdlFaces[i].mdlfVerts[j + 1]->x;
                         tempVerts[tempVertIndex].vertY = inModel->mdlFaces[i].mdlfVerts[j + 1]->y;
                         tempVerts[tempVertIndex].vertZ = inModel->mdlFaces[i].mdlfVerts[j + 1]->z;
                    }
                    tempVertIndex++;
               }
          }
          sceGumDrawArray(GU_LINES, GU_VERTEX_32BITF | GU_TRANSFORM_3D, tempVertCount, 0, tempVerts);
     } else {
          for(i = 0; i < inModel->mdlFaceCount; i++)
               tempVertCount += (inModel->mdlFaces[i].mdlfVertCount - 2);
          tempVertCount += (tempVertCount << 1);
          tempVerts = sceGuGetMemory(sizeof(vertVf) * tempVertCount);
          int tempVertIndex = 0;
          for(i = 0; i < inModel->mdlFaceCount; i++) {
               for(j = 0; j < (inModel->mdlFaces[i].mdlfVertCount - 2); j++) {
                    tempVerts[tempVertIndex].vertX = inModel->mdlFaces[i].mdlfVerts[0]->x;
                    tempVerts[tempVertIndex].vertY = inModel->mdlFaces[i].mdlfVerts[0]->y;
                    tempVerts[tempVertIndex].vertZ = inModel->mdlFaces[i].mdlfVerts[0]->z;
                    tempVertIndex++;
          
                    tempVerts[tempVertIndex].vertX = inModel->mdlFaces[i].mdlfVerts[j + 1]->x;
                    tempVerts[tempVertIndex].vertY = inModel->mdlFaces[i].mdlfVerts[j + 1]->y;
                    tempVerts[tempVertIndex].vertZ = inModel->mdlFaces[i].mdlfVerts[j + 1]->z;
                    tempVertIndex++;

                    tempVerts[tempVertIndex].vertX = inModel->mdlFaces[i].mdlfVerts[j + 2]->x;
                    tempVerts[tempVertIndex].vertY = inModel->mdlFaces[i].mdlfVerts[j + 2]->y;
                    tempVerts[tempVertIndex].vertZ = inModel->mdlFaces[i].mdlfVerts[j + 2]->z;
                    tempVertIndex++;
               }
          }
          sceGumDrawArray(GU_TRIANGLES, GU_VERTEX_32BITF | GU_TRANSFORM_3D, tempVertCount, 0, tempVerts);
     }
    
     sceGuEnable(GU_TEXTURE_2D);
}

void mdl3dStatOptimize(Model3dStatic* inModel) {
     if(!inModel) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to optimize a NULL model.");
          #endif
          return;
     }
     if(inModel->mdlOptimized) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to optimize an already optimized model.");
          #endif
          return;
     }
     vect3f   tempVerts[inModel->mdlVertCount];
     vect2f   tempTexVerts[inModel->mdlTexVertCount];
     vect3f   tempNormals[inModel->mdlNormalCount];
     Texture* tempTextures[inModel->mdlTextureCount];
     int tempVertCount = 0;
     int tempTexVertCount = 0;
     int tempNormalCount = 0;
     int tempTextureCount = 0;
     
     int i, j;
     bool tempDuped;
     
     // Remove duped verts
     for(i = 0; i < inModel->mdlVertCount; i++) {
          tempDuped = false;
          for(j = 0; j < tempVertCount; j++) {
               if(vect3f_Cmp(tempVerts[j], inModel->mdlVerts[i])) {
                    tempDuped = true;
                    break;
               }
          }
          if(!tempDuped) {
               tempVerts[tempVertCount] = inModel->mdlVerts[i];
               tempVertCount++;
          }
     }
     
     // Remove duped tex verts
     for(i = 0; i < inModel->mdlTexVertCount; i++) {
          tempDuped = false;
          for(j = 0; j < tempTexVertCount; j++) {
               if(vect2f_Cmp(tempTexVerts[j], inModel->mdlTexVerts[i])) {
                    tempDuped = true;
                    break;
               }
          }
          if(!tempDuped) {
               tempTexVerts[tempTexVertCount] = inModel->mdlTexVerts[i];
               tempTexVertCount++;
          }
     }
     
     // Remove duped normals
     for(i = 0; i < inModel->mdlNormalCount; i++) {
          tempDuped = false;
          for(j = 0; j < tempNormalCount; j++) {
               if(vect3f_Cmp(tempNormals[j], inModel->mdlNormals[i])) {
                    tempDuped = true;
                    break;
               }
          }
          if(!tempDuped) {
               tempNormals[tempNormalCount] = inModel->mdlNormals[i];
               tempNormalCount++;
          }
     }
     
     // Remove duped textures
     for(i = 0; i < inModel->mdlTextureCount; i++) {
          tempDuped = false;
          for(j = 0; j < tempTextureCount; j++) {
               if(tempTextures[j] == inModel->mdlTextures[i]) {
                    tempDuped = true;
                    break;
               }
          }
          if(!tempDuped) {
               tempTextures[tempTextureCount] = inModel->mdlTextures[i];
               tempTextureCount++;
          }
     }
     
     Model3dStatic tempOut;
     
     tempOut.mdlVertCount = tempVertCount;
     tempOut.mdlTextureCount = tempTextureCount;
     tempOut.mdlTexVertCount = tempTexVertCount;
     tempOut.mdlNormalCount = tempNormalCount;
     tempOut.mdlFaceCount = inModel->mdlFaceCount;

     u32 tempSize = (sizeof(vect3f) * tempVertCount);
     tempSize += (sizeof(vect2f) * tempTexVertCount);
     tempSize += (sizeof(vect3f) * tempNormalCount);
     tempSize += (sizeof(Model3dStaticFace) * inModel->mdlFaceCount);
     tempSize += (sizeof(Texture*) * tempTextureCount);

     u8* tempAllocBlock = memAlloc(tempSize);
     if(!tempAllocBlock) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Couldn't allocate optimized model data.\nProbably out of memory.");
          #endif
          return;
     }
     
     u32 tempOffset = 0;
     tempOut.mdlVerts = (vect3f*)(tempAllocBlock);
     tempOffset += (sizeof(vect3f) * tempOut.mdlVertCount);
     tempOut.mdlTexVerts = (vect2f*)((u32)(tempAllocBlock) + tempOffset);
     tempOffset += (sizeof(vect2f) * tempOut.mdlTexVertCount);
     tempOut.mdlNormals = (vect3f*)((u32)(tempAllocBlock) + tempOffset);
     tempOffset += (sizeof(vect3f) * tempOut.mdlNormalCount);
     tempOut.mdlFaces = (Model3dStaticFace*)((u32)(tempAllocBlock) + tempOffset);
     tempOffset += (sizeof(Model3dStaticFace) * tempOut.mdlFaceCount);     
     tempOut.mdlTextures = (Texture**)((u32)(tempAllocBlock) + tempOffset);
     
     for(i = 0; i < tempVertCount; i++)
          tempOut.mdlVerts[i] = tempVerts[i];
     for(i = 0; i < tempTexVertCount; i++)
          tempOut.mdlTexVerts[i] = tempTexVerts[i];
     for(i = 0; i < tempNormalCount; i++)
          tempOut.mdlNormals[i] = tempNormals[i];
     for(i = 0; i < tempTextureCount; i++)
          tempOut.mdlTextures[i] = tempTextures[i];

     int k;
     // Apply new pointers to existing faces
     for(i = 0; i < inModel->mdlFaceCount; i++) {
          for(k = 0; k < inModel->mdlFaces[i].mdlfVertCount; k++) {
               for(j = 0; j < tempOut.mdlVertCount; j++) {
                    if(vect3f_Cmp(*inModel->mdlFaces[i].mdlfVerts[k], tempOut.mdlVerts[j])) {
                         inModel->mdlFaces[i].mdlfVerts[k] = &tempOut.mdlVerts[j];
                         break;
                    }
               }
               for(j = 0; j < tempOut.mdlTexVertCount; j++) {
                    if(vect2f_Cmp(*inModel->mdlFaces[i].mdlfTexVerts[k], tempOut.mdlTexVerts[j])) {
                         inModel->mdlFaces[i].mdlfTexVerts[k] = &tempOut.mdlTexVerts[j];
                         break;
                    }
               }
               for(j = 0; j < tempOut.mdlNormalCount; j++) {
                    if(vect3f_Cmp(*inModel->mdlFaces[i].mdlfNormals[k], tempOut.mdlNormals[j])) {
                        inModel->mdlFaces[i].mdlfNormals[k] = &tempOut.mdlNormals[j];
                        break;
                    }
               }
          }
          tempOut.mdlFaces[i] = inModel->mdlFaces[i];
     }
     
     memFree(inModel->mdlVerts);
     inModel->mdlVertCount = tempOut.mdlVertCount;
     inModel->mdlTexVertCount = tempOut.mdlTexVertCount;
     inModel->mdlNormalCount = tempOut.mdlNormalCount;
     inModel->mdlTextureCount = tempOut.mdlTextureCount;
     inModel->mdlVerts = tempOut.mdlVerts;
     inModel->mdlTexVerts = tempOut.mdlTexVerts;
     inModel->mdlNormals = tempOut.mdlNormals;
     inModel->mdlTextures = tempOut.mdlTextures;
     inModel->mdlOptimized = true;
     
     // TODO - Free textures by appropriate number of times.
     // TODO - Remove duped faces
}

void mdl3dStatFree(Model3dStatic* inModel) {
     if(!inModel) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to free NULL model.");
          #endif
          return;
     }
     #if FL_MEMORY != 0
     if(inModel->mdlVerts) {
          int i;
          for(i = 0; i < inModel->mdlFaceCount; i++) {
               if(inModel->mdlFaces[i].mdlfVerts)
                    memFree(inModel->mdlFaces[i].mdlfVerts);
          }
          for(i = 0; i < inModel->mdlTextureCount; i++)
               texFree(inModel->mdlTextures[i]);
          memFree(inModel->mdlVerts);
     }
     #else
     int i;
     for(i = 0; i < inModel->mdlFaceCount; i++) {
          memFree(inModel->mdlFaces[i].mdlfVerts);
     for(i = 0; i < inModel->mdlTextureCount; i++)
          texFree(inModel->mdlTextures[i]);
     memFree(inModel->mdlVerts);
     #endif
     memFree(inModel);
}

Model3dAnimated* mdl3dAnimLoad(char* inPath) {
     if(!inPath || (inPath[0] == 0)) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Invalid model file path.");
          #endif
          return NULL;
     }
     char* tempExt = fileExtension(inPath);
     if(!tempExt) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Cannot determine file extension for model.");
          #endif
          return NULL;
     }
     strLCase((u8*)tempExt);
     Model3dAnimated* tempOut = NULL;
     if(strncmp(tempExt, "md2", 4)) {
          #if FL_MODEL_MD2 != 0
          tempOut = mdl3dAnimLoadMD2(inPath);
          #else
          #if FL_DEBUG_DEVWARNING != 0
          debugDevWarning("MD2 models not supported in this build\nSet FL_MODEL_MD2 to non-zero to enable them.");
          #endif
          #endif
     }
     #if FL_DEBUG_WARNING != 0
     if(!tempOut)
          debugWarning("Cannot open this file type as model.");
     #endif
     memFree(tempExt);
     
     return tempOut;
}

bool mdl3dAnimSave(char* inPath, Model3dAnimated* inModel) {
     return false;
}

void mdl3dAnimDraw(Model3dAnimated* inModel) {
     if(!inModel)
          return;
     if(inModel->mdlType == MODEL_ANIM_TYPE_MD2)
          mdl3dAnimDrawMD2(inModel->mdlData, 0.0f);
}

void mdl3dAnimFree(Model3dAnimated* inModel) {

}

#endif
