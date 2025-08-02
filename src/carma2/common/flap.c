#include "flap.h"

#include "car.h"
#include "compress.h"
#include "crush.h"
#include "globvars.h"
#include "globvrpb.h"
#include "network.h"
#include "physics.h"
#include "platform.h"
#include "piping.h"
#include "utility.h"


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

void (C2_HOOK_FASTCALL * SendSemiDetachBit_original)(tCar_spec* pCar, br_actor* pActor, float pArg3, undefined4* pArg4);
void C2_HOOK_FASTCALL SendSemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, undefined4* pArg4) {

#if defined(C2_HOOKS_ENABLED)
    SendSemiDetachBit_original(pCar, pActor, pArg3, pArg4);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0042dab0, SendSemiDetachBit, SendSemiDetachBit_original)

int C2_HOOK_FASTCALL BitIsInBentPartOfCar(br_actor* pActor, float pArg2, float pArg3) {
    tCar_spec* car;
    float delta;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, old_frame_mat, 0x1c);

    if (pActor->user == NULL) {
        return 1;
    }
    /* FIXME: what type is stored in pActor->user? */
    car = *(tCar_spec**)pActor->user;
    if (car == NULL) {
        return 1;
    }
    delta = .4f * (pArg2 - pArg3);
    if (delta < 0.f) {
        return pArg3 + delta > car->old_frame_mat.m[2][2];
    } else {
        return pArg3 + delta < car->old_frame_mat.m[3][2];
    }
}

void C2_HOOK_FASTCALL SendDetachBit(tCar_spec* pCar, br_actor* pActor) {
    tNet_message* message;
    br_vector3 p1;
    br_vector3 p2;
    br_vector3 p3;
    br_bounds3 bnds;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, guaranteed.contents.detach_bit.ID, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, guaranteed.contents.detach_bit.field_0x4, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, guaranteed.contents.detach_bit.field_0x8, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, guaranteed.contents.detach_bit.bounds_min, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, guaranteed.contents.detach_bit.bounds_max, 0x2e);

#if !defined(C2_HOOKS_ENABLED)
    NOT_IMPLEMENTED(); /* FIXME: what type is pActor->user? */
#endif

    message = NetBuildGuaranteedMessage(51, 0);
    message->guaranteed.contents.detach_bit.ID = NetPlayerFromCar(pCar)->ID;
    message->guaranteed.contents.detach_bit.field_0x4 = ((undefined**)pActor->user)[0x8][0x20];
    message->guaranteed.contents.detach_bit.field_0x8 = C2V(gPHIL_last_physics_tick) + 120;
    if (!GetSDBJointPosAndBounds(&p1, &p2, &p3, &bnds, pActor)) {
        CompressVector3(&message->guaranteed.contents.detach_bit.bounds_min, &bnds.min, -10.f, 10.f);
        CompressVector3(&message->guaranteed.contents.detach_bit.bounds_max, &bnds.max, -10.f, 10.f);
        NetGuaranteedSendMessageToEverybody(C2V(gCurrent_net_game), message, 0);
    }
}

