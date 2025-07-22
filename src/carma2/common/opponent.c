#include "opponent.h"

#include "car.h"
#include "controls.h"
#include "errors.h"
#include "finteray.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrme.h"
#include "globvrpb.h"
#include "loading.h"
#include "mainloop.h"
#include "physics.h"
#include "piping.h"
#include "platform.h"
#include "structur.h"
#include "trig.h"
#include "utility.h"

#include <brender/brender.h>

#include "rec2_macros.h"

#include "c2_string.h"

#include <stdarg.h>

#define CAR_SPEC_IS_ROZZER(CAR_SPEC) (VEHICLE_TYPE_FROM_ID((CAR_SPEC)->car_ID) == eVehicle_rozzer)
#define CAR_SPEC_IS_OPPONENT(CAR_SPEC) (VEHICLE_TYPE_FROM_ID((CAR_SPEC)->car_ID) == eVehicle_opponent)

#define GET_CAR_SPEED_FACTOR(CAR) (CAR_SPEC_IS_ROZZER(CAR) ? C2V(gCop_speed_factor) : C2V(gOpponent_speed_factor))

C2_HOOK_VARIABLE_IMPLEMENT(int, gActive_car_list_rebuild_required, 0x0069173c);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gBIG_APC_index, 0x0065a3c4, -1);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNumber_of_cops_before_faffage, 0x00691744);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gNext_grudge_reduction, 0x00691754);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFirst_frame, 0x00691738);
C2_HOOK_VARIABLE_IMPLEMENT(int, gGrudge_reduction_per_period, 0x00691724);
C2_HOOK_VARIABLE_IMPLEMENT(int, gAcknowledged_start, 0x00691750);
C2_HOOK_VARIABLE_IMPLEMENT(int, gStart_jumped, 0x00691720);
C2_HOOK_VARIABLE_IMPLEMENT(br_scalar, gMinimum_yness_before_knackerisation, 0x00691760);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gAcme_frame_count, 0x00691728);
C2_HOOK_VARIABLE_IMPLEMENT(br_scalar, gHead_on_cos_value, 0x00691768);
C2_HOOK_VARIABLE_IMPLEMENT(int, gBig_bang, 0x00691714);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(float, gOpponent_nastyness_frigger, 0x0065a3cc, 1.f);
C2_HOOK_VARIABLE_IMPLEMENT(br_scalar, gIn_view_distance, 0x00691764);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gChallenger_index__opponent, 0x0065a3c8, -1);
C2_HOOK_VARIABLE_IMPLEMENT(tU8*, gBit_per_node, 0x0069175c);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_material*, gMat_dk_yel, 0x00676858, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_material*, gMat_md_yel, 0x0067685c, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_material*, gMat_lt_yel, 0x00676860, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_material*, gMat_dk_red, 0x00676864, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_material*, gMat_md_red, 0x00676868, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_material*, gMat_lt_red, 0x0067686c, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_material*, gMat_dk_grn, 0x00676870, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_material*, gMat_md_grn, 0x00676874, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_material*, gMat_lt_grn, 0x00676878, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_material*, gMat_dk_blu, 0x0067687c, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_material*, gMat_lt_blu, 0x00676880, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_material*, gMat_dk_turq, 0x00676884, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_material*, gMat_lt_turq, 0x00676888, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_material*, gMat_dk_gry, 0x0067688c, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_material*, gMat_md_gry, 0x00676890, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_material*, gMat_lt_gry, 0x00676894, NULL);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNum_of_opponents_pursuing, 0x00691718);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNum_of_opponents_getting_near, 0x00691740);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNum_of_opponents_completing_race, 0x00691758);
C2_HOOK_VARIABLE_IMPLEMENT(float, gDefinite_no_cop_pursuit_speed, 0x00691734);
C2_HOOK_VARIABLE_IMPLEMENT(float, gCop_pursuit_speed_percentage_multiplier, 0x00691748);

void C2_HOOK_FASTCALL PointActorAlongThisBloodyVector(br_actor* pThe_actor, br_vector3* pThe_vector) {
    br_transform trans;

    trans.type = BR_TRANSFORM_LOOK_UP;
    BrVector3Copy(&trans.t.look_up.look, pThe_vector);
    BrVector3Set(&trans.t.look_up.up, 0.f, 1.f, 0.f);
    BrVector3Copy(&trans.t.look_up.t, &pThe_actor->t.t.translate.t);
    BrTransformToTransform(&pThe_actor->t, &trans);
}
C2_HOOK_FUNCTION(0x004013d0, PointActorAlongThisBloodyVector)

void C2_HOOK_FASTCALL PointActorAlongVectorWithUp(br_actor* pThe_actor, br_vector3* pLook, br_vector3* pUp) {
    br_transform trans;

    trans.type = BR_TRANSFORM_LOOK_UP;
    BrVector3Copy(&trans.t.look_up.look, pLook);
    BrVector3Copy(&trans.t.look_up.up, pUp);
    BrVector3Copy(&trans.t.look_up.t, &pThe_actor->t.t.translate.t);
    BrTransformToTransform(&pThe_actor->t, &trans);
}
C2_HOOK_FUNCTION(0x00401430, PointActorAlongVectorWithUp)

void C2_HOOK_FASTCALL InitOpponentPsyche(int pOpponent_index) {

    C2V(gOpponents)[pOpponent_index].psyche.grudge_against_player = 0;
}
C2_HOOK_FUNCTION(0x004aee90, InitOpponentPsyche)

void C2_HOOK_FASTCALL ReallocExtraPathNodes(int pCount) {
    tPath_node* nodes;

    C2_HOOK_BUG_ON(sizeof(tPath_node) != 0x20);

    if (pCount != 0) {

        nodes = BrMemAllocate((C2V(gProgram_state).AI_vehicles.number_of_path_nodes + pCount) * sizeof(tPath_node), kMem_oppo_new_nodes);
        c2_memcpy(nodes, C2V(gProgram_state).AI_vehicles.path_nodes, C2V(gProgram_state).AI_vehicles.number_of_path_nodes * sizeof(tPath_node));

        if (C2V(gProgram_state).AI_vehicles.path_nodes != NULL) {
            BrMemFree(C2V(gProgram_state).AI_vehicles.path_nodes);
        }
        C2V(gProgram_state).AI_vehicles.path_nodes = nodes;
        C2V(gProgram_state).AI_vehicles.number_of_path_nodes += pCount;
    }
    DoNotDprintf_opponent("ReallocExtraPathNodes(): Allocated %d bytes for %d path nodes", (pCount + C2V(gProgram_state).AI_vehicles.number_of_path_nodes) * sizeof(tPath_node), pCount);
}

void C2_HOOK_FASTCALL ReallocExtraPathSections(int pCount) {
    tPath_section* sections;

    C2_HOOK_BUG_ON(sizeof(tPath_section) != 0x14);

    if (pCount != 0) {

        sections = BrMemAllocate((C2V(gProgram_state).AI_vehicles.number_of_path_sections + pCount) * sizeof(tPath_section), kMem_oppo_new_sections);
        c2_memcpy(sections, C2V(gProgram_state).AI_vehicles.path_sections, C2V(gProgram_state).AI_vehicles.number_of_path_sections * sizeof(tPath_section));

        if (C2V(gProgram_state).AI_vehicles.path_sections != NULL) {
            BrMemFree(C2V(gProgram_state).AI_vehicles.path_sections);
        }
        C2V(gProgram_state).AI_vehicles.path_sections = sections;
        C2V(gProgram_state).AI_vehicles.number_of_path_sections += pCount;
    }
    DoNotDprintf_opponent("ReallocExtraPathSections(): Allocated %d bytes for %d path sections", (pCount + C2V(gProgram_state).AI_vehicles.number_of_path_sections) * sizeof(tPath_section), pCount);
}

void (C2_HOOK_FASTCALL * LoadInOppoPaths_original)(FILE* pF);
void C2_HOOK_FASTCALL LoadInOppoPaths(FILE* pF) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadInOppoPaths_original(pF);
#else
    char s[256];
    int count_nodes;
    int count_sections;
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tProgram_state, current_car, 0xac);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tProgram_state, player_name, 0x1a10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tProgram_state, special_volumes, 0x1c00);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tProgram_state, AI_vehicles, 0x1c20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tProgram_state, AI_vehicles.number_of_path_nodes, 0x1c28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tProgram_state, AI_vehicles.number_of_path_sections, 0x1c2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tProgram_state, AI_vehicles.path_nodes, 0x5ec0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tProgram_state, AI_vehicles.path_sections, 0x5ec4);

    DoNotDprintf_opponent("Start of LoadInOppoPaths()...");
    C2V(gProgram_state).AI_vehicles.number_of_path_nodes = 0;
    C2V(gProgram_state).AI_vehicles.number_of_path_sections = 0;
    C2V(gProgram_state).AI_vehicles.path_nodes = NULL;
    C2V(gProgram_state).AI_vehicles.path_sections = NULL;
    C2V(gBit_per_node) = NULL;
    C2V(gBIG_APC_index) = -1;

    for (;;) {
        if (!GetALineAndDontArgue(pF, s)) {
            return;
        }
        if (c2_strcmp(s, "START OF OPPONENT PATHS") == 0) {
            break;
        }
    }

    /* Number of path nodes */
    count_nodes = GetAnInt(pF);
    ReallocExtraPathNodes(count_nodes);

    for (i = 0; i < C2V(gProgram_state).AI_vehicles.number_of_path_nodes; i++) {
        br_vector3 pos;

        /* Node #i */
        GetThreeFloats(pF, &pos.v[0], &pos.v[1], &pos.v[2]);
        BrVector3Copy(&C2V(gProgram_state).AI_vehicles.path_nodes[i].pos, &pos);
        C2V(gProgram_state).AI_vehicles.path_nodes[i].number_of_sections = 0;
    }

    /* Number of path sections */
    count_sections = GetAnInt(pF);
    ReallocExtraPathSections(count_sections);

    for (i = 0; i < C2V(gProgram_state).AI_vehicles.number_of_path_sections; i++) {
        br_scalar scalars[8];
        br_vector3 dp;
        int j;

        C2_HOOK_BUG_ON(BR_ASIZE(scalars) != 8);

        PossibleService();

        /* Section #i: node[0], node[1], min_speed[0], max_speed[0], min_speed[1], max_speed[1], width, type*/
        GetNScalars(pF, BR_ASIZE(scalars), scalars);
        C2V(gProgram_state).AI_vehicles.path_sections[i].node_indices[0] = (tU16)scalars[0];
        C2V(gProgram_state).AI_vehicles.path_sections[i].node_indices[1] = (tU16)scalars[1];
        C2V(gProgram_state).AI_vehicles.path_sections[i].min_speed[0] = (tU8)scalars[2];
        C2V(gProgram_state).AI_vehicles.path_sections[i].max_speed[0] = (tU8)scalars[3];
        C2V(gProgram_state).AI_vehicles.path_sections[i].min_speed[1] = (tU8)scalars[4];
        C2V(gProgram_state).AI_vehicles.path_sections[i].max_speed[1] = (tU8)scalars[5];
        C2V(gProgram_state).AI_vehicles.path_sections[i].width = scalars[6];

        BrVector3Sub(&dp,
            &C2V(gProgram_state).AI_vehicles.path_nodes[C2V(gProgram_state).AI_vehicles.path_sections[i].node_indices[0]].pos,
            &C2V(gProgram_state).AI_vehicles.path_nodes[C2V(gProgram_state).AI_vehicles.path_sections[i].node_indices[1]].pos);
        C2V(gProgram_state).AI_vehicles.path_sections[i].length = BrVector3Length(&dp);

        if (scalars[7] < 1000.0f) {
            C2V(gProgram_state).AI_vehicles.path_sections[i].type = (tU8)scalars[7];
            C2V(gProgram_state).AI_vehicles.path_sections[i].one_way = 0;
        } else {
            C2V(gProgram_state).AI_vehicles.path_sections[i].type = (tU8)(scalars[7] - 1000.0f);
            C2V(gProgram_state).AI_vehicles.path_sections[i].one_way = 1;
        }
        for (j = 0; j < 2; j++) {
            tPath_node* node_ptr;

            node_ptr = &C2V(gProgram_state).AI_vehicles.path_nodes[C2V(gProgram_state).AI_vehicles.path_sections[i].node_indices[j]];
            if (node_ptr->number_of_sections >= 8) {
                DoNotDprintf_opponent("ERROR: Too many sections (including section #%d) attached to node #%d",
                    i, C2V(gProgram_state).AI_vehicles.path_sections[i].node_indices[j]);
            } else {
                node_ptr->sections[node_ptr->number_of_sections] = (tS16)i;
                node_ptr->number_of_sections += 1;
            }
        }
    }

    if (C2V(gAusterity_mode) || C2V(gNet_mode) != eNet_mode_none) {

        C2V(gProgram_state).AI_vehicles.number_of_cops = GetAnInt(pF);
        for (i = 0; i < C2V(gProgram_state).AI_vehicles.number_of_cops; i++) {
            GetALineAndDontArgue(pF, s);
        }
        C2V(gProgram_state).AI_vehicles.number_of_cops = 0;
    } else {

        C2V(gProgram_state).AI_vehicles.number_of_cops = GetAnInt(pF);
        for (i = 0; i < C2V(gProgram_state).AI_vehicles.number_of_cops; i++) {
            br_scalar scalars[6];
            br_vector3 cop_to_section;
            br_vector3 intersect;
            br_vector3 section_v;
            br_scalar distance;

            C2_HOOK_BUG_ON(REC2_ASIZE(scalars) != 6);

            PossibleService();

            /* x, y, z, type[0], type[1], type[2] */
            GetNScalars(pF, REC2_ASIZE(scalars), scalars);
            BrVector3Set(&C2V(gProgram_state).AI_vehicles.cop_start_points[i],
                scalars[0], scalars[1], scalars[2]);

            if (scalars[3] == 9.0f && scalars[4] == 9.0f && scalars[5] == 9.0f) {
                C2V(gBIG_APC_index) = i;
            }

            FindNearestPathSection(&C2V(gProgram_state).AI_vehicles.cop_start_points[i], &cop_to_section, &intersect, &distance);
            BrVector3Cross(&C2V(gProgram_state).AI_vehicles.cop_start_vectors[i],
                &C2V(y_unit_vector), &cop_to_section);
            BrVector3Sub(&section_v, &intersect, &C2V(gProgram_state).AI_vehicles.cop_start_points[i]);
            if (BrVector3Dot(&C2V(gProgram_state).AI_vehicles.cop_start_vectors[i], &section_v) < 0.0f) {
                BrVector3Negate(&C2V(gProgram_state).AI_vehicles.cop_start_vectors[i], &C2V(gProgram_state).AI_vehicles.cop_start_vectors[i]);
            }
        }
    }

    for (;;) {
        GetALineAndDontArgue(pF, s);
        if (c2_strcmp(s, "END OF OPPONENT PATHS") == 0) {
            break;
        }
    }

    if (C2V(gProgram_state).AI_vehicles.number_of_path_sections != 0) {
        C2V(gBit_per_node) = BrMemAllocate((C2V(gProgram_state).AI_vehicles.number_of_path_nodes + 7) / 8, kMem_oppo_bit_per_node);
    } else {
        C2V(gBit_per_node) = NULL;
    }
    DoNotDprintf_opponent("End of LoadInOppoPaths(), totals:");
    DoNotDprintf_opponent("Nodes: %d", C2V(gProgram_state).AI_vehicles.number_of_path_nodes);
    DoNotDprintf_opponent("Sections: %d", C2V(gProgram_state).AI_vehicles.number_of_path_sections);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004a9610, LoadInOppoPaths, LoadInOppoPaths_original)

