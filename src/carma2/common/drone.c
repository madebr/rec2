#include "drone.h"

#include "compress.h"
#include "52-errors.h"
#include "finteray.h"
#include "globvars.h"
#include "globvrpb.h"
#include "loading.h"
#include "opponent.h"
#include "physics.h"
#include "piping.h"
#include "platform.h"
#include "utility.h"
#include "world.h"

#include <brender/brender.h>

#include <stdio.h>
#include "c2_string.h"

#include "rec2_macros.h"

#include <stdarg.h>


// GLOBAL: CARMA2_HW 0x006820d0
int gCount_drones;

// GLOBAL: CARMA2_HW 0x00682178
tDrone_form gDrone_forms[64];

// GLOBAL: CARMA2_HW 0x0068450c
int gCount_drone_forms;

// GLOBAL: CARMA2_HW 0x00594738
tDrone_state_function* gDrone_state_functions[6] = {
    NULL,
    DroneStateFuncReset,
    DroneStateFuncControlledMovement,
    DroneStateFuncPhysicsActive,
    DroneStateFuncStationaryPassive,
    NULL,
};

// GLOBAL: CARMA2_HW 0x0074a5f0
tCollision_info* gList_collision_infos;

// GLOBAL: CARMA2_HW 0x006820b8
int gDrones_unmodified;

// GLOBAL: CARMA2_HW 0x00684500
int gCount_rendered_drones;

// GLOBAL: CARMA2_HW 0x006844f8
int gCurrent_selected_drone;

// GLOBAL: CARMA2_HW 0x006820d4
int gINT_006820d4;

// GLOBAL: CARMA2_HW 0x006844fc
int gINT_006844fc;

// GLOBAL: CARMA2_HW 0x00681fb0
int gINT_00681fb0;

// GLOBAL: CARMA2_HW 0x00684504
tDrone_spec* gDrone_specs;

// GLOBAL: CARMA2_HW 0x0068452c
int gShow_drone_paths;

// GLOBAL: CARMA2_HW 0x00681fb4
int gCount_active_drones;

// GLOBAL: CARMA2_HW 0x00684514
int gFrame;

// GLOBAL: CARMA2_HW 0x00684508
tDrone_path_node* gDrone_path_nodes;

// GLOBAL: CARMA2_HW 0x00684510
int gCount_drone_path_nodes;

// GLOBAL: CARMA2_HW 0x006820c0
br_vector3 gRender_bounds_centre;

// GLOBAL: CARMA2_HW 0x00594760
tDrone_form_within_rendering_distance_cbfn* gDrone_form_within_rendering_distance_functions[4] = {
    DroneCarWithinRenderingDistance,
    DronePlaneWithinRenderingDistance,
    DroneTrainWithinRenderingDistance,
    DronePlaneWithinRenderingDistance,
};

// GLOBAL: CARMA2_HW 0x00594750
tDrone_form_within_processing_distance_cbfn* gDrone_form_within_processing_distance_functions[4] = {
    DroneCarWithinProcessingDistance,
    DronePlaneWithinProcessingDistance,
    DroneTrainWithinProcessingDistance,
    DronePlaneWithinProcessingDistance,
};

// GLOBAL: CARMA2_HW 0x005947c0
br_vector3 gDefault_drone_direction = {{ 0.f, 0.f, -1.f }};

// GLOBAL: CARMA2_HW 0x00684520
br_model* gElasticatey_drone_path_model;

// GLOBAL: CARMA2_HW 0x00684528
br_actor* gDrone_path_model_actor;

// GLOBAL: CARMA2_HW 0x00684530
int gDrone_paths_elasticating;

// GLOBAL: CARMA2_HW 0x005947b0
int gSelected_drone_path_node_index = -1;

// GLOBAL: CARMA2_HW 0x005947b4
int gSelected_edit_drone_path = -1;

// GLOBAL: CARMA2_HW 0x006820cc
tU32 gNext_drone_edit_path_munge;

// GLOBAL: CARMA2_HW 0x005947b8
int gSelected_drone_path_index = -1;

// FUNCTION: CARMA2_HW 0x0044cfc0
void C2_HOOK_CDECL DoNotDprintf(const char* format, ...) {
// Disabled because too noisy
#if 0
    va_list ap;

    va_start(ap, format);
    vfprintf(stderr, format, ap);
    fputc('\n', stderr);
    va_end(ap);
#endif
}

// FUNCTION: CARMA2_HW 0x00451210
void C2_HOOK_FASTCALL InitDroneSpec(tDrone_spec* pDrone_spec, int pNode) {

    C2_HOOK_BUG_ON(sizeof(*pDrone_spec) != 0x5d8);

    DoNotDprintf("InitDroneSpec() - INITIALISING DRONE SPEC");
    memset(pDrone_spec, 0, sizeof(*pDrone_spec));
    pDrone_spec->field_0xa_pathnode_id = pNode;
    pDrone_spec->field_0x8_pathnode_id = pNode;
    pDrone_spec->field_0xe = -1;
    pDrone_spec->field_0xc = -1;
    BrVector3Set(&pDrone_spec->field_0x18, 0.f, 0.f, 0.f);
    BrVector3Copy(&pDrone_spec->pos, &gDrone_path_nodes[pNode].position);
    pDrone_spec->field_0xdc = 0;
    pDrone_spec->field_0xf4 = -1;
    pDrone_spec->current_state = 0;
    pDrone_spec->field_0x5d4 = 0;
    pDrone_spec->form = &gDrone_forms[gDrone_path_nodes[pNode].type];
    pDrone_spec->field_0xe4 = 1.0f;
    pDrone_spec->field_0x48 = 1.0f;
}

