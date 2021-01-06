#ifndef FUNCLIB_H
#define FUNCLIB_H

#include <pspkernel.h>
#include "flGlobal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * A boolean to represent the running state of funclib,
  * a non-zero value indicates that funclib is running.
  * @note This should be used in infinite loops (e.g while(1) -> while(flRunning) )
  */
extern bool flRunning;


/**
  * Initialize funclib and all of its modules.
  * @note This sets flRunning to a non-zero value.
  */
extern void flInitialize();

/**
  * Terminate funclib and all of its modules.
  * @note This sets flRunning to zero.
  */
extern void flTerminate();

#if FL_INCLUDE_ALL_H != 0
#include "flIncludeH.h"
#endif

#ifdef __cplusplus
}
#endif

#endif
