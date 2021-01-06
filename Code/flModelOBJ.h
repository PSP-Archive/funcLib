// Funclib ModelOBJ v1.0.0
// 
// This module contains functions for loading OBJ models.
// 
// Contributor(s): Flatmush



#ifndef FLMODELOBJ_H
#define FLMODELOBJ_H

#include "flGlobal.h"
#include "flTexture.h"
#include "flModel.h"

#if FL_MODEL_OBJ != 0

#ifdef __cplusplus
extern "C" {
#endif

extern Model3dStatic* mdl3dStatLoadOBJ(char* inPath);
extern bool           mdl3dStatSaveOBJ(char* inPath, Model3dStatic* inModel);

#ifdef __cplusplus
}
#endif

#endif

#endif
