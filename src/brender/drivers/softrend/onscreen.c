#include "onscreen.h"

#define MAKE_EQN(sign,row)                                                      \
    do {                                                                        \
        eqn.v[0] =   model_to_screen->m[0][3] sign model_to_screen->m[0][row];  \
        eqn.v[1] =   model_to_screen->m[1][3] sign model_to_screen->m[1][row];  \
        eqn.v[2] =   model_to_screen->m[2][3] sign model_to_screen->m[2][row];  \
        eqn.v[3] = -(model_to_screen->m[3][3] sign model_to_screen->m[3][row]); \
    } while (0)

#define TEST_NOT_IN                                                             \
    (                                                                           \
        BR_MAC3(                                                                \
            eqn.v[0], ((eqn.v[0] > 0.f) ? bounds->min.v[0] : bounds->max.v[0]), \
            eqn.v[1], ((eqn.v[1] > 0.f) ? bounds->min.v[1] : bounds->max.v[1]), \
            eqn.v[2], ((eqn.v[2] > 0.f) ? bounds->min.v[2] : bounds->max.v[2])  \
        ) < eqn.v[3]                                                            \
    )

#define TEST_OUT                                                                \
    (                                                                           \
        BR_MAC3(                                                                \
            eqn.v[0], ((eqn.v[0] > 0) ? bounds->max.v[0] : (bounds->min.v[0])), \
            eqn.v[1], ((eqn.v[1] > 0) ? bounds->max.v[1] : (bounds->min.v[1])), \
            eqn.v[2], ((eqn.v[2] > 0) ? bounds->max.v[2] : (bounds->min.v[2]))  \
        ) < eqn.v[3]                                                            \
    )

br_token (C2_HOOK_STDCALL * OnScreenCheck_original)(br_soft_renderer* self, br_matrix4* model_to_screen, br_bounds3* bounds);
br_token C2_HOOK_STDCALL OnScreenCheck(br_soft_renderer* self, br_matrix4* model_to_screen, br_bounds3* bounds) {

#if 0//defined(C2_HOOKS_ENABLED)
    return OnScreenCheck_original(self, model_to_screen, bounds);
#else
    int accept = 1;
    br_vector4 eqn;
    int c;

    MAKE_EQN(+, 0);

    if (TEST_OUT) {
        return BRT_REJECT;
    }

    if (TEST_NOT_IN) {
        accept = 0;
    }

    MAKE_EQN(-, 0);

    if (TEST_OUT) {
        return BRT_REJECT;
    }

    if (accept && TEST_NOT_IN) {
        accept = 0;
    }

    MAKE_EQN(+, 1);

    if (TEST_OUT) {
        return BRT_REJECT;
    }

    if (accept && TEST_NOT_IN) {
        accept = 0;
    }

    MAKE_EQN(-, 1);

    if (TEST_OUT) {
        return BRT_REJECT;
    }

    if (accept && TEST_NOT_IN) {
        accept = 0;
    }

    MAKE_EQN(+, 2);

    if (TEST_OUT) {
        return BRT_REJECT;
    }

    if (accept && TEST_NOT_IN) {
        accept = 0;
    }

    MAKE_EQN(-, 2);

    if (TEST_OUT) {
        return BRT_REJECT;
    }

    if (accept && TEST_NOT_IN) {
        accept = 0;
    }

    for (c = 0; c < MAX_STATE_CLIP_PLANES; c++) {

        if (self->state.clip[c].type != BRT_PLANE) {
            continue;
        }

        BrMatrix4TApply(&eqn, &self->state.clip[c].plane, model_to_screen);
        eqn.v[3] = -eqn.v[3];

        if (TEST_OUT) {
            return BRT_REJECT;
        }

        if (accept && TEST_NOT_IN) {
            accept = 0;
        }
    }

    return accept ? BRT_ACCEPT : BRT_PARTIAL;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00544830, OnScreenCheck, OnScreenCheck_original)
