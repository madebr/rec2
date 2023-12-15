#include "animation.h"

#include "errors.h"
#include "loading.h"

#include "brender/brender.h"
#include "rec2_macros.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gExplosion_rotate_names, 2, 0x0065fee8, {
    "norotate",
    "randomrotate",
});

void C2_HOOK_FASTCALL ReadExplosionAnimation(FILE *pF, tExplosion_animation* pAnimation) {
    int i;
    int j;
    char s[256];
    tExplosion_animation_group* group;
    float f1, f2;
    int d1, d2;

    C2_HOOK_BUG_ON(sizeof(tExplosion_animation) != 8);
    C2_HOOK_BUG_ON(sizeof(tExplosion_animation_group) != 68);
    C2_HOOK_BUG_ON(sizeof(tExplosion_animation_frame) != 8);

    /* Number of explosion groups */
    pAnimation->count_groups = GetAnInt(pF);
    if (pAnimation->count_groups == 0) {
        return;
    }
    pAnimation->groups = BrMemAllocate(pAnimation->count_groups * sizeof(tExplosion_animation_group), kMem_explosion_data);
    for (i = 0; i < pAnimation->count_groups; i++) {
        group = &pAnimation->groups[i];

        /* min count, max count */
        GetPairOfInts(pF, &d1, &d2);
        group->min_count = d1;
        group->max_count = d2;
        /* min start delay, max start delay */
        GetPairOfFloats(pF, &f1, &f2);
        group->min_start_delay_ms = (tU16)(1000 * f1);
        group->max_start_delay_ms = (tU16)(1000 * f2);
        /* offset */
        GetThreeFloats(pF, &group->offset.v[0], &group->offset.v[1], &group->offset.v[2]);
        /* min x factor, max x factor */
        GetPairOfFloats(pF, &group->min_xyz_factor.v[0], &group->max_xyz_factor.v[0]);
        /* min y factor, max y factor */
        GetPairOfFloats(pF, &group->min_xyz_factor.v[1], &group->max_xyz_factor.v[1]);
        /* min z factor, max z factor */
        GetPairOfFloats(pF, &group->min_xyz_factor.v[2], &group->max_xyz_factor.v[2]);
        /*  min frame rate, max frame rate */
        GetPairOfFloats(pF, &f1, &f2);
        group->min_frametime_ms = (tU16)(1000.f / f1);
        group->max_frametime_ms = (tU16)(1000.f / f2);
        /* min scaling factor, max scaling factor */
        GetPairOfFloats(pF, &group->min_scaling_factor, &group->max_scaling_factor);
        /* rotate mode */
        group->rotate_modus = GetALineAndInterpretCommand(pF, C2V(gExplosion_rotate_names), REC2_ASIZE(C2V(gExplosion_rotate_names)));
        /* number of frames */
        group->count_frames = GetAnInt(pF);
        group->frames = BrMemAllocate(group->count_frames * sizeof(tExplosion_animation_frame), kMem_explosion_data);
        for (j = 0; j < group->count_frames; j++) {
            /* opacity */
            group->frames[j].opacity = GetAScalar(pF);
            group->frames[j].opacity = 100.f;

            /* frame pix name */
            GetAString(pF, s);
            group->frames[j].map = BrMapFind(s);
            if (group->frames[j].map == NULL) {
                FatalError(kFatalError_CantLoadSmashPix_S, s);
            }
        }
    }
}
C2_HOOK_FUNCTION(0x004ee780, ReadExplosionAnimation)
