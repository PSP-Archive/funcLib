#include "flGlobal.h"
#if FL_COLLISION != 0

#if FL_INCLUDE_ALL_C == 0
#include "flCollision.h"
#endif

vect2f collisionLast;

vect2f collision2dLineClosestPoint(Line2d inLine, vect2f inPoint) {
    vect2f tempDir;
    tempDir.x = (inLine.linePoints[1].x - inLine.linePoints[0].x);
    tempDir.y = (inLine.linePoints[1].y - inLine.linePoints[0].y);
    
    vect2f tempDot;
    tempDot.x = (inPoint.x - inLine.linePoints[0].x);
    tempDot.y = (inPoint.y - inLine.linePoints[0].y);
    
    float tempNumer = ((tempDot.x * tempDir.x) + (tempDot.y * tempDir.y));
    if (tempNumer <= 0.0f)
        return inLine.linePoints[0];
    float tempDenom = ((tempDir.x * tempDir.x) + (tempDir.y * tempDir.y));
    if (tempNumer >= tempDenom)
        return inLine.linePoints[1];
    tempDot.x = (inLine.linePoints[0].x + ((tempNumer / tempDenom) * tempDir.x));
    tempDot.y = (inLine.linePoints[0].y + ((tempNumer / tempDenom) * tempDir.y));
    return tempDot;
}

bool collision2dLineLine(Line2d inLine0, Line2d inLine1) {
     vect2f tempCollision;
     
     float tempM[2];
     float tempC[2];
     
     if(inLine0.linePoints[0].x == inLine0.linePoints[1].x) {
          tempCollision.x = inLine0.linePoints[0].x;
          if(inLine1.linePoints[0].x == inLine1.linePoints[1].x) {
               if(inLine1.linePoints[0].x == inLine0.linePoints[0].x) {
                    if(min(inLine1.linePoints[0].y, inLine1.linePoints[0].y) <= max(inLine0.linePoints[1].y, inLine0.linePoints[1].y)) {
                         if(max(inLine1.linePoints[0].y, inLine1.linePoints[0].y) >= min(inLine0.linePoints[1].y, inLine0.linePoints[1].y)) {
                              tempM[0] = min(inLine0.linePoints[0].y, inLine0.linePoints[1].y);
                              tempM[1] = min(inLine1.linePoints[0].y, inLine1.linePoints[1].y);
                              tempM[0] = max(tempM[0], tempM[1]);
                              tempC[0] = max(inLine0.linePoints[0].y, inLine0.linePoints[1].y);
                              tempC[1] = max(inLine1.linePoints[0].y, inLine1.linePoints[1].y);
                              tempC[0] = min(tempC[0], tempC[1]);
                              tempCollision.y = ((tempM[0] + tempC[0]) * 0.5f);
                              collisionLast = tempCollision;
                              return true;
                         }
                    }
               }
               return false;
          }
          tempM[1] = ((inLine1.linePoints[0].y - inLine1.linePoints[1].y) / (inLine1.linePoints[0].x - inLine1.linePoints[1].x));          
          tempC[1] = (inLine1.linePoints[0].y - (tempM[1] * inLine1.linePoints[0].x));
          tempCollision.y = ((tempM[1] * tempCollision.x) + tempC[1]);
          if(tempCollision.y < min(inLine0.linePoints[0].y, inLine0.linePoints[1].y))
               return false;
          if(tempCollision.y < min(inLine1.linePoints[0].y, inLine1.linePoints[1].y))
               return false;
          if(tempCollision.y > max(inLine0.linePoints[0].y, inLine0.linePoints[1].y))
               return false;
          if(tempCollision.y > max(inLine1.linePoints[0].y, inLine1.linePoints[1].y))
               return false;
          if(tempCollision.x < min(inLine0.linePoints[0].x, inLine0.linePoints[1].x))
               return false;
          if(tempCollision.x < min(inLine1.linePoints[0].x, inLine1.linePoints[1].x))
               return false;
          if(tempCollision.x > max(inLine0.linePoints[0].x, inLine0.linePoints[1].x))
               return false;
          if(tempCollision.x > max(inLine1.linePoints[0].x, inLine1.linePoints[1].x))
               return false;
          collisionLast = tempCollision;
          return true;
     } else if(inLine1.linePoints[0].x == inLine1.linePoints[1].x) {
          tempCollision.x = inLine1.linePoints[0].x;
          tempM[0] = ((inLine0.linePoints[0].y - inLine0.linePoints[1].y) / (inLine0.linePoints[0].x - inLine0.linePoints[1].x));          
          tempC[0] = (inLine0.linePoints[0].y - (tempM[0] * inLine0.linePoints[0].x));
          tempCollision.y = ((tempM[0] * tempCollision.x) + tempC[0]);
          if(tempCollision.y < min(inLine0.linePoints[0].y, inLine0.linePoints[1].y))
               return false;
          if(tempCollision.y < min(inLine1.linePoints[0].y, inLine1.linePoints[1].y))
               return false;
          if(tempCollision.y > max(inLine0.linePoints[0].y, inLine0.linePoints[1].y))
               return false;
          if(tempCollision.y > max(inLine1.linePoints[0].y, inLine1.linePoints[1].y))
               return false;
          if(tempCollision.x < min(inLine0.linePoints[0].x, inLine0.linePoints[1].x))
               return false;
          if(tempCollision.x < min(inLine1.linePoints[0].x, inLine1.linePoints[1].x))
               return false;
          if(tempCollision.x > max(inLine0.linePoints[0].x, inLine0.linePoints[1].x))
               return false;
          if(tempCollision.x > max(inLine1.linePoints[0].x, inLine1.linePoints[1].x))
               return false;
          collisionLast = tempCollision;
          return true;
     }
     
     tempM[0] = ((inLine0.linePoints[0].y - inLine0.linePoints[1].y) / (inLine0.linePoints[0].x - inLine0.linePoints[1].x));
     tempM[1] = ((inLine1.linePoints[0].y - inLine1.linePoints[1].y) / (inLine1.linePoints[0].x - inLine1.linePoints[1].x));
     
     tempC[0] = (inLine0.linePoints[0].y - (tempM[0] * inLine0.linePoints[0].x));
     tempC[1] = (inLine1.linePoints[0].y - (tempM[1] * inLine1.linePoints[0].x));
     
     if(tempM[0] == tempM[1]) {
          if(tempC[0] == tempC[1]) {
               // TODO - Stop being lazy and calculate hit point properly.
               tempCollision.x = ((inLine0.linePoints[0].x + inLine0.linePoints[1].x + inLine1.linePoints[0].x + inLine1.linePoints[1].x) * 0.25f);
               tempCollision.y = ((inLine0.linePoints[0].y + inLine0.linePoints[1].y + inLine1.linePoints[0].y + inLine1.linePoints[1].y) * 0.25f);
               collisionLast = tempCollision;
               return true;
          }
          return false;
     }
     
     tempCollision.x = ((tempC[1] - tempC[0]) / (tempM[0] - tempM[1]));
     tempCollision.y = ((tempM[0] * tempCollision.x) + tempC[0]);
     
     if(tempCollision.y < min(inLine0.linePoints[0].y, inLine0.linePoints[1].y))
          return false;
     if(tempCollision.y < min(inLine1.linePoints[0].y, inLine1.linePoints[1].y))
          return false;
     if(tempCollision.y > max(inLine0.linePoints[0].y, inLine0.linePoints[1].y))
          return false;
     if(tempCollision.y > max(inLine1.linePoints[0].y, inLine1.linePoints[1].y))
          return false;
     if(tempCollision.x < min(inLine0.linePoints[0].x, inLine0.linePoints[1].x))
          return false;
     if(tempCollision.x < min(inLine1.linePoints[0].x, inLine1.linePoints[1].x))
          return false;
     if(tempCollision.x > max(inLine0.linePoints[0].x, inLine0.linePoints[1].x))
          return false;
     if(tempCollision.x > max(inLine1.linePoints[0].x, inLine1.linePoints[1].x))
          return false;
     collisionLast = tempCollision;
     return true;
}