void (C2_HOOK_FASTCALL * DoDetaching_original)(void);
void C2_HOOK_FASTCALL DoDetaching(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    DoDetaching_original();
#else

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_spec, field_0x174, 0x174);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_spec, field_0x180, 0x180);

    if (C2V(gNet_mode) == eNet_mode_none || C2V(gNet_mode) == eNet_mode_host) {
        int new_count_crush_detach_list;
        int i;

        new_count_crush_detach_list = 0;
        for (i = 0; i < C2V(gCount_crush_detach_list); i++) {
            tCrush_detach_list_item* detach_list_item;
            int keep;
            int synced;

            detach_list_item = &C2V(gCrush_detach_list)[i];
            keep = 1;
            synced = 0;
            if (C2V(gPHIL_last_physics_tick) >= detach_list_item->time) {
                keep = 0;

                if (!((detach_list_item->car->car_crush_spec->field_0x144
                            && BitIsInBentPartOfCar(detach_list_item->actor,
                                   detach_list_item->car->car_crush_spec->field_0x174,
                                   detach_list_item->car->car_crush_spec->field_0x180))
                        || detach_list_item->car->car_crush_spec->field_0x4b8)) {

                    if (C2V(gNet_mode) == eNet_mode_none) {
                        if (detach_list_item->field_0x10) {

                            DetachBit(detach_list_item->car, detach_list_item->actor, NULL);
                        } else {
                            SemiDetachBit(detach_list_item->car, detach_list_item->actor, detach_list_item->field_0x8, &keep, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        if (detach_list_item->field_0x10) {
                            int j;

                            for (j = 0; j < C2V(gCount_net_crush_detach_list); j++) {
                                if (C2V(gNet_crush_detach_list)[j].actor == detach_list_item->actor) {
                                    synced = 1;
                                    break;
                                }
                            }
                            if (!synced) {
                                SendDetachBit(detach_list_item->car, detach_list_item->actor);
                            }
                        } else {
                            int j;

                            for (j = 0; j < C2V(gCount_net_crush_semi_detach_bit_list); j++) {
                                if (C2V(gNet_crush_semi_detach_bit_list)[j].actor == detach_list_item->actor) {
                                    synced = 1;
                                    break;
                                }
                            }
                            if (!synced) {
                                SendSemiDetachBit(detach_list_item->car, detach_list_item->actor, detach_list_item->field_0x8, NULL);
                            }
                        }
                    }
                }
            }
            if (keep && i != new_count_crush_detach_list) {
                C2V(gCrush_detach_list)[new_count_crush_detach_list] = C2V(gCrush_detach_list)[i];
                new_count_crush_detach_list += 1;
            }
        }
        C2V(gCount_crush_detach_list) = new_count_crush_detach_list;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00436ad0, DoDetaching, DoDetaching_original)

void (C2_HOOK_FASTCALL * SetBitForDetachment_original)(br_actor* pActor, tCar_spec* pCar, float pArg3, int pArg4);
void C2_HOOK_FASTCALL SetBitForDetachment(br_actor* pActor, tCar_spec* pCar, float pArg3, int pArg4) {

#if defined(C2_HOOKS_ENABLED)
    SetBitForDetachment_original(pActor, pCar, pArg3, pArg4);
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

int C2_HOOK_FASTCALL BitObjectIsSufficientlyOutsideCarObjectToDetach(tCollision_info* pObject) {
    tPhysics_joint* joint = pObject->physics_joint1;
    int i;

    for (i = 0; i < joint->count_limits; i++) {
        br_vector3 tv;
        br_vector3 tv2;

        BrMatrix34ApplyV(&tv, &joint->limits[i].child, &pObject->actor->t.t.mat);
        BrMatrix34TApplyV(&tv2, &tv, &pObject->parent->actor->t.t.mat);
        if (BrVector3Dot(&joint->limits[i].parent, &tv2) > sqrtf(.5f)) {
            return 1;
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00434910, BitObjectIsSufficientlyOutsideCarObjectToDetach)

void (C2_HOOK_FASTCALL * FullyDetachBit_original)(tCar_spec *pCar, tCollision_info* pObject);
void C2_HOOK_FASTCALL FullyDetachBit(tCar_spec* pCar, tCollision_info* pObject) {

#if defined(C2_HOOKS_ENABLED)
    FullyDetachBit_original(pCar, pObject);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0042f4e0, FullyDetachBit, FullyDetachBit_original)

void (C2_HOOK_FASTCALL * SendFullyDetachBit_original)(tCar_spec* pCar, tCollision_info* pObject);
void C2_HOOK_FASTCALL SendFullyDetachBit(tCar_spec* pCar, tCollision_info* pObject) {

#if defined(C2_HOOKS_ENABLED)
    SendFullyDetachBit_original(pCar, pObject);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004314b0, SendFullyDetachBit, SendFullyDetachBit_original)

void (C2_HOOK_FASTCALL * MungeDetachLists_original)(tCar_crush_spec* pCar_crush);
void C2_HOOK_FASTCALL MungeDetachLists(tCar_crush_spec* pCar_crush) {

#if 0//defined(C2_HOOKS_ENABLED)
    MungeDetachLists_original(pCar_crush);
#else
    int i;
    int new_count;

    new_count = 0;
    for (i = 0; i < pCar_crush->field_0x270; i++) {
        tCar_bit_spec* car_bit = &pCar_crush->field_0x274[i];

        /* FIXME: what is type ot tCar_bit_spec::field_0xc? */
        if (car_bit->field_0xc[10] == 1) {
            if (i != new_count) {
                pCar_crush->field_0x274[new_count] = *car_bit;
                new_count += 1;
            }
        } else if (car_bit->field_0xc[10] == 2) {
            pCar_crush->field_0x2b4[pCar_crush->field_0x2b0] = *car_bit;
            pCar_crush->field_0x2b0 += 1;
        }
    }
    pCar_crush->field_0x270 = new_count;

    new_count = 0;
    for (i = 0; i < pCar_crush->field_0x2b0; i++) {
        tCar_bit_spec* car_bit = &pCar_crush->field_0x2b4[i];

        /* FIXME: what is type ot tCar_bit_spec::field_0xc? */
        if (car_bit->field_0xc[10] == 1) {
            pCar_crush->field_0x274[pCar_crush->field_0x270] = *car_bit;
            pCar_crush->field_0x270 += 1;
        } else if (car_bit->field_0xc[10] == 2) {
            if (i != new_count) {
                pCar_crush->field_0x2b4[new_count] = *car_bit;
                new_count += 1;
            }
        }
    }
    pCar_crush->field_0x2b0 = new_count;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0042f3d0, MungeDetachLists, MungeDetachLists_original)

void (C2_HOOK_FASTCALL * DoFullyDetaching_original)(void);
void C2_HOOK_FASTCALL DoFullyDetaching(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    DoFullyDetaching_original();
#else

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, car_crush_spec, 0x18d4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_spec, field_0x270, 0x270);

    if (C2V(gNet_mode) == eNet_mode_none || C2V(gNet_mode) == eNet_mode_host) {
        int i;

        for (i = 0; i < C2V(gNum_active_cars); i++) {
            tCar_spec* car;

            car = C2V(gActive_car_list)[i];
            if (car != NULL && car->driver >= eDriver_oppo && car->car_crush_spec != NULL) {
                tCar_crush_spec* car_crush;
                int j;

                car_crush = car->car_crush_spec;
                for (j = 0; j < car_crush->field_0x270; j++) {
                    int detach;
                    tCar_bit_spec* bit_car = &car_crush->field_0x274[j];
                    tU32 dt;

#if !defined(C2_HOOKS_ENABLED)
                    NOT_IMPLEMENTED();
#endif
                    /* FIXME: What (enum) type does field_0xc have? */
                    switch (bit_car->field_0xc[9]) {
                    case 0:
                    case 1:
                        detach = BitObjectIsSufficientlyOutsideCarObjectToDetach(bit_car->field_0x8);
                        dt = PDGetTotalTime() - bit_car->field_0x10;
                        if (dt < C2V(gMax_detach_time_ms)) {
                            detach &= PercentageChance((int)((float)(100 * dt) / (float)C2V(gMax_detach_time_ms)));
                        }
                        break;
                    case 2:
                    case 4:
                        detach = 1;
                        break;
                    default:
                        detach = 0;
                        break;
                    }
                    if (detach) {
                        if (C2V(gNet_mode) == eNet_mode_none) {
                            FullyDetachBit(car, bit_car->field_0x8);
                        } else {
                            int bit_synced;
                            int k;

                            bit_synced = 0;
                            for (k = 0; k < C2V(gCount_net_crush_full_detach_bit_list); k++) {
                                if (C2V(gNet_crush_full_detach_bit_list)[k].object == bit_car->field_0x8) {
                                    bit_synced = 1;
                                    break;
                                }
                            }
                            if (!bit_synced) {
                                SendFullyDetachBit(car, bit_car->field_0x8);
                            }
                        }
                    }
                }
                MungeDetachLists(car_crush);
            }
        }
    }
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
