// Funclib Collision v1.0.0
// 
// This module contains collision detection functions.
// For all you lazy programmers who can't be bothered doing
// it yourselves :P
// 
// Contributor(s): Flatmush



#ifndef FLCOLLISION_H
#define FLCOLLISION_H

#include "flGlobal.h"

#if FL_COLOR != 0

#ifdef __cplusplus
extern "C" {
#endif

extern vect2f collisionLast;

extern vect2f collision2dLineClosestPoint(Line2d inLine, vect2f inPoint);
extern bool   collision2dLineLine(Line2d inLine0, Line2d inLine1);
extern bool   collision2dCircleCircle(Circle2d inCirc0, Circle2d inCirc1);
extern bool   collision2dLineCircle(Line2d inLine, Circle2d inCirc);
extern bool   collision2dRectRect(Rect2d inRect0, Rect2d inRect1);

#ifdef __cplusplus
}
#endif

#endif
#endif
