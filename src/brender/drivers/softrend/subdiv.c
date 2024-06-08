#include "subdiv.h"

#include "setup.h"

void C2_HOOK_CDECL averageVerticesOnScreen(br_soft_renderer* renderer, brp_vertex* dest1, brp_vertex* dest2, brp_vertex* dest3, brp_vertex* src1, brp_vertex* src2, brp_vertex* src3) {

#define COMPUTE_COMPONENT_MID_POINT_VALUES(COMP) \
    do {                                         \
        br_scalar c1 = src1->comp[COMP];         \
        br_scalar c2 = src2->comp[COMP];         \
        br_scalar c3 = src3->comp[COMP];         \
        dest3->comp[COMP] = .5f * (c1 + c2);     \
        dest1->comp[COMP] = .5f * (c2 + c3);     \
        dest2->comp[COMP] = .5f * (c1 + c3);     \
    } while (0)

    COMPUTE_COMPONENT_MID_POINT_VALUES(C_X);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_Y);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_Z);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_W);

    br_scalar one_div_w1 = dest2->comp[C_W] * dest3->comp[C_W]; /* = w2 * w3 */
    br_scalar one_div_w2 = dest1->comp[C_W] * dest3->comp[C_W]; /* = w1 * w3 */
    br_scalar one_div_w3 = dest1->comp[C_W] * dest2->comp[C_W]; /* = w1 * w2 */
    br_scalar w1w2w3 = 1.f / (dest1->comp[C_W] * one_div_w1); /* 1 / ( w1 * w2 * w3) */

    if (C2V(rend).block->convert_mask_f & (CM_R | CM_G | CM_B)) {
        COMPUTE_COMPONENT_MID_POINT_VALUES(C_R);
        COMPUTE_COMPONENT_MID_POINT_VALUES(C_G);
        COMPUTE_COMPONENT_MID_POINT_VALUES(C_B);
    }
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_I);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_U);
    COMPUTE_COMPONENT_MID_POINT_VALUES(C_V);

#undef COMPUTE_COMPONENT_MID_POINT_VALUES

    one_div_w1 *= w1w2w3; /* (w2 * w3) * 1 / ( w1 * w2 * w3) = 1 / w1 */
    one_div_w2 *= w1w2w3; /* (w1 * w3) * 1 / ( w1 * w2 * w3) = 1 / w2 */
    one_div_w3 *= w1w2w3; /* (w1 * w2) * 1 / ( w1 * w2 * w3) = 1 / w3 */

#define PROJECT_COMPONENT(C_P, C_SP)                                                                                                        \
    do {                                                                                                                                    \
    dest1->comp[C_SP] = renderer->state.cache.comp_offsets[C_SP] + renderer->state.cache.comp_scales[C_SP] * dest1->comp[C_P] * one_div_w1; \
    dest2->comp[C_SP] = renderer->state.cache.comp_offsets[C_SP] + renderer->state.cache.comp_scales[C_SP] * dest2->comp[C_P] * one_div_w2; \
    dest3->comp[C_SP] = renderer->state.cache.comp_offsets[C_SP] + renderer->state.cache.comp_scales[C_SP] * dest3->comp[C_P] * one_div_w3; \
    } while (0)

    PROJECT_COMPONENT(C_X, C_SX);
    PROJECT_COMPONENT(C_Y, C_SY);
    PROJECT_COMPONENT(C_Z, C_SZ);

#undef PROJECT_COMPONENT
}
C2_HOOK_FUNCTION(0x0054c400, averageVerticesOnScreen)