bool collision2dCircleCircle(Circle2d inCirc0, Circle2d inCirc1) {
     vect2f tempCollision;
     
     tempCollision.x = (max(inCirc0.circCenter.x, inCirc1.circCenter.x) - min(inCirc0.circCenter.x, inCirc1.circCenter.x));
     tempCollision.y = (max(inCirc0.circCenter.y, inCirc1.circCenter.y) - min(inCirc0.circCenter.y, inCirc1.circCenter.y));
     float tempDist = sqrt((tempCollision.x * tempCollision.x) + (tempCollision.y * tempCollision.y));
     if(tempDist < min(inCirc0.circRadius, inCirc1.circRadius)) {
          tempCollision.x *= 0.5f;
          tempCollision.y *= 0.5f;
          tempCollision.x += min(inCirc0.circCenter.x, inCirc1.circCenter.x);
          tempCollision.y += min(inCirc0.circCenter.y, inCirc1.circCenter.y);
          collisionLast = tempCollision;
          return true;
     }
     
     return false;
}

bool collision2dLineCircle(Line2d inLine, Circle2d inCirc) {
     vect2f tempCollision;
     vect2f tempPoint = collision2dLineClosestPoint(inLine, inCirc.circCenter);
     
     tempCollision.x = (max(tempPoint.x, inCirc.circCenter.x) - min(tempPoint.x, inCirc.circCenter.x));
     tempCollision.y = (max(tempPoint.y, inCirc.circCenter.y) - min(tempPoint.y, inCirc.circCenter.y));
     float tempDist = sqrt((tempCollision.x * tempCollision.x) + (tempCollision.y * tempCollision.y));
     if(tempDist < inCirc.circRadius) {
          collisionLast = tempPoint;
          return true;
     }
     
     return false;
}

bool collision2dRectRect(Rect2d inRect0, Rect2d inRect1) {
     if(inRect1.rectPoints.x > (inRect0.rectPoints.x + inRect0.rectSize.x))
          return false;
     if((inRect1.rectPoints.x + inRect1.rectSize.x) < inRect0.rectPoints.x)
          return false;
     if(inRect1.rectPoints.y > (inRect0.rectPoints.y + inRect0.rectSize.y))
          return false;
     if((inRect1.rectPoints.y + inRect1.rectSize.y) < inRect0.rectPoints.y)
          return false;

     collisionLast.x = inRect1.rectPoints.x + (((inRect0.rectPoints.x + inRect0.rectSize.x) - inRect1.rectPoints.x) / 2.0f);
     collisionLast.y = inRect1.rectPoints.y + (((inRect0.rectPoints.y + inRect0.rectSize.y) - inRect1.rectPoints.y) / 2.0f);     
     
     return true;
}

#endif
