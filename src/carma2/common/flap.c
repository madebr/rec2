#include "flap.h"

#include "car.h"
#include "crush.h"
#include "physics.h"
#include "piping.h"


tCollision_info* (C2_HOOK_FASTCALL * DetachBit_original)(tCar_spec* pCar, br_actor* pActor, br_bounds3* pBounds);
tCollision_info* C2_HOOK_FASTCALL DetachBit(tCar_spec* pCar, br_actor* pActor, br_bounds3* pBounds) {

#if defined(C2_HOOKS_ENABLED)
    return DetachBit_original(pCar, pActor, pBounds);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00433350, DetachBit, DetachBit_original)

tCollision_info* (C2_HOOK_FASTCALL * SemiDetachBit_original)(tCar_spec* pCar, br_actor* pActor, float pArg3, int* pArg4, br_vector3* pArg5, br_bounds3* pArg6, br_vector3* pArg7, br_vector3* pArg8);
tCollision_info* C2_HOOK_FASTCALL SemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, int* pArg4, br_vector3* pArg5, br_bounds3* pArg6, br_vector3* pArg7, br_vector3* pArg8) {

#if defined(C2_HOOKS_ENABLED)
    return SemiDetachBit_original(pCar, pActor, pArg3, pArg4, pArg5, pArg6, pArg7, pArg8);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00436d60, SemiDetachBit, SemiDetachBit_original)

int (C2_HOOK_FASTCALL * GetSDBJointPosAndBounds_original)(br_vector3* pP1, br_vector3* pP2, br_vector3* pP33, br_bounds3* pBounds, br_actor* pActor);
int C2_HOOK_FASTCALL GetSDBJointPosAndBounds(br_vector3* pP1, br_vector3* pP2, br_vector3* pP33, br_bounds3* pBounds, br_actor* pActor) {

#if defined(C2_HOOKS_ENABLED)
    return GetSDBJointPosAndBounds_original(pP1, pP2, pP33, pBounds, pActor);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0042dbd0, GetSDBJointPosAndBounds, GetSDBJointPosAndBounds_original)

void (C2_HOOK_FASTCALL * DoDetaching_original)(void);
void C2_HOOK_FASTCALL DoDetaching(void) {

#if defined(C2_HOOKS_ENABLED)
    DoDetaching_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00436ad0, DoDetaching, DoDetaching_original)

void (C2_HOOK_FASTCALL * SetBitForDetachment_original)(br_actor* pActor, tCar_spec* pCar, float pArg3, int pArg4);
void C2_HOOK_FASTCALL SetBitForDetachment(br_actor* pActor, tCar_spec* pCar, float pArg3, int pArg4) {

#if defined(C2_HOOKS_ENABLED)
    SetBitForDetachment(pActor, pCar, pArg3, pArg4);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0042d7e0, SetBitForDetachment, SetBitForDetachment_original)

void C2_HOOK_FASTCALL MakeModelMaterialsDoubleSided(br_model* pModel) {
    int g;

    if (pModel == NULL) {
        return;
    }
    for (g = 0; g < pModel->prepared->ngroups; g++) {
        br_material* material;

        material = pModel->faces[*pModel->prepared->groups[g].face_user].material;
        if (material != NULL && !(material->flags & BR_MATF_TWO_SIDED)) {
            material->flags |= BR_MATF_TWO_SIDED;
            BrMaterialUpdate(material, BR_MATU_RENDERING);
        }
    }
}
C2_HOOK_FUNCTION(0x0042d950, MakeModelMaterialsDoubleSided)

intptr_t (C2_HOOK_FASTCALL * FlapBit_original)(br_actor* pActor, br_matrix34* pMat, void* pContext);
intptr_t C2_HOOK_FASTCALL FlapBit(br_actor* pActor, br_matrix34* pMat, void* pContext) {

#if defined(C2_HOOKS_ENABLED)
    return FlapBit_original(pActor, pMat, pContext);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004384c0, FlapBit, FlapBit_original)