int (C2_HOOK_FASTCALL * PointVisibleFromHere_original)(br_vector3* pFrom, br_vector3* pTo);
int C2_HOOK_FASTCALL PointVisibleFromHere(br_vector3* pFrom, br_vector3* pTo) {
    br_vector3 from;
    br_vector3 dir;
    br_vector3 norm;
    br_scalar t;
    br_material* material;

    BrVector3Sub(&dir, pTo, pFrom);
    dir.v[1] += 0.15f;
    BrVector3Copy(&from, pFrom);
    from.v[1] += 0.15f;
    FindFace(&from, &dir, &norm, &t, &material);
    return t > 1.0;
}
C2_HOOK_FUNCTION_ORIGINAL(0x004aa400, PointVisibleFromHere, PointVisibleFromHere_original)

void (C2_HOOK_FASTCALL * CalcPlayerConspicuousness_original)(tOpponent_spec* pOpponent_spec);
void C2_HOOK_FASTCALL CalcPlayerConspicuousness(tOpponent_spec* pOpponent_spec)  {

#if 0//defined(C2_HOOKS_ENABLED)
    CalcPlayerConspicuousness_original(pOpponent_spec);
#else
    br_vector3 pos_in_cop_space;
    br_matrix34 inverse_transform;

    if (pOpponent_spec->next_player_visibility_check >= C2V(gTime_stamp_for_this_munging)) {
        return;
    }
    pOpponent_spec->player_in_view_now = 0;
    if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
        pOpponent_spec->next_player_visibility_check = C2V(gTime_stamp_for_this_munging) + IRandomBetween(0, 900) + 100;
        if (pOpponent_spec->player_to_oppo_d < 20.f) {
            BrMatrix34LPInverse(&inverse_transform, &pOpponent_spec->car_spec->car_master_actor->t.t.mat);
            BrMatrix34ApplyP(&pos_in_cop_space, &C2V(gProgram_state).current_car.car_master_actor->t.t.translate.t, &inverse_transform);
            if (pos_in_cop_space.v[2] < 0.f && PointVisibleFromHere(&C2V(gProgram_state).current_car.car_master_actor->t.t.translate.t, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t)) {
                pOpponent_spec->player_in_view_now = 1;
                pOpponent_spec->acknowledged_piv = 0;
            }
        }
    } else {
        pOpponent_spec->next_player_visibility_check = C2V(gTime_stamp_for_this_munging) + IRandomBetween(0, 900) + 4000;
        DoNotDprintf_opponent("%s: Time now: %9.2f; next vis check at %9.2f", pOpponent_spec->car_spec->driver_name, C2V(gTime_stamp_for_this_munging) / 1000.f, pOpponent_spec->next_player_visibility_check / 1000.0f);
        if (pOpponent_spec->player_to_oppo_d < 50.f && PointVisibleFromHere(&C2V(gProgram_state).current_car.car_master_actor->t.t.translate.t, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t)) {
            pOpponent_spec->player_in_view_now = 1;
            pOpponent_spec->acknowledged_piv = 0;
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004adae0, CalcPlayerConspicuousness, CalcPlayerConspicuousness_original)

void (C2_HOOK_FASTCALL * ProcessPursueAndTwat_original)(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);
void C2_HOOK_FASTCALL ProcessPursueAndTwat(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {

#if defined(C2_HOOKS_ENABLED)
    ProcessPursueAndTwat_original(pOpponent_spec, pCommand);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ab190, ProcessPursueAndTwat, ProcessPursueAndTwat_original)

void C2_HOOK_FASTCALL DisposeOpponentPaths(void) {

    if (C2V(gProgram_state).AI_vehicles.path_nodes != NULL) {
        BrMemFree(C2V(gProgram_state).AI_vehicles.path_nodes);
    }
    C2V(gProgram_state).AI_vehicles.number_of_path_nodes = 0;
    C2V(gProgram_state).AI_vehicles.path_nodes = NULL;

    if (C2V(gProgram_state).AI_vehicles.path_sections != NULL) {
        BrMemFree(C2V(gProgram_state).AI_vehicles.path_sections);
    }
    C2V(gProgram_state).AI_vehicles.number_of_path_sections = 0;
    C2V(gProgram_state).AI_vehicles.path_sections = NULL;

    if (C2V(gBit_per_node) != NULL) {
        BrMemFree(C2V(gBit_per_node));
    }
    C2V(gBit_per_node) = NULL;
}
C2_HOOK_FUNCTION(0x004a9c50, DisposeOpponentPaths)

void C2_HOOK_FASTCALL AddIfNotInList(tCollision_info* pObject, tCollision_info** pList, int pCount) {
    int i;

    for (i = 0; i < pCount; i++) {
        if (pList[i] == pObject) {
            pList[i] = NULL;
            return;
        }
    }
    PHILAddActiveObject(pObject, NULL, NULL, NULL);
}
C2_HOOK_FUNCTION(0x004a7ea0, AddIfNotInList)

void C2_HOOK_FASTCALL RemoveAnythingStillInList(tCollision_info** pList, int pCount) {
    int i;

    for (i = 0; i < pCount; i++) {
        if (pList[i] != NULL) {
            PHILRemoveObject(pList[i]);
            pList[i]->field183_0x1d8 = 0;
        }
    }
}
C2_HOOK_FUNCTION(0x004a7ef0, RemoveAnythingStillInList)

void C2_HOOK_FASTCALL NoteCarsCurrentlyUsed(tCollision_info** pCollision_infos, int* pCount) {
    tCollision_info* info;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, flags_0x238, 0x238);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, owner, 0x23c);

    for (info = PHILGetFirstObject(); info != NULL; info = PHILGetNextObject(info)) {
        if (info != NULL && info->owner != NULL && info->flags_0x238 == 1 && ((tCar_spec*)info->owner)->driver > 5) {
            pCollision_infos[*pCount] = info;
            *pCount += 1;
        }
    }
}

int C2_HOOK_FASTCALL IsNetCarActive(const br_vector3* pPoint) {
    br_vector3 tv;

    BrVector3Sub(&tv, &C2V(gProgram_state).current_car.car_master_actor->t.t.translate.t, pPoint);
    if (BrVector3LengthSquared(&tv) < 100.f) {
        return 1;
    }
    if (C2V(gCar_to_view) != &C2V(gProgram_state).current_car) {
        BrVector3Sub(&tv, &C2V(gCar_to_view)->car_master_actor->t.t.translate.t, pPoint);
        return BrVector3LengthSquared(&tv) < 100.f;
    }
    return 0;
}

void (C2_HOOK_FASTCALL * RebuildActiveCarList_original)(void);
void C2_HOOK_FASTCALL RebuildActiveCarList(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    RebuildActiveCarList_original();
#else
    int new_count_active_cars = C2V(gNum_active_cars);

    if (C2V(gActive_car_list_rebuild_required)) {
        int count_used_cars;
        tCollision_info* used_cars[1000];
        int i;

        C2V(gActive_car_list_rebuild_required) = 0;

        count_used_cars = 0;
        NoteCarsCurrentlyUsed(used_cars, &count_used_cars);

        new_count_active_cars = 0;
        for (i = 0; i < GetCarCount(eVehicle_self); i++) {
            tCar_spec* car;

            if (i > 0) {
                PDFatalError("mGet_car_count( eVehicle_self ) > 1 - I didn't know this could happen!");
            }
            car = GetCarSpec(eVehicle_self, i);
            if (!car->disabled || C2V(gAction_replay_mode)) {
                car->active = 1;
                C2V(gActive_car_list)[new_count_active_cars] = car;
                new_count_active_cars += 1;
                AddIfNotInList(car->collision_info, used_cars, count_used_cars);
            }
        }
        if (C2V(gNet_mode) == eNet_mode_host) {
            for (i = 0; i < GetCarCount(eVehicle_net_player); i++) {
                tCar_spec* car;

                car = GetCarSpec(eVehicle_net_player, i);
                if (!car->disabled) {
                    car->active = 1;
                    C2V(gActive_car_list)[new_count_active_cars] = car;
                    new_count_active_cars += 1;

                    AddIfNotInList(car->collision_info, used_cars, count_used_cars);
                } else {
                    car->active = 0;
                }
            }
        } else if (C2V(gNet_mode) == eNet_mode_client) {
            for (i = 0; i < GetCarCount(eVehicle_net_player); i++) {
                tCar_spec* car;

                car = GetCarSpec(eVehicle_net_player, i);
                if (!car->disabled && IsNetCarActive(&car->car_master_actor->t.t.translate.t)) {
                    car->active = 1;
                    C2V(gActive_car_list)[new_count_active_cars] = car;
                    new_count_active_cars += 1;
                    AddIfNotInList(car->collision_info, used_cars, count_used_cars);
                } else {
                    car->active = 0;
                }
            }
        }

        for (i = 0; i < C2V(gProgram_state).AI_vehicles.number_of_opponents; i++) {
            tCar_spec* car;

            car = GetCarSpec(eVehicle_opponent, i);
            if (C2V(gProgram_state).AI_vehicles.opponents[i].physics_me || C2V(gAction_replay_mode)) {
                car->active = 1;
                C2V(gActive_car_list)[new_count_active_cars] = car;
                new_count_active_cars += 1;
                AddIfNotInList(car->collision_info,used_cars, count_used_cars);
            } else {
                car->active = 0;
            }
        }

        for (i = 0; i < C2V(gNumber_of_cops_before_faffage); i++) {
            tCar_spec* car;

            car = GetCarSpec(eVehicle_rozzer, i);
            if (C2V(gProgram_state).AI_vehicles.cops[i].physics_me || C2V(gAction_replay_mode)) {
                car->active = 1;
                C2V(gActive_car_list)[new_count_active_cars] = car;
                new_count_active_cars += 1;
                AddIfNotInList(car->collision_info, used_cars, count_used_cars);
            } else {
                car->active = 0;
            }
        }
        RemoveAnythingStillInList(used_cars, count_used_cars);
    }
    if (C2V(gNum_active_cars) != new_count_active_cars) {
        C2V(gNum_active_cars) = new_count_active_cars;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004a7a80, RebuildActiveCarList, RebuildActiveCarList_original)

void C2_HOOK_FASTCALL ForceRebuildActiveCarList(void) {

    C2V(gActive_car_list_rebuild_required) = 1;
    if (C2V(gProgram_state).racing) {
        RebuildActiveCarList();
    }
}
C2_HOOK_FUNCTION(0x004a7a60, ForceRebuildActiveCarList)

void C2_HOOK_FASTCALL DisposeOpponents(void) {
    int i;

    C2_HOOK_BUG_ON(offsetof(tIntelligent_vehicles, cops) != 12856);
    for (i = 0; i < C2V(gProgram_state).AI_vehicles.number_of_cops; i++) {
        DisposeCar(C2V(gProgram_state).AI_vehicles.cops[i].car_spec, (i == C2V(gBIG_APC_index)) ? 99 : 98);
        BrMemFree(C2V(gProgram_state).AI_vehicles.cops[i].car_spec);
    }
}
C2_HOOK_FUNCTION(0x004ae5d0, DisposeOpponents)

tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex) {

    switch (pCategory) {
    case eVehicle_self:
        return &C2V(gProgram_state).current_car;
    case eVehicle_net_player:
        if (pIndex < C2V(gThis_net_player_index)) {
            return C2V(gNet_players)[pIndex].car;
        } else {
            return C2V(gNet_players)[pIndex + 1].car;
        }
    case eVehicle_opponent:
        return C2V(gProgram_state).AI_vehicles.opponents[pIndex].car_spec;
    case eVehicle_rozzer:
        return C2V(gProgram_state).AI_vehicles.cops[pIndex].car_spec;
    case eVehicle_drone:
        PDEnterDebugger("OPPONENT.C: GetCarSpec() can't return drone car_specs");
        return NULL;
    case eVehicle_not_really:
        return (tCar_spec*)C2V(gActive_non_car_list)[pIndex];
    default:
        return NULL;
    }
}
C2_HOOK_FUNCTION(0x004ae7e0, GetCarSpec)

