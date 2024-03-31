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

#define V11MODEL(model) (((v11model*)model->prepared))

#define WORLD_SCALE 6.9f

#define GROOVE_FUNK_MAX_PER_CAR 36
#define REC2_PI 3.141592653589793

#define Vector2Equals(V1, V2) \
    ((V1)->v[0] == (V2)->v[0] && (V1)->v[1] == (V2)->v[1])
#define Vector3Equals(V1, V2) \
    ((V1)->v[0] == (V2)->v[0] && (V1)->v[1] == (V2)->v[1] && (V1)->v[2] == (V2)->v[2])
#define Vector3EqualElements(V, A, B, C) \
    ((V)->v[0] == (A) && (V)->v[1] == (B) && (V)->v[2] == (C))
#define Vector3IsZero(V) Vector3EqualElements((V), 0.f, 0.f, 0.f)
#define Vector3Average(V, V1, V2) do {                \
        (V)->v[0] = 0.5f * ((V1)->v[0] + (V2)->v[0]); \
        (V)->v[1] = 0.5f * ((V1)->v[1] + (V2)->v[1]); \
        (V)->v[2] = 0.5f * ((V1)->v[2] + (V2)->v[2]); \
    } while (0)

#endif // REC2_REC2_MACROS_H
