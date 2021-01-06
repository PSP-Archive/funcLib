#include "flGlobal.h"
#if FL_MATH != 0
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#if FL_MATH_RANDTYPE != FL_MATH_RAND_VFPU
#include <psprtc.h>
#endif

#if FL_INCLUDE_ALL_C == 0
#include "flMath.h"
#endif

#if FL_MATH_TRIGCACHE > 0
MathTrigCache* mathTrigCacheSin         = NULL;
bool           mathTrigCacheInitialized = false;
u64            mathTrigCacheCalls       = 0;
#endif

#if FL_MATH_RANDTYPE == FL_MATH_RAND_MERSENNETWISTER
SceKernelUtilsMt19937Context mathMtContext;
#endif

void mathInit() {
     pspTime tempTime;
     sceRtcGetCurrentClockLocalTime(&tempTime);
     mathRandSeed(tempTime.microseconds * tempTime.seconds);
     
     #if FL_MATH_TRIGCACHE > 0
     mathTrigCacheInit();
     #endif
}

void mathRandSeed(u32 inSeed) {
     #if FL_MATH_RANDTYPE == FL_MATH_RAND_VFPU
     __asm__ volatile (
          "mtv %0, S000\n"
          "vrnds.s S000"
          : : "r"(inSeed));
     #else
     #if FL_MATH_RANDTYPE == FL_MATH_RAND_MERSENNETWISTER
     sceKernelUtilsMt19937Init(&mathMtContext, inSeed);
     #else
     srand(inSeed);
     #endif
     #endif
}

float mathRandf(float inSize) {
     #if FL_MATH_RANDTYPE == FL_MATH_RAND_VFPU
     float tempOut;
     __asm__ volatile (
          "mtv      %1, S000\n"
          "vrndf1.s S000\n"
          "vone.s   S002\n"
          "vsub.s   S000, S000, S002\n"
          "vmul.s   S000, S000, S001\n"
          "mfv      %0, S000\n"
          : "=r"(tempOut) : "r"(inSize));
     return tempOut;
     #else
     #if FL_MATH_RANDTYPE == FL_MATH_RAND_MERSENNETWISTER
     float tempOut = sceKernelUtilsMt19937UInt(&mathMtContext);
     return (((float)tempOut * inSize) / (float)MAX_INT);
     #else
     #if FL_MATH_RANDOMSEED != 0
     int tempRand = ((rand() * FL_MATH_RANDOMSEED_FREQUENCY) / RAND_MAX);
     if(tempRand == 0) {
          pspTime tempTime;
          sceRtcGetCurrentClockLocalTime(&tempTime);
          srand(tempTime.microseconds * tempTime.seconds);
     }
     #endif
     return (((float)rand() * inSize) / (float)RAND_MAX);
     #endif
     #endif
}

float mathRandFractf() {
     #if FL_MATH_RANDTYPE == FL_MATH_RAND_VFPU
     float tempOut;
     __asm__ volatile (
          "vrndf1.s S000\n"
          "vone.s   S001\n"
          "vsub.s   S000, S000, S001\n"
          "mfv      %0, S000\n"
          : "=r"(tempOut) :);
     return tempOut;
     #else
     #if FL_MATH_RANDTYPE == FL_MATH_RAND_MERSENNETWISTER
     float tempOut = sceKernelUtilsMt19937UInt(&mathMtContext);
     return ((float)tempOut / (float)MAX_INT);
     #else
     #if FL_MATH_RANDOMSEED != 0
     int tempRand = ((rand() * FL_MATH_RANDOMSEED_FREQUENCY) / RAND_MAX);
     if(tempRand == 0) {
          pspTime tempTime;
          sceRtcGetCurrentClockLocalTime(&tempTime);
          srand(tempTime.microseconds * tempTime.seconds);
     }
     #endif
     return ((float)rand() / (float)RAND_MAX);
     #endif
     #endif
}

