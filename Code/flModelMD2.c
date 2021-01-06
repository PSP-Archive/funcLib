#include "flGlobal.h"
#if FL_MODEL_MD2 != 0
#include <stdlib.h>
#include <stdio.h>
#include <psprtc.h>

#if FL_INCLUDE_ALL_C == 0
#include "flModelMD2.h"
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

typedef struct {
     char md2Ident[4];
     int  md2Version;
     int  md2SkinWidth;
     int  md2SkinHeight;
     int  md2FrameSize;
     
     int  md2TextureCount;
     int  md2VertCount;
     int  md2TexVertCount;
     int  md2TriCount;
     int  md2GlCmdCount;
     int  md2FrameCount;

     int  md2TextureOffset;
     int  md2TexVertOffset;
     int  md2TriOffset;
     int  md2FrameOffset;
     int  md2GlCmdOffset;
     int  md2EndOffset;
} md2Header;

typedef struct {
     char md2TextureName[64];
} md2Texture;

typedef struct {
     s16 md2TexVertU;
     s16 md2TexVertV;
} md2TexVert;

typedef struct {
     u16 md2TriVerts[3];
     u16 md2TriTexVerts[3];
} md2Tri;

typedef struct {
     u8 md2VertexPos[3];
     u8 md2VertexNormal;
} md2Vertex;

typedef struct {
     vect3f     md2FrameScale;
     vect3f     md2FrameTranslate;
     char       md2FrameName[16];
     md2Vertex* md2FrameVerts;
} md2Frame;

typedef struct {
     u32         md2TextureCount;
     u32         md2TexVertCount;
     u32         md2VertCount;
     u32         md2TriCount;
     u32         md2FrameCount;
     
     Texture**   md2Textures;
     md2TexVert* md2TexVerts;
     md2Tri*     md2Tris;
     md2Frame*   md2Frames;
} Model3dAnimatedMD2;

