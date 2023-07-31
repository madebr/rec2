#ifndef REC2_REC2_MACROS_H
#define REC2_REC2_MACROS_H

#define REC2_THISCALL_UNUSED(VAR) do { (void)VAR; } while (0)
#define REC2_THISCALL_EDX , 0

#define REC2_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

#define REC2_ASIZE(ARR)  ((sizeof(ARR))/sizeof(*(ARR)))

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define DEG_TO_RAD(degrees) ((degrees)*3.141592653589793 / 180.0)

#define REC2_RGB555_B(X) ((X) & 0x1f)
#define REC2_RGB888_R(X) (((X) >> 16) & 0xff)
#define REC2_RGB888_G(X) (((X) >>  8) & 0xff)
#define REC2_RGB888_B(X) (((X) >>  0) & 0xff)
#define REC2_CLAMP(X, LOWER, UPPER) (((X) < (LOWER)) ? (LOWER) : (((X) > (UPPER)) ? (UPPER) : (X)))

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define GROOVE_FUNK_MAX_PER_CAR 36

#endif // REC2_REC2_MACROS_H
