// Funclib Math v1.0.0 (CORE)
// 
// This module contains maths functions.
// It will in the future also act as a wrapper for most math.h functions.
// This module requires doesn't require anything else.
// 
// Contributor(s): Flatmush



#ifndef FLMATH_H
#define FLMATH_H

#include "flGlobal.h"

#if FL_MATH

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>

#if FL_MATH_TRIGCACHE > 0
/**
  * This structure is used when FL_MATH_TRIGCACHE is more than 0,
  * it is used to store cached sin parameters and their value.
  * @see FL_MATH_TRIGCACHE
  */
typedef struct {
     /**
       * The last tick in which this value was used.
       */
     u64   mtgLastUsed;
     /**
       * The input value.
       */
     float mtgIn;
     /**
       * The cached answer.
       */
     float mtgOut;
} MathTrigCache;

/**
  * This is an array of sin values and their answers,
  * since sin is just 90 degrees out of phase with cos,
  * the cos functions also use this array.
  * @see FL_MATH_TRIGCACHE, mathSinf(), mathCosf()
  */
MathTrigCache* mathTrigCacheSin;

/**
  * This stores the state of the trig cache, where non-zero means it is enabled.
  * @see FL_MATH_TRIGCACHE
  */
bool           mathTrigCacheInitialized;
#endif



/**
  * This is called by flInitialize(), it initializes the maths system,
  * by seeding the random number generator and if trigcaching is used then it creates the cache.
  * @see flInitialize()
  */
extern void         mathInit();



/**
  * This function reseeds the random number generator.
  * @param inSeed The seed to use for random number generation.
  * @see mathRandf(), mathRandFractf(), mathRandi(), mathRandl()
  */
extern void         mathRandSeed(u32 inSeed);

/**
  * This function returns a pseudo random number between 0 and inSize.
  * @param inSize The range of the random number returned.
  * @return Pseudo-random number between 0 and inSize.
  */
extern float        mathRandf(float inSize);
/**
  * This function returns a pseudo random number between 0 and 1.
  * @return Pseudo-random number between 0 and 1.
  */
extern float        mathRandFractf();

/**
  * This function returns a pseudo random integer between 0 and inSize - 1,
  * for example mathRandi(10) would return a number from 0 to 9.
  * @param inSize One more than the maximum integer that will be returned.
  * @return Pseudo-random integer between 0 and inSize - 1.
  */
extern unsigned int mathRandi(unsigned int inSize);

/**
  * This function returns a pseudo random integer between 0 and inSize - 1,
  * this function is the same as mathRandi() however it uses double accuracy signed integers.
  * @param inSize One more than the maximum integer that will be returned.
  * @return Pseudo-random integer between 0 and inSize - 1.
  * @see mathRandi()
  */
extern s64          mathRandl(s64 inSize);



/** 
  * Fixes numbers to be within the standard range for degrees which is 0 to 360.
  * @param inDeg The value whose range needs to be fixed.
  * @return The fixed range version of inDeg (e.g if inDeg = 370, 10 is returned).
  */
extern float        mathFixDegRange(float inDeg);

/** 
  * Fixes numbers to be within the standard range for radians which is 0 to 2pi.
  * @param inRad The value whose range needs to be fixed.
  * @return The fixed range version of inRad (e.g if inDeg = 3pi, 1pi is returned).
  */
extern float        mathFixRadRange(float inRad);

/**
  * Converts a degree value to radians (e.g 180 would return pi).
  * @param inDeg The degree value that is to be converted.
  * @return The equivalent angle in radians.
  */
extern float        mathDegToRad(float inDeg);

/**
  * Converts a radian value to degrees (e.g pi/2 would return 90).
  * @param inRad The radian value that is to be converted.
  * @return The equivalent angle in degrees.
  */
extern float        mathRadToDeg(float inRad);



/**
  * This is basically a wrapper for sinf from libmath,
  * however if FL_MATH_VFPU is enabled then it will use the VFPU ro speed up the process.
  * @param inAngle The angle whose sine will be calculated.
  * @return The sine of inAngle.
  * @see FL_MATH_VFPU
  */
extern float        mathSinf(float inAngle);

/**
  * This is basically a wrapper for cosf from libmath,
  * however if FL_MATH_VFPU is enabled then it will use the VFPU ro speed up the process.
  * @param inAngle The angle whose cosine will be calculated.
  * @return The cosine of inAngle.
  * @see FL_MATH_VFPU
  */
extern float        mathCosf(float inAngle);

/**
  * This is basically a wrapper for acosf from libmath,
  * however if FL_MATH_VFPU is enabled then it will use the VFPU ro speed up the process.
  * @param inAngle The angle whose arc-cosine will be calculated.
  * @return The arc-cosine of inAngle.
  * @see FL_MATH_VFPU
  */
extern float        mathACosf(float inAngle);

/**
  * This is basically a wrapper for atanf from libmath,
  * however if FL_MATH_VFPU is enabled then it will use the VFPU ro speed up the process.
  * @param inAngle The angle whose arc-tangent will be calculated.
  * @return The arc-tangent of inAngle.
  * @see FL_MATH_VFPU
  * @warning A VFPU version of this function has not yet been written.
  */