unsigned int mathRandi(unsigned int inSize) {
     #if FL_MATH_RANDTYPE == FL_MATH_RAND_VFPU
     unsigned int tempOut;
     __asm__ volatile (
          "vrndi.s  S000\n"
          "mfv      %0, S000\n"
          : "=r"(tempOut) :);
     tempOut = tempOut % inSize;
     return tempOut;
     #else
     #if FL_MATH_RANDTYPE == FL_MATH_RAND_MERSENNETWISTER
     unsigned int tempOut = sceKernelUtilsMt19937UInt(&mathMtContext);
     return (tempOut % inSize);
     #else
     #if FL_MATH_RANDOMSEED != 0
     int tempRand = ((rand() * FL_MATH_RANDOMSEED_FREQUENCY) / RAND_MAX);
     if(tempRand == 0) {
          pspTime tempTime;
          sceRtcGetCurrentClockLocalTime(&tempTime);
          srand(tempTime.microseconds * tempTime.seconds);
     }
     #endif
     return (rand() % inSize);
     #endif
     #endif
}

s64 mathRandl(s64 inSize) {
     long tempOut = ((s64)mathRandi((int)(inSize >> 32))) << 32;
     tempOut += (s64)mathRandi((int)(inSize & 0xFFFFFFFF));
     return tempOut;
}

inline float mathFixDegRange(float inDeg) {
     while(inDeg >= 360)
          inDeg -= 360;
     while(inDeg < 0)
          inDeg += 360;
     return inDeg;
}

inline float mathFixRadRange(float inRad) {
     while(inRad >= (MATH_PI * 2.0f))
          inRad -= (MATH_PI * 2.0f);
     while(inRad < 0)
          inRad += (MATH_PI * 2.0f);
     return inRad;
}

float mathDegToRad(float inDeg) {
     inDeg = mathFixDegRange(inDeg);
     #if FL_MATH_VFPU != 0
     float tempOut;
     __asm__ volatile (
          "mtv     %1, S000\n"
          "mtv     %2, S001\n"
          "vcst.s  S002, VFPU_PI\n"
          "vmul.s  S000, S000, S002\n"
          "vrcp.s  S001, S001\n"
          "vmul.s  S000, S000, S001\n"
          "mfv     %0, S000\n"
          : "=r"(tempOut) : "r"(inDeg), "r"(180.0f));
     return tempOut;
     #else
     return ((inDeg * MATH_PI) / 180.0f);
     #endif
}

float mathRadToDeg(float inRad) {
     inRad = mathFixRadRange(inRad);
     #if FL_MATH_VFPU != 0
     float tempOut;
     __asm__ volatile (
          "mtv     %1, S000\n"
          "mtv     %2, S001\n"
          "vmul.s  S000, S000, S001\n"
          "vcst.s  S001, VFPU_PI\n"
          "vrcp.s  S001, S001\n"
          "vmul.s  S000, S000, S001\n"
          "mfv     %0, S000\n"
          : "=r"(tempOut) : "r"(inRad), "r"(180.0f));
     return tempOut;
     #else
     return ((inRad * 180.0f) / MATH_PI);
     #endif
}

float mathSinf(float inAngle) {
     #if FL_MATH_TRIGCACHE > 0
     float* tempCacheAns = mathTrigCacheCheckSin(inAngle);
     if(tempCacheAns)
          return *tempCacheAns;
     #endif

     float tempOut;     
     #if FL_MATH_VFPU != 0
     __asm__ volatile (
          "mtv     %1, S000\n"
          "vcst.s  S001, VFPU_2_PI\n"
          "vmul.s  S000, S000, S001\n"
          "vsin.s  S000, S000\n"
          "mfv     %0, S000\n"
          : "=r"(tempOut) : "r"(inAngle));
     #else
     tempOut = sinf(inAngle);
     #endif
     
     #if FL_MATH_TRIGCACHE > 0
     mathTrigCacheAddSin(inAngle, tempOut);
     #endif
     
     return tempOut;
}

