#ifndef REC2_MATH_H
#define REC2_MATH_H

#include <math.h>

#if defined(_MSC_VER) && _MSC_VER < 1200

#include <float.h>

#define exp2f(V) ((float)exp2((double)(V)))
#define fabsf(V) ((float)fabs((double)(V)))
#define sqrtf(V) ((float)sqrt((double)(V)))
#define cosf(V) ((float)cos((double)(V)))
#define sinf(V) ((float)sin((double)(V)))
#define tanf(V) ((float)tan((double)(V)))
#define acosf(V) ((float)acos((double)(V)))
#define asinf(V) ((float)asin((double)(V)))
#define atan2f(X,Y) ((float)atan2((double)(X),(double)(Y)))
#define atanf(V) ((float)atan((double)(V)))
#define powf(X,Y) ((float)pow((double)(X),(double)(Y)))
#define fmodf(X,Y) ((float)fmod((double)(X),(double)(Y)))
#define ceilf(V) ((float)ceil(V))
#define floorf(V) ((float)floor(V))
#define isnan(V) _isnan(V)
#define ldexpf(V,E) (float)ldexp(V,E)
#define isfinite(V) !!((~*(unsigned int*)(&(V))) & 0x7f800000)

#endif

#endif // REC2_MATH_H