Model3dAnimated* mdl3dAnimLoadMD2(char* inPath) {
     Model3dAnimatedMD2* tempModel = (Model3dAnimatedMD2*)memAlloc(sizeof(Model3dAnimatedMD2));
     if(!tempModel) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Can't create md2 model struct.\nProbably out of memory.");
          #endif
          return NULL;
     }
     Model3dAnimated* tempOut = (Model3dAnimated*)memAlloc(sizeof(Model3dAnimated));
     if(!tempOut) {
          memFree(tempModel);
          #if FL_DEBUG_WARNING != 0
          debugWarning("Can't create model struct.\nProbably out of memory.");
          #endif
          return NULL;
     }
     tempOut->mdlType = MODEL_ANIM_TYPE_MD2;
     tempOut->mdlData = (void*)tempModel;
     
     
     md2Header tempHeader;
     int i, j;
     
     #if FL_FILE != 0
     File* tempFile = fileOpen(inPath, FILE_MODE_READ | FILE_MODE_BINARY);
     #else
     FILE* tempFile = fopen(inPath, "rb");
     #endif
     
     fileRead(tempHeader.md2Ident, 4, tempFile);
     fileRead(&tempHeader.md2Version, 4, tempFile);
     if(strncmp(tempHeader.md2Ident, "IDP2", 4) || (tempHeader.md2Version != 8)) {
          fileClose(tempFile);
          memFree(tempModel);
          memFree(tempOut);
          #if FL_DEBUG_WARNING != 0
          debugWarning("Only IDP2 version 8 md2 files are supported.");
          #endif
          return NULL;
     }
     fileRead(&tempHeader.md2SkinWidth, 4, tempFile);
     fileRead(&tempHeader.md2SkinHeight, 4, tempFile);
     fileRead(&tempHeader.md2FrameSize, 4, tempFile);
     fileRead(&tempHeader.md2TextureCount, 4, tempFile);
     fileRead(&tempHeader.md2VertCount, 4, tempFile);
     fileRead(&tempHeader.md2TexVertCount, 4, tempFile);
     fileRead(&tempHeader.md2TriCount, 4, tempFile);
     fileRead(&tempHeader.md2GlCmdCount, 4, tempFile);
     fileRead(&tempHeader.md2FrameCount, 4, tempFile);
     fileRead(&tempHeader.md2TextureOffset, 4, tempFile);
     fileRead(&tempHeader.md2TexVertOffset, 4, tempFile);
     fileRead(&tempHeader.md2TriOffset, 4, tempFile);
     fileRead(&tempHeader.md2FrameOffset, 4, tempFile);
     fileRead(&tempHeader.md2GlCmdOffset, 4, tempFile);
     fileRead(&tempHeader.md2EndOffset, 4, tempFile);
     
     tempModel->md2TextureCount = tempHeader.md2TextureCount;
     tempModel->md2TexVertCount = tempHeader.md2TexVertCount;
     tempModel->md2VertCount = tempHeader.md2VertCount;
     tempModel->md2TriCount = tempHeader.md2TriCount;
     tempModel->md2FrameCount = tempHeader.md2FrameCount;
     
     u32 tempSize = (sizeof(Texture*) * tempHeader.md2TextureCount);
     tempSize += (sizeof(md2TexVert) * tempHeader.md2TexVertCount);
     tempSize += (sizeof(md2Tri) * tempHeader.md2TriCount);
     tempSize += (sizeof(md2Frame) * tempHeader.md2FrameCount);
     tempSize += (sizeof(md2Vertex) * tempHeader.md2VertCount * tempHeader.md2FrameCount);
     void* tempBlockAlloc = memAlloc(tempSize);
     if(!tempBlockAlloc) {
          memFree(tempOut);
          memFree(tempModel);
          fileClose(tempFile);
          #if FL_DEBUG_WARNING != 0
          debugWarning("Can't allocate memory for model data.\nProbably out of memory.");
          #endif
          return NULL;
     }
     
     tempModel->md2Textures = (Texture**)tempBlockAlloc;
     u32 tempOffset = (sizeof(Texture*) * tempHeader.md2TextureCount);
     tempModel->md2TexVerts = (md2TexVert*)((u32)tempBlockAlloc + tempOffset);
     tempOffset += (sizeof(md2TexVert) * tempHeader.md2TexVertCount);
     tempModel->md2Tris = (md2Tri*)((u32)tempBlockAlloc + tempOffset);
     tempOffset += (sizeof(md2Tri) * tempHeader.md2TexVertCount);
     tempModel->md2Frames = (md2Frame*)((u32)tempBlockAlloc + tempOffset);
     tempOffset += (sizeof(md2Frame) * tempHeader.md2FrameCount);

     char tempTexPath[64];     
     fileSeek(tempFile, tempHeader.md2TextureOffset, FILE_SEEK_SET);
     for(i = 0; i < tempHeader.md2TextureCount; i++) {
          fileRead(tempTexPath, 64, tempFile);
          tempModel->md2Textures[i] = texLoad(tempTexPath);
          if(!tempModel->md2Textures[i]) {
               mdl3dAnimFree(tempOut);
               fileClose(tempFile);
               #if FL_DEBUG_WARNING != 0
               debugWarning("Can't open texture file in MD2.");
               #endif
               return NULL;
          }
     }
     
     fileSeek(tempFile, tempHeader.md2TexVertOffset, FILE_SEEK_SET);
     for(i = 0; i < tempHeader.md2TexVertCount; i++) {
          fileRead(&tempModel->md2TexVerts[i].md2TexVertU, 2, tempFile);
          fileRead(&tempModel->md2TexVerts[i].md2TexVertV, 2, tempFile);
     }
     
     fileSeek(tempFile, tempHeader.md2TriOffset, FILE_SEEK_SET);
     for(i = 0; i < tempHeader.md2TriCount; i++) {
          for(j = 0; j < 3; j++)
               fileRead(&tempModel->md2Tris[i].md2TriVerts[j], 2, tempFile);
          for(j = 0; j < 3; j++)
               fileRead(&tempModel->md2Tris[i].md2TriTexVerts[j], 2, tempFile);
     }

     fileSeek(tempFile, tempHeader.md2FrameOffset, FILE_SEEK_SET);
     for(i = 0; i < tempHeader.md2FrameCount; i++) {
          fileRead(&tempModel->md2Frames[i].md2FrameScale.x, 4, tempFile);
          fileRead(&tempModel->md2Frames[i].md2FrameScale.y, 4, tempFile);
          fileRead(&tempModel->md2Frames[i].md2FrameScale.z, 4, tempFile);
          fileRead(&tempModel->md2Frames[i].md2FrameTranslate.x, 4, tempFile);
          fileRead(&tempModel->md2Frames[i].md2FrameTranslate.x, 4, tempFile);
          fileRead(&tempModel->md2Frames[i].md2FrameTranslate.x, 4, tempFile);
          fileRead(tempModel->md2Frames[i].md2FrameName, 16, tempFile);
          tempModel->md2Frames[i].md2FrameVerts = (md2Vertex*)((u32)tempBlockAlloc + tempOffset);
          tempOffset += (sizeof(md2Vertex) * tempHeader.md2VertCount);
          for(j = 0; j < tempHeader.md2VertCount; j++) {
               fileRead(tempModel->md2Frames[i].md2FrameVerts[j].md2VertexPos, 3, tempFile);
               fileRead(&tempModel->md2Frames[i].md2FrameVerts[j].md2VertexNormal, 1, tempFile);
          }
     }
     
     fileClose(tempFile);
     
     return tempOut;
}