float mathCosf(float inAngle) {
     #if FL_MATH_TRIGCACHE > 0
     float* tempCacheAns = mathTrigCacheCheckCos(inAngle);
     if(tempCacheAns)
          return *tempCacheAns;
     #endif
     
     float tempOut;
     #if FL_MATH_VFPU != 0
     __asm__ volatile (
          "mtv     %1, S000\n"
          "vcst.s  S001, VFPU_2_PI\n"
          "vmul.s  S000, S000, S001\n"
          "vcos.s  S000, S000\n"
          "mfv     %0, S000\n"
          : "=r"(tempOut) : "r"(inAngle));
     #else
     tempOut = cosf(inAngle);
     #endif
     
     #if FL_MATH_TRIGCACHE > 0
     mathTrigCacheAddCos(inAngle, tempOut);
     #endif
     
     return tempOut;
}

float mathACosf(float inAngle) {
     #if FL_MATH_VFPU != 0
     float tempOut;
     __asm__ volatile (
          "mtv     %1, S000\n"
          "vcst.s  S001, VFPU_PI_2\n"
          "vasin.s S000, S000\n"
          "vocp.s  S000, S000\n"
          "vmul.s  S000, S000, S001\n"
          "mfv     %0, S000\n"
          : "=r"(tempOut) : "r"(inAngle));
     return tempOut;
     #else
     return acosf(inAngle);
     #endif
}

float mathATanf(float inAngle) {
     /*#if FL_MATH_VFPU != 0
     float tempOut;
     __asm__ volatile (
          "mtv     %1, S000\n"
          "vcst.s  S001, VFPU_PI_2\n"
          "vasin.s S000, S000\n"
          "vocp.s  S000, S000\n"
          "vmul.s  S000, S000, S001\n"
          "mfv     %0, S000\n"
          : "=r"(tempOut) : "r"(inAngle));
     return tempOut;
     #else*/
     return atanf(inAngle);
     //#endif
}


float mathExpnf(float inValue) {
     #if FL_MATH_VFPU != 0
     float tempOut;
     __asm__ volatile (
          "mtv     %1, S000\n"
          "vcst.s  S001, VFPU_LN2\n"
          "vrcp.s  S001, S001\n"
          "vmul.s  S000, S000, S001\n"
          "vexp2.s S000, S000\n"
          "mfv     %0, S000\n"
          : "=r"(tempOut) : "r"(inValue));
     return tempOut;
     #else
     return expf(inValue);
     #endif
}

float mathLnf(float inValue) {
     #if FL_MATH_VFPU != 0
     float tempOut;
     __asm__ volatile (
          "mtv     %1, S000\n"
          "vcst.s  S001, VFPU_LOG2E\n"
          "vrcp.s  S001, S001\n"
          "vlog2.s S000, S000\n"
          "vmul.s  S000, S000, S001\n"
          "mfv     %0, S000\n"
          : "=r"(tempOut) : "r"(inValue));
     return tempOut;
     #else
     return logf(inValue);
     #endif
}

/*float mathSqrtf(float inValue) {
     #if FL_MATH_VFPU != 0
     float tempOut;
     __asm__ volatile (
          "mtv     %1, S000\n"
          "vsqrt.s  S000, S000\n"
          "mfv     %0, S000\n"
          : "=r"(tempOut) : "r"(inValue));
     return tempOut;
     #else
     return sqrtf(inValue);
     #endif
}*/

float mathPythag2f(float inX, float inY) {
     #if FL_MATH_VFPU != 0
     float tempOut;
     __asm__ volatile (
          "mtv     %1, S000\n"
          "mtv     %2, S001\n"
          "vmul.s  S000, S000, S000\n"
          "vmul.s  S001, S001, S001\n"
          "vadd.s  S000, S000, S001\n"
          "mfv     %0, S000\n"
          : "=r"(tempOut) : "r"(inX), "r"(inY));
     return sqrtf(tempOut);
     #else
     return sqrtf((inX * inX) + (inY * inY));
     #endif
}

