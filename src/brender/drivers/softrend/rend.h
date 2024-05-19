#ifndef SOFTREND_REND_H
#define SOFTREND_REND_H

#include "types.h"

#include "core/v1db/custsupt.h"

#include <brender/brender.h>

#define TRANSFORM_VERTEX(dest, src, mat) do {   \
        (dest)->v[0] = BR_MAC3(                 \
            (src)->v[0], (mat)->m[0][0],        \
            (src)->v[1], (mat)->m[1][0],        \
            (src)->v[2], (mat)->m[2][0]) +      \
                (mat)->m[3][0];                 \
                                                \
        (dest)->v[1] = BR_MAC3(                 \
            (src)->v[0], (mat)->m[0][1],        \
            (src)->v[1], (mat)->m[1][1],        \
            (src)->v[2], (mat)->m[2][1]) +      \
                (mat)->m[3][1];                 \
                                                \
        (dest)->v[2] = BR_MAC3(                 \
            (src)->v[0], (mat)->m[0][2],        \
            (src)->v[1], (mat)->m[1][2],        \
            (src)->v[2], (mat)->m[2][2]) +      \
                (mat)->m[3][2];                 \
                                                \
        (dest)->v[3] = BR_MAC3(                 \
            (src)->v[0], (mat)->m[0][3],        \
            (src)->v[1], (mat)->m[1][3],        \
            (src)->v[2], (mat)->m[2][3]) +      \
                (mat)->m[3][3];                 \
    } while (0)


#define OUTCODE_POINT(outcode, screen) do {                                         \
        if ((screen)->v[0] >= (screen)->v[3]) {                                     \
            (outcode) ^= (OUTCODE_RIGHT | OUTCODE_N_RIGHT);                         \
        }                                                                           \
        if ((screen)->v[0] < -(screen)->v[3]) {                                     \
            (outcode) ^= (OUTCODE_LEFT | OUTCODE_N_LEFT);                           \
        }                                                                           \
        if ((screen)->v[1] >=  (screen)->v[3]) {                                    \
            (outcode) ^= (OUTCODE_TOP | OUTCODE_N_TOP);                             \
        }                                                                           \
        if ((screen)->v[1] < -(screen)->v[3]) {                                     \
            (outcode) ^= (OUTCODE_BOTTOM | OUTCODE_N_BOTTOM);                       \
        }                                                                           \
        if ((screen)->v[2] >= (screen)->v[3]) {                                     \
            (outcode) ^= (OUTCODE_HITHER | OUTCODE_N_HITHER);                       \
        }                                                                           \
        if ((screen)->v[2] < -(screen)->v[3]) {                                     \
            (outcode) ^= (OUTCODE_YON | OUTCODE_N_YON);                             \
        }                                                                           \
        if (C2V(scache).user_clip_active) {                                         \
            int c;                                                                  \
            for (c = 0; c < MAX_STATE_CLIP_PLANES; c++) {                           \
                if (renderer->state.clip[c].type == BRT_PLANE &&                    \
                    BrVector4Dot((screen),&renderer->state.clip[c].plane) < .0f) {  \
                    (outcode) ^= (OUTCODE_USER | OUTCODE_N_USER) << c;              \
                }                                                                   \
            }                                                                       \
        }                                                                                                                                                                                                               \
    } while (0)

#define PROJECT_VERTEX(tvp, sx, sy, sz, sw) do {                                       \
        br_scalar q = 1.f / (sw);                                                      \
        (tvp)->comp[C_SX] = renderer->state.cache.comp_offsets[C_SX] +                 \
            renderer->state.cache.comp_scales[C_SX] * (sx) * q;                        \
        (tvp)->comp[C_SY] = renderer->state.cache.comp_offsets[C_SY] +                 \
            renderer->state.cache.comp_scales[C_SY] * (sy) * q;                        \
        (tvp)->comp[C_SZ] = renderer->state.cache.comp_offsets[C_SZ] +                 \
            renderer->state.cache.comp_scales[C_SZ] * (sz) * q + C2V(gScreenZOffset);  \
    } while (0)

#define UPDATE_BOUNDS(tv) do {                                                                      \
        if ((tv)->comp[C_SX] > C2V(scache).max.v[0]) { C2V(scache).max.v[0] = (tv)->comp[C_SX]; }   \
        if ((tv)->comp[C_SX] < C2V(scache).min.v[0]) { C2V(scache).min.v[0] = (tv)->comp[C_SX]; }   \
        if ((tv)->comp[C_SY] > C2V(scache).max.v[1]) { C2V(scache).max.v[1] = (tv)->comp[C_SY]; }   \
        if ((tv)->comp[C_SY] < C2V(scache).min.v[1]) { C2V(scache).min.v[1] = (tv)->comp[C_SY]; }   \
    } while (0)

#endif /* SOFTREND_REND_H */
