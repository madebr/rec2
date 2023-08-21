#include "smashing.h"

#include "loading.h"

#include <brender/brender.h>

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tSmash_vertex, gSmash_vertices, 200, 0x006b78e0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tSmash_quad, gSmash_quads, 50, 0x006a80f8);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gInitial_smashable_position_type_names, 3, 0x0065fed0, {
    "sphereclumped",
    "boxclumped",
    "actorbased",
});

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gInitial_position_sphere_where, 2, 0x0065fee0, {
    "impact",
    "model",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tSmashable_race_target, gSmashable_race_targets, 300, 0x0068c898);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_smashable_race_targets, 0x0074abe0);

void (C2_HOOK_FASTCALL * InitSmashing_original)(void);
void C2_HOOK_FASTCALL InitSmashing(void) {

#if defined(C2_HOOKS_ENABLED)
    InitSmashing_original();
#else
    int i;

    C2_HOOK_BUG_ON(sizeof(tSmash_vertex) != 0x38);
    for (i = 0; i < REC2_ASiZE(gSmash_vertices); i++) {
        C2V(gSmash_vertices)[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        C2V(gSmash_vertices)[i].actor->model = BrModelAllocate(NULL, 3, 1);
        C2V(gSmash_vertices)[i].actor->model->faces[0].vertices[0] = 0;
        C2V(gSmash_vertices)[i].actor->model->faces[0].vertices[1] = 1;
        C2V(gSmash_vertices)[i].actor->model->faces[0].vertices[2] = 2;
        C2V(gSmash_vertices)[i].actor->model->flags |= BR_MODF_UPDATEABLE;
        BrModelAdd(C2V(gSmash_vertices)[i].actor->model);
        C2V(gSmash_vertices)[i].actor->model->render_style = BR_RSTYLE_FACES;
        C2V(gSmash_vertices)[i].field_0x8 = 0;
    }

    C2_HOOK_BUG_ON(sizeof(tSmash_quad) != 0x8);
    for (i = 0; i < REC2_ASiZE(gSmash_quads); i++) {
        C2V(gSmash_quads)[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        C2V(gSmash_quads)[i].actor->model= BrModelAllocate(NULL, 4, 2);
        C2V(gSmash_quads)[i].actor->model->faces[0].vertices[0] = 0;
        C2V(gSmash_quads)[i].actor->model->faces[0].vertices[1] = 1;
        C2V(gSmash_quads)[i].actor->model->faces[0].vertices[2] = 3;
        C2V(gSmash_quads)[i].actor->model->faces[1].vertices[0] = 0;
        C2V(gSmash_quads)[i].actor->model->faces[1].vertices[1] = 3;
        C2V(gSmash_quads)[i].actor->model->faces[1].vertices[2] = 2;
        C2V(gSmash_quads)[i].actor->model->flags |= BR_MODF_UPDATEABLE;
        BrModelAdd(C2V(gSmash_quads)[i].actor->model);
        C2V(gSmash_quads)[i].actor->model->render_style = BR_RSTYLE_FACES;
        C2V(gSmash_quads)[i].field_0x4 = 0;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004efe00, InitSmashing, InitSmashing_original)

void C2_HOOK_FASTCALL ReadSmashableInitialPosition(FILE* pFile, tSmashable_initial_position_spec*  pInitial_pos) {

    /* Initial position type */
    pInitial_pos->type = GetALineAndInterpretCommand(pFile, C2V(gInitial_smashable_position_type_names), REC2_ASIZE(C2V(gInitial_smashable_position_type_names)));

    switch (pInitial_pos->type) {
        case kInitialSmashablePosition_SphereClumped:
            /* Sphere radius */
            pInitial_pos->position.sphere.radius = GetAScalar(pFile);
            /* Sphere centre */
            pInitial_pos->position.sphere.where = GetALineAndInterpretCommand(pFile, C2V(gInitial_position_sphere_where), REC2_ASIZE(C2V(gInitial_position_sphere_where)));
            break;
        case kInitialSmashablePosition_BoxClumped:
            GetThreeFloats(pFile, &pInitial_pos->position.box.v[0], &pInitial_pos->position.box.v[1], &pInitial_pos->position.box.v[2]);
            break;
        default:
            break;
    }
}
C2_HOOK_FUNCTION(0x004ee550, ReadSmashableInitialPosition)

void C2_HOOK_FASTCALL ReadSmashableInitialSpeed(FILE* pFile, tSmashable_initial_speed_spec* pInitial_speed) {

    C2_HOOK_BUG_ON(sizeof(tSmashable_initial_speed_spec) != 28);

    /* Min,max speed towards car */
    GetPairOfFloats(pFile, &pInitial_speed->min_speed, &pInitial_speed->max_speed);
    /* Impactee velocity factor */
    pInitial_speed->impactee_velocity_factor = GetAScalar(pFile);
    /* Random velocity (max) */
    pInitial_speed->random_velocity_max = GetAScalar(pFile);
    /* Random up velocity (max) */
    pInitial_speed->random_up_velocity_max = GetAScalar(pFile);
    /* Random normal velocity (max) */
    pInitial_speed->random_normal_velocity_max = GetAScalar(pFile);
    /* Random spin rate (max) */
    pInitial_speed->random_spin_rate_max = GetAScalar(pFile);
}
C2_HOOK_FUNCTION(0x004ee500, ReadSmashableInitialSpeed)

void C2_HOOK_FASTCALL ReadMinMaxTimeInMilliseconds(FILE* pFile, int* pTimes) {
    float f1;
    float f2;

    /* Min time, Max time */
    GetPairOfFloats(pFile, &f1, &f2);
    pTimes[0] = (int)(1000.f * f1);
    pTimes[1] = (int)(1000.f * f2);
}
C2_HOOK_FUNCTION(0x004ee5a0, ReadMinMaxTimeInMilliseconds)

void C2_HOOK_FASTCALL AddSmashableRaceTarget(br_model* pModel, br_actor* pActor, int pUnknown) {

    C2_HOOK_BUG_ON(sizeof(tSmashable_race_target) != 12);
    if (C2V(gCount_smashable_race_targets) < REC2_ASIZE(C2V(gSmashable_race_targets))) {
        C2V(gSmashable_race_targets)[C2V(gCount_smashable_race_targets)].model = pModel;
        C2V(gSmashable_race_targets)[C2V(gCount_smashable_race_targets)].actor = pActor;
        C2V(gSmashable_race_targets)[C2V(gCount_smashable_race_targets)].field_0x8 = pUnknown;
        C2V(gCount_smashable_race_targets)++;
    }
}
C2_HOOK_FUNCTION(0x004977f0, AddSmashableRaceTarget)

void (C2_HOOK_FASTCALL * PrepareSmashableTrackItemIdentifiers_original)(void);
void C2_HOOK_FASTCALL PrepareSmashableTrackItemIdentifiers(void) {

#if defined(C2_HOOKS_ENABLED)
    PrepareSmashableTrackItemIdentifiers_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004f0bc0, PrepareSmashableTrackItemIdentifiers, PrepareSmashableTrackItemIdentifiers_original)