float mathPythag3f(float inX, float inY, float inZ) {
     #if FL_MATH_VFPU != 0
     float tempOut;
     __asm__ volatile (
          "mtv     %1, S000\n"
          "mtv     %2, S001\n"
          "mtv     %3, S002\n"
          "vmul.s  S000, S000, S000\n"
          "vmul.s  S001, S001, S001\n"
          "vmul.s  S002, S002, S002\n"
          "vadd.s  S000, S000, S001\n"
          "vadd.s  S000, S000, S002\n"
          "mfv     %0, S000\n"
          : "=r"(tempOut) : "r"(inX), "r"(inY), "r"(inZ));
     return sqrtf(tempOut);
     #else
     return sqrtf((inX * inX) + (inY * inY) + (inZ * inZ));
     #endif
}

float mathDotProdN(float* inValue0, float* inValue1, u8 inCount) {
     if(!inCount)
          return 0;
     float tempOut = 0.0f;
     int i;
     #if FL_MATH_VFPU != 0
     for(i = 0; i < inCount; i++) {
           __asm__ volatile (
               "mtv     %1, S000\n"
               "mtv     %2, S001\n"
               "mtv     %3, S002\n"
               "vmul.s  S000, S000, S001\n"
               "vadd.s  S000, S000, S002\n"
               "mfv     %0, S000\n"
               : "=r"(tempOut) : "r"(inValue0[i]), "r"(inValue1[i]), "r"(tempOut));
     }
     #else
     for(i = 0; i < inCount; i++)
          tempOut += (inValue0[i] * inValue1[i]);
     #endif
     return tempOut;
}

float mathQuatLength(quat4f inQuat) {
     #if FL_MATH_VFPU != 0
     float tempOut = 0.0f;
      __asm__ volatile (
          "mtv     %1, S000\n"
          "mtv     %2, S001\n"
          "mtv     %3, S002\n"
          "mtv     %4, S003\n"
          "vmul.s  S000, S000, S000\n"  
          "vmul.s  S001, S001, S001\n"
          "vmul.s  S002, S002, S002\n"  
          "vmul.s  S003, S003, S003\n"
          "vadd.s  S000, S000, S001\n"
          "vadd.s  S001, S002, S003\n"
          "vadd.s  S000, S000, S001\n"
          "mfv     %0, S000\n"
          : "=r"(tempOut) : "r"(inQuat.x), "r"(inQuat.y), "r"(inQuat.z), "r"(inQuat.w));
     return sqrtf(tempOut);
     #else
     return sqrtf((inQuat.x * inQuat.x) + (inQuat.y * inQuat.z) + (inQuat.y * inQuat.z) + (inQuat.w * inQuat.w));
     #endif
}

quat4f* mathQuatNormalize(quat4f* inQuat) {
     float tempLength = mathQuatLength(*inQuat);
     #if FL_MATH_VFPU != 0
      __asm__ volatile (
          "mtv     %4, S000\n"
          "mtv     %5, S001\n"
          "mtv     %6, S002\n"
          "mtv     %7, S003\n"
          "mtv     %8, S010\n"
          "vrcp.s  S010, S010\n"
          "vmul.s  S000, S000, S010\n"  
          "vmul.s  S001, S001, S010\n"
          "vmul.s  S002, S002, S010\n"  
          "vmul.s  S003, S003, S010\n"
          "mfv     %0, S000\n"
          "mfv     %1, S001\n"
          "mfv     %2, S002\n"
          "mfv     %3, S003\n"
          : "=r"(inQuat->x), "=r"(inQuat->y), "=r"(inQuat->z), "=r"(inQuat->w) : "r"(inQuat->x), "r"(inQuat->y), "r"(inQuat->z), "r"(inQuat->w), "r"(tempLength));
     return inQuat;
     #else
     inQuat->x /= tempLength;
     inQuat->y /= tempLength;
     inQuat->z /= tempLength;
     inQuat->w /= tempLength;
     return inQuat;
     #endif
}