void mdl3dAnimDrawMD2(void* inModel, float inFrame) {
     if(!inModel) {
          #if FL_DEBUG_WARNING != 0
          debugWarning("Trying to draw NULL md2.");
          #endif
          return;
     }

     Model3dAnimatedMD2* tempModel = (Model3dAnimatedMD2*)inModel;

     while(inFrame >= (float)tempModel->md2FrameCount)
          inFrame -= (float)tempModel->md2FrameCount;
     
     int tempFrames[2] = { (int)inFrame, (int)(inFrame + 1.0f) };
     if(tempFrames[1] >= tempModel->md2FrameCount)
          tempFrames[1] -= tempModel->md2FrameCount;
     float tempFract = (inFrame - (float)tempFrames[0]);

     vect3f tempScale = vect3f_Mulf(tempModel->md2Frames[tempFrames[0]].md2FrameScale, (1.0f - tempFract));
     tempScale = vect3f_Add(tempScale, vect3f_Mulf(tempModel->md2Frames[tempFrames[1]].md2FrameScale, tempFract));
     vect3f tempTranslate = vect3f_Mulf(tempModel->md2Frames[tempFrames[0]].md2FrameTranslate, (1.0f - tempFract));
     tempTranslate = vect3f_Add(tempTranslate, vect3f_Mulf(tempModel->md2Frames[tempFrames[1]].md2FrameTranslate, tempFract));

     texBind(tempModel->md2Textures[0]);

     sceGumPushMatrix();
     sceGumScale(&tempScale);
     sceGumTranslate(&tempTranslate);

     int i, j;
     vertTsVf* tempVerts = (vertTsVf*)sceGuGetMemory(sizeof(vertTsVf) * (tempModel->md2TriCount * 3));
     for(i = 0; i < tempModel->md2TriCount; i++) {
          for(j = 0; j < 3; j++) {
               tempVerts[(i * 3) + j].vertX = (float)tempModel->md2Frames[tempFrames[0]].md2FrameVerts[tempModel->md2Tris[i].md2TriVerts[j]].md2VertexPos[0] * (1.0f - tempFract);
               tempVerts[(i * 3) + j].vertX += (float)tempModel->md2Frames[tempFrames[1]].md2FrameVerts[tempModel->md2Tris[i].md2TriVerts[j]].md2VertexPos[0] * tempFract;
               tempVerts[(i * 3) + j].vertY = (float)tempModel->md2Frames[tempFrames[0]].md2FrameVerts[tempModel->md2Tris[i].md2TriVerts[j]].md2VertexPos[1] * (1.0f - tempFract);
               tempVerts[(i * 3) + j].vertY += (float)tempModel->md2Frames[tempFrames[1]].md2FrameVerts[tempModel->md2Tris[i].md2TriVerts[j]].md2VertexPos[1] * tempFract;
               tempVerts[(i * 3) + j].vertZ = (float)tempModel->md2Frames[tempFrames[0]].md2FrameVerts[tempModel->md2Tris[i].md2TriVerts[j]].md2VertexPos[2] * (1.0f - tempFract);
               tempVerts[(i * 3) + j].vertZ += (float)tempModel->md2Frames[tempFrames[1]].md2FrameVerts[tempModel->md2Tris[i].md2TriVerts[j]].md2VertexPos[2] * tempFract;
               tempVerts[(i * 3) + j].vertU += tempModel->md2TexVerts[tempModel->md2Tris[i].md2TriTexVerts[j]].md2TexVertU;
               tempVerts[(i * 3) + j].vertV += tempModel->md2TexVerts[tempModel->md2Tris[i].md2TriTexVerts[j]].md2TexVertV;
          }
     }
     sceGumDrawArray(GU_TRIANGLES, GU_TEXTURE_16BIT | GU_VERTEX_32BITF | GU_TRANSFORM_3D, (tempModel->md2TriCount * 3), 0, tempVerts);
     sceGumPopMatrix();
}

bool mdl3dAnimSaveMD2(char* inPath, Model3dAnimated* inModel) {
     #if FL_DEBUG_DEVWARNING != 0
     debugDevWarning("MD2 saving is not yet implemented.");
     #endif
     return false;
}

#endif