const char* C2_HOOK_FASTCALL GetDriverName(tVehicle_type pCategory, int pIndex) {
    tCar_spec* c;
    switch (pCategory) {
    case eVehicle_self:
        return C2V(gProgram_state).player_name;
    case eVehicle_opponent:
        c = C2V(gProgram_state).AI_vehicles.opponents[pIndex].car_spec;
        if (c != NULL) {
            return c->driver_name;
        }
        return NULL;
    case eVehicle_rozzer:
        return "Faceless Cop";
    case eVehicle_drone:
        return "Innocent Civilian";
    case eVehicle_not_really:
    default:
        return NULL;
    }
}
C2_HOOK_FUNCTION(0x004ae880, GetDriverName)

int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory) {

    switch (pCategory) {
    case eVehicle_self:
        return 1;
    case eVehicle_net_player:
        if (C2V(gNet_mode) != eNet_mode_none) {
            return C2V(gNumber_of_net_players) - 1;
        } else {
            return 0;
        }
        break;
    case eVehicle_opponent:
        return C2V(gProgram_state).AI_vehicles.number_of_opponents;
    case eVehicle_rozzer:
        return C2V(gNumber_of_cops_before_faffage);
    case eVehicle_drone:
        return 0;
    case eVehicle_not_really:
        return C2V(gNum_active_non_cars);
    default:
        return 0;
    }
}
C2_HOOK_FUNCTION(0x004ae790, GetCarCount)

void C2_HOOK_FASTCALL StunTheBugger(tOpponent_spec* pOpponent_spec, int pMilliseconds) {

    pOpponent_spec->car_spec->acc_force = 0.f;
    pOpponent_spec->car_spec->brake_force = 0.f;
    pOpponent_spec->car_spec->curvature = 0.f;
    pOpponent_spec->stun_time_ends = MAX(C2V(gTime_stamp_for_this_munging) + pMilliseconds, pOpponent_spec->stun_time_ends);
}

tCar_spec* C2_HOOK_FASTCALL GetCarSpecFromGlobalOppoIndex(int pIndex) {
    int i;

    for (i = 0; i < C2V(gProgram_state).AI_vehicles.number_of_opponents; i++) {
        if (C2V(gProgram_state).AI_vehicles.opponents[i].index == pIndex) {
            return C2V(gProgram_state).AI_vehicles.opponents[i].car_spec;
        }
    }
    return NULL;
}
C2_HOOK_FUNCTION(0x004ae970, GetCarSpecFromGlobalOppoIndex)

tOpponent_spec* C2_HOOK_FASTCALL GetOpponentSpecFromCarSpec(tCar_spec* pCar_spec) {
    int i;

    if ((pCar_spec->car_ID & 0xff00) == 0x200) {
        for (i = 0; i < GetCarCount(eVehicle_opponent); i++) {
            if (C2V(gProgram_state).AI_vehicles.opponents[i].car_spec == pCar_spec) {
                return &C2V(gProgram_state).AI_vehicles.opponents[i];
            }
        }
    } else if ((pCar_spec->car_ID & 0xff00) == 0x300) {
        for (i = 0; i < GetCarCount(eVehicle_rozzer); i++) {
            if (C2V(gProgram_state).AI_vehicles.cops[i].car_spec == pCar_spec) {
                return &C2V(gProgram_state).AI_vehicles.cops[i];
            }
        }
    }
    return NULL;
}
C2_HOOK_FUNCTION(0x004ae8e0, GetOpponentSpecFromCarSpec)

void C2_HOOK_CDECL DoNotDprintf_opponent(const char* pMessage, ...) {
}
C2_HOOK_FUNCTION(0x004a8160, DoNotDprintf_opponent)

void (C2_HOOK_FASTCALL * InitOpponents_original)(tRace_info* pRace_info);
void C2_HOOK_FASTCALL InitOpponents(tRace_info* pRace_info) {
    int i;
    int opponent_number;
    int rank_dependent_difficulty;
    br_bounds bounds;
    tCar_spec* car_spec;
    tOpponent_spec* opponent_spec;
    int opponents_left;

    C2V(gNext_grudge_reduction) = C2V(gTime_stamp_for_this_munging) + 10000;
    C2V(gFirst_frame) = 1;
    C2V(gNum_viewable_cars) = 1;
    C2V(gGrudge_reduction_per_period) = 3 - C2V(gProgram_state).skill_level;
    C2V(gAcknowledged_start) = 0;
    C2V(gStart_jumped) = 0;
    C2V(gViewable_car_list)[0] = &C2V(gProgram_state).current_car;
    BrActorToBounds(&bounds, C2V(gProgram_state).track_spec.the_actor);
    C2V(gMinimum_yness_before_knackerisation) = bounds.min.v[1] - 2.f;
    C2V(gAcme_frame_count) = 0;
    C2V(gProgram_state).current_car.no_of_processes_recording_my_trail = 0;
    C2V(gProgram_state).current_car.field_0x1540 = 0;
    C2V(gProgram_state).current_car.field_0x155c = -1;
    C2V(gHead_on_cos_value) = cosf(.5235668420791626f); /* around 30 degrees */
    rank_dependent_difficulty = (int)((101.f - 99.f * (1.f - (float)C2V(gCurrent_race).index / (float)C2V(gNumber_of_races))) / -10.f);
    C2V(gBig_bang) = (int)(70.f - (10 * C2V(gProgram_state).skill_level - 3 * rank_dependent_difficulty) * C2V(gOpponent_nastyness_frigger));
    C2V(gIn_view_distance) = C2V(gCamera_yon) + 10.f;
    if (C2V(gIn_view_distance) < 45.f) {
        C2V(gIn_view_distance) = 45.f;
    }
    if (C2V(gNet_mode) == eNet_mode_none) {
        C2V(gProgram_state).AI_vehicles.number_of_opponents = pRace_info->number_of_racers - 1;
    } else {
        C2V(gProgram_state).AI_vehicles.number_of_opponents = 0;
    }
    for (i = 0, opponent_number = 0; i < C2V(gProgram_state).AI_vehicles.number_of_opponents; i++, opponent_number++) {
        PossibleService();
        if (pRace_info->opponent_list[opponent_number].index < 0) {
            opponent_number++;
        }
        C2V(gProgram_state).AI_vehicles.opponents[i].car_spec = pRace_info->opponent_list[opponent_number].car_spec;
        C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->car_ID = i | 0x200;
        DoNotDprintf_opponent("Car '%s', car_ID %x",
            C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->driver_name,
            C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->car_ID);
        C2V(gProgram_state).AI_vehicles.opponents[i].index = pRace_info->opponent_list[opponent_number].index;
        C2V(gProgram_state).AI_vehicles.opponents[i].time_last_processed = C2V(gTime_stamp_for_this_munging);
        C2V(gProgram_state).AI_vehicles.opponents[i].next_repair_check = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].repair_interval = 60000 * IRandomBetween(C2V(gMinTimeOpponentRepair), C2V(gMaxTimeOpponentRepair));
        C2V(gProgram_state).AI_vehicles.opponents[i].last_repair_time = C2V(gTime_stamp_for_this_munging);
        DoNotDprintf_opponent("*** Opponent %s, repair_interval is %d",
            C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->driver_name,
            C2V(gProgram_state).AI_vehicles.opponents[i].repair_interval);
        C2V(gProgram_state).AI_vehicles.opponents[i].time_this_objective_started = C2V(gTime_stamp_for_this_munging);
        C2V(gProgram_state).AI_vehicles.opponents[i].last_moved_ok = C2V(gTime_stamp_for_this_munging);
        C2V(gProgram_state).AI_vehicles.opponents[i].last_in_view = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].stun_time_ends = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].next_player_visibility_check = C2V(gTime_stamp_for_this_munging) + IRandomBetween(0, 900) + 2000;
        C2V(gProgram_state).AI_vehicles.opponents[i].next_out_of_world_check = C2V(gTime_stamp_for_this_munging) + 500;

        /*
         * C2V(gProgram_state).AI_vehicles.opponents[i].flags &= ~0x36;
         * C2V(gProgram_state).AI_vehicles.opponents[i].flags |= 0x8;
         */
        C2V(gProgram_state).AI_vehicles.opponents[i].finished_for_this_race = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].physics_me = 1;
        C2V(gProgram_state).AI_vehicles.opponents[i].pursue_from_start = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].cheating = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].knackeredness_detected = 0;

        C2V(gProgram_state).AI_vehicles.opponents[i].players_section_when_last_calced_full_path = -1;
        C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->last_person_to_hit_us = NULL;
        C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->last_person_we_hit = NULL;
        C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->field_0x1544 = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->field_0x1548 = C2V(gTime_stamp_for_this_munging);
        C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->grudge_raised_recently = 1;
        C2V(gProgram_state).AI_vehicles.opponents[i].car_spec->no_of_processes_recording_my_trail = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].nnext_sections = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].new_objective_required = 1;
        C2V(gProgram_state).AI_vehicles.opponents[i].current_objective = eOOT_none;

        /* C2V(gProgram_state).AI_vehicles.opponents[i].flags &= ~0x700; */
        C2V(gProgram_state).AI_vehicles.opponents[i].has_moved_at_some_point = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].player_in_view_now = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].acknowledged_piv = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].field_0xb4 = 0;
        C2V(gProgram_state).AI_vehicles.opponents[i].nastiness = (
                  C2V(gCurrent_race).race_spec->opponent_nastiness_level
                + C2V(gProgram_state).skill_level / 2.f
                + (C2V(gOpponents)[C2V(gProgram_state).AI_vehicles.opponents[i].index].strength_rating - 1) / 4.f
                ) / 3.f * C2V(gOpponent_nastyness_frigger);

        BrVector3Set(&C2V(gProgram_state).AI_vehicles.opponents[i].pos_last_frame, 0.f, 0.f, 0.f);
        C2V(gOpponents)[C2V(gProgram_state).AI_vehicles.opponents[i].index].psyche.grudge_against_player = 10;
        C2V(gViewable_car_list)[C2V(gNum_viewable_cars)] = C2V(gProgram_state).AI_vehicles.opponents[i].car_spec;
        C2V(gNum_viewable_cars)++;
        StunTheBugger(&C2V(gProgram_state).AI_vehicles.opponents[i], 0xfffffff);
    }
    if (C2V(gChallenger_index__opponent) >= 0) {
        car_spec = GetCarSpecFromGlobalOppoIndex(C2V(gChallenger_index__opponent));
        opponent_spec = GetOpponentSpecFromCarSpec(car_spec);
        if (opponent_spec == NULL) {
            DoNotDprintf_opponent("ERROR - can't record dare - no opponent_spec for car_spec");
        } else {
            opponent_spec->pursue_from_start = 1;
        }
        C2V(gChallenger_index__opponent) = -1;
    }
    /* cops are not initialized in rec2 */

    opponents_left = NumberOfOpponentsStillRunning();
    if (opponents_left != 0) {
        if (C2V(gCurrent_race).race_spec->opponent_nastiness_level < 10.f) {
            int count_type_2;
            int count_type_1;

            for (i = 0; i < GetCarCount(eVehicle_opponent); i++) {
                C2V(gProgram_state).AI_vehicles.opponents[i].field_0xb4 = 0;
            }

            count_type_2 = (int)(0.6f * opponents_left);
            for (i = 0; i < count_type_2; i++) {
                int rstart;
                int r;

                rstart = IRandomBetween(0, opponents_left - 1 - i);
                for (r = rstart;;) {
                    if (C2V(gProgram_state).AI_vehicles.opponents[r].field_0xb4 == 0) {
                        C2V(gProgram_state).AI_vehicles.opponents[r].field_0xb4 = 2;
                        break;
                    }
                    r++;
                    if (r >= opponents_left) {
                        r = 0;
                    }
                    if (r == rstart) {
                        break;
                    }
                }
            }
            count_type_1 = (int)(0.2f * opponents_left);
            for (i = 0; i < count_type_1; i++) {
                int rstart;
                int r;

                rstart = IRandomBetween(0, opponents_left - count_type_2 - 1 - i);
                for (r = rstart;;) {
                    if (C2V(gProgram_state).AI_vehicles.opponents[r].field_0xb4 == 0) {
                        C2V(gProgram_state).AI_vehicles.opponents[r].field_0xb4 = 1;
                        break;
                    }
                    r++;
                    if (r >= opponents_left) {
                        r = 0;
                    }
                    if (r == rstart) {
                        break;
                    }
                }

            }
        } else {
            for (i = 0; i < GetCarCount(eVehicle_opponent); i++) {
                C2V(gProgram_state).AI_vehicles.opponents[i].field_0xb4 = 3;
            }
        }
    }

    C2V(gActive_car_list_rebuild_required) = 1;
    RebuildActiveCarList();
}
C2_HOOK_FUNCTION_ORIGINAL(0x004add50, InitOpponents, InitOpponents_original)