quat4f mathQuatNormal(quat4f inQuat) {
     quat4f tempOut = inQuat;
     mathQuatNormalize(&tempOut);
     return tempOut;
}

quat4f* mathQuatNegate(quat4f* inQuat) {
     inQuat->x = -inQuat->x;
     inQuat->y = -inQuat->y;
     inQuat->z = -inQuat->z;
     inQuat->w = -inQuat->w;
     return inQuat;
}

quat4f mathQuatNegation(quat4f inQuat) {
     quat4f tempOut = inQuat;
     mathQuatNegate(&tempOut);
     return tempOut;
}

quat4f mathQuatMult(quat4f inQuat0, quat4f inQuat1) {
     quat4f tempOut;
     #if FL_MATH_VFPU != 0
      __asm__ volatile (
          "mtv     %4, S000\n"
          "mtv     %5, S001\n"
          "mtv     %6, S002\n"
          "mtv     %7, S003\n"
          "mtv     %8, S010\n"
          "mtv     %9, S011\n"
          "mtv     %10, S012\n"
          "mtv     %11, S013\n"
          
          "vmul.s  S020, S003, S010\n"  
          "vmul.s  S021, S000, S013\n"
          "vmul.s  S022, S001, S012\n"  
          "vmul.s  S023, S002, S011\n"
          "vadd.s  S020, S020, S021\n"
          "vadd.s  S020, S020, S010\n"
          "vsub.s  S020, S020, S023\n"
          "mfv     %0, S020\n"
          
          "vmul.s  S020, S003, S011\n"  
          "vmul.s  S021, S000, S012\n"
          "vmul.s  S010, S001, S013\n"  
          "vmul.s  S023, S002, S010\n"
          "vsub.s  S020, S020, S021\n"
          "vadd.s  S020, S020, S010\n"
          "vadd.s  S020, S020, S023\n"
          "mfv     %1, S020\n"
          
          "vmul.s  S020, S003, S012\n"  
          "vmul.s  S021, S000, S011\n"
          "vmul.s  S010, S001, S013\n"  
          "vmul.s  S023, S002, S010\n"
          "vadd.s  S020, S020, S021\n"
          "vsub.s  S020, S020, S010\n"
          "vadd.s  S020, S020, S023\n"
          "mfv     %2, S020\n"
          
          "vmul.s  S020, S003, S013\n"  
          "vmul.s  S021, S000, S010\n"
          "vmul.s  S010, S001, S011\n"  
          "vmul.s  S023, S002, S012\n"
          "vsub.s  S020, S020, S021\n"
          "vsub.s  S020, S020, S010\n"
          "vsub.s  S020, S020, S023\n"
          "mfv     %3, S020\n"
          : "=r"(tempOut.x), "=r"(tempOut.y), "=r"(tempOut.z), "=r"(tempOut.w) : "r"(inQuat0.x), "r"(inQuat0.y), "r"(inQuat0.z), "r"(inQuat0.w), "r"(inQuat1.x), "r"(inQuat1.y), "r"(inQuat1.z), "r"(inQuat1.w));
     return tempOut;
     #else
     tempOut.x = ((inQuat0.w * inQuat1.x) + (inQuat0.x * inQuat1.w) + (inQuat0.y * inQuat1.z) - (inQuat0.z * inQuat1.y));
     tempOut.y = ((inQuat0.w * inQuat1.y) - (inQuat0.x * inQuat1.z) + (inQuat0.y * inQuat1.w) + (inQuat0.z * inQuat1.x));
     tempOut.z = ((inQuat0.w * inQuat1.z) + (inQuat0.x * inQuat1.y) - (inQuat0.y * inQuat1.x) + (inQuat0.z * inQuat1.w));
     tempOut.w = ((inQuat0.w * inQuat1.w) - (inQuat0.x * inQuat1.x) - (inQuat0.y * inQuat1.y) - (inQuat0.z * inQuat1.z));
     return tempOut;
     #endif
}

