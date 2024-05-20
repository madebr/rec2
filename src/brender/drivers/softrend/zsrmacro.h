#ifndef SOFTREND_ZSRMACRO_H
#define SOFTREND_ZSRMACRO_H

#include "types.h"

enum {
    BR_SORT_AVERAGE             = 0x0001,
    BR_SORT_MIN                 = 0x0002,
    BR_SORT_MAX                 = 0x0003,
    BR_SORT_FIRST_VERTEX        = 0x0004
};

#define VIEW_Z(v) ((v)->comp[C_W])

#define SORT_VALUE_TRIANGLE(type, v0, v1, v2) do {          \
        zprim = VIEW_Z(v0);                                 \
        switch (type) {                                     \
        case BR_SORT_AVERAGE:                               \
            zprim += VIEW_Z(v1) + VIEW_Z(v2);               \
            zprim *= 1.f / 3.f;                             \
            break;                                          \
        case BR_SORT_MIN:                                   \
            if (VIEW_Z(v1) < zprim) { zprim = VIEW_Z(v1); } \
            if (VIEW_Z(v2) < zprim) { zprim = VIEW_Z(v2); } \
            break;                                          \
        case BR_SORT_MAX:                                   \
            if (VIEW_Z(v1) > zprim) { zprim = VIEW_Z(v1); } \
            if (VIEW_Z(v2) > zprim) { zprim = VIEW_Z(v2); } \
            break;                                          \
        case BR_SORT_FIRST_VERTEX:                          \
            break;                                          \
        }                                                   \
    } while(0)

#define INSERT_PRIMITIVE_FIRST_BUCKET(ot, prim, depth) do { \
        (prim)->next=(ot)->table[0];                        \
        (ot)->table[0]=(prim);                              \
    } while(0)

#define INSERT_PRIMITIVE_HEAD(ot, prim, depth) do {         \
        int bucket;                                         \
        (depth)=(depth)-(ot)->min_z;                        \
        if ((depth) < 0.f) {                                \
            bucket = 0;                                     \
        } else {                                            \
            bucket = (int)((ot)->scale * (depth));          \
            if (bucket >= (ot)->size) {                     \
                bucket = (ot)->size - 1;                    \
            }                                               \
        }                                                   \
        (prim)->next = (ot)->table[bucket];                 \
        (ot)->table[bucket] = (prim);                       \
    } while(0)


#define INSERT_PRIMITIVE(ot, prim, depth) do {              \
        if ((ot)->flags & 0x4) {                            \
            INSERT_PRIMITIVE_FIRST_BUCKET(ot,prim,depth);   \
        } else {                                            \
            INSERT_PRIMITIVE_HEAD(ot,prim,depth);           \
        }                                                   \
    } while(0)

#define SORT_VALUE_EDGE(type,z0,z1) do {                    \
        zprim = VIEW_Z(z0);                                 \
        switch (type) {                                     \
        case BR_SORT_AVERAGE:                               \
            zprim += VIEW_Z(z1);                            \
            zprim *= 1.f / 2.f;                             \
            break;                                          \
        case BR_SORT_MIN:                                   \
            if (VIEW_Z(z1) < zprim) { zprim = VIEW_Z(z1); } \
            break;                                          \
        case BR_SORT_MAX:                                   \
            if (VIEW_Z(z1) > zprim) { zprim = VIEW_Z(z1); } \
            break;                                          \
        case BR_SORT_FIRST_VERTEX:                          \
            break;                                          \
        }                                                   \
    } while (0)

#endif /* SOFTREND_ZSRMACRO_H */