// FUNCTION: CARMA2_HW 0x00451070
void C2_HOOK_FASTCALL PreprocessDronePaths(void) {
    int i;
    for (i = 0; i < gCount_drone_path_nodes; i++) {
        tDrone_path_node* node;
        int j;

        node = &gDrone_path_nodes[i];
        for (j = 0; j < node->count_sections; j++) {
            tDrone_path_node_section* section;

            section = &node->sections[j];
            BrVector3Sub(&section->field_0x04, &gDrone_path_nodes[section->node1].position, &node->position);
            section->field_0x1c = BrVector3Length(&section->field_0x04);
            BrVector3Normalise(&section->field_0x10, &section->field_0x04);
        }
    }
}

void C2_HOOK_FASTCALL AllocateAndInitDrones(void) {
    int i;
    int drone_i;

    C2_HOOK_BUG_ON(sizeof(tDrone_spec) != 0x5d8);

    gDrone_specs = BrMemAllocate(gCount_drones * sizeof(tDrone_spec), kMem_drone_specs);
    drone_i = 0;
    for (i = 0; i < gCount_drone_path_nodes; i++) {
        tDrone_path_node* node;

        node = &gDrone_path_nodes[i];
        if (node->type >= 0) {
            InitDroneSpec(&gDrone_specs[drone_i], i);
            drone_i += 1;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00450bf0
void C2_HOOK_FASTCALL LoadInDronePaths(FILE* pF) {
    char s[256];

    DoNotDprintf("Start of LoadInDronePaths()...");
    for (;;) {
        if (!GetALineAndDontArgue(pF, s)) {
            PDFatalError("Corrupt race text file - No drone path info");
            break;
        }
        if (strcmp(s, "START OF DRONE PATHS") == 0) {
            break;
        }
    }
    gCount_drones = 0;
    gCount_drone_path_nodes = 0;
    if (gNet_mode == eNet_mode_none && !gTraffic_disabled) {
        int version;

        /* version */
        version = GetAnInt(pF);

        if (version > 0) {
            int i;

            if (version != 2) {
                PDFatalError("Drones paths are not version 2");
            }

            C2_HOOK_BUG_ON(sizeof(tDrone_path_node) != 0x134);

            /* n_nodes */
            gCount_drone_path_nodes = GetAnInt(pF);
            if (gCount_drone_path_nodes != 0) {
                gDrone_path_nodes = BrMemAllocate(gCount_drone_path_nodes * sizeof(tDrone_path_node), kMem_drone_paths);
            }
            for (i = 0; i < gCount_drone_path_nodes; i++) {
                br_scalar float_buffer[4];
                tDrone_path_node *node = &gDrone_path_nodes[i];
                int j;

                GetNScalars(pF, 3, float_buffer);
                node->position.v[0] = float_buffer[0];
                node->position.v[1] = float_buffer[1];
                node->position.v[2] = float_buffer[2];

                s[0] = '\0';
                GetAString(pF, s);
                Uppercaseificate(s, s);
                node->type = -1;
                if (strlen(s) == 0) {
                    sprintf(s, "Corrupt race text file - drone type corrupt in drone node %d", i);
                    PDFatalError(s);
                }

                if (strcmp(s, "NONE") != 0) {
                    for (j = 0; j < gCount_drone_forms; j++) {
                        if (strcmp(gDrone_forms[j].name, s) == 0) {
                            node->type = j;
                        }
                    }
                    if (node->type < 0) {
                        char s2[256];

                        sprintf(s2, "ERROR - '%s' no such drone type in drone node %d", s, i);
                        PDFatalError(s2);
                    }
                }
                GetNScalars(pF, 1, float_buffer);
                node->field_0x12e = (int)float_buffer[0];

                C2_HOOK_BUG_ON(REC2_ASIZE(node->sections) != 8);

                node->count_sections = GetAnInt(pF);
                if (node->count_sections > REC2_ASIZE(node->sections)) {

                    sprintf(s, "ERROR - drone node %d has too many sections", i);
                    PDFatalError(s);
                }

                C2_HOOK_BUG_ON(sizeof(tDrone_path_node_section) != 0x24);
                for (j = 0; j < node->count_sections; j++) {
                    tDrone_path_node_section *section = &node->sections[j];

                    GetNScalars(pF, 4, float_buffer);
                    section->node1 = (int)float_buffer[0];
                    section->node2 = (int)float_buffer[1];
                    section->field_0x21 = (int)float_buffer[2];
                    section->type = (int)float_buffer[3];
                    if (section->type > 7) {
                        PDEnterDebugger("Weird type for drone section");
                    }
                }
                if (node->type >= 0) {
                    /* FIXME: use constant for drone limit */
                    if (gCount_drones >= 200) {
                        sprintf(s, "Too many drones in race (limit %d)", 200);
                        PDFatalError(s);
                    }
                    gCount_drones += 1;
                }
            }
        }
    }
    for (;;) {
        const char *str = GetALineAndDontArgue(pF, s);
        if (str == NULL) {
            break;
        }
        if (strcmp(s, "END OF DRONE PATHS") == 0) {
            break;
        }
    }
    PreprocessDronePaths();
    AllocateAndInitDrones();
    DoNotDprintf("End of LoadInDronePaths(), totals:");
    DoNotDprintf("Nodes: %d", gCount_drone_path_nodes);
}

// FUNCTION: CARMA2_HW 0x004516b0
void C2_HOOK_FASTCALL DoDefaultDroneStateAction(tDrone_spec* pDrone_spec) {

    if (gDrone_state_functions[pDrone_spec->current_state] != NULL) {
        gDrone_state_functions[pDrone_spec->current_state](pDrone_spec, eDrone_state_DEFAULT);
    }
}

void C2_HOOK_FASTCALL NewDroneState(tDrone_spec* pDrone_spec, int pNew_state) {
    DoNotDprintf("NewDroneState() Drone %d, Current state %d, New state %d",
        pDrone_spec->id, pDrone_spec->current_state, pNew_state);
    if (pDrone_spec->current_state != pNew_state) {
        if (gDrone_state_functions[pDrone_spec->current_state] != NULL) {
            gDrone_state_functions[pDrone_spec->current_state](pDrone_spec, eDrone_state_STOP);
        }
        pDrone_spec->time_last_munge = gTime_stamp_for_this_munging;
        pDrone_spec->prev_states[1] = pDrone_spec->prev_states[0];
        pDrone_spec->prev_states[0] = pDrone_spec->current_state;
        pDrone_spec->current_state = pNew_state;
        if (gDrone_state_functions[pNew_state] != NULL) {
            gDrone_state_functions[pNew_state](pDrone_spec, eDrone_state_START);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00428d40
int C2_HOOK_FASTCALL TestObjectOverlap(tCollision_info* pCollision_1, tCollision_info* pCollision_2) {

    NOT_IMPLEMENTED();
}

int C2_HOOK_FASTCALL DroneHasCollided(tDrone_spec* pDrone_spec) {
    tCollision_info* collision = gList_collision_infos;
    for (collision = gList_collision_infos; collision != NULL; collision = collision->next) {
        if (&pDrone_spec->collision_info != collision && TestObjectOverlap(&pDrone_spec->collision_info, collision)) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0044f980
void C2_HOOK_FASTCALL InitDroneCollisionObject(tDrone_spec *pDrone_spec) {
    br_bounds3 bnds;
    br_vector3 size;
    tCollision_shape* original_shape;

#ifdef REC2_FIX_BUGS
    original_shape = NULL;
#endif

    BrActorToBounds(&bnds, pDrone_spec->actor);
    bnds.min.v[1] += .001f;
    if (pDrone_spec->field_0x46) {
        original_shape = pDrone_spec->collision_info.shape;
    }
    memset(&pDrone_spec->collision_info, 0, sizeof(pDrone_spec->collision_info));
    if (pDrone_spec->field_0x46) {
        pDrone_spec->collision_info.shape = original_shape;
    } else {
        pDrone_spec->collision_info.shape = (tCollision_shape*)AllocateBoxCollisionShape(kMem_drone_collision_stuff);
        pDrone_spec->collision_info.shape->box.common.bb = bnds;
        pDrone_spec->collision_info.shape->box.common.field_0x1c = bnds;
        pDrone_spec->collision_info.shape->box.common.next = NULL;
        pDrone_spec->field_0x46 = 1;
    }
    pDrone_spec->collision_info.flags_0x238 = 16;
    pDrone_spec->collision_info.owner = pDrone_spec;
    pDrone_spec->collision_info.actor = pDrone_spec->actor;
    pDrone_spec->collision_info.M =  pDrone_spec->form->M;
    BrVector3Set(&pDrone_spec->collision_info.cmpos, .0f, .09f, .0f);
    BrVector3Set(&pDrone_spec->collision_info.field_0x54, .0f, -.4f / WORLD_SCALE, .0f);
    BrMatrix34Copy(&pDrone_spec->collision_info.transform_matrix, &pDrone_spec->actor->t.t.mat);
    pDrone_spec->collision_info.box_face_ref = gFace_num__car - 2;
    pDrone_spec->collision_info.field_0x1a0 = 0x100000;
    FillInShape(pDrone_spec->collision_info.shape);
    UpdateCollisionObject(&pDrone_spec->collision_info);
    GetNewBoundingBox(&pDrone_spec->collision_info.field_0xf4,
        &pDrone_spec->collision_info.bb1,
        &pDrone_spec->collision_info.transform_matrix);
    BrVector3Sub(&size, &bnds.max, &bnds.min);
    pDrone_spec->collision_info.I.v[0] = (size.v[2] * size.v[2] + size.v[1] * size.v[1]) * pDrone_spec->collision_info.M / 12.f;
    pDrone_spec->collision_info.I.v[1] = (size.v[2] * size.v[2] + size.v[0] * size.v[0]) * pDrone_spec->collision_info.M / 12.f;
    pDrone_spec->collision_info.I.v[2] = (size.v[1] * size.v[1] + size.v[0] * size.v[0]) * pDrone_spec->collision_info.M / 12.f;
    // FIXME: is this calculation correct?
    pDrone_spec->collision_info.cmpos.v[0] = bnds.min.v[0] + .01f * pDrone_spec->form->center.v[1] * size.v[0];
    pDrone_spec->collision_info.cmpos.v[1] = bnds.min.v[1] + .01f * pDrone_spec->form->center.v[2] * size.v[1];
    pDrone_spec->collision_info.cmpos.v[2] = bnds.min.v[2] + .01f * pDrone_spec->form->center.v[0] * size.v[2];
    if (pDrone_spec->form->type == kDroneType_train) {
        pDrone_spec->collision_info.world_friction = -.75;
    }
    pDrone_spec->collision_info.disable_move_rotate = 1;
    pDrone_spec->collision_info.field_0xed = 1;
    pDrone_spec->collision_info.flags |= 0x40;
    pDrone_spec->collision_info.drivable_on = !!(pDrone_spec->form->flags & 0x8);
    BrVector3SetFloat(&pDrone_spec->collision_info.v, .0f, .0f, .0f);
    BrVector3SetFloat(&pDrone_spec->collision_info.omega, .0f, .0f, .0f);
    BrVector3SetFloat(&pDrone_spec->collision_info.rotate_omega, .0f, .0f, .0f);
    BrVector3SetFloat(&pDrone_spec->collision_info.velocity_car_space, .0f, .0f, .0f);
}

// FUNCTION: CARMA2_HW 0x00451620
void C2_HOOK_FASTCALL CrappyLittleVector3DPrintf(const char* pMessage, br_vector3* pPosition) {
    DoNotDprintf("%d: %s: %3.3f, %3.3f, %3.3f", gFrame, pMessage, pPosition->v[0], pPosition->v[1], pPosition->v[2]);
}

// FUNCTION: CARMA2_HW 0x004518e0
int C2_HOOK_FASTCALL CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing(tDrone_spec* pDrone) {

    if (gTime_stamp_for_this_munging > pDrone->last_collide_check + 1000) {
        pDrone->last_collide_check = gTime_stamp_for_this_munging;
        InitDroneCollisionObject(pDrone);

        return !DroneHasCollided(pDrone);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00451650
int C2_HOOK_FASTCALL ReallyAddDroneToPHIL(tDrone_spec* pDrone) {

    if (PHILAddObject(&pDrone->collision_info)) {
        return 0;
    }
    PHILSetObjectProperty(&pDrone->collision_info, 0, 0, 1.875);
    PHILSetObjectProperty(&pDrone->collision_info, 3, 1);
    PHILSetObjectProperty(&pDrone->collision_info, 7, 1);
    PHILSetObjectProperty(&pDrone->collision_info, 6, 1);
    return 1;
}

int C2_HOOK_FASTCALL AddDroneToPHIL(tDrone_spec* pDrone) {

    return ReallyAddDroneToPHIL(pDrone);
}

void C2_HOOK_FASTCALL UnPauseDroneState(tDrone_spec* pDrone) {

    if (gDrone_state_functions[pDrone->current_state] != NULL) {
        gDrone_state_functions[pDrone->current_state](pDrone, eDrone_state_DEFAULT);
    }
}

void C2_HOOK_FASTCALL StartProcessingThisDrone(tDrone_spec* pDrone) {
    tDrone_form* form = pDrone->form;

    if (!gShow_drone_paths && pDrone->field_0x44 == 0 && ((form->flags & 0x4) != 0 || gCount_active_drones < 12))  {

        if (CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing(pDrone) && AddDroneToPHIL(pDrone)) {

            BrMatrix34Copy(&pDrone->collision_info.transform_matrix, &pDrone->actor->t.t.mat);
            pDrone->field_0x44 = 1;
            if ((pDrone->form->flags & 0x4) == 0) {
                gCount_active_drones += 1;
            }
            UnPauseDroneState(pDrone);
            DoNotDprintf("PROCESSING ON: Frame %d, Drone %d, state %d", gFrame, pDrone->id, pDrone->current_state);
            CrappyLittleVector3DPrintf("    Pos", &pDrone->actor->t.t.translate.t);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0044f700
void C2_HOOK_FASTCALL InitDrones(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tDrone_spec) != 1496);

    gDrones_unmodified = 1;
    gCount_rendered_drones = 0;
    gCurrent_selected_drone = 0;
    gINT_006820d4 = 0;
    gINT_006844fc = 0;
    gINT_00681fb0 = 0;

    for (i = 0; i < gCount_drones; i++) {
        tDrone_spec* drone = &gDrone_specs[i];
        tDrone_form* form = drone->form;

        NewDroneState(drone, 1);
        if (form->type == kDroneType_plane) {
            StartProcessingThisDrone(drone);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c5e70
void C2_HOOK_FASTCALL FreeThingForm(void* pData) {

    BrMemFree(pData);
}

// FUNCTION: CARMA2_HW 0x0044fc10
void C2_HOOK_FASTCALL DisposeDronesRaceStuff(void) {
    int i;

    if (gCount_drones == 0) {
        return;
    }

    for (i = 0; i < gCount_drones; i++) {
        tDrone_spec* drone = &gDrone_specs[i];

        if (drone->actor->parent != NULL) {
            BrActorRemove(drone->actor);
        }
        BrActorFree(drone->actor);
        PHILRemoveObject(&drone->collision_info);
        if (drone->field_0x46) {
            FreeThingForm(drone->collision_info.shape);
            drone->field_0x46 = 0;
        }

        if (drone->funk_grooves != NULL) {
            BrMemFree(drone->funk_grooves);
            drone->funk_grooves = NULL;
        }
    }

    for (i = 0; i < gCount_drone_forms; i++) {
        tDrone_form* form = &gDrone_forms[i];

        if (form->field_0x80 != NULL) {
            int j;

            BrMemFree(form->field_0x80);
            form->field_0x80 = NULL;
            for (j = 0; j < form->count_models; j++) {
                BrModelRemove(form->models[j]);
                form->models[j]->nfaces = 0;
                form->models[j]->faces = NULL;
                BrModelFree(form->models[j]);
            }
            if (form->models != NULL) {
                BrMemFree(form->models);
                form->models = NULL;
            }
        }
    }
    ClearOutStorageSpace(&gDroneStorage);
    BrMemFree(gDrone_specs);
    gDrone_specs = NULL;
    gCount_drones = 0;
    BrMemFree(gDrone_path_nodes);
    gDrone_path_nodes = NULL;
    gCount_drone_path_nodes = 0;
}

// FUNCTION: CARMA2_HW 0x004020e0
void C2_HOOK_FASTCALL MakeAISimpleEditSectionHere(br_model* pModel, int pVert_index, int pFace_index, tDrone_path_node* pNode, br_vector3* pPos, br_material* pMaterial1, br_material* pMaterial2) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00451e70
void C2_HOOK_FASTCALL DoDroneFunkyGroovyThings(tDrone_spec *pDrone) {
    float delta_time;
    float flt_unk0;
    float flt_unk1;
    int left_turn;
    int i;

    if (gAction_replay_mode) {
        delta_time = gFrame_period / 1000.f;
    } else {
        delta_time = gDrone_delta_time;
    }
    if (gAction_replay_mode) {
        flt_unk0 = pDrone->field_0xe0;
        flt_unk1 = pDrone->field_0xe4;
        if (flt_unk1 < 0.f) {
            flt_unk1 = -flt_unk1;
            left_turn = 0;
        } else {
            left_turn = 1;
        }
    } else {
        flt_unk0 = pDrone->field_0x74;
        flt_unk1 = pDrone->field_0x48;
        left_turn = pDrone->left_turn;
    }
    if (pDrone->funk_grooves == NULL) {
        return;
    }
    if (!gAction_replay_mode && (pDrone->current_state == 3 || pDrone->current_state == 4)) {
        return;
    }
    for (i = 0; i < pDrone->funk_grooves->count; i++) {
        tFunk_groove* groove = &pDrone->funk_grooves->items[i];

        switch (groove->type) {
        case 0: /* steering */
            {
                float angle;
                if (groove->spinny.speed_control) {
                    angle = delta_time * flt_unk1 / groove->spinny.omega;
                } else {
                    angle = 0.f;
                }
                if (groove->spinny.reverse) {
                    angle = -angle;
                }
                switch (groove->spinny.axis) {
                case 0:
                    BrMatrix34PreRotateX(&groove->actor->t.t.mat, (br_angle)(angle * 65536.f));
                    break;
                case 1:
                    BrMatrix34PreRotateY(&groove->actor->t.t.mat, (br_angle)(angle * 65536.f));
                    break;
                case 2:
                    BrMatrix34PreRotateZ(&groove->actor->t.t.mat, (br_angle)(angle * 65536.f));
                    break;
                }
            }
            break;
        case 1: /* spinning */
            {
                float f;
                float factor;
                float angle;
                if (gAction_replay_mode) {
                    if (pDrone->field_0xe8 < 0) {
                        f = 0;
                    } else {
                        f = pDrone->field_0xe8;
                    }
                    if (f > 10.f) {
                        f = 0.f;
                    } else if (pDrone->field_0xe8 < 0.f) {
                        f = 1.f / WORLD_SCALE;
                    } else {
                        f = (10.f - pDrone->field_0xe8) / WORLD_SCALE / 10.f;
                    }
                } else {
                    if (pDrone->field_0xdc == 2) {
                        if (pDrone->h_radius < 0.f) {
                            f = 0.f;
                        } else {
                            f = pDrone->h_radius;
                        }
                        if (f > 10.f) {
                            f = 0.f;
                        } else if (pDrone->h_radius < 0.f) {
                            f = 1.f / WORLD_SCALE;
                        } else {
                            f = (10.f - pDrone->h_radius) / WORLD_SCALE / 10.f;
                        }
                    } else {
                        f = 0.0;
                    }
                }
                if (flt_unk0 < .5f) {
                    factor = flt_unk0;
                } else {
                    factor = 1.f - flt_unk0;
                }
                angle = 2 * factor * f;
                if (groove->steering.reverse) {
                    angle = -angle;
                }
                if (left_turn) {
                    angle = -angle;
                }
                BrVector3Set((br_vector3*)groove->actor->t.t.mat.m[0], 1.f, 0.f, 0.f);
                BrVector3Set((br_vector3*)groove->actor->t.t.mat.m[1], 0.f, 1.f, 0.f);
                BrVector3Set((br_vector3*)groove->actor->t.t.mat.m[2], 0.f, 0.f, 1.f);
                BrMatrix34PreRotateY(&groove->actor->t.t.mat, (br_angle)(angle * 65536.f));
                if (pDrone->field_0xdc == 2) {
                    DoNotDprintf("STEERING GROOVE: rot %f, h_radius %f, reverse %d, left_turn %d",
                        angle, pDrone->h_radius, groove->steering.reverse, pDrone->left_turn);
                }
            }
            break;
        }
    }
}

void C2_HOOK_FASTCALL CrappyLittleDrivingStateInfoDprintf(void) {
}

int C2_HOOK_FASTCALL DroneOrientationChanged(tDrone_spec* pDrone, float pDot) {
    br_actor* actor;

    actor = pDrone->actor;

    if (BrVector3Dot((br_vector3*)pDrone->collision_info.transform_matrix.m[0], (br_vector3*)actor->t.t.mat.m[0]) < pDot) {
        return 1;
    }
    if (BrVector3Dot((br_vector3*)pDrone->collision_info.transform_matrix.m[1], (br_vector3*)actor->t.t.mat.m[1]) < pDot) {
        return 1;
    }
    if (BrVector3Dot((br_vector3*)pDrone->collision_info.transform_matrix.m[2], (br_vector3*)actor->t.t.mat.m[2]) < pDot) {
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00451ca0
void C2_HOOK_FASTCALL ProcessThisDrone(int pIndex) {
    tDrone_spec* drone;

    drone = &gDrone_specs[pIndex];
    BrVector3Copy(&drone->pos, &drone->actor->t.t.translate.t);
    if (gDrone_state_functions[drone->current_state] != NULL) {
        gDrone_state_functions[drone->current_state](drone, eDrone_state_RUN);
    }
    if (drone->field_0x45) {
        DoDroneFunkyGroovyThings(drone);
    }
    BrVector3Sub(&drone->field_0x18, &drone->actor->t.t.translate.t, &drone->pos);
    if (drone->field_0x44 && DroneOrientationChanged(drone, .92f) && BrVector3Length(&drone->field_0x18) > 1.f) {
        BrMatrix34Copy(&drone->collision_info.transform_matrix, &drone->actor->t.t.mat);
        DoNotDprintf("REASSERTING OBJECT MATRIX: v length %f, Frame %d, Drone %d, state %d: ",
            BrVector3Length(&drone->field_0x18), gFrame, drone->id, drone->current_state);
        if (drone->current_state == 2) {
            CrappyLittleDrivingStateInfoDprintf();
        }
    }
    BrVector3InvScale(&drone->field_0x18, &drone->field_0x18, gDrone_delta_time);
}

// FUNCTION: CARMA2_HW 0x00452ba0
void C2_HOOK_FASTCALL InitialiseEditModelsEtc(void) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL UpdateEditModels(void) {

    if (gDrone_path_model_actor != NULL && gElasticatey_drone_path_model != NULL && gDrone_paths_elasticating && gSelected_drone_path_node_index >= 0) {
        MakeAISimpleEditSectionHere(gElasticatey_drone_path_model, 0, 0,
            &gDrone_path_nodes[gSelected_drone_path_node_index],
            &gProgram_state.current_car.car_master_actor->t.t.translate.t,
            gMat_dk_grn, gMat_lt_grn);
        BrModelUpdate(gElasticatey_drone_path_model, BR_MATU_ALL);
    }
    if (gSelected_edit_drone_path >= 0 && gTime_stamp_for_this_munging > gNext_drone_edit_path_munge) {
        gSelected_edit_drone_path = -1;
        InitialiseEditModelsEtc();
    }
    if (gSelected_drone_path_index >= 0) {
        if (gTime_stamp_for_this_munging % 700 <= 700) {
            gDrone_specs[gSelected_drone_path_index].actor->render_style = BR_RSTYLE_FACES;
        } else {
            gDrone_specs[gSelected_drone_path_index].actor->render_style = BR_RSTYLE_BOUNDING_FACES;
        }
    }
}

void C2_HOOK_FASTCALL DoDroneDistanceChecks(tDrone_spec* pDrone) {

    if (gTrack_drone_min_y - 50 > pDrone->actor->t.t.translate.t.v[1]) {
        NewDroneState(pDrone, 5);
    } else {
        int process_drone;

        CalcRenderBoundsCentre();
        process_drone = gDrone_form_within_rendering_distance_functions[pDrone->form->type](&pDrone->actor->t.t.translate.t);
        pDrone->field_0x45 = process_drone;
        if (!process_drone) {
            if (!(pDrone->form->flags & 0x4)) {
                process_drone = gDrone_form_within_processing_distance_functions[pDrone->form->type](&pDrone->actor->t.t.translate.t);
            }
        }
        if (!process_drone) {
            StopProcessingThisDrone(pDrone, 0);
        } else if (!pDrone->field_0x45 || gFirst_drone_processing) {
            StartProcessingThisDrone(pDrone);
        }
        if (pDrone->field_0x45) {
            StartRenderingThisDrone(pDrone);
        } else {
            StopRenderingThisDrone(pDrone);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004512f0
void C2_HOOK_FASTCALL DoDronePerGameFrameStuff(void) {

    UpdateEditModels();
    gINT_006844fc = 0;
    gINT_006820d4 = 0;
    if (gDrones_unmodified) {
        int i;

        for (i = 0; i < gCount_drones; i++) {
            tDrone_spec *drone = &gDrone_specs[i];

            DoDroneDistanceChecks(drone);
            if (drone->current_state != 5) {
                ProcessThisDrone(i);
            }
            DoNotDprintf("FINISHED processing Drone %d, state %d", drone->id, drone->current_state);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004516d0
void C2_HOOK_FASTCALL StopRenderingThisDrone(tDrone_spec* pDrone_spec) {
    if (pDrone_spec->actor->render_style == BR_RSTYLE_FACES) {
        DoNotDprintf("STOP RENDERING: %d", pDrone_spec->id);
        gCount_rendered_drones -= 1;
        pDrone_spec->actor->render_style = BR_RSTYLE_NONE;
        PipeSingleDroneRender(pDrone_spec, 0);
    }
}

void C2_HOOK_FASTCALL RemoveDroneFromPHIL(tDrone_spec* pDrone) {
    ReallyRemoveDroneFromPHIL(pDrone);
}

void C2_HOOK_FASTCALL ReallyRemoveDroneFromPHIL(tDrone_spec* pDrone) {
    PHILRemoveObject(&pDrone->collision_info);
}

void C2_HOOK_FASTCALL PauseDroneState(tDrone_spec* pDrone) {
    if (gDrone_state_functions[pDrone->current_state] != NULL) {
        gDrone_state_functions[pDrone->current_state](pDrone, 2);
    }
}

// FUNCTION: CARMA2_HW 0x00451710
void C2_HOOK_FASTCALL StopProcessingThisDrone(tDrone_spec* pDrone, int pForce) {

    if (pDrone->field_0x44) {
        if (pForce || (!(pDrone->form->flags & 0x4) && pDrone->current_state != 3 && gCount_active_drones > 0)) {
            pDrone->field_0x44 = 0;
            gCount_active_drones -= 1;
            StopRenderingThisDrone(pDrone);
            RemoveDroneFromPHIL(pDrone);
            PauseDroneState(pDrone);
            DoNotDprintf("PROCESSING OFF: Frame %d, Drone %d, state %d", gFrame, pDrone->id, pDrone->current_state);
            CrappyLittleVector3DPrintf("     Pos", &pDrone->actor->t.t.translate.t);
        }
    }
}

void C2_HOOK_FASTCALL ResetDroneModel(tDrone_spec* pDrone) {

    C2_HOOK_BUG_ON(sizeof(pDrone->form->field_0x80[0]) != 1);

    if (pDrone->field_0xf4 >= 0) {
        ResetDroneCrushyModel(gDroneStorage.models[pDrone->form->model_index],
            pDrone->form->models[pDrone->field_0xf4]);
        pDrone->form->field_0x80[pDrone->field_0xf4] = 0;
        pDrone->field_0xf4 = -1;
        pDrone->model_actor->model = gDroneStorage.models[pDrone->form->model_index];
    }
}

void C2_HOOK_FASTCALL SemiInitDroneSpec(tDrone_spec* pDrone) {

    DoNotDprintf("SemiInitDroneSpec() - REINITIALISING DRONE SPEC");
    pDrone->field_0xa_pathnode_id = pDrone->field_0x8_pathnode_id;
    pDrone->field_0xe = -1;
    pDrone->field_0xc = -1;
    pDrone->field_0x14 = 0;
    pDrone->field_0x10 = 0;
    BrVector3Set(&pDrone->field_0x18, 0.f, 0.f, 0.f);
    BrVector3Copy(&pDrone->pos, &gDrone_path_nodes[pDrone->field_0xa_pathnode_id].position);
    pDrone->field_0xdc = 0;
    pDrone->field_0xe4 = 1.0;
    pDrone->field_0x48 = 1.0;
    ResetDroneModel(pDrone);
    pDrone->field_0x45 = gDrone_form_within_rendering_distance_functions[pDrone->form->type](&pDrone->actor->t.t.translate.t);
    pDrone->field_0x5d4 = 0;
    PipeDroneMatrix(pDrone);
}

// FUNCTION: CARMA2_HW 0x0044cc70
void C2_HOOK_FASTCALL DroneStateFuncReset(tDrone_spec* pDrone, tDroneStateFuncState state) {

    switch (state) {
    case eDrone_state_START:
        DoNotDprintf("DroneStateFuncReset() START");
        StopRenderingThisDrone(pDrone);
        StopProcessingThisDrone(pDrone, 1);
        SemiInitDroneSpec(pDrone);
        BrVector3Copy(&pDrone->actor->t.t.translate.t, &gDrone_path_nodes[pDrone->field_0x8_pathnode_id].position);
        break;
    case eDrone_state_RUN:
        DoNotDprintf("DroneStateFuncReset() RUN");
        if (!gShow_drone_paths) {
            NewDroneState(pDrone,2);
        }
        break;
    case eDrone_state_STOP:
        DoNotDprintf("DroneStateFuncReset() STOP");
        break;
    default:
        break;
    }
}

// FUNCTION: CARMA2_HW 0x0044d2a0
void C2_HOOK_FASTCALL MoveThisDronePlane(tDrone_spec* pDrone) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0044e540
void C2_HOOK_FASTCALL MoveThisDroneCar(tDrone_spec* pDrone) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL InitDroneDrivingInfo(tDrone_spec* pDrone) {

    pDrone->field_0xdc = 0;
    pDrone->field_0xe4 = 0.f;
    pDrone->field_0x48 = 0.f;
    pDrone->field_0x4c = 0.f;
}

void C2_HOOK_FASTCALL PossiblyPipeDroneMovement(tDrone_spec* pDrone) {

    switch (pDrone->field_0xdc) {
    case 1:
        PipeSingleDroneStraightPos(pDrone,
            pDrone->field_0x10,
            DRScalarToU16(pDrone->field_0x48, -500.f, 500.f),
            DRScalarToU16(pDrone->field_0x4c, 0.f, 255.f));
        break;
    case 2:
        PipeDroneMatrix(pDrone);
        break;
    default:
        PDEnterDebugger("Keith");
        break;
    }
}

// FUNCTION: CARMA2_HW 0x0044d1d0
void C2_HOOK_FASTCALL DroneStateFuncControlledMovement(tDrone_spec* pDrone, tDroneStateFuncState state) {

    switch (state) {
    case eDrone_state_START:
        DoNotDprintf("DroneStateFuncControlledMovement() START");
        InitDroneDrivingInfo(pDrone);
        break;
    case eDrone_state_RUN:
        DoNotDprintf("DroneStateFuncControlledMovement() RUN");
        if (pDrone->field_0x44) {
            if (pDrone->form->type == kDroneType_plane) {
                MoveThisDronePlane(pDrone);
            } else {
                MoveThisDroneCar(pDrone);
            }
            PossiblyPipeDroneMovement(pDrone);
            return;
        }
        break;
    default:
        DoNotDprintf("DroneStateFuncControlledMovement() DEFAULT");
        break;
    }
}

// FUNCTION: CARMA2_HW 0x0044eb70
void C2_HOOK_FASTCALL DroneStateFuncPhysicsActive(tDrone_spec* pDrone, tDroneStateFuncState state) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0044ec50
void C2_HOOK_FASTCALL DroneStateFuncStationaryPassive(tDrone_spec* pDrone, tDroneStateFuncState state) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0044ca90
int C2_HOOK_FASTCALL DroneCarWithinRenderingDistance(const br_vector3* pPos) {

    return fabsf(gRender_bounds_centre.v[0] - pPos->v[0]) < 15.f
        && fabsf(gRender_bounds_centre.v[1] - pPos->v[1]) < 12.f
        && fabsf(gRender_bounds_centre.v[2] - pPos->v[2]) < 15.f;
}

// FUNCTION: CARMA2_HW 0x0044cae0
int C2_HOOK_FASTCALL DronePlaneWithinRenderingDistance(const br_vector3* pPos) {

    return fabsf(gRender_bounds_centre.v[0] - pPos->v[0]) < 40.f
           && fabsf(gRender_bounds_centre.v[1] - pPos->v[1]) < 30.f
           && fabsf(gRender_bounds_centre.v[2] - pPos->v[2]) < 40.f;
}

// FUNCTION: CARMA2_HW 0x0044cb30
int C2_HOOK_FASTCALL DroneTrainWithinRenderingDistance(const br_vector3* pPos) {

    return fabsf(gRender_bounds_centre.v[0] - pPos->v[0]) < 50.f
           && fabsf(gRender_bounds_centre.v[1] - pPos->v[1]) < 20.f
           && fabsf(gRender_bounds_centre.v[2] - pPos->v[2]) < 50.f;

}

// FUNCTION: CARMA2_HW 0x0044cb80
int C2_HOOK_FASTCALL DroneCarWithinProcessingDistance(const br_vector3* pPos) {

    return fabsf(gRender_bounds_centre.v[0] - pPos->v[0]) < 18.f
           && fabsf(gRender_bounds_centre.v[1] - pPos->v[1]) < 13.f
           && fabsf(gRender_bounds_centre.v[2] - pPos->v[2]) < 18.f;
}

// FUNCTION: CARMA2_HW 0x0044cbd0
int C2_HOOK_FASTCALL DronePlaneWithinProcessingDistance(const br_vector3* pPos) {

    return fabsf(gRender_bounds_centre.v[0] - pPos->v[0]) < 43.f
           && fabsf(gRender_bounds_centre.v[1] - pPos->v[1]) < 31.f
           && fabsf(gRender_bounds_centre.v[2] - pPos->v[2]) < 43.f;
}

// FUNCTION: CARMA2_HW 0x0044cc20
int C2_HOOK_FASTCALL DroneTrainWithinProcessingDistance(const br_vector3* pPos) {

    return fabsf(gRender_bounds_centre.v[0] - pPos->v[0]) < 43.f
           && fabsf(gRender_bounds_centre.v[1] - pPos->v[1]) < 31.f
           && fabsf(gRender_bounds_centre.v[2] - pPos->v[2]) < 43.f;

}

// FUNCTION: CARMA2_HW 0x00451bd0
void C2_HOOK_FASTCALL CalcRenderBoundsCentre(void) {
    br_vector3 tv;

    BrVector3Negate(&tv, (br_vector3 *) gCamera_to_world.m[2]);
    BrVector3Normalise(&tv, &tv);
    BrVector3Scale(&tv, &tv, 5.f);
    BrVector3Add(&gRender_bounds_centre, (br_vector3 *) gCamera_to_world.m[3], &tv);
}

// FUNCTION: CARMA2_HW 0x0044cfd0
void C2_HOOK_FASTCALL PipeDroneMatrix(tDrone_spec* pDrone) {
    tCompressed_matrix34 compressed_mat;
    tS16 compressed_field_0x70;
    tS16 compressed_field_0x48;
    tS16 compressed_field_0x74;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_spec, field_0x48, 0x48);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_spec, left_turn, 0xd8);
    C2_HOOK_BUG_ON(sizeof(pDrone->left_turn) != 1);

    compressed_mat.m[0].v[0] = DRScalarToU16(pDrone->actor->t.t.mat.m[0][0], -1.f, 1.f);
    compressed_mat.m[0].v[1] = DRScalarToU16(pDrone->actor->t.t.mat.m[0][1], -1.f, 1.f);
    compressed_mat.m[0].v[2] = DRScalarToU16(pDrone->actor->t.t.mat.m[0][2], -1.f, 1.f);
    compressed_mat.m[1].v[0] = DRScalarToU16(pDrone->actor->t.t.mat.m[1][0], -1.f, 1.f);
    compressed_mat.m[1].v[1] = DRScalarToU16(pDrone->actor->t.t.mat.m[1][1], -1.f, 1.f);
    compressed_mat.m[1].v[2] = DRScalarToU16(pDrone->actor->t.t.mat.m[1][2], -1.f, 1.f);
    compressed_mat.m[2].v[0] = DRScalarToU16(pDrone->actor->t.t.mat.m[2][0], -1.f, 1.f);
    compressed_mat.m[2].v[1] = DRScalarToU16(pDrone->actor->t.t.mat.m[2][1], -1.f, 1.f);
    compressed_mat.m[2].v[2] = DRScalarToU16(pDrone->actor->t.t.mat.m[2][2], -1.f, 1.f);
    compressed_mat.m[3].v[0] = DRScalarToU16(pDrone->actor->t.t.mat.m[3][0], -1000.f, 1000.f);
    compressed_mat.m[3].v[1] = DRScalarToU16(pDrone->actor->t.t.mat.m[3][1], -1000.f, 1000.f);
    compressed_mat.m[3].v[2] = DRScalarToU16(pDrone->actor->t.t.mat.m[3][2], -1000.f, 1000.f);

    compressed_field_0x70 = DRScalarToU16(MIN(100.f, pDrone->h_radius), 0.f, 100.f);
    compressed_field_0x74 = DRScalarToU16(pDrone->field_0x74, 0.f, 1.f);
    compressed_field_0x48 = DRScalarToU16(pDrone->left_turn ? pDrone->field_0x48 : -pDrone->field_0x48, -500.f, 500.f);
    PipeSingleDroneCornerPos(pDrone, compressed_field_0x48, compressed_field_0x74, compressed_field_0x70, &compressed_mat);
}

// FUNCTION: CARMA2_HW 0x00452680
int C2_HOOK_FASTCALL OKToViewDrones(void) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x00452690
br_matrix34* C2_HOOK_FASTCALL GetCurrentViewDroneMat(void) {

    return &gDrone_specs[gCurrent_selected_drone].actor->t.t.mat;
}

// FUNCTION: CARMA2_HW 0x00451810
void C2_HOOK_FASTCALL StartRenderingThisDrone(tDrone_spec* pDrone) {

    if (pDrone->actor->render_style == BR_RSTYLE_FACES) {
        return;
    }
    if (gShow_drone_paths && pDrone->actor->render_style == BR_RSTYLE_BOUNDING_FACES) {
        return;
    }
    if (gShow_drone_paths || (pDrone->field_0x44 && gCount_rendered_drones < 10)) {

        DoNotDprintf("START RENDERING: %d", pDrone->id);
        gCount_rendered_drones += 1;
        pDrone->actor->render_style = BR_RSTYLE_FACES;
        PipeSingleDroneRender(pDrone, 1);
    } else {
        NewDroneState(pDrone,1);
    }
}

// FUNCTION: CARMA2_HW 0x004526c0
br_vector3* C2_HOOK_FASTCALL GetCurrentViewDroneDirection(void) {

    StartRenderingThisDrone(&gDrone_specs[gCurrent_selected_drone]);
    if (BrVector3LengthSquared(&gDrone_specs[gCurrent_selected_drone].field_0x18) > 0.f) {
        return &gDrone_specs[gCurrent_selected_drone].field_0x18;
    } else {
        return &gDefault_drone_direction;
    }
}