void C2_HOOK_FASTCALL CalcNegativeXVector(br_vector3* pNegative_x_vector, br_vector3* pStart, br_vector3* pFinish, br_scalar pLength) {
    br_vector3 positive_y_vector;
    br_vector3 path_vector;

    BrVector3Sub(&path_vector, pFinish, pStart);
    BrVector3Set(&positive_y_vector, 0.f, 1.f, 0.f);
    BrVector3Cross(pNegative_x_vector, &path_vector, &positive_y_vector);
    BrVector3Normalise(pNegative_x_vector, pNegative_x_vector);
    BrVector3Scale(pNegative_x_vector, pNegative_x_vector, pLength);
}
C2_HOOK_FUNCTION(0x00401fe0, CalcNegativeXVector)

void C2_HOOK_FASTCALL CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck(tOpponent_spec* pOpponent_spec) {

    BrVector3Sub(&pOpponent_spec->player_to_oppo_v, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &C2V(gProgram_state).current_car.car_master_actor->t.t.translate.t);
    pOpponent_spec->player_to_oppo_d = BrVector3Length(&pOpponent_spec->player_to_oppo_v);
    if (pOpponent_spec->player_to_oppo_d < C2V(gIn_view_distance)) {
        pOpponent_spec->last_in_view = C2V(gTime_stamp_for_this_munging);
    }
}
C2_HOOK_FUNCTION(0x004a9550, CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck)

void C2_HOOK_FASTCALL TurnOpponentPhysicsOn(tOpponent_spec* pOpponent_spec) {
    pOpponent_spec->car_spec->car_master_actor->render_style = BR_RSTYLE_DEFAULT;
    PipeSingleOppoRenderage(pOpponent_spec, 1);
    if (!pOpponent_spec->physics_me) {
        pOpponent_spec->physics_me = 1;
        C2V(gActive_car_list_rebuild_required) = 1;
    }
}
C2_HOOK_FUNCTION(0x004a7960, TurnOpponentPhysicsOn)

void C2_HOOK_FASTCALL TurnOpponentPhysicsOff(tOpponent_spec* pOpponent_spec) {
    tCar_spec* car = pOpponent_spec->car_spec;

    car->acc_force = 0.f;
    car->brake_force = 0.f;
    car->curvature = 0.f;
    car->gear = 0.f;
    car->revs = 0.f;
    BrVector3Set(&car->collision_info->omega, 0.f, 0.f, 0.f);
    BrVector3Set(&car->collision_info->v, 0.f, 0.f, 0.f);
    if (!pOpponent_spec->physics_me) {
        pOpponent_spec->physics_me = 0;
        pOpponent_spec->car_spec->car_master_actor->render_style = BR_RSTYLE_NONE;
        PipeSingleOppoRenderage(pOpponent_spec, 0);
        C2V(gActive_car_list_rebuild_required) = 1;
    }
}
C2_HOOK_FUNCTION(0x004a79a0, TurnOpponentPhysicsOff)

int C2_HOOK_FASTCALL  TimeToStopStruggling(tOpponent_spec* pOpponent_spec) {

    return (float)(750 * (pOpponent_spec->follow_path.number_of_struggles - 1))
        + 30.f * pOpponent_spec->car_spec->collision_info->M
        + (float)(pOpponent_spec->follow_path.struggle_time + 2750) < C2V(gTime_stamp_for_this_munging);
}
C2_HOOK_FUNCTION(0x004af8d0, TimeToStopStruggling)

void C2_HOOK_FASTCALL DisplayOpponentRecoveringHeadup(tOpponent_spec* pOpponent_spec) {

    // empty
}

void C2_HOOK_FASTCALL OiStopCheating(tOpponent_spec* pOpponent_spec) {

    DoNotDprintf_opponent("%s: OiStopCheating() - End of cheating sesh", pOpponent_spec->car_spec->driver_name);
    pOpponent_spec->cheating = 0;
    if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
        DoNotDprintf_opponent ("%s: OiStopCheating() - Turning physics ON", pOpponent_spec->car_spec->driver_name);
        TurnOpponentPhysicsOn(pOpponent_spec);
        RebuildActiveCarList();
    }
}

void C2_HOOK_FASTCALL StartToCheat(tOpponent_spec* pOpponent_spec) {

    if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
        DoNotDprintf_opponent("%s: StartToCheat() - Turning physics OFF", pOpponent_spec->car_spec->driver_name);
        TurnOpponentPhysicsOff(pOpponent_spec);
        RebuildActiveCarList();
    } else {
        DoNotDprintf_opponent("%s: StartToCheat() - Starting to cheat", pOpponent_spec->car_spec->driver_name);
        pOpponent_spec->cheating = 1;
        switch (pOpponent_spec->current_objective) {
        case eOOT_levitate:
        case eOOT_knackered_and_freewheeling:
        case eOOT_none:
            break;
        default:
            if (!pOpponent_spec->car_spec->knackered
                    && (pOpponent_spec->car_spec->collision_info->last_special_volume != NULL && pOpponent_spec->car_spec->collision_info->last_special_volume->gravity_multiplier < 1.f)
                        || !pOpponent_spec->follow_path.has_moved_during_this_task
                        || (pOpponent_spec->follow_path.struggle_time != 0 && !TimeToStopStruggling(pOpponent_spec))) {

                DisplayOpponentRecoveringHeadup(pOpponent_spec);
            }
            break;
        }

        switch (pOpponent_spec->current_objective) {
        case eOOT_complete_race:
        case eOOT_pursue_and_twat:
        case eOOT_run_away:
        case eOOT_get_near_player:
        case eOOT_return_to_start:
            if (pOpponent_spec->follow_path.section_no < 15000
                    || pOpponent_spec->follow_path.section_no >= 20000) {
                int section = pOpponent_spec->follow_path.section_no - 20000;
                if (section >= 0 && section < pOpponent_spec->nnext_sections - 1) {
                    br_vector3 direction_v;
                    br_vector3 intersect;
                    br_scalar distance;

                    tS16 nearest_section = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction_v, &intersect, &distance);
                    tS16 real_section = GetOpponentsRealSection(pOpponent_spec, pOpponent_spec->follow_path.section_no);

                    if (nearest_section != real_section) {
                        int i;

                        for (i = section; i < pOpponent_spec->nnext_sections; i++) {
                            if (pOpponent_spec->next_sections[i].section_no == nearest_section) {
                                break;
                            }
                        }
                        if (i < pOpponent_spec->nnext_sections) {
                            ShiftOpponentsProjectedRoute(pOpponent_spec, i);
                            pOpponent_spec->follow_path.section_no = 20000;
                        } else {
                            br_vector3 dir;

                            pOpponent_spec->follow_path.section_no = 20000;
                            pOpponent_spec->nnext_sections = 1;
                            pOpponent_spec->next_sections[0].section_no = nearest_section;
                            BrVector3Sub(&dir, &intersect, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
                            pOpponent_spec->next_sections[0].direction = BrVector3Dot(&dir, (br_vector3*)pOpponent_spec->car_spec->car_master_actor->t.t.mat.m[2]) >= 0;
                        }
                    }
                }
            }
        }
    }
}

