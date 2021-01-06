// Funclib ModelMD2 v1.0.0
// 
// This module contains functions for loading animated MD2 models.
// 
// Contributor(s): Flatmush



#ifndef FLMODELMD2_H
#define FLMODELMD2_H

#include "flGlobal.h"
#include "flTexture.h"
#include "flModel.h"

#if FL_MODEL_MD2 != 0

#ifdef __cplusplus
extern "C" {
#endif

extern Model3dAnimated* mdl3dAnimLoadMD2(char* inPath);
extern void             mdl3dAnimDrawMD2(void* inModel, float inFrame);
extern bool             mdl3dAnimSaveMD2(char* inPath, Model3dAnimated* inModel);

#ifdef __cplusplus
}
#endif

#endif

#endif