void C2_HOOK_FASTCALL StartFlapping(br_actor* pActor, tCar_spec* pCar, tU8 pBits, float pArg4) {
    tUser_crush_data* user_crush;

    user_crush = pActor->user;
    if (user_crush != NULL && user_crush->crush_data != NULL && user_crush->crush_data->flap_data != NULL) {
        tCar_crush_flap_data* flap_data = user_crush->crush_data->flap_data;
        if (flap_data->kev_o_flap) {
            SetBitForDetachment(pActor, pCar, pArg4, 1);
        } else {
            MakeModelMaterialsDoubleSided(user_crush->models[0]);
            PipeSingleDSModel(1, user_crush->models[0]);
            pCar->car_crush_spec->field_0x134 += 1;
            flap_data->field_0x0 = 1;
        }
        pCar->use_shell_model = 0;
    }
}

void C2_HOOK_FASTCALL FlapBits(tCar_spec* pCar) {
    static C2_HOOK_VARIABLE_IMPLEMENT(tU32, last_flap_bits, 0x0067be88);
    tCar_crush_spec* car_crush_spec;

    car_crush_spec = pCar->car_crush_spec;
    if (car_crush_spec != NULL) {
        if (car_crush_spec->field_0x134) {
            float dt;
            br_vector3 force;
            tFlapBit_arg flapbit_arg;

            dt = (float)(C2V(gPHIL_last_physics_tick) - C2V(last_flap_bits)) / 1000.f;
            BrVector3Sub(&force, &pCar->collision_info->v, &car_crush_spec->field_0x138);
            BrVector3InvScale(&force, &force, dt);
            BrVector3Add(&force, &C2V(gBatty_gravity), &force);
            BrMatrix34TApplyV(&flapbit_arg.force, &force, &pCar->car_master_actor->t.t.mat);
            flapbit_arg.car = pCar;
            ARStartPipingSession(ePipe_chunk_flap_bits);
            DRActorEnumRecurseWithSnart(pCar->car_model_actor, NULL, FlapBit, &flapbit_arg);
            AREndPipingSession();
        }
        BrVector3Copy(&car_crush_spec->field_0x138,  &pCar->collision_info->v);
        C2V(last_flap_bits) = C2V(gPHIL_last_physics_tick);
    }
}

void (C2_HOOK_FASTCALL * DoFlapping_original)(void);
void C2_HOOK_FASTCALL DoFlapping(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    DoFlapping_original();
#else
    int i;

    for (i = 0; i < C2V(gCount_toggled_doors); i++) {
        tToggled_door *toggle_door = &C2V(gToggled_doors)[i];
        if (toggle_door->actor != NULL) {
            tUser_crush_data *user_crush_data = toggle_door->actor->user;
            if (user_crush_data != NULL && user_crush_data->crush_data != NULL &&
                user_crush_data->crush_data->flap_data != NULL) {
                tCar_crush_flap_data *flap_data = user_crush_data->crush_data->flap_data;
                if (!flap_data->field_0x0 && (toggle_door->field_0x8 & 0x1)) {
                    StartFlapping(toggle_door->actor, toggle_door->car, flap_data->field_0x0, toggle_door->field_0xc);
                } else if (flap_data->field_0x0 && !(toggle_door->field_0x8 & 0x1)) {
                    StopFlapping(toggle_door->actor, toggle_door->car);
                }
            }
        }
    }
    C2V(gCount_toggled_doors) = 0;
    for (i = 0; i < C2V(gNum_active_cars); i++) {
        tCar_spec* car;

        car = C2V(gActive_car_list)[i];
        if (car != NULL && car->driver >= eDriver_oppo) {
            FlapBits(car);
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004381b0, DoFlapping, DoFlapping_original)

void (C2_HOOK_FASTCALL * DoFullyDetaching_original)(void);
void C2_HOOK_FASTCALL DoFullyDetaching(void) {

#if defined(C2_HOOKS_ENABLED)
    DoFullyDetaching_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00438900, DoFullyDetaching, DoFullyDetaching_original)

void (C2_HOOK_FASTCALL * DoBending_original)(void);
void C2_HOOK_FASTCALL DoBending(void) {

#if defined(C2_HOOKS_ENABLED)
    DoBending_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00438a90, DoBending, DoBending_original)