void (C2_HOOK_FASTCALL * ProcessThisOpponent_original)(tOpponent_spec* pOpponent_spec);
void C2_HOOK_FASTCALL ProcessThisOpponent(tOpponent_spec* pOpponent_spec) {

#if 0//defined(C2_HOOKS_ENABLED)
    ProcessThisOpponent_original(pOpponent_spec);
#else

    if ((C2V(gMap_view) == 2 && C2V(gShow_opponents)) || C2V(gTime_stamp_for_this_munging) <= pOpponent_spec->last_in_view + 3000) {
        if (pOpponent_spec->cheating) {
            OiStopCheating(pOpponent_spec);
        }
    } else {
        if (!pOpponent_spec->cheating) {
            StartToCheat(pOpponent_spec);
        }
    }
    ChooseNewObjective(pOpponent_spec, pOpponent_spec->new_objective_required);
    pOpponent_spec->new_objective_required = 0;
    if (C2V(gCountdown) != 0 || C2V(gRace_finished)) {
        pOpponent_spec->car_spec->brake_force = 10.f * pOpponent_spec->car_spec->collision_info->M;
    }
    if (!pOpponent_spec->finished_for_this_race && !C2V(gStop_opponents_moving) && !C2V(gRace_finished) && pOpponent_spec->stun_time_ends < C2V(gTime_stamp_for_this_munging)) {
        ProcessCurrentObjective(pOpponent_spec, ePOC_run);
    }
    if (pOpponent_spec->cheating) {
        BrVector3Copy(&pOpponent_spec->car_spec->pos, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004aa570, ProcessThisOpponent, ProcessThisOpponent_original)

void C2_HOOK_FASTCALL StartRecordingTrail(tCar_spec* pPursuee) {

    if (pPursuee->no_of_processes_recording_my_trail == 0) {
        DoNotDprintf_opponent("StartRecordingTrail - starting from scratch");
        pPursuee->no_of_processes_recording_my_trail = 1;
        pPursuee->my_trail.nodes_shifted_this_frame = 0;
        pPursuee->my_trail.has_deviated_recently = 0;
        pPursuee->my_trail.number_of_nodes = 2;
        pPursuee->my_trail.time_of_next_recording = C2V(gTime_stamp_for_this_munging) + 500;
        BrVector3Copy(&pPursuee->my_trail.base_heading, &pPursuee->direction);
        BrVector3Copy(&pPursuee->my_trail.trail_nodes[0], &pPursuee->car_master_actor->t.t.translate.t);
        BrVector3Copy(&pPursuee->my_trail.trail_nodes[1], &pPursuee->car_master_actor->t.t.translate.t);
        pPursuee->my_trail.trail_nodes[0].v[2] += 0.2f;
    } else {
        DoNotDprintf_opponent("StartRecordingTrail - another pursuer attaching");
        pPursuee->no_of_processes_recording_my_trail += 1;
    }
}
C2_HOOK_FUNCTION(0x004aa490, StartRecordingTrail)

void C2_HOOK_FASTCALL RecordNextTrailNode(tCar_spec* pPursuee) {
    tPursuee_trail* trail;
    br_scalar length;
    br_vector3 car_to_last_point_v;
    br_vector3 offset_v;
    br_vector3 start1;
    br_vector3 finish1;
    br_vector3 start2;
    br_vector3 finish2;
    int visible;

    trail = &pPursuee->my_trail;
    trail->nodes_shifted_this_frame = 0;
    if (trail->time_of_next_recording >= C2V(gTime_stamp_for_this_munging)) {
        return;
    }
    trail->time_of_next_recording = C2V(gTime_stamp_for_this_munging) + 500;
    if (BrVector3Dot(&pPursuee->direction, &trail->base_heading) < FastScalarCos(30)) {
        trail->has_deviated_recently = 1;
    }
    BrVector3Sub(&car_to_last_point_v, &trail->trail_nodes[trail->number_of_nodes - 2], &pPursuee->car_master_actor->t.t.translate.t);
    length = BrVector3Length(&car_to_last_point_v);
    if (length < .3f) {
        return;
    }
    CalcNegativeXVector(&offset_v, &trail->trail_nodes[trail->number_of_nodes - 2], &pPursuee->car_master_actor->t.t.translate.t, .5f);

    BrVector3Add(&start1, &trail->trail_nodes[trail->number_of_nodes - 2], &offset_v);
    BrVector3Add(&finish1, &pPursuee->car_master_actor->t.t.translate.t, &offset_v);
    BrVector3Sub(&start2, &trail->trail_nodes[trail->number_of_nodes - 2], &offset_v);
    BrVector3Sub(&finish2, &pPursuee->car_master_actor->t.t.translate.t, &offset_v);
    visible = 1;
    if (!pPursuee->my_trail.has_deviated_recently) {
        visible = PointVisibleFromHere(&start1, &finish1);
        if (!visible) {
            visible = PointVisibleFromHere(&start2, &finish2);
        }
        if (!visible) {
            visible = PointVisibleFromHere(&trail->trail_nodes[trail->number_of_nodes - 2], &pPursuee->car_master_actor->t.t.translate.t);
        }
    }

    C2_HOOK_BUG_ON(sizeof(pPursuee->my_trail.trail_nodes) != 0x12c);

    if (pPursuee->my_trail.has_deviated_recently || !visible) {
        if ((visible && length > 2.0f) || (!visible && length > 1.5f)) {
            if (pPursuee->my_trail.number_of_nodes < REC2_ASIZE(pPursuee->my_trail.trail_nodes)) {
                pPursuee->my_trail.number_of_nodes += 1;
            } else {
                c2_memmove(&pPursuee->my_trail.trail_nodes[0], &pPursuee->my_trail.trail_nodes[1], sizeof(pPursuee->my_trail.trail_nodes[0]));
                pPursuee->my_trail.nodes_shifted_this_frame = 1;
            }
            pPursuee->my_trail.has_deviated_recently = 0;
            BrVector3Copy(&trail->base_heading, &pPursuee->direction);
        }
    }
    BrVector3Copy(&trail->trail_nodes[trail->number_of_nodes - 1], &pPursuee->car_master_actor->t.t.translate.t);
}

void C2_HOOK_FASTCALL ClearTwattageOccurrenceVariables(tOpponent_spec* pOpponent_spec) {

    pOpponent_spec->car_spec->big_bang = 0;
    pOpponent_spec->car_spec->scary_bang = 0;
    pOpponent_spec->car_spec->grudge_raised_recently = 0;
    pOpponent_spec->car_spec->last_person_to_hit_us = NULL;
    pOpponent_spec->car_spec->last_person_we_hit = NULL;
}

void (C2_HOOK_FASTCALL * MungeOpponents_original)(void);
void C2_HOOK_FASTCALL MungeOpponents(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    MungeOpponents_original();
#else
    int i;
    int un_stun_flag;

    un_stun_flag = 0;
    if (C2V(gProgram_state).AI_vehicles.number_of_opponents == 0 && C2V(gNumber_of_cops_before_faffage) == 0) {
        return;
    }
    if (C2V(gProgram_state).AI_vehicles.number_of_path_nodes == 0) {
        return;
    }
    if (C2V(gProgram_state).AI_vehicles.number_of_path_sections == 0) {
        return;
    }
    C2V(gAcme_frame_count) += 1;
    if (!C2V(gAcknowledged_start) && C2V(gCountdown) == 0) {
        C2V(gAcknowledged_start) = 1;
        if (!C2V(gStart_jumped)) {
            un_stun_flag = 1;
        }
    }
    if (C2V(gProgram_state).current_car.no_of_processes_recording_my_trail == 0) {
        StartRecordingTrail(&C2V(gProgram_state).current_car);
    } else {
        RecordNextTrailNode(&C2V(gProgram_state).current_car);
    }
    C2V(gNum_of_opponents_pursuing) = 0;
    C2V(gNum_of_opponents_getting_near) = 0;
    C2V(gNum_of_opponents_completing_race) = 0;
    for (i = 0; i < C2V(gProgram_state).AI_vehicles.number_of_opponents; i++) {
        tOpponent_spec* oppo = &C2V(gProgram_state).AI_vehicles.opponents[i];

        if (!oppo->finished_for_this_race) {
            switch (oppo->current_objective) {
            case eOOT_pursue_and_twat:
                C2V(gNum_of_opponents_pursuing) += 1;
                break;
            case eOOT_get_near_player:
                C2V(gNum_of_opponents_getting_near) += 1;
                break;
            case eOOT_complete_race:
                C2V(gNum_of_opponents_completing_race) += 1;
                break;
            }
        }
    }
    for (i = 0; i < C2V(gProgram_state).AI_vehicles.number_of_opponents; i++) {
        tOpponent_spec* oppo = &C2V(gProgram_state).AI_vehicles.opponents[i];

        if (!oppo->finished_for_this_race) {
            if (un_stun_flag) {
                UnStunTheBugger(oppo);
            }
            CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck(oppo);
            CalcPlayerConspicuousness(oppo);
            ProcessThisOpponent(oppo);
            ClearTwattageOccurrenceVariables(oppo);
        }
    }
    for (i = 0; i < C2V(gNumber_of_cops_before_faffage); i++) {
        tOpponent_spec* oppo = &C2V(gProgram_state).AI_vehicles.cops[i];

        if (!oppo->finished_for_this_race) {
            if (un_stun_flag) {
                UnStunTheBugger(oppo);
            }
            CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck(oppo);
            CalcPlayerConspicuousness(oppo);
            ProcessThisOpponent(oppo);
            ClearTwattageOccurrenceVariables(oppo);
            oppo->murder_reported = 0;
        }
    }
    if (C2V(gNext_grudge_reduction) < C2V(gTime_stamp_for_this_munging)) {
        C2V(gNext_grudge_reduction) = C2V(gTime_stamp_for_this_munging) + 5000;

        for (i = 0; i < C2V(gProgram_state).AI_vehicles.number_of_opponents; i++) {
            tOpponent_spec* oppo = &C2V(gProgram_state).AI_vehicles.opponents[i];

            if (!oppo->finished_for_this_race) {
                if (C2V(gOpponents)[i].psyche.grudge_against_player > C2V(gGrudge_reduction_per_period)) {
                    C2V(gOpponents)[i].psyche.grudge_against_player -= C2V(gGrudge_reduction_per_period);
                }
            }
        }
    }
    RebuildActiveCarList();
    C2V(gFirst_frame) = 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004a9e10, MungeOpponents, MungeOpponents_original)

void C2_HOOK_FASTCALL UnStunTheBugger(tOpponent_spec* pOpponent_spec) {

    pOpponent_spec->stun_time_ends = 0;
}

void C2_HOOK_FASTCALL WakeUpOpponentsToTheFactThatTheStartHasBeenJumped(int pWhat_the_countdown_was) {
    int i;

    for (i = 0; i < C2V(gProgram_state).AI_vehicles.number_of_opponents; i++) {
        UnStunTheBugger(&C2V(gProgram_state).AI_vehicles.opponents[i]);
        if (IRandomBetween(1000, 2500) < 1000 * pWhat_the_countdown_was) {
            StunTheBugger(&C2V(gProgram_state).AI_vehicles.opponents[i], IRandomBetween(1000, 2500));
        } else {
            StunTheBugger(&C2V(gProgram_state).AI_vehicles.opponents[i], 1000 * pWhat_the_countdown_was);
        }
    }
    for (i = 0; i < C2V(gProgram_state).AI_vehicles.number_of_cops; i++) {
        UnStunTheBugger(&C2V(gProgram_state).AI_vehicles.cops[i]);
        if (IRandomBetween(1000, 2500) < 1000 * pWhat_the_countdown_was) {
            StunTheBugger(&C2V(gProgram_state).AI_vehicles.cops[i], IRandomBetween(1000, 2500));
        } else {
            StunTheBugger(&C2V(gProgram_state).AI_vehicles.cops[i], 1000 * pWhat_the_countdown_was);
        }
    }
    C2V(gAcknowledged_start) = 1;
    C2V(gStart_jumped) = 1;
}
C2_HOOK_FUNCTION(0x004ae620, WakeUpOpponentsToTheFactThatTheStartHasBeenJumped)

tS16 C2_HOOK_FASTCALL FindNearestPathSection(br_vector3* pActor_coords, br_vector3* pPath_direction, br_vector3* pIntersect, br_scalar* pDistance) {

    return FindNearestGeneralSection(NULL, pActor_coords, pPath_direction, pIntersect, pDistance);
}

void C2_HOOK_FASTCALL WeightedFindNearestNodeAndSection(tCar_spec* pCar, br_vector3* pActor_coords, tS16* pNearest_node_section_no, tS16* pNearest_node, br_scalar* pT, float pWeight) {
    int section_no;
    int no_sections;
    tS16 nearest_section;
    br_scalar nearest_node_distance_squared;
    br_scalar closest_distance_squared;
    br_scalar the_distance_squared;
    br_scalar t;
    br_scalar length_squared_a;
    br_vector3 a;
    br_vector3 p;

    *pNearest_node_section_no = -1;
    *pNearest_node = -1;
    nearest_node_distance_squared = BR_SCALAR_MAX;
    closest_distance_squared = BR_SCALAR_MAX;
    if (pCar != NULL) {
        no_sections = pCar->my_trail.number_of_nodes - 1;
    } else {
        no_sections = C2V(gProgram_state).AI_vehicles.number_of_path_sections;
    }
    for (section_no = 0; section_no < no_sections; section_no++) {
        br_vector3* start;
        br_vector3* finish;

        if (pCar != NULL) {
            start = &pCar->my_trail.trail_nodes[section_no + 0];
            finish = &pCar->my_trail.trail_nodes[section_no + 1];
        } else {
            start = &C2V(gProgram_state).AI_vehicles.path_nodes[C2V(gProgram_state).AI_vehicles.path_sections[section_no].node_indices[0]].pos;
            finish = &C2V(gProgram_state).AI_vehicles.path_nodes[C2V(gProgram_state).AI_vehicles.path_sections[section_no].node_indices[1]].pos;
        }
        BrVector3Sub(&a, finish, start);
        a.v[1] *= pWeight;
        BrVector3Sub(&p, pActor_coords, start);
        p.v[1] *= pWeight;
        the_distance_squared = Vector3DistanceSquared(&p, &a);
        if (the_distance_squared < closest_distance_squared) {
            if (pCar != NULL) {
                *pNearest_node = section_no;
            } else {
                *pNearest_node = C2V(gProgram_state).AI_vehicles.path_sections[section_no].node_indices[1];
            }
            nearest_section = section_no + 0;
            closest_distance_squared = the_distance_squared;
        }
        the_distance_squared = BrVector3LengthSquared(&p);
        if (the_distance_squared < closest_distance_squared) {
            if (pCar != NULL) {
                *pNearest_node = section_no + 1;
            } else {
                *pNearest_node = C2V(gProgram_state).AI_vehicles.path_sections[section_no].node_indices[0];
            }
            nearest_section = section_no;
            closest_distance_squared = the_distance_squared;
        }
        length_squared_a = BrVector3LengthSquared(&a);
        if (length_squared_a >= 0.0001f) {
            t = BrVector3Dot(&p, &a) / length_squared_a;
            if (t >= 0 && t <= 1.f) {
                *pNearest_node_section_no = section_no;
                *pT = t;
                nearest_node_distance_squared = the_distance_squared;
            }
        }
    }
    if (nearest_node_distance_squared <= closest_distance_squared) {
        *pNearest_node = -1;
    } else {
        *pNearest_node_section_no = nearest_section;
    }
}

tS16 (C2_HOOK_FASTCALL * FindNearestGeneralSection_original)(tCar_spec* pCar, br_vector3* pActor_coords, br_vector3* pPath_direction, br_vector3* pIntersect, br_scalar* pDistance);
tS16 C2_HOOK_FASTCALL FindNearestGeneralSection(tCar_spec* pCar, br_vector3* pActor_coords, br_vector3* pPath_direction, br_vector3* pIntersect, br_scalar* pDistance) {

#if 0//defined(C2_HOOKS_ENABLED)
    return FindNearestGeneralSection_original(pCar, pActor_coords, pPath_direction, pIntersect, pDistance);
#else
    tS16 nearest_node_section_no;
    tS16 nearest_node;
    float t;
    br_vector3 delta;

    WeightedFindNearestNodeAndSection(pCar, pActor_coords, &nearest_node_section_no, &nearest_node, &t, 2.f);

    if (pCar != NULL) {
        BrVector3Sub(&delta,
                &pCar->my_trail.trail_nodes[nearest_node_section_no + 1],
                &pCar->my_trail.trail_nodes[nearest_node_section_no + 0]);
    } else {
        BrVector3Sub(&delta,
            &C2V(gProgram_state).AI_vehicles.path_nodes[C2V(gProgram_state).AI_vehicles.path_sections[nearest_node_section_no].node_indices[1]].pos,
            &C2V(gProgram_state).AI_vehicles.path_nodes[C2V(gProgram_state).AI_vehicles.path_sections[nearest_node_section_no].node_indices[0]].pos);
    }
    if (nearest_node >= 0) {
        BrVector3Copy(pIntersect, pCar != NULL ? &pCar->my_trail.trail_nodes[nearest_node] : &C2V(gProgram_state).AI_vehicles.path_nodes[nearest_node_section_no].pos);
    } else {
        BrVector3Scale(pIntersect, &delta, t);
        BrVector3Accumulate(pIntersect, &C2V(gProgram_state).AI_vehicles.path_nodes[C2V(gProgram_state).AI_vehicles.path_sections[nearest_node_section_no].node_indices[0]].pos);
    }
    BrVector3InvScale(pPath_direction, &delta, BrVector3Length(&delta));
    *pDistance = Vector3Distance(pActor_coords, pIntersect);
    if (pCar != NULL) {
        nearest_node_section_no += 15000;
    }
    return nearest_node_section_no;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004a82d0, FindNearestGeneralSection, FindNearestGeneralSection_original)

int C2_HOOK_FASTCALL GetOpponentsRealSection(tOpponent_spec* pOpponent_spec, int pSection_no) {

    if (pSection_no >= 20000) {
        return pOpponent_spec->next_sections[pSection_no - 20000].section_no;
    } else if (pSection_no >= 15000) {
        return pSection_no - 15000;
    } else {
        return pSection_no;
    }
}
C2_HOOK_FUNCTION(0x004ae9e0, GetOpponentsRealSection)

int C2_HOOK_FASTCALL ShiftOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, int pPlaces) {
    size_t i;

    if (pOpponent_spec->nnext_sections <= pPlaces) {
        return 0;
    }
    for (i = 0; i < REC2_ASIZE(pOpponent_spec->next_sections) - pPlaces; i++) {
        pOpponent_spec->next_sections[i].section_no = pOpponent_spec->next_sections[pPlaces + i].section_no;
        pOpponent_spec->next_sections[i].direction = pOpponent_spec->next_sections[pPlaces + i].direction;
    }
    pOpponent_spec->nnext_sections -= pPlaces;
    return 1;
}
C2_HOOK_FUNCTION(0x004ab100, ShiftOpponentsProjectedRoute)

int C2_HOOK_FASTCALL AlreadyPursuingCar(tOpponent_spec* pOpponent_spec, tCar_spec* pPursuee) {

    return pOpponent_spec->current_objective == eOOT_pursue_and_twat && pOpponent_spec->pursue_car_data__pursuee == pPursuee;
}

int C2_HOOK_FASTCALL LastTwatterAPlayer(tOpponent_spec* pOpponent_spec) {

    return pOpponent_spec->car_spec->last_person_to_hit_us != NULL && pOpponent_spec->car_spec->last_person_to_hit_us->driver == eDriver_local_human;
}

int C2_HOOK_FASTCALL LastTwatteeAPlayer(tOpponent_spec* pOpponent_spec) {

    return pOpponent_spec->car_spec->last_person_we_hit != NULL && pOpponent_spec->car_spec->last_person_we_hit->driver == eDriver_local_human;
}

int C2_HOOK_FASTCALL HeadOnWithPlayerPossible(tOpponent_spec* pOpponent_spec) {
    br_vector3 oppo_to_player_norm;

    BrVector3Sub(&oppo_to_player_norm,
        &C2V(gProgram_state).current_car.car_master_actor->t.t.translate.t,
        &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
    BrVector3Normalise(&oppo_to_player_norm, &oppo_to_player_norm);
    if (BrVector3Dot(&pOpponent_spec->car_spec->direction, &oppo_to_player_norm) > C2V(gHead_on_cos_value)
            && BrVector3Dot(&C2V(gProgram_state).current_car.direction, &oppo_to_player_norm) < -C2V(gHead_on_cos_value)) {
        DoNotDprintf_opponent("HOORAY! Head-on imminent");
        return 1;
    }
    return 0;
}

void (C2_HOOK_FASTCALL * ChooseNewObjective_original)(tOpponent_spec* pOpponent_spec, int pMust_choose_one);
void C2_HOOK_FASTCALL ChooseNewObjective(tOpponent_spec* pOpponent_spec, int pMust_choose_one) {

#if 0//defined(C2_HOOKS_ENABLED)
    ChooseNewObjective_original(pOpponent_spec, pMust_choose_one);
#else
    char str[256];

    if (pOpponent_spec->current_objective == eOOT_knackered_and_freewheeling || pOpponent_spec->knackeredness_detected) {
        return;
    }
    if (C2V(gTime_stamp_for_this_munging) > pOpponent_spec->next_out_of_world_check) {
        pOpponent_spec->next_out_of_world_check = C2V(gTime_stamp_for_this_munging) + 500;
        if (HasCarFallenOffWorld(pOpponent_spec->car_spec)) {
            DisplayOpponentRecoveringHeadup(pOpponent_spec);
            TeleportOpponentToNearestSafeLocation(pOpponent_spec);
            NewObjective(pOpponent_spec, eOOT_get_near_player);
            return;
        }
    }
    if (pOpponent_spec->car_spec->knackered && !pOpponent_spec->knackeredness_detected) {
        pOpponent_spec->knackeredness_detected = 1;
        DoNotDprintf_opponent("%s: Knackered - dealing with appropriately", pOpponent_spec->car_spec->driver_name);
        if (pOpponent_spec->car_spec->has_been_stolen) {
            NewObjective(pOpponent_spec, eOOT_levitate);
        } else {
            NewObjective(pOpponent_spec, eOOT_knackered_and_freewheeling);
        }
        return;
    }

    if (pOpponent_spec->current_objective == eOOT_frozen) {
        if (GET_CAR_SPEED_FACTOR(pOpponent_spec->car_spec) == 0.f) {
            return;
        }
        DoNotDprintf_opponent("%s: Time to unfreeze", pOpponent_spec->car_spec->driver_name);
        if (pOpponent_spec->pursuing_player_before_freeze) {
            NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &C2V(gProgram_state).current_car);
        } else {
            NewObjective(pOpponent_spec, eOOT_get_near_player);
        }
        return;
    }
    if (GET_CAR_SPEED_FACTOR(pOpponent_spec->car_spec) == 0.f) {
        DoNotDprintf_opponent("%s: Decided to freeze", pOpponent_spec->car_spec->driver_name);
        pOpponent_spec->pursuing_player_before_freeze = pOpponent_spec->current_objective == eOOT_pursue_and_twat || pOpponent_spec->pursue_car_data__pursuee == &C2V(gProgram_state).current_car;
        NewObjective(pOpponent_spec, eOOT_frozen);
        return;
    }
    if (!C2V(gFirst_frame)) {
        int general_grudge_increase;

        general_grudge_increase = (int)(pOpponent_spec->nastiness * 40.0f + 10.0f);
        if (pOpponent_spec->car_spec->scary_bang && pOpponent_spec->player_to_oppo_d < 10.f) {
            if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
                if (PercentageChance(20)) {
                    DoNotDprintf_opponent("%s: Decided to run away", pOpponent_spec->car_spec->driver_name);
                    NewObjective(pOpponent_spec, eOOT_run_away);
                    return;
                }
            } else if (C2V(gOpponents)[pOpponent_spec->index].strength_rating < 5) {
                if (PercentageChance((int)((pOpponent_spec->current_objective == eOOT_pursue_and_twat ? 100 : 60) - 50.f * pOpponent_spec->nastiness))) {
                    DoNotDprintf_opponent("%s: Decided to run away", pOpponent_spec->car_spec->driver_name);
                    NewObjective(pOpponent_spec, eOOT_run_away);
                    return;
                }
            }
        }

        if (pOpponent_spec->current_objective != eOOT_run_away || C2V(gTime_stamp_for_this_munging) >= pOpponent_spec->time_this_objective_started + 20000) {
            if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec) && pOpponent_spec->murder_reported && pOpponent_spec->player_to_oppo_d < 20.f && !AlreadyPursuingCar(pOpponent_spec, &C2V(gProgram_state).current_car)) {
                C2V(gOpponents)[pOpponent_spec->index].psyche.grudge_against_player = MIN(100, MAX(20, C2V(gOpponents)[pOpponent_spec->index].psyche.grudge_against_player) + general_grudge_increase);
                c2_sprintf(str, "%s: Furderous melon!", pOpponent_spec->car_spec->driver_name);
                DoNotDprintf_opponent("%s: Decided to pursue after MURDER", pOpponent_spec->car_spec->driver_name);
                NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &C2V(gProgram_state).current_car);
                return;
            }
            if (pOpponent_spec->car_spec->big_bang
                    && LastTwatterAPlayer(pOpponent_spec)
                    && !AlreadyPursuingCar(pOpponent_spec, pOpponent_spec->car_spec->last_person_to_hit_us)) {

                C2V(gOpponents)[pOpponent_spec->index].psyche.grudge_against_player = MIN(100, MAX(20, C2V(gOpponents)[pOpponent_spec->index].psyche.grudge_against_player) + general_grudge_increase);
                c2_sprintf(str, "%s: Christ! What was that?", pOpponent_spec->car_spec->driver_name);
                DoNotDprintf_opponent("%s: Decided to pursue after big bang; last person to twat us was %s",
                    pOpponent_spec->car_spec->driver_name,
                    pOpponent_spec->car_spec->last_person_to_hit_us->driver_name);
                NewObjective(pOpponent_spec, eOOT_pursue_and_twat, pOpponent_spec->car_spec->last_person_to_hit_us);
                return;
            }
            if (LastTwatteeAPlayer(pOpponent_spec) && !AlreadyPursuingCar(pOpponent_spec, pOpponent_spec->car_spec->last_person_we_hit)) {

                C2V(gOpponents)[pOpponent_spec->index].psyche.grudge_against_player = MIN(100, MAX(20, C2V(gOpponents)[pOpponent_spec->index].psyche.grudge_against_player) + general_grudge_increase);
                c2_sprintf(str, "%s: Ha! Bet you weren't expecting that!", pOpponent_spec->car_spec->driver_name);
                DoNotDprintf_opponent("%s: Decided to pursue %s after accidentally hitting them",
                    pOpponent_spec->car_spec->driver_name,
                    pOpponent_spec->car_spec->last_person_we_hit->driver_name);
                NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &C2V(gProgram_state).current_car);
                return;
            }
            if (!AlreadyPursuingCar(pOpponent_spec, &C2V(gProgram_state).current_car)) {
                if (pOpponent_spec->car_spec->grudge_raised_recently
                        && (!CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec) || pOpponent_spec->player_to_oppo_d <= 20.f)
                        && LastTwatterAPlayer(pOpponent_spec)) {
                    int grudge = C2V(gOpponents)[pOpponent_spec->index].psyche.grudge_against_player;
                    if (grudge > 20) {
                        C2V(gOpponents)[pOpponent_spec->index].psyche.grudge_against_player = MIN(grudge + general_grudge_increase, 100);
                        c2_sprintf(str, "%s: Right! That's enough, %s!", pOpponent_spec->car_spec->driver_name, C2V(gProgram_state).current_car.driver_name);
                        DoNotDprintf_opponent("%s: Decided to pursue after grudginess raised; last person to twat us was %s",
                            pOpponent_spec->car_spec->driver_name, pOpponent_spec->car_spec->last_person_to_hit_us->driver_name);
                        NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &C2V(gProgram_state).current_car);
                        return;
                    }
                }
                if (pOpponent_spec->player_in_view_now && !pOpponent_spec->acknowledged_piv) {
                    int pursuit_percentage;
                    int do_it;

                    pOpponent_spec->acknowledged_piv = 1;
                    if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
                        pursuit_percentage = (int)((BrVector3Length(&C2V(gProgram_state).current_car.collision_info->v) * WORLD_SCALE - C2V(gDefinite_no_cop_pursuit_speed)) * C2V(gCop_pursuit_speed_percentage_multiplier));
                    } else if (C2V(gProgram_state).skill_level + 3 > C2V(gNum_of_opponents_pursuing)) {
                        pursuit_percentage = (int)((float)(C2V(gOpponents)[pOpponent_spec->index].psyche.grudge_against_player - 20) + 30.f * pOpponent_spec->nastiness);
                    } else {
                        pursuit_percentage = 0;
                    }

                    pursuit_percentage += HeadOnWithPlayerPossible(pOpponent_spec) ? 50 : 0;
                    do_it = PercentageChance(pursuit_percentage);
                    DoNotDprintf_opponent("%s: Spotted player; chance of pursuing %d%%: %s", pOpponent_spec->car_spec->driver_name, pursuit_percentage, do_it ? "YES, Decided to pursue" : "NO, Decided NOT to pursue");
                    if (do_it) {
                        C2V(gOpponents)[pOpponent_spec->index].psyche.grudge_against_player = MIN(100, MAX(20, C2V(gOpponents)[pOpponent_spec->index].psyche.grudge_against_player) + general_grudge_increase);
                        c2_sprintf(str, "%s: I've decided to kill you for the fun of it", pOpponent_spec->car_spec->driver_name);
                        NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &C2V(gProgram_state).current_car);
                        return;
                    }
                }
            }
        }
    }
    if (pMust_choose_one) {
        DoNotDprintf_opponent("%s: Choosing new objective because we have to...", pOpponent_spec->car_spec->driver_name);
        if (pOpponent_spec->has_moved_at_some_point) {
            int pursuit_percentage;

            if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
                NewObjective(pOpponent_spec, eOOT_return_to_start);
                return;
            }
            switch (pOpponent_spec->field_0xb4) {
            case 1:
                NewObjective(pOpponent_spec, eOOT_complete_race);
                break;
            case 2:
                NewObjective(pOpponent_spec, eOOT_get_near_player);
                break;
            case 3:
                NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &C2V(gProgram_state).current_car);
                break;
            default:
                pursuit_percentage = (int)pOpponent_spec->player_to_oppo_d - 15;
                if (PercentageChance(pursuit_percentage)) {
                    DoNotDprintf_opponent("%s: Choosing to get_near because chance dictated it (%d%%)", pOpponent_spec->car_spec->driver_name, pursuit_percentage);
                    NewObjective(pOpponent_spec, eOOT_get_near_player);
                } else {
                    DoNotDprintf_opponent("%s: Choosing to complete_race because chance dictated it (%d%%)", pOpponent_spec->car_spec->driver_name, pursuit_percentage);
                    NewObjective(pOpponent_spec, eOOT_complete_race);
                }
                return;
            }
        } else if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
            NewObjective(pOpponent_spec, eOOT_wait_for_some_hapless_sod);
            return;
        } else if (pOpponent_spec->pursue_from_start) {
            int grudge;

            grudge = MAX(20, C2V(gOpponents)[pOpponent_spec->index].psyche.grudge_against_player);
            C2V(gOpponents)[pOpponent_spec->index].psyche.grudge_against_player = (int)MIN(100, MAX(20, C2V(gOpponents)[pOpponent_spec->index].psyche.grudge_against_player) + 20 + 40 * pOpponent_spec->nastiness);
            NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &C2V(gProgram_state).current_car);
            return;
        } else {
            NewObjective(pOpponent_spec, eOOT_complete_race);
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ace70, ChooseNewObjective, ChooseNewObjective_original)