vect3f mathVect3fRotate(vect3f inVect, vect3f inAxis, float inAngle) {
     quat4f tempView = { inVect.x, inVect.y, inVect.z, 0 };
     quat4f tempRot;
     
     #if FL_MATH_VFPU != 0
     __asm__ volatile (
          "mtv     %4, S000\n"
          "mtv     %5, S001\n"
          "mtv     %6, S002\n"
          "mtv     %7, S010\n"
          "mtv     %8, S011\n"
          
          "vcst.s  S012, VFPU_2_PI\n"
          "vmul.s  S012, S010, S012\n"
          "vsin.s  S012, S012\n"
          "vmul.s  S012, S012, S011\n"
          
          "vmul.s S000, S000, S012\n"
          "vmul.s S001, S001, S012\n"
          "vmul.s S002, S002, S012\n"

          "vcst.s  S012, VFPU_2_PI\n"
          "vmul.s  S012, S010, S012\n"
          "vcos.s  S012, S012\n"
          "vmul.s  S003, S012, S011\n"          

          "mfv     %0, S000\n"
          "mfv     %1, S001\n"
          "mfv     %2, S002\n"
          "mfv     %3, S003\n"
          : "=r"(tempRot.x), "=r"(tempRot.y), "=r"(tempRot.z), "=r"(tempRot.w) : "r"(inAxis.x), "r"(inAxis.y), "r"(inAxis.z), "r"(inAngle), "r"(0.5f));
     #else
     float tempSin = sinf(inAngle * 0.5f);
     tempRot.x = inAxis.x * tempSin;
     tempRot.y = inAxis.y * tempSin;
     tempRot.z = inAxis.z * tempSin;
     tempRot.w = cosf(inAngle * 0.5f);
     #endif

     quat4f tempResult = mathQuatMult(tempRot, tempView);
     mathQuatNegate(&tempRot);
     tempResult = mathQuatMult(tempResult, tempRot);

     return (vect3f){ tempResult.x, tempResult.y, tempResult.z };
}

vect3f mathVect3fCrossProd(vect3f inVect0, vect3f inVect1) {
     vect3f tempOut;
     #if FL_MATH_VFPU != 0
     __asm__ volatile (
          "mtv     %3, S000\n"
          "mtv     %4, S001\n"
          "mtv     %5, S002\n"
          
          "mtv     %6, S010\n"
          "mtv     %7, S011\n"
          "mtv     %8, S012\n"
          
          "vmul.s  S020, S001, S012\n"
          "vmul.s  S021, S002, S011\n"
          "vsub.s  S020, S020, S021\n"
          "mfv     %0, S020\n"

          "vmul.s  S020, S002, S010\n"
          "vmul.s  S021, S000, S012\n"
          "vsub.s  S020, S020, S021\n"
          "mfv     %1, S020\n"
          
          "vmul.s  S020, S000, S011\n"
          "vmul.s  S021, S001, S010\n"
          "vsub.s  S020, S020, S021\n"
          "mfv     %2, S020\n"
          : "=r"(tempOut.x), "=r"(tempOut.y), "=r"(tempOut.z) : "r"(inVect0.x), "r"(inVect0.y), "r"(inVect0.z), "r"(inVect1.x), "r"(inVect1.y), "r"(inVect1.z));
     #else
     tempOut.x = ((inVect0.y * inVect1.z) - (inVect0.z * inVect1.y));
     tempOut.y = ((inVect0.z * inVect1.x) - (inVect0.x * inVect1.z));
     tempOut.z = ((inVect0.x * inVect1.y) - (inVect0.y * inVect1.x));
     #endif
     return tempOut;
}

