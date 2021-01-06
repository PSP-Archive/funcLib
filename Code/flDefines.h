#ifndef FLDEFINES_H
#define FLDEFINES_H

#define max(x, y) ((x) > (y) ? (x) : (y))
#define min(x, y) ((x) < (y) ? (x) : (y))
#define pos(x) ((x) < (0) ? (0 - (x)) : (x))
#define cmp(inType, inVal0, inVal1) (memcmp((inVal0), (inVal1), sizeof(inType)))

#define LOGICAL_XOR(inExpr0, inExpr1) (((inExpr0) || (inExpr1)) && !((inExpr0) && (inExpr1)))

#define SCREEN_WIDTH    480
#define SCREEN_HEIGHT   272
#define SCREEN_PITCH    512
#define SCREEN_STRIDE   SCREEN_PITCH
#define SCREEN_LINESIZE SCREEN_PITCH
#define SCREEN_GAMMA    2.5f

#define ASCII_NULL           0
#define ASCII_BELL           7
#define ASCII_AUDIBLE_BELL   ASCII_BELL
#define ASCII_AUDIBLEBELL    ASCII_BELL
#define ASCII_BS             8
#define ASCII_BACKSPACE      ASCII_BS
#define ASCII_TAB            9
#define ASCII_LF             10
#define ASCII_LINEFEED       ASCII_LF
#define ASCII_CR             13
#define ASCII_CARRIAGERETURN ASCII_CR
#define ASCII_SHIFT_OUT      14
#define ASCII_SHIFT_IN       15
#define ASCII_SPACE          32
#define ASCII_DEL            127
#define ASCII_DELETE         ASCII_DEL

#define ASCII_A 'A'
#define ASCII_Z 'Z'
#define ASCII_a 'a'
#define ASCII_z 'z'
#define ASCII_0 '0'
#define ASCII_9 '9'

#define ASCII_MULTIPLY 158

#define MATH_PI          3.14159265358979323846264338327950288f
#define MATH_E           2.71828182845904523536028747135266249f
#define MATH_GOLDENRATIO 1.61803398874989484820458683436563811f

// Fixes for macros that should exist in the psp sdk
#define GU_SYNC_WAIT 0
#define GU_SYNC_NOWAIT 1



// Add extra compatability for other compilers
#ifndef __cplusplus
     #ifdef _LANGUAGE_C_PLUS_PLUS
          #define __cplusplus
     #endif
     #ifdef c_plusplus
          #define __cplusplus
     #endif
#endif

#endif