void (C2_HOOK_FASTCALL * CalcRaceRoute_original)(tOpponent_spec *pOpponent_spec);
void C2_HOOK_FASTCALL CalcRaceRoute(tOpponent_spec *pOpponent_spec) {

#if 0//defined(C2_HOOKS_ENABLED)
    CalcRaceRoute_original(pOpponent_spec);
#else
    tS16 section_no;
    tS16 node_no;
    tS16 race_section_count;
    tS16 normal_section_ok_direction_count;
    tS16 normal_section_wrong_direction_count;
    tS16 temp_section_array[8];
    br_scalar distance;
    br_vector3 direction_v;
    br_vector3 intersect;
    int i;

    if (pOpponent_spec->nnext_sections >= REC2_ASIZE(pOpponent_spec->next_sections)) {
        DoNotDprintf_opponent("%s: CalcRaceRoute() - Pissing off 'cos projected route full up", pOpponent_spec->car_spec->driver_name);
        return;
    }
    if (pOpponent_spec->nnext_sections == 0) {
        DoNotDprintf_opponent("%s: CalcRaceRoute() - Projected route empty; starting from nearest section", pOpponent_spec->car_spec->driver_name);
        pOpponent_spec->complete_race_data.finished_calcing_race_route = 0;
        pOpponent_spec->complete_race_data.found_race_section = 0;
        section_no = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction_v, &intersect, &distance);
        if (section_no < 0) {
            return;
        }
        AddToOpponentsProjectedRoute(pOpponent_spec, section_no, 1);
        if (C2V(gProgram_state).AI_vehicles.path_sections[section_no].type == ePST_race_path) {
            pOpponent_spec->complete_race_data.found_race_section = 1;
        }
    }
    while (pOpponent_spec->nnext_sections < REC2_ASIZE(pOpponent_spec->next_sections) && !pOpponent_spec->complete_race_data.finished_calcing_race_route) {
        node_no = C2V(gProgram_state).AI_vehicles.path_sections[pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no].node_indices[pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].direction];
        race_section_count = 0;
        normal_section_ok_direction_count = 0;
        normal_section_wrong_direction_count = 0;
        for (i = 0; i < C2V(gProgram_state).AI_vehicles.path_nodes[node_no].number_of_sections; i++) {
            section_no = C2V(gProgram_state).AI_vehicles.path_nodes[node_no].sections[i];
            if (pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no != section_no) {
                if (C2V(gProgram_state).AI_vehicles.path_sections[section_no].type == ePST_race_path && C2V(gProgram_state).AI_vehicles.path_sections[section_no].node_indices[0] == node_no) {
                    pOpponent_spec->complete_race_data.found_race_section = 1;
                    temp_section_array[race_section_count] = section_no;
                    race_section_count += 1;
                } else if (race_section_count == 0 && C2V(gProgram_state).AI_vehicles.path_sections[section_no].node_indices[0] == node_no) {
                    temp_section_array[normal_section_ok_direction_count] = section_no;
                    normal_section_ok_direction_count++;
                } else if (race_section_count == 0 && normal_section_ok_direction_count == 0 && (!C2V(gProgram_state).AI_vehicles.path_sections[section_no].one_way || C2V(gProgram_state).AI_vehicles.path_sections[section_no].node_indices[1] != node_no)) {
                    temp_section_array[normal_section_wrong_direction_count] = section_no;
                    normal_section_wrong_direction_count++;
                }
            }
        }
        if (race_section_count != 0) {
            AddToOpponentsProjectedRoute(pOpponent_spec, temp_section_array[IRandomBetween(0, race_section_count - 1)], 1);
        } else if (normal_section_ok_direction_count != 0) {
            AddToOpponentsProjectedRoute(pOpponent_spec, temp_section_array[IRandomBetween(0, normal_section_ok_direction_count - 1)], 1);
        } else if (normal_section_wrong_direction_count != 0) {
            AddToOpponentsProjectedRoute(pOpponent_spec, temp_section_array[IRandomBetween(0, normal_section_wrong_direction_count - 1)], 1);
        } else if (!pOpponent_spec->complete_race_data.found_race_section) {
            AddToOpponentsProjectedRoute(pOpponent_spec, pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no, pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].direction == 0);
        } else {
            pOpponent_spec->complete_race_data.finished_calcing_race_route = 1;
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004aae20, CalcRaceRoute, CalcRaceRoute_original)

