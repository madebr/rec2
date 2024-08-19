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

void C2_HOOK_FASTCALL InitGlassFragments(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gSmash_vertices)) != 200);

    for (i = 0; i < REC2_ASIZE(C2V(gSmash_vertices)); i++) {
        tSmash_vertex *smash_vertex;

        smash_vertex = &C2V(gSmash_vertices)[i];
        smash_vertex->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        smash_vertex->actor->model = BrModelAllocate(NULL, 3, 1);
        smash_vertex->actor->model->faces[0].vertices[0] = 0;
        smash_vertex->actor->model->faces[0].vertices[1] = 1;
        smash_vertex->actor->model->faces[0].vertices[2] = 2;
        smash_vertex->actor->model->flags |= BR_MODF_UPDATEABLE;
        BrModelAdd(smash_vertex->actor->model);
        smash_vertex->model = smash_vertex->actor->model;
        smash_vertex->actor->render_style = BR_RSTYLE_FACES;
        smash_vertex->field_0x8 = 0;
    }
}

void C2_HOOK_FASTCALL InitDecals(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gSmash_quads)) != 50);

    for (i = 0; i < REC2_ASIZE(C2V(gSmash_quads)); i++) {
        tSmash_quad *smash_quad;

        smash_quad = &C2V(gSmash_quads)[i];
        smash_quad->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        smash_quad->actor->model = BrModelAllocate(NULL, 4, 2);
        smash_quad->actor->model->faces[0].vertices[0] = 0;
        smash_quad->actor->model->faces[0].vertices[1] = 1;
        smash_quad->actor->model->faces[0].vertices[2] = 3;
        smash_quad->actor->model->faces[1].vertices[0] = 0;
        smash_quad->actor->model->faces[1].vertices[1] = 3;
        smash_quad->actor->model->faces[1].vertices[2] = 2;
        smash_quad->actor->model->flags |= BR_MODF_UPDATEABLE;
        BrModelAdd(smash_quad->actor->model);
        smash_quad->actor->render_style = BR_RSTYLE_FACES;
        smash_quad->field_0x4 = 0;
    }
}

void (C2_HOOK_FASTCALL * InitSmashing_original)(void);
void C2_HOOK_FASTCALL InitSmashing(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitSmashing_original();
#else

    InitGlassFragments();
    InitDecals();
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

void C2_HOOK_FASTCALL ResetSmashableRaceTargets(void) {

    C2V(gCount_smashable_race_targets) = 0;
}
C2_HOOK_FUNCTION(0x004977e0, ResetSmashableRaceTargets)

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

void (C2_HOOK_FASTCALL * DisposeSmashingAndExplosions_original)(void);
void C2_HOOK_FASTCALL DisposeSmashingAndExplosions(void) {

#if defined(C2_HOOKS_ENABLED)
    DisposeSmashingAndExplosions_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004f02b0, DisposeSmashingAndExplosions,DisposeSmashingAndExplosions_original)
