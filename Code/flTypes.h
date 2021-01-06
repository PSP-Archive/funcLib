#ifndef FLTYPES_H
#define FLTYPES_H

#include <psptypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef uint8
typedef u8 uint8;
#endif

#ifndef uint16
typedef u16 uint16;
#endif

#ifndef uint32
typedef u32 uint32;
#endif

#ifndef uint64
typedef u64 uint64;
#endif

#ifndef u128
typedef struct {
     u32 val[4];
} u128;
#endif

#ifndef uint128
typedef u128 uint128;
#endif

#ifndef u256
typedef struct {
     u32 val[8];
} u256;
#endif

#ifndef uint256
typedef u256 uint256;
#endif

#ifndef int8
typedef s8 int8;
#endif

#ifndef byte
typedef unsigned char byte;
#endif

#ifndef word
typedef unsigned short word;
#endif

#ifndef dword
typedef unsigned long dword;
#endif

#ifndef bool
//typedef char bool; ///< This could be an int, but I prefer to use a char as it's smaller
#define bool char
#endif

#ifndef boolean
typedef bool boolean;
#endif

#ifndef false
#define false 0
#endif

#ifndef true
#define true !false
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE !FALSE
#endif

#ifndef False
#define False 0
#endif

#ifndef True
#define True !False
#endif

#ifndef null
#define null ((void *)0)
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef INVALID
#define INVALID ((dword)-1)
#endif

// Naming convention for vertex structures
//
// <Textured><Color><VertexType>
// Tf = Textured, U/V floats.
// Ts = Textured, U/V short16.
// C = 32-bit color.
// c = 16-bit color.
// Vs = Short16 verteces.
// Vf = Float32 verteces.

typedef struct {
	short vertX, vertY, vertZ;
} vertVs;

typedef struct {
     float vertX, vertY, vertZ;
} vertVf;

typedef struct {
	u32 vertColor;
	short vertX, vertY, vertZ;
} vertCVs;

typedef struct {
     u32 vertColor;
     float vertX, vertY, vertZ;
} vertCVf;

typedef struct {
     u16 vertColor;
     float vertX, vertY, vertZ;
} vertcVf;

typedef struct {
     float vertU, vertV;
     u32   vertColor;
     short vertX, vertY, vertZ;
} vertTfCVs;

typedef struct {
     float vertU, vertV;
     u32   vertColor;
     float vertX, vertY, vertZ;
} vertTfCVf;

typedef struct {
     float vertU, vertV;
     float vertX, vertY, vertZ;
} vertTfVf;

typedef struct {
     float vertU, vertV;
     short vertX, vertY, vertZ;
} vertTfVs;

typedef struct {
     short vertU, vertV;
     float vertX, vertY, vertZ;
} vertTsVf;

typedef struct {
     short vertU, vertV;
     short vertX, vertY, vertZ;
} vertTsVs;

typedef ScePspFVector2 vect2f;
typedef ScePspFVector3 vect3f;
typedef ScePspFVector4 vect4f;

typedef vect4f quat4f;

typedef struct {
     vect2f linePoints[2];
} Line2d;

typedef struct {
     vect2f triPoints[3];
} Triangle2d;

typedef struct {
     vect2f circCenter;
     float  circRadius;
} Circle2d;

typedef struct {
     vect2f ellipseCenter;
     vect2f ellipseSize;
} Ellipse2d;

typedef struct {
     vect2f quadPoints[4];
} Quad2d;

typedef struct {
     vect2f rectPoints;
     vect2f rectSize;
} Rect2d;

#define vect2f_Cmp(inVect0, inVect1) (((inVect0).x == (inVect1).x) && ((inVect0).y == (inVect1).y))
#define vect2f_Add(inVect0, inVect1) ((vect2f){ ((inVect0).x + (inVect1).x), ((inVect0).y + (inVect1).y) })
#define vect2f_Sub(inVect0, inVect1) ((vect2f){ ((inVect0).x - (inVect1).x), ((inVect0).y - (inVect1).y) })

#define vect3f_Cmp(inVect0, inVect1) (((inVect0).x == (inVect1).x) && ((inVect0).y == (inVect1).y) && ((inVect0).z == (inVect1).z))
#define vect3f_Add(inVect0, inVect1) ((vect3f){ ((inVect0).x + (inVect1).x), ((inVect0).y + (inVect1).y), ((inVect0).z + (inVect1).z) })
#define vect3f_Sub(inVect0, inVect1) ((vect3f){ ((inVect0).x - (inVect1).x), ((inVect0).y - (inVect1).y), ((inVect0).z - (inVect1).z) })
#define vect3f_Mulf(inVect, inFloat) ((vect3f){ ((inVect).x * (inFloat)), ((inVect).y * (inFloat)), ((inVect).y * (inFloat)) })

#ifdef __cplusplus
}
#endif

#endif