tFollow_path_result (C2_HOOK_FASTCALL * ProcessFollowPath_original)(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand, int pPursuit_mode, int pIgnore_end, int pNever_struggle);
tFollow_path_result C2_HOOK_FASTCALL ProcessFollowPath(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand, int pPursuit_mode, int pIgnore_end, int pNever_struggle) {

#if defined(C2_HOOKS_ENABLED)
    return ProcessFollowPath_original(pOpponent_spec, pCommand, pPursuit_mode, pIgnore_end, pNever_struggle);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004af960, ProcessFollowPath, ProcessFollowPath_original)

void C2_HOOK_FASTCALL ObjectiveComplete(tOpponent_spec* pOpponent_spec) {

    DoNotDprintf_opponent("%s: Objective Completed", pOpponent_spec->car_spec->driver_name);
    pOpponent_spec->new_objective_required = 1;
    switch (pOpponent_spec->current_objective) {
    case eOOT_complete_race:
        C2V(gNum_of_opponents_completing_race) -= 1;
        break;
    case eOOT_pursue_and_twat:
        C2V(gNum_of_opponents_pursuing) -= 1;
        break;
    case eOOT_get_near_player:
        C2V(gNum_of_opponents_getting_near) -= 1;
        break;
    default:
        break;
    }
}
C2_HOOK_FUNCTION(0x004ace20, ObjectiveComplete)

int C2_HOOK_FASTCALL AddToOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, tS16 pSection_no, int pDirection) {

    if (pOpponent_spec->nnext_sections >= REC2_ASIZE(pOpponent_spec->next_sections)) {
        return 0;
    }
    pOpponent_spec->next_sections[pOpponent_spec->nnext_sections].section_no = pSection_no;
    pOpponent_spec->next_sections[pOpponent_spec->nnext_sections].direction = pDirection;
    pOpponent_spec->nnext_sections += 1;
    return 1;
}
C2_HOOK_FUNCTION(0x004ab150, AddToOpponentsProjectedRoute)

void (C2_HOOK_FASTCALL * TopUpRandomRoute_original)(tOpponent_spec* pOpponent_spec, int pSections_to_add);
void C2_HOOK_FASTCALL TopUpRandomRoute(tOpponent_spec* pOpponent_spec, int pSections_to_add) {

#if defined(C2_HOOKS_ENABLED)
    TopUpRandomRoute_original(pOpponent_spec, pSections_to_add);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004a93c0, TopUpRandomRoute, TopUpRandomRoute_original)

