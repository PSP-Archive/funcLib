// This file is the global funcLib configuration header,
// it should be included in all your source files.
// Unless you have FL_INCLUDE_ALL enabled obviously.

#ifndef FLGLOBAL_H
#define FLGLOBAL_H

#define FL_MATH_RAND_STDIO           0 ///< Constant for use in the FL_MATH_RANDTYPE option, to enable STDIO type random number generation.
#define FL_MATH_RAND_VFPU            1 ///< Constant for use in the FL_MATH_RANDTYPE option, to enable VFPU type random number generation.
#define FL_MATH_RAND_MERSENNETWISTER 2 ///< Constant for use in the FL_MATH_RANDTYPE option, to enable MERSENNETWISTER type random number generation.
#define FL_MATH_RAND_MT              FL_MATH_RAND_MERSENNETWISTER ///< Alias for FL_MATH_RAND_MERSENNETWISTER.

#include <pspgu.h>             // Needed for diplay type constants.

#include "flConfiguration.h"          // Users custom configuration.
#include "flConfigurationDefault.h"   // Default working configuration.
#include "flConfigurationValidate.h"  // Validate user configuration.
#include "flTypes.h"           // Define all funcLib types.
#include "flDefines.h"         // Define useful constants, etc.

#if FL_MATH_VFPU != 0
#define PSP_THREAD_ATTR_FUNCLIB (PSP_THREAD_ATTR_VFPU) ///< FuncLib required thread attributes.
#else
#define PSP_THREAD_ATTR_FUNCLIB 0 ///< FuncLib required thread attributes.
#endif

// TODO - Get PSP_THREAD_ATTR_SCRATCH_SRAM to work.
// I think it crashes because of the kernel that the code runs on or sth.

#endif