float mathVect3fLength(vect3f inVect) {
     #if FL_MATH_VFPU != 0
     float tempOut = 0.0f;
      __asm__ volatile (
          "mtv     %1, S000\n"
          "mtv     %2, S001\n"
          "mtv     %3, S002\n"
          "vmul.s  S000, S000, S000\n"  
          "vmul.s  S001, S001, S001\n"
          "vmul.s  S002, S002, S002\n"  
          "vadd.s  S000, S000, S001\n"
          "vadd.s  S000, S000, S002\n"
          "mfv     %0, S000\n"
          : "=r"(tempOut) : "r"(inVect.x), "r"(inVect.y), "r"(inVect.z));
     return sqrtf(tempOut);
     #else
     return sqrtf((inVect.x * inVect.x) + (inVect.y * inVect.z) + (inVect.y * inVect.z));
     #endif
}

vect3f* mathVect3fNormalize(vect3f* inVect) {
     float tempLength = mathVect3fLength(*inVect);
     #if FL_MATH_VFPU != 0
      __asm__ volatile (
          "mtv     %3, S000\n"
          "mtv     %4, S001\n"
          "mtv     %5, S002\n"
          "mtv     %6, S003\n"
          "vrcp.s  S003, S003\n"
          "vmul.s  S000, S000, S003\n"  
          "vmul.s  S001, S001, S003\n"
          "vmul.s  S002, S002, S003\n"  
          "mfv     %0, S000\n"
          "mfv     %1, S001\n"
          "mfv     %2, S002\n"
          : "=r"(inVect->x), "=r"(inVect->y), "=r"(inVect->z) : "r"(inVect->x), "r"(inVect->y), "r"(inVect->z), "r"(tempLength));
     return inVect;
     #else
     inVect->x /= tempLength;
     inVect->y /= tempLength;
     inVect->z /= tempLength;
     return inVect;
     #endif
}

vect3f mathVect3fNormal(vect3f inVect) {
     vect3f tempOut = inVect;
     mathVect3fNormalize(&tempOut);
     return tempOut;
}

#if FL_MATH_TRIGCACHE > 0
void mathTrigCacheInit() {
     if(mathTrigCacheInitialized)
          return;
     mathTrigCacheSin = (MathTrigCache*)memCalloc(sizeof(MathTrigCache), FL_MATH_TRIGCACHE);
     mathTrigCacheInitialized = true;
}

float* mathTrigCacheCheckSin(float inAngle) {
     if(!mathTrigCacheInitialized)
          return NULL;
     inAngle = mathFixRadRange(inAngle);
     
     mathTrigCacheCalls++;
     
     u32 i;
     for(i = 0; i < FL_MATH_TRIGCACHE; i++) {
          if(pos(inAngle - mathTrigCacheSin[i].mtgIn) < 0.001f) {
               mathTrigCacheSin[i].mtgLastUsed = mathTrigCacheCalls;
               return &mathTrigCacheSin[i].mtgOut;
          }
     }
     
     return NULL;
}

void mathTrigCacheAddSin(float inAngle, float inAnswer) {
     if(!mathTrigCacheInitialized)
          return;
     
     mathTrigCacheCalls++;

     u32 tempDrop = 0;
     u64 tempDropAge = 0;
     u64 tempAge;
     
     u32 i;
     for(i = 0; i < FL_MATH_TRIGCACHE; i++) {
          tempAge = (mathTrigCacheCalls - mathTrigCacheSin[i].mtgLastUsed);
          if(!mathTrigCacheSin[i].mtgLastUsed) {
               tempDrop = i;
               break;
          }
          if((tempAge > tempDropAge) || !i) {
               tempDrop = i;
               tempDropAge = tempAge;
          }
     }

     mathTrigCacheSin[tempDrop].mtgLastUsed = mathTrigCacheCalls;
     mathTrigCacheSin[tempDrop].mtgIn = mathFixRadRange(inAngle);
     mathTrigCacheSin[tempDrop].mtgOut = inAnswer;
}

float* mathTrigCacheCheckCos(float inAngle) {
     return mathTrigCacheCheckSin(inAngle + (MATH_PI / 2.0f));
}

void mathTrigCacheAddCos(float inAngle, float inAnswer) {
     return mathTrigCacheAddSin((inAngle + (MATH_PI / 2.0f)), inAnswer);
}
#endif
     
#endif