void (C2_HOOK_FASTCALL * ProcessGetNearPlayer_original)(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);
void C2_HOOK_FASTCALL ProcessGetNearPlayer(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {

#if defined(C2_HOOKS_ENABLED)
    ProcessGetNearPlayer_original(pOpponent_spec, pCommand);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ac890, ProcessGetNearPlayer, ProcessGetNearPlayer_original)

void (C2_HOOK_FASTCALL * ProcessLevitate_original)(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);
void C2_HOOK_FASTCALL ProcessLevitate(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {

#if defined(C2_HOOKS_ENABLED)
    ProcessLevitate_original(pOpponent_spec, pCommand);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ac540, ProcessLevitate, ProcessLevitate_original)

int (C2_HOOK_FASTCALL * RematerialiseOpponent_original)(tOpponent_spec* pOpponent_spec, br_scalar pSpeed);
int C2_HOOK_FASTCALL RematerialiseOpponent(tOpponent_spec* pOpponent_spec, br_scalar pSpeed) {

#if defined(C2_HOOKS_ENABLED)
    return RematerialiseOpponent_original(pOpponent_spec, pSpeed);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004a87a0, RematerialiseOpponent, RematerialiseOpponent_original)

int C2_HOOK_FASTCALL RematerialiseOpponentOnNearestSection(tOpponent_spec* pOpponent_spec, br_scalar pSpeed) {
    br_vector3 intersect;
    br_vector3 direction_v;
    br_vector3 car_to_end;
    br_vector3* finish;
    br_scalar distance;
    br_scalar distance_to_end;
    tS16 section_no;

    if (pOpponent_spec->physics_me) {
        DoNotDprintf_opponent("%s: Actually, we're already materialised", pOpponent_spec->car_spec->driver_name);
        return 1;
    }
    section_no = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction_v, &intersect, &distance);
    finish = &C2V(gProgram_state).AI_vehicles.path_nodes[C2V(gProgram_state).AI_vehicles.path_sections[section_no].node_indices[1]].pos;
    BrVector3Copy(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &intersect);
    PointActorAlongThisBloodyVector(pOpponent_spec->car_spec->car_master_actor, &direction_v);
    BrVector3Sub(&car_to_end, finish, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
    if (RematerialiseOpponent(pOpponent_spec, pSpeed)) {
        pOpponent_spec->car_spec->brake_force = 0.0f;
        pOpponent_spec->car_spec->acc_force = 0.0f;

        distance_to_end = BrVector3Length(&car_to_end);
        if (distance_to_end < 5.0f) {
            pOpponent_spec->car_spec->brake_force = pOpponent_spec->car_spec->collision_info->M * 15.0f;
        } else {
            pOpponent_spec->car_spec->acc_force = pOpponent_spec->car_spec->collision_info->M / 2.0f;
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x004a8170, RematerialiseOpponentOnNearestSection)

void C2_HOOK_FASTCALL ProcessWaitForSomeHaplessSod(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {

    switch (pCommand) {
    case ePOC_start:
        pOpponent_spec->car_spec->brake_force = 15.f * pOpponent_spec->car_spec->collision_info->M;
        break;
    case ePOC_run:
        pOpponent_spec->car_spec->brake_force = 15.f * pOpponent_spec->car_spec->collision_info->M;
        break;
    default:
        break;
    }
}
C2_HOOK_FUNCTION(0x004abc00, ProcessWaitForSomeHaplessSod)

void (C2_HOOK_FASTCALL * CalcReturnToStartPointRoute_original)(tOpponent_spec* pOpponent_spec);
void C2_HOOK_FASTCALL CalcReturnToStartPointRoute(tOpponent_spec* pOpponent_spec) {

#if defined(C2_HOOKS_ENABBLED)
    CalcReturnToStartPointRoute_original(pOpponent_spec);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004abf60, CalcReturnToStartPointRoute, CalcReturnToStartPointRoute_original)

int C2_HOOK_FASTCALL TeleportCopToStart(tOpponent_spec* pOpponent_spec) {
    br_vector3 wank;

    if (!pOpponent_spec->cheating || !CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
        return 0;
    }
    BrVector3Sub(&wank, &C2V(gProgram_state).current_car.car_master_actor->t.t.translate.t, &pOpponent_spec->start_pos);
    if (BrVector3Length(&wank) <= C2V(gIn_view_distance)) {
        return 0;
    }
    BrVector3Copy(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &pOpponent_spec->start_pos);
    PointActorAlongThisBloodyVector(pOpponent_spec->car_spec->car_master_actor, &pOpponent_spec->start_direction);
    pOpponent_spec->physics_me = 0;
    RematerialiseOpponent(pOpponent_spec, 0.0);
    TurnOpponentPhysicsOff(pOpponent_spec);
    RebuildActiveCarList();
    NewObjective(pOpponent_spec, eOOT_wait_for_some_hapless_sod);
    return 1;
}

void C2_HOOK_FASTCALL ProcessReturnToStart(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {
    br_vector3 section_v;
    br_vector3 our_pos_xz;
    br_vector3 cop_to_start;
    br_scalar distance;
    int res;

    switch (pCommand) {
    case ePOC_start:
        DoNotDprintf_opponent("%s: ProcessReturnToStart() - new objective started", pOpponent_spec->car_spec->driver_name);
        pOpponent_spec->return_to_start_data.waiting_near_start = 0;
        pOpponent_spec->return_to_start_data.section_no = FindNearestPathSection(&pOpponent_spec->start_pos, &section_v, &pOpponent_spec->return_to_start_data.nearest_path_point, &distance);
        pOpponent_spec->return_to_start_data.nearest_path_point.v[1] = 0.0;
        CalcReturnToStartPointRoute(pOpponent_spec);
        ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
        break;
    case ePOC_run:
        if (TeleportCopToStart(pOpponent_spec)) {
            break;
        }
        if (pOpponent_spec->return_to_start_data.waiting_near_start) {
            pOpponent_spec->car_spec->brake_force = pOpponent_spec->car_spec->collision_info->M * 15.0f;
        } else {
            BrVector3Copy(&our_pos_xz, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
            our_pos_xz.v[1] = 0.0f;
            BrVector3Sub(&cop_to_start, &pOpponent_spec->start_pos, &our_pos_xz);
            if (BrVector3Length(&cop_to_start) >= 10.0) {
                if (pOpponent_spec->follow_path.section_no > 20000) {
                    ShiftOpponentsProjectedRoute(pOpponent_spec, pOpponent_spec->follow_path.section_no - 20000);
                    pOpponent_spec->follow_path.section_no = 20000;
                }
                if (pOpponent_spec->nnext_sections < REC2_ASIZE(pOpponent_spec->next_sections) - 1) {
                    CalcReturnToStartPointRoute(pOpponent_spec);
                }
                res = ProcessFollowPath(pOpponent_spec, ePOC_run, 0, 0, 0);
                if (res == eFPR_given_up || res == eFPR_end_of_path) {
                    if (res == eFPR_given_up) {
                        DoNotDprintf_opponent("%s: Restarting return_to_start route because ProcessFollowPath() gave up.", pOpponent_spec->car_spec->driver_name);
                    } else {
                        DoNotDprintf_opponent("%s: Restarting return_to_start route because ran out of path!", pOpponent_spec->car_spec->driver_name);
                    }
                    ClearOpponentsProjectedRoute(pOpponent_spec);
                    CalcReturnToStartPointRoute(pOpponent_spec);
                    ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
                }
            } else {
                pOpponent_spec->return_to_start_data.waiting_near_start = 1;
                pOpponent_spec->car_spec->brake_force = pOpponent_spec->car_spec->collision_info->M * 15.0f;
            }
        }
        break;
    default:
        break;
    }
}
C2_HOOK_FUNCTION(0x004abc40, ProcessReturnToStart)

void C2_HOOK_FASTCALL ClearOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec) {

    pOpponent_spec->nnext_sections = 0;
}

void C2_HOOK_FASTCALL ProcessCompleteRace(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {

    switch (pCommand) {
    case ePOC_start:
        DoNotDprintf_opponent("%s: ProcessCompleteRace() - new objective started", pOpponent_spec->car_spec->driver_name);
        ClearOpponentsProjectedRoute(pOpponent_spec);
        CalcRaceRoute(pOpponent_spec);
        ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
        break;
    case ePOC_run:
        if (pOpponent_spec->follow_path.section_no > 20000) {
            ShiftOpponentsProjectedRoute(pOpponent_spec, pOpponent_spec->follow_path.section_no - 20000);
            pOpponent_spec->follow_path.section_no = 20000;
        }
        if (pOpponent_spec->nnext_sections < REC2_ASIZE(pOpponent_spec->next_sections) - 1 && !pOpponent_spec->complete_race_data.finished_calcing_race_route) {
            CalcRaceRoute(pOpponent_spec);
        }
        if (pOpponent_spec->nnext_sections == 0) {
            switch (ProcessFollowPath(pOpponent_spec, ePOC_run, 0, 0, 0)) {
            case eFPR_end_of_path:
                DoNotDprintf_opponent("%s: Giving up following race path because ran out of race path", pOpponent_spec->car_spec->driver_name);
                NewObjective(pOpponent_spec, eOOT_get_near_player);
                break;
            case eFPR_given_up:
                ClearOpponentsProjectedRoute(pOpponent_spec);
                CalcRaceRoute(pOpponent_spec);
                ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
                break;
            default:
                ObjectiveComplete(pOpponent_spec);
                break;
            }
        }
        break;
    }
}

void C2_HOOK_FASTCALL ProcessRunAway(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {
    tS16 section_no;
    br_scalar distance;
    br_vector3 intersect;
    br_vector3 direction_v;
    char str[256];

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOpponent_spec, run_away_data, 0x190);

    switch (pCommand) {
    case ePOC_start:
        DoNotDprintf_opponent("%s: ProcessRunAway() - new objective started", pOpponent_spec->car_spec->driver_name);
        pOpponent_spec->run_away_data.time_to_stop = C2V(gTime_stamp_for_this_munging) + 1000 * IRandomBetween(30, 90);
        ClearOpponentsProjectedRoute(pOpponent_spec);
        section_no = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction_v, &intersect, &distance);
        if (BrVector3Dot(&pOpponent_spec->car_spec->direction, &direction_v) < 0.0f) {
            AddToOpponentsProjectedRoute(pOpponent_spec, section_no, 0);
        } else {
            AddToOpponentsProjectedRoute(pOpponent_spec, section_no, 1);
        }
        TopUpRandomRoute(pOpponent_spec, -1);
        ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
        sprintf(str, "%s: Shit! I'm out of here...", pOpponent_spec->car_spec->driver_name);
        break;
    case ePOC_run:
        if (C2V(gTime_stamp_for_this_munging) <= pOpponent_spec->run_away_data.time_to_stop) {
            if (pOpponent_spec->follow_path.section_no > 20000) {
                ShiftOpponentsProjectedRoute(pOpponent_spec, pOpponent_spec->follow_path.section_no - 20000);
                pOpponent_spec->follow_path.section_no = 20000;
            }
            if (pOpponent_spec->nnext_sections < REC2_ASIZE(pOpponent_spec->next_sections)) {
                TopUpRandomRoute(pOpponent_spec, REC2_ASIZE(pOpponent_spec->next_sections) - pOpponent_spec->nnext_sections);
            }
            if (ProcessFollowPath(pOpponent_spec, ePOC_run, 0, 0, 0) == eFPR_given_up) {
                ClearOpponentsProjectedRoute(pOpponent_spec);
                section_no = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction_v, &intersect, &distance);
                if (BrVector3Dot(&pOpponent_spec->car_spec->direction, &direction_v) < 0.0f) {
                    AddToOpponentsProjectedRoute(pOpponent_spec, section_no, 0);
                } else {
                    AddToOpponentsProjectedRoute(pOpponent_spec, section_no, 1);
                }
                TopUpRandomRoute(pOpponent_spec, -1);
                ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
            }
        } else {
            ObjectiveComplete(pOpponent_spec);
        }
        break;
    default:
        break;
    }
}

void C2_HOOK_FASTCALL ProcessFrozen(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {

    switch (pCommand) {
    case ePOC_start:
        DoNotDprintf_opponent("%d ProcessFrozen() - new task started", pOpponent_spec->index);
        DoNotDprintf_opponent("%s: Rematerialising from ePOC_start in ProcessFrozen()...", pOpponent_spec->car_spec->driver_name);
        RematerialiseOpponentOnNearestSection(pOpponent_spec, 0.f);
        pOpponent_spec->car_spec->acc_force = 0.f;
        pOpponent_spec->car_spec->brake_force = 15.f * pOpponent_spec->car_spec->collision_info->M;
        break;
    case ePOC_run:
        pOpponent_spec->car_spec->brake_force = 15.f * pOpponent_spec->car_spec->collision_info->M;
        break;
    case ePOC_die:
        pOpponent_spec->car_spec->brake_force = 0.f;
        break;
    }
}

void (C2_HOOK_FASTCALL * ProcessCurrentObjective_original)(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);
void C2_HOOK_FASTCALL ProcessCurrentObjective(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {
#if 0//defined(C2_HOOKS_ENABLED)
    ProcessCurrentObjective_original(pOpponent_spec, pCommand);
#else
    switch (pOpponent_spec->current_objective) {
    case eOOT_none:
        break;
    case eOOT_complete_race:
        ProcessCompleteRace(pOpponent_spec, pCommand);
        break;
    case eOOT_pursue_and_twat:
        ProcessPursueAndTwat(pOpponent_spec, pCommand);
        break;
    case eOOT_run_away:
        ProcessRunAway(pOpponent_spec, pCommand);
        break;
    case eOOT_get_near_player:
        ProcessGetNearPlayer(pOpponent_spec, pCommand);
        break;
    case eOOT_levitate:
        ProcessLevitate(pOpponent_spec, pCommand);
        break;
    case eOOT_knackered_and_freewheeling:
        // FIXME: is keys correct?
        c2_memset(&pOpponent_spec->car_spec->keys, 0, sizeof(pOpponent_spec->car_spec->keys));
        pOpponent_spec->car_spec->acc_force = 0.f;
        pOpponent_spec->car_spec->brake_force = 0.f;
        pOpponent_spec->car_spec->curvature = 0.f;
        break;
    case eOOT_frozen:
        ProcessFrozen(pOpponent_spec, pCommand);
        break;
    case eOOT_wait_for_some_hapless_sod:
        ProcessWaitForSomeHaplessSod(pOpponent_spec, pCommand);
        break;
    case eOOT_return_to_start:
        ProcessReturnToStart(pOpponent_spec, pCommand);
        break;
    default:
        break;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004aa900, ProcessCurrentObjective, ProcessCurrentObjective_original)

void (C2_HOOK_FASTCALL * TeleportOpponentToNearestSafeLocation_original)(tOpponent_spec* pOpponent_spec);
void C2_HOOK_FASTCALL TeleportOpponentToNearestSafeLocation(tOpponent_spec* pOpponent_spec) {

#if defined(C2_HOOKS_ENABLED)
    TeleportOpponentToNearestSafeLocation_original(pOpponent_spec);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004a9180, TeleportOpponentToNearestSafeLocation, TeleportOpponentToNearestSafeLocation_original)

void C2_HOOK_CDECL NewObjective(tOpponent_spec* pOpponent_spec, tOpponent_objective_type pObjective_type, ...) {
    va_list ap;

    if (pOpponent_spec->current_objective != eOOT_none) {
        ProcessCurrentObjective(pOpponent_spec, ePOC_die);
    }
    pOpponent_spec->current_objective = pObjective_type;
    pOpponent_spec->time_this_objective_started = C2V(gTime_stamp_for_this_munging);
    pOpponent_spec->time_for_this_objective_to_finish = C2V(gTime_stamp_for_this_munging) + IRandomBetween(20, 60) * 1000;
    if (pObjective_type == eOOT_pursue_and_twat) {
        pOpponent_spec->time_for_this_objective_to_finish += 90000;
    }
    switch (pObjective_type) {
    case eOOT_complete_race:
        C2V(gNum_of_opponents_completing_race) += 1;
        break;
    case eOOT_pursue_and_twat:
        va_start(ap, pObjective_type);
        pOpponent_spec->pursue_car_data__pursuee = va_arg(ap, tCar_spec*);
        va_end(ap);
        C2V(gNum_of_opponents_pursuing) += 1;
        break;
    case eOOT_get_near_player:
        C2V(gNum_of_opponents_getting_near) += 1;
        break;
    default:
        break;
    }
    DoNotDprintf_opponent("%s: NewObjective() - type %d", pOpponent_spec->car_spec->driver_name, pObjective_type);
    ProcessCurrentObjective(pOpponent_spec, ePOC_start);
}
C2_HOOK_FUNCTION(0x004aad70, NewObjective)