extern float        mathATanf(float inAngle);



/**
  * This is basically a wrapper for expf from libmath,
  * it returns the natural exponent of a number.
  * @param inValue The value whose natural exponent will be calculated.
  * @return The natural exponent of inValue.
  */
extern float        mathExpnf(float inValue);

/**
  * This is basically a wrapper for logf from libmath,
  * it returns the natural logarithm of a number.
  * @param inValue The value whose natural logarithm will be calculated.
  * @return The natural logarithm of inValue.
  */
extern float        mathLnf(float inValue);



/**
  * This returns the the square root of inX^2 + inY^2.
  * @param inX The length of side A of a right angle triangle.
  * @param inY The length of side B of a right angle triangle.
  * @return The length of side C of a right angle triangle.
  * @see mathPythag2f
  */
extern float        mathPythag2f(float inX, float inY);

/**
  * This returns the the square root of inX^2 + inY^2 + inZ^2.
  * @param inX Size 0.
  * @param inY Side 1.
  * @param inZ Side 2
  * @return Side 3.
  * @see mathPythag3f
  */
extern float        mathPythag3f(float inX, float inY, float inZ);



/**
  * This returns the dot product of inCount sets of values.
  * @param inValue0 An array of floats of length inCount to be dotted with inValue1.
  * @param inValue1 An array of floats of length inCount to be dotted with inValue0.
  * @param inCount The number of elements in the arrays passed.
  * @return The dot product of inValue0 and inValue1.
  */
extern float        mathDotProdN(float* inValue0, float* inValue1, u8 inCount);



extern float        mathQuatLength(quat4f inQuat);
extern quat4f*      mathQuatNormalize(quat4f* inQuat);
extern quat4f       mathQuatNormal(quat4f inQuat);
extern quat4f*      mathQuatNegate(quat4f* inQuat);
extern quat4f       mathQuatNegation(quat4f inQuat);
extern quat4f       mathQuatMult(quat4f inQuat0, quat4f inQuat1);



/**
  * This function is meant to rotate a direction vector around an axix by inAngle.
  * @param inVect The vector to be rotated.
  * @param inAxis The axis to rotate around.
  * @param inAngle The angle to rotate by.
  * @return The rotated vector.
  * @warning This function doesn't seem to work.
  */
extern vect3f       mathVect3fRotate(vect3f inVect, vect3f inAxis, float inAngle);
extern vect3f       mathVect3fCrossProd(vect3f inVect0, vect3f inVect1);
extern float        mathVect3fLength(vect3f inVect);
extern vect3f*      mathVect3fNormalize(vect3f* inVect);
extern vect3f       mathVect3fNormal(vect3f inVect);
#define mathVect3fRotateX(inView, inAngle) mathVect3fRotate(inView, { 1.0f, 0.0f, 0.0f }, inAngle)
#define mathVect3fRotateY(inView, inAngle) mathVect3fRotate(inView, { 0.0f, 1.0f, 0.0f }, inAngle)
#define mathVect3fRotateZ(inView, inAngle) mathVect3fRotate(inView, { 0.0f, 0.0f, 1.0f }, inAngle)

#if FL_MATH_TRIGCACHE > 0
extern void   mathTrigCacheInit();
extern float* mathTrigCacheCheckSin(float inAngle);
extern void   mathTrigCacheAddSin(float inAngle, float inAnswer);
extern float* mathTrigCacheCheckCos(float inAngle);
extern void   mathTrigCacheAddCos(float inAngle, float inAnswer);
#endif

#ifdef __cplusplus
}
#endif

#else
#include <math.h>

#define mathDegToRad(inDeg) (((inDeg) * MATH_PI) * 180.0f)
#define mathRadToDeg(inRad) (((inRad) * 180.0f) / MATH_PI)

#define mathSinf(inAngle) sinf(inAngle)
#define mathCosf(inAngle) cosf(inAngle)
#define mathACosf(inAngle) acosf(inAngle)
#define mathATanf(inAngle) atanf(inAngle)

#define mathExpf(inValue) expf(inValue)
#define mathLnf(inValue) logf(inValue)

#define mathPythag2f(inX, inY) sqrtf(((inX) * (inX)) + ((inY) * (inY)))
#define mathPythag3f(inX, inY, inZ) sqrtf(((inX) * (inX)) + ((inY) * (inY)) + ((inZ) * (inZ)))

#endif


#define mathSign(inValue) ((inValue) < 0 ? -1 : 1)
#define mathSignf(inValue) ((inValue) < 0.0f ? -1.0f : 1.0f)
#define mathSigni(inValue) mathSign(inValue)
#define mathAbsf(inValue) fabsf(inValue)

#define mathLogef(inValue) mathLnf(inValue)

#define mathSqrtf(inValue) sqrtf(inValue)

#define mathDegreesToRadians(inDeg) mathDegToRad(inDeg)
#define mathRadiansToDegrees(inRad) mathRadToDeg(inRad)

#endif
