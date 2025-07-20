#include "drone.h"

#include "compress.h"
#include "errors.h"
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

#include "c2_stdio.h"
#include "c2_string.h"

#include "rec2_macros.h"

#include <stdarg.h>

C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_drones, 0x006820d0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tDrone_form, gDrone_forms, 64, 0x00682178);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_drone_forms, 0x0068450c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(tDrone_state_function*, gDrone_state_functions, 6, 0x00594738, {
    NULL,
    DroneStateFuncReset,
    DroneStateFuncControlledMovement,
    DroneStateFuncPhysicsActive,
    DroneStateFuncStationaryPassive,
    NULL,
});
C2_HOOK_VARIABLE_IMPLEMENT(tCollision_info*, gList_collision_infos, 0x0074a5f0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDrones_unmodified, 0x006820b8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_rendered_drones, 0x00684500);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCurrent_selected_drone, 0x006844f8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_006820d4, 0x006820d4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_006844fc, 0x006844fc);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_00681fb0, 0x00681fb0);
C2_HOOK_VARIABLE_IMPLEMENT(tDrone_spec*, gDrone_specs, 0x00684504);
C2_HOOK_VARIABLE_IMPLEMENT(int, gShow_drone_paths, 0x0068452c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_active_drones, 0x00681fb4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrame, 0x00684514);
C2_HOOK_VARIABLE_IMPLEMENT(tDrone_path_node*, gDrone_path_nodes, 0x00684508);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_drone_path_nodes, 0x00684510);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gRender_bounds_centre, 0x006820c0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(tDrone_form_within_rendering_distance_cbfn*, gDrone_form_within_rendering_distance_functions, 4, 0x00594760, {
    DroneCarWithinRenderingDistance,
    DronePlaneWithinRenderingDistance,
    DroneTrainWithinRenderingDistance,
    DronePlaneWithinRenderingDistance,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(tDrone_form_within_processing_distance_cbfn*, gDrone_form_within_processing_distance_functions, 4, 0x00594760, {
    DroneCarWithinProcessingDistance,
    DronePlaneWithinProcessingDistance,
    DroneTrainWithinProcessingDistance,
    DronePlaneWithinProcessingDistance,
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_vector3, gDefault_drone_direction, 0x005947c0, { 0.f, 0.f, -1.f });
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gElasticatey_drone_path_model, 0x00684520);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gDrone_path_model_actor, 0x00684528);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDrone_paths_elasticating, 0x00684530);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gSelected_drone_path_node_index, 0x005947b0, -1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gSelected_edit_drone_path, 0x005947b4, -1);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gNext_drone_edit_path_munge, 0x006820cc);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gSelected_drone_path_index, 0x005947b8, -1);

void C2_HOOK_CDECL DoNotDprintf(const char* format, ...) {
// Disabled because too noisy
#if 0
    va_list ap;

    va_start(ap, format);
    c2_vfprintf(c2_stderr, format, ap);
    c2_fputc('\n', c2_stderr);
    va_end(ap);
#endif
}
C2_HOOK_FUNCTION(0x0044cfc0, DoNotDprintf)

void C2_HOOK_FASTCALL InitDroneSpec(tDrone_spec* pDrone_spec, int pNode) {

    C2_HOOK_BUG_ON(sizeof(*pDrone_spec) != 0x5d8);

    DoNotDprintf("InitDroneSpec() - INITIALISING DRONE SPEC");
    c2_memset(pDrone_spec, 0, sizeof(*pDrone_spec));
    pDrone_spec->field_0xa_pathnode_id = pNode;
    pDrone_spec->field_0x8_pathnode_id = pNode;
    pDrone_spec->field_0xe = -1;
    pDrone_spec->field_0xc = -1;
    BrVector3Set(&pDrone_spec->field_0x18, 0.f, 0.f, 0.f);
    BrVector3Copy(&pDrone_spec->pos, &C2V(gDrone_path_nodes)[pNode].position);
    pDrone_spec->field_0xdc = 0;
    pDrone_spec->field_0xf4 = -1;
    pDrone_spec->current_state = 0;
    pDrone_spec->field_0x5d4 = 0;
    pDrone_spec->form = &C2V(gDrone_forms)[C2V(gDrone_path_nodes)[pNode].type];
    pDrone_spec->field_0xe4 = 1.0f;
    pDrone_spec->field_0x48 = 1.0f;
}
C2_HOOK_FUNCTION(0x00451210, InitDroneSpec)

void C2_HOOK_FASTCALL PreprocessDronePaths(void) {
    int i;
    for (i = 0; i < C2V(gCount_drone_path_nodes); i++) {
        tDrone_path_node* node;
        int j;

        node = &C2V(gDrone_path_nodes)[i];
        for (j = 0; j < node->count_sections; j++) {
            tDrone_path_node_section* section;

            section = &node->sections[j];
            BrVector3Sub(&section->field_0x04, &C2V(gDrone_path_nodes)[section->node1].position, &node->position);
            section->field_0x1c = BrVector3Length(&section->field_0x04);
            BrVector3Normalise(&section->field_0x10, &section->field_0x04);
        }
    }
}
C2_HOOK_FUNCTION(0x00451070, PreprocessDronePaths)

void C2_HOOK_FASTCALL AllocateAndInitDrones(void) {
    int i;
    int drone_i;

    C2_HOOK_BUG_ON(sizeof(tDrone_spec) != 0x5d8);

    C2V(gDrone_specs) = BrMemAllocate(C2V(gCount_drones) * sizeof(tDrone_spec), kMem_drone_specs);
    drone_i = 0;
    for (i = 0; i < C2V(gCount_drone_path_nodes); i++) {
        tDrone_path_node* node;

        node = &C2V(gDrone_path_nodes)[i];
        if (node->type >= 0) {
            InitDroneSpec(&C2V(gDrone_specs)[drone_i], i);
            drone_i += 1;
        }
    }
}

void (C2_HOOK_FASTCALL * LoadInDronePaths_original)(FILE* pF);
void C2_HOOK_FASTCALL LoadInDronePaths(FILE* pF) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadInDronePaths_original(pF);
#else
    char s[256];

    DoNotDprintf("Start of LoadInDronePaths()...");
    for (;;) {
        if (!GetALineAndDontArgue(pF, s)) {
            PDFatalError("Corrupt race text file - No drone path info");
            break;
        }
        if (c2_strcmp(s, "START OF DRONE PATHS") == 0) {
            break;
        }
    }
    C2V(gCount_drones) = 0;
    C2V(gCount_drone_path_nodes) = 0;
    if (C2V(gNet_mode) == eNet_mode_none && !C2V(gDronesOff)) {
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
            C2V(gCount_drone_path_nodes) = GetAnInt(pF);
            if (C2V(gCount_drone_path_nodes) != 0) {
                C2V(gDrone_path_nodes) = BrMemAllocate(C2V(gCount_drone_path_nodes) * sizeof(tDrone_path_node), kMem_drone_paths);
            }
            for (i = 0; i < C2V(gCount_drone_path_nodes); i++) {
                br_scalar float_buffer[4];
                tDrone_path_node *node = &C2V(gDrone_path_nodes)[i];
                int j;

                GetNScalars(pF, 3, float_buffer);
                node->position.v[0] = float_buffer[0];
                node->position.v[1] = float_buffer[1];
                node->position.v[2] = float_buffer[2];

                s[0] = '\0';
                GetAString(pF, s);
                Uppercaseificate(s, s);
                node->type = -1;
                if (c2_strlen(s) == 0) {
                    c2_sprintf(s, "Corrupt race text file - drone type corrupt in drone node %d", i);
                    PDFatalError(s);
                }

                if (c2_strcmp(s, "NONE") != 0) {
                    for (j = 0; j < C2V(gCount_drone_forms); j++) {
                        if (c2_strcmp(C2V(gDrone_forms)[j].name, s) == 0) {
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
                    if (C2V(gCount_drones) >= 200) {
                        c2_sprintf(s, "Too many drones in race (limit %d)", 200);
                        PDFatalError(s);
                    }
                    C2V(gCount_drones) += 1;
                }
            }
        }
    }
    for (;;) {
        const char *str = GetALineAndDontArgue(pF, s);
        if (str == NULL) {
            break;
        }
        if (c2_strcmp(s, "END OF DRONE PATHS") == 0) {
            break;
        }
    }
    PreprocessDronePaths();
    AllocateAndInitDrones();
    DoNotDprintf("End of LoadInDronePaths(), totals:");
    DoNotDprintf("Nodes: %d", C2V(gCount_drone_path_nodes));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00450bf0, LoadInDronePaths, LoadInDronePaths_original)

void C2_HOOK_FASTCALL DoDefaultDroneStateAction(tDrone_spec* pDrone_spec) {

    if (C2V(gDrone_state_functions)[pDrone_spec->current_state] != NULL) {
        C2V(gDrone_state_functions)[pDrone_spec->current_state](pDrone_spec, eDrone_state_DEFAULT);
    }
}
C2_HOOK_FUNCTION(0x004516b0, DoDefaultDroneStateAction)

void C2_HOOK_FASTCALL NewDroneState(tDrone_spec* pDrone_spec, int pNew_state) {
    DoNotDprintf("NewDroneState() Drone %d, Current state %d, New state %d",
        pDrone_spec->id, pDrone_spec->current_state, pNew_state);
    if (pDrone_spec->current_state != pNew_state) {
        if (C2V(gDrone_state_functions)[pDrone_spec->current_state] != NULL) {
            C2V(gDrone_state_functions)[pDrone_spec->current_state](pDrone_spec, eDrone_state_STOP);
        }
        pDrone_spec->time_last_munge = C2V(gTime_stamp_for_this_munging);
        pDrone_spec->prev_states[1] = pDrone_spec->prev_states[0];
        pDrone_spec->prev_states[0] = pDrone_spec->current_state;
        pDrone_spec->current_state = pNew_state;
        if (C2V(gDrone_state_functions)[pNew_state] != NULL) {
            C2V(gDrone_state_functions)[pNew_state](pDrone_spec, eDrone_state_START);
        }
    }
}

int (C2_HOOK_FASTCALL * TestObjectOverlap_original)(tCollision_info* pCollision_1, tCollision_info* pCollision_2);
int C2_HOOK_FASTCALL TestObjectOverlap(tCollision_info* pCollision_1, tCollision_info* pCollision_2) {

#if defined(C2_HOOKS_ENABLED)
    return TestObjectOverlap_original(pCollision_1, pCollision_2);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00428d40, TestObjectOverlap, TestObjectOverlap_original)

int C2_HOOK_FASTCALL DroneHasCollided(tDrone_spec* pDrone_spec) {
    tCollision_info* collision = C2V(gList_collision_infos);
    for (collision = C2V(gList_collision_infos); collision != NULL; collision = collision->next) {
        if (&pDrone_spec->collision_info != collision && TestObjectOverlap(&pDrone_spec->collision_info, collision)) {
            return 1;
        }
    }
    return 0;
}

void (C2_HOOK_FASTCALL * InitDroneCollisionInfo_original)(tDrone_spec *pDrone_spec);
void C2_HOOK_FASTCALL InitDroneCollisionObject(tDrone_spec *pDrone_spec) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitDroneCollisionInfo_original(pDrone_spec);
#else
    br_bounds3 bnds;
    br_vector3 size;
    tCollision_shape* original_shape;

    BrActorToBounds(&bnds, pDrone_spec->actor);
    bnds.min.v[1] += .001f;
    if (pDrone_spec->field_0x46) {
        original_shape = pDrone_spec->collision_info.shape;
    }
    c2_memset(&pDrone_spec->collision_info, 0, sizeof(pDrone_spec->collision_info));
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
    pDrone_spec->collision_info.box_face_ref = C2V(gFace_num__car) - 2;
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
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044f980, InitDroneCollisionObject, InitDroneCollisionInfo_original)

void C2_HOOK_FASTCALL CrappyLittleVector3DPrintf(const char* pMessage, br_vector3* pPosition) {
    DoNotDprintf("%d: %s: %3.3f, %3.3f, %3.3f", C2V(gFrame), pMessage, pPosition->v[0], pPosition->v[1], pPosition->v[2]);
}
C2_HOOK_FUNCTION(0x00451620, CrappyLittleVector3DPrintf)

int C2_HOOK_FASTCALL CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing(tDrone_spec* pDrone) {

    if (C2V(gTime_stamp_for_this_munging) > pDrone->last_collide_check + 1000) {
        pDrone->last_collide_check = C2V(gTime_stamp_for_this_munging);
        InitDroneCollisionObject(pDrone);

        return !DroneHasCollided(pDrone);
    } else {
        return 0;
    }
}
C2_HOOK_FUNCTION(0x004518e0, CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing)

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
C2_HOOK_FUNCTION(0x00451650, ReallyAddDroneToPHIL)

int C2_HOOK_FASTCALL AddDroneToPHIL(tDrone_spec* pDrone) {

    return ReallyAddDroneToPHIL(pDrone);
}

void C2_HOOK_FASTCALL UnPauseDroneState(tDrone_spec* pDrone) {

    if (C2V(gDrone_state_functions)[pDrone->current_state] != NULL) {
        C2V(gDrone_state_functions)[pDrone->current_state](pDrone, eDrone_state_DEFAULT);
    }
}

void C2_HOOK_FASTCALL StartProcessingThisDrone(tDrone_spec* pDrone) {
    tDrone_form* form = pDrone->form;

    if (!C2V(gShow_drone_paths) && pDrone->field_0x44 == 0 && ((form->flags & 0x4) != 0 || C2V(gCount_active_drones) < 12))  {

        if (CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing(pDrone) && AddDroneToPHIL(pDrone)) {

            BrMatrix34Copy(&pDrone->collision_info.transform_matrix, &pDrone->actor->t.t.mat);
            pDrone->field_0x44 = 1;
            if ((pDrone->form->flags & 0x4) == 0) {
                C2V(gCount_active_drones) += 1;
            }
            UnPauseDroneState(pDrone);
            DoNotDprintf("PROCESSING ON: Frame %d, Drone %d, state %d", C2V(gFrame), pDrone->id, pDrone->current_state);
            CrappyLittleVector3DPrintf("    Pos", &pDrone->actor->t.t.translate.t);
        }
    }
}

void C2_HOOK_FASTCALL InitDrones(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tDrone_spec) != 1496);

    C2V(gDrones_unmodified) = 1;
    C2V(gCount_rendered_drones) = 0;
    C2V(gCurrent_selected_drone) = 0;
    C2V(gINT_006820d4) = 0;
    C2V(gINT_006844fc) = 0;
    C2V(gINT_00681fb0) = 0;

    for (i = 0; i < C2V(gCount_drones); i++) {
        tDrone_spec* drone = &C2V(gDrone_specs)[i];
        tDrone_form* form = drone->form;

        NewDroneState(drone, 1);
        if (form->type == kDroneType_plane) {
            StartProcessingThisDrone(drone);
        }
    }
}
C2_HOOK_FUNCTION(0x0044f700, InitDrones)

void C2_HOOK_FASTCALL FreeThingForm(void* pData) {

    BrMemFree(pData);
}
C2_HOOK_FUNCTION(0x004c5e70, FreeThingForm)

void C2_HOOK_FASTCALL DisposeDronesRaceStuff(void) {
    int i;

    if (C2V(gCount_drones) == 0) {
        return;
    }

    for (i = 0; i < C2V(gCount_drones); i++) {
        tDrone_spec* drone = &C2V(gDrone_specs)[i];

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

    for (i = 0; i < C2V(gCount_drone_forms); i++) {
        tDrone_form* form = &C2V(gDrone_forms)[i];

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
    ClearOutStorageSpace(&C2V(gDroneStorage));
    BrMemFree(C2V(gDrone_specs));
    C2V(gDrone_specs) = NULL;
    C2V(gCount_drones) = 0;
    BrMemFree(C2V(gDrone_path_nodes));
    C2V(gDrone_path_nodes) = NULL;
    C2V(gCount_drone_path_nodes) = 0;
}
C2_HOOK_FUNCTION(0x0044fc10, DisposeDronesRaceStuff)

void (C2_HOOK_FASTCALL * MakeAISimpleEditSectionHere_original)(br_model* pModel, int pVert_index, int pFace_index, tDrone_path_node* pNode, br_vector3* pPos, br_material* pMaterial1, br_material* pMaterial2);
void C2_HOOK_FASTCALL MakeAISimpleEditSectionHere(br_model* pModel, int pVert_index, int pFace_index, tDrone_path_node* pNode, br_vector3* pPos, br_material* pMaterial1, br_material* pMaterial2) {

#if defined(C2_HOOKS_ENABLED)
    MakeAISimpleEditSectionHere_original(pModel, pVert_index, pFace_index, pNode, pPos, pMaterial1, pMaterial2);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004020e0, MakeAISimpleEditSectionHere, MakeAISimpleEditSectionHere_original)

void (C2_HOOK_FASTCALL * DoDroneFunkyGroovyThings_original)(tDrone_spec *pDrone);
void C2_HOOK_FASTCALL DoDroneFunkyGroovyThings(tDrone_spec *pDrone) {

#if 0//defined(C2_HOOKS_ENABLED)
    DoDroneFunkyGroovyThings_original(pDrone);
#else
    float delta_time;
    float flt_unk0;
    float flt_unk1;
    int left_turn;
    int i;

    if (C2V(gAction_replay_mode)) {
        delta_time = C2V(gFrame_period) / 1000.f;
    } else {
        delta_time = C2V(gDrone_delta_time);
    }
    if (C2V(gAction_replay_mode)) {
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
    if (!C2V(gAction_replay_mode) && (pDrone->current_state == 3 || pDrone->current_state == 4)) {
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
                if (C2V(gAction_replay_mode)) {
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
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00451e70, DoDroneFunkyGroovyThings, DoDroneFunkyGroovyThings_original)

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

void (C2_HOOK_FASTCALL * ProcessThisDrone_original)(int pIndex);
void C2_HOOK_FASTCALL ProcessThisDrone(int pIndex) {

#if 0//defined(C2_HOOKS_ENABLED)
    ProcessThisDrone_original(pIndex);
#else
    tDrone_spec* drone;

    drone = &C2V(gDrone_specs)[pIndex];
    BrVector3Copy(&drone->pos, &drone->actor->t.t.translate.t);
    if (C2V(gDrone_state_functions)[drone->current_state] != NULL) {
        C2V(gDrone_state_functions)[drone->current_state](drone, eDrone_state_RUN);
    }
    if (drone->field_0x45) {
        DoDroneFunkyGroovyThings(drone);
    }
    BrVector3Sub(&drone->field_0x18, &drone->actor->t.t.translate.t, &drone->pos);
    if (drone->field_0x44 && DroneOrientationChanged(drone, .92f) && BrVector3Length(&drone->field_0x18) > 1.f) {
        BrMatrix34Copy(&drone->collision_info.transform_matrix, &drone->actor->t.t.mat);
        DoNotDprintf("REASSERTING OBJECT MATRIX: v length %f, Frame %d, Drone %d, state %d: ",
            BrVector3Length(&drone->field_0x18), C2V(gFrame), drone->id, drone->current_state);
        if (drone->current_state == 2) {
            CrappyLittleDrivingStateInfoDprintf();
        }
    }
    BrVector3InvScale(&drone->field_0x18, &drone->field_0x18, C2V(gDrone_delta_time));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00451ca0, ProcessThisDrone, ProcessThisDrone_original)

void (C2_HOOK_FASTCALL * InitialiseEditModelsEtc_original)(void);
void C2_HOOK_FASTCALL InitialiseEditModelsEtc(void) {

#if defined(C2_HOOKS_ENABLED)
    InitialiseEditModelsEtc_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00452ba0, InitialiseEditModelsEtc, InitialiseEditModelsEtc_original)

void C2_HOOK_FASTCALL UpdateEditModels(void) {

    if (C2V(gDrone_path_model_actor) != NULL && C2V(gElasticatey_drone_path_model) != NULL && C2V(gDrone_paths_elasticating) && C2V(gSelected_drone_path_node_index) >= 0) {
        MakeAISimpleEditSectionHere(C2V(gElasticatey_drone_path_model), 0, 0,
            &C2V(gDrone_path_nodes)[C2V(gSelected_drone_path_node_index)],
            &C2V(gProgram_state).current_car.car_master_actor->t.t.translate.t,
            C2V(gMat_dk_grn), C2V(gMat_lt_grn));
        BrModelUpdate(C2V(gElasticatey_drone_path_model), BR_MATU_ALL);
    }
    if (C2V(gSelected_edit_drone_path) >= 0 && C2V(gTime_stamp_for_this_munging) > C2V(gNext_drone_edit_path_munge)) {
        C2V(gSelected_edit_drone_path) = -1;
        InitialiseEditModelsEtc();
    }
    if (C2V(gSelected_drone_path_index) >= 0) {
        if (C2V(gTime_stamp_for_this_munging) % 700 <= 700) {
            C2V(gDrone_specs)[C2V(gSelected_drone_path_index)].actor->render_style = BR_RSTYLE_FACES;
        } else {
            C2V(gDrone_specs)[C2V(gSelected_drone_path_index)].actor->render_style = BR_RSTYLE_BOUNDING_FACES;
        }
    }
}

void C2_HOOK_FASTCALL DoDroneDistanceChecks(tDrone_spec* pDrone) {

    if (C2V(gTrack_drone_min_y) - 50 > pDrone->actor->t.t.translate.t.v[1]) {
        NewDroneState(pDrone, 5);
    } else {
        int process_drone;

        CalcRenderBoundsCentre();
        process_drone = C2V(gDrone_form_within_rendering_distance_functions)[pDrone->form->type](&pDrone->actor->t.t.translate.t);
        pDrone->field_0x45 = process_drone;
        if (!process_drone) {
            if (!(pDrone->form->flags & 0x4)) {
                process_drone = C2V(gDrone_form_within_processing_distance_functions)[pDrone->form->type](&pDrone->actor->t.t.translate.t);
            }
        }
        if (!process_drone) {
            StopProcessingThisDrone(pDrone, 0);
        } else if (!pDrone->field_0x45 || C2V(gFirst_drone_processing)) {
            StartProcessingThisDrone(pDrone);
        }
        if (pDrone->field_0x45) {
            StartRenderingThisDrone(pDrone);
        } else {
            StopRenderingThisDrone(pDrone);
        }
    }
}

void (C2_HOOK_FASTCALL * DoDronePerGameFrameStuff_original)(void);
void C2_HOOK_FASTCALL DoDronePerGameFrameStuff(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    DoDronePerGameFrameStuff_original();
#else

    UpdateEditModels();
    C2V(gINT_006844fc) = 0;
    C2V(gINT_006820d4) = 0;
    if (C2V(gDrones_unmodified)) {
        int i;

        for (i = 0; i < C2V(gCount_drones); i++) {
            tDrone_spec *drone = &C2V(gDrone_specs)[i];

            DoDroneDistanceChecks(drone);
            if (drone->current_state != 5) {
                ProcessThisDrone(i);
            }
            DoNotDprintf("FINISHED processing Drone %d, state %d", drone->id, drone->current_state);
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004512f0, DoDronePerGameFrameStuff, DoDronePerGameFrameStuff_original)

void C2_HOOK_FASTCALL StopRenderingThisDrone(tDrone_spec* pDrone_spec) {
    if (pDrone_spec->actor->render_style == BR_RSTYLE_FACES) {
        DoNotDprintf("STOP RENDERING: %d", pDrone_spec->id);
        C2V(gCount_rendered_drones) -= 1;
        pDrone_spec->actor->render_style = BR_RSTYLE_NONE;
        PipeSingleDroneRender(pDrone_spec, 0);
    }
}
C2_HOOK_FUNCTION(0x004516d0, StopRenderingThisDrone)

void C2_HOOK_FASTCALL RemoveDroneFromPHIL(tDrone_spec* pDrone) {
    ReallyRemoveDroneFromPHIL(pDrone);
}

void C2_HOOK_FASTCALL ReallyRemoveDroneFromPHIL(tDrone_spec* pDrone) {
    PHILRemoveObject(&pDrone->collision_info);
}

void C2_HOOK_FASTCALL PauseDroneState(tDrone_spec* pDrone) {
    if (C2V(gDrone_state_functions)[pDrone->current_state] != NULL) {
        C2V(gDrone_state_functions)[pDrone->current_state](pDrone, 2);
    }
}

void C2_HOOK_FASTCALL StopProcessingThisDrone(tDrone_spec* pDrone, int pForce) {

    if (pDrone->field_0x44) {
        if (pForce || (!(pDrone->form->flags & 0x4) && pDrone->current_state != 3 && C2V(gCount_active_drones) > 0)) {
            pDrone->field_0x44 = 0;
            C2V(gCount_active_drones) -= 1;
            StopRenderingThisDrone(pDrone);
            RemoveDroneFromPHIL(pDrone);
            PauseDroneState(pDrone);
            DoNotDprintf("PROCESSING OFF: Frame %d, Drone %d, state %d", C2V(gFrame), pDrone->id, pDrone->current_state);
            CrappyLittleVector3DPrintf("     Pos", &pDrone->actor->t.t.translate.t);
        }
    }
}
C2_HOOK_FUNCTION(0x00451710, StopProcessingThisDrone)

void C2_HOOK_FASTCALL ResetDroneModel(tDrone_spec* pDrone) {

    C2_HOOK_BUG_ON(sizeof(pDrone->form->field_0x80[0]) != 1);

    if (pDrone->field_0xf4 >= 0) {
        ResetDroneCrushyModel(C2V(gDroneStorage).models[pDrone->form->model_index],
            pDrone->form->models[pDrone->field_0xf4]);
        pDrone->form->field_0x80[pDrone->field_0xf4] = 0;
        pDrone->field_0xf4 = -1;
        pDrone->model_actor->model = C2V(gDroneStorage).models[pDrone->form->model_index];
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
    BrVector3Copy(&pDrone->pos, &C2V(gDrone_path_nodes)[pDrone->field_0xa_pathnode_id].position);
    pDrone->field_0xdc = 0;
    pDrone->field_0xe4 = 1.0;
    pDrone->field_0x48 = 1.0;
    ResetDroneModel(pDrone);
    pDrone->field_0x45 = C2V(gDrone_form_within_rendering_distance_functions)[pDrone->form->type](&pDrone->actor->t.t.translate.t);
    pDrone->field_0x5d4 = 0;
    PipeDroneMatrix(pDrone);
}

void (C2_HOOK_FASTCALL * DroneStateFuncReset_original)(tDrone_spec* pDrone, tDroneStateFuncState state);
void C2_HOOK_FASTCALL DroneStateFuncReset(tDrone_spec* pDrone, tDroneStateFuncState state) {

#if 0//defined(C2_HOOKS_ENABLED)
    DroneStateFuncReset_original(pDrone, state);
#else
    switch (state) {
    case eDrone_state_START:
        DoNotDprintf("DroneStateFuncReset() START");
        StopRenderingThisDrone(pDrone);
        StopProcessingThisDrone(pDrone, 1);
        SemiInitDroneSpec(pDrone);
        BrVector3Copy(&pDrone->actor->t.t.translate.t, &C2V(gDrone_path_nodes)[pDrone->field_0x8_pathnode_id].position);
        break;
    case eDrone_state_RUN:
        DoNotDprintf("DroneStateFuncReset() RUN");
        if (!C2V(gShow_drone_paths)) {
            NewDroneState(pDrone,2);
        }
        break;
    case eDrone_state_STOP:
        DoNotDprintf("DroneStateFuncReset() STOP");
        break;
    default:
        break;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044cc70, DroneStateFuncReset, DroneStateFuncReset_original)

void (C2_HOOK_FASTCALL * MoveThisDronePlane_original)(tDrone_spec* pDrone);
void C2_HOOK_FASTCALL MoveThisDronePlane(tDrone_spec* pDrone) {

#if defined(C2_HOOKS_ENABLED)
    MoveThisDronePlane_original(pDrone);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044d2a0, MoveThisDronePlane, MoveThisDronePlane_original)

void (C2_HOOK_FASTCALL * MoveThisDroneCar_original)(tDrone_spec* pDrone);
void C2_HOOK_FASTCALL MoveThisDroneCar(tDrone_spec* pDrone) {

#if defined(C2_HOOKS_ENABLED)
    MoveThisDroneCar_original(pDrone);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044e540, MoveThisDroneCar, MoveThisDroneCar_original)

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

void (C2_HOOK_FASTCALL * DroneStateFuncControlledMovement_original)(tDrone_spec* pDrone, tDroneStateFuncState state);
void C2_HOOK_FASTCALL DroneStateFuncControlledMovement(tDrone_spec* pDrone, tDroneStateFuncState state) {

#if 0//defined(C2_HOOKS_ENABLED)
    DroneStateFuncControlledMovement_original(pDrone, state);
#else

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
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044d1d0, DroneStateFuncControlledMovement, DroneStateFuncControlledMovement_original)

void (C2_HOOK_FASTCALL * DroneStateFuncPhysicsActive_original)(tDrone_spec* pDrone, tDroneStateFuncState state);
void C2_HOOK_FASTCALL DroneStateFuncPhysicsActive(tDrone_spec* pDrone, tDroneStateFuncState state) {

#if defined(C2_HOOKS_ENABLED)
    DroneStateFuncPhysicsActive_original(pDrone, state);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044eb70, DroneStateFuncPhysicsActive, DroneStateFuncPhysicsActive_original)

void (C2_HOOK_FASTCALL * DroneStateFuncStationaryPassive_original)(tDrone_spec* pDrone, tDroneStateFuncState state);
void C2_HOOK_FASTCALL DroneStateFuncStationaryPassive(tDrone_spec* pDrone, tDroneStateFuncState state) {

#if defined(C2_HOOKS_ENABLED)
    DroneStateFuncStationaryPassive_original(pDrone, state);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044ec50, DroneStateFuncStationaryPassive, DroneStateFuncStationaryPassive_original)

int C2_HOOK_FASTCALL DroneCarWithinRenderingDistance(const br_vector3* pPos) {

    return fabsf(C2V(gRender_bounds_centre).v[0] - pPos->v[0]) < 15.f
        && fabsf(C2V(gRender_bounds_centre).v[1] - pPos->v[1]) < 12.f
        && fabsf(C2V(gRender_bounds_centre).v[2] - pPos->v[2]) < 15.f;
}
C2_HOOK_FUNCTION(0x0044ca90, DroneCarWithinRenderingDistance)

int C2_HOOK_FASTCALL DronePlaneWithinRenderingDistance(const br_vector3* pPos) {

    return fabsf(C2V(gRender_bounds_centre).v[0] - pPos->v[0]) < 40.f
           && fabsf(C2V(gRender_bounds_centre).v[1] - pPos->v[1]) < 30.f
           && fabsf(C2V(gRender_bounds_centre).v[2] - pPos->v[2]) < 40.f;
}
C2_HOOK_FUNCTION(0x0044cae0, DronePlaneWithinRenderingDistance)

int C2_HOOK_FASTCALL DroneTrainWithinRenderingDistance(const br_vector3* pPos) {

    return fabsf(C2V(gRender_bounds_centre).v[0] - pPos->v[0]) < 50.f
           && fabsf(C2V(gRender_bounds_centre).v[1] - pPos->v[1]) < 20.f
           && fabsf(C2V(gRender_bounds_centre).v[2] - pPos->v[2]) < 50.f;

}
C2_HOOK_FUNCTION(0x0044cb30, DroneTrainWithinRenderingDistance)

int C2_HOOK_FASTCALL DroneCarWithinProcessingDistance(const br_vector3* pPos) {

    return fabsf(C2V(gRender_bounds_centre).v[0] - pPos->v[0]) < 18.f
           && fabsf(C2V(gRender_bounds_centre).v[1] - pPos->v[1]) < 13.f
           && fabsf(C2V(gRender_bounds_centre).v[2] - pPos->v[2]) < 18.f;
}
C2_HOOK_FUNCTION(0x0044cb80, DroneCarWithinProcessingDistance)

int C2_HOOK_FASTCALL DronePlaneWithinProcessingDistance(const br_vector3* pPos) {

    return fabsf(C2V(gRender_bounds_centre).v[0] - pPos->v[0]) < 43.f
           && fabsf(C2V(gRender_bounds_centre).v[1] - pPos->v[1]) < 31.f
           && fabsf(C2V(gRender_bounds_centre).v[2] - pPos->v[2]) < 43.f;
}
C2_HOOK_FUNCTION(0x0044cbd0, DronePlaneWithinProcessingDistance)

int C2_HOOK_FASTCALL DroneTrainWithinProcessingDistance(const br_vector3* pPos) {

    return fabsf(C2V(gRender_bounds_centre).v[0] - pPos->v[0]) < 43.f
           && fabsf(C2V(gRender_bounds_centre).v[1] - pPos->v[1]) < 31.f
           && fabsf(C2V(gRender_bounds_centre).v[2] - pPos->v[2]) < 43.f;

}
C2_HOOK_FUNCTION(0x0044cc20, DroneTrainWithinProcessingDistance)

void C2_HOOK_FASTCALL CalcRenderBoundsCentre(void) {
    br_vector3 tv;

    BrVector3Negate(&tv, (br_vector3 *) C2V(gCamera_to_world).m[2]);
    BrVector3Normalise(&tv, &tv);
    BrVector3Scale(&tv, &tv, 5.f);
    BrVector3Add(&C2V(gRender_bounds_centre), (br_vector3 *) C2V(gCamera_to_world).m[3], &tv);
}
C2_HOOK_FUNCTION(0x00451bd0, CalcRenderBoundsCentre)

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
C2_HOOK_FUNCTION(0x0044cfd0, PipeDroneMatrix)

int C2_HOOK_FASTCALL OKToViewDrones(void) {
    return 0;
}
C2_HOOK_FUNCTION(0x00452680, OKToViewDrones)

br_matrix34* C2_HOOK_FASTCALL GetCurrentViewDroneMat(void) {

    return &C2V(gDrone_specs)[C2V(gCurrent_selected_drone)].actor->t.t.mat;
}
C2_HOOK_FUNCTION(0x00452690, GetCurrentViewDroneMat)

void C2_HOOK_FASTCALL StartRenderingThisDrone(tDrone_spec* pDrone) {

    if (pDrone->actor->render_style == BR_RSTYLE_FACES) {
        return;
    }
    if (C2V(gShow_drone_paths) && pDrone->actor->render_style == BR_RSTYLE_BOUNDING_FACES) {
        return;
    }
    if (C2V(gShow_drone_paths) || (pDrone->field_0x44 && C2V(gCount_rendered_drones) < 10)) {

        DoNotDprintf("START RENDERING: %d", pDrone->id);
        C2V(gCount_rendered_drones) += 1;
        pDrone->actor->render_style = BR_RSTYLE_FACES;
        PipeSingleDroneRender(pDrone, 1);
    } else {
        NewDroneState(pDrone,1);
    }
}
C2_HOOK_FUNCTION(0x00451810, StartRenderingDrone)

br_vector3* C2_HOOK_FASTCALL GetCurrentViewDroneDirection(void) {

    StartRenderingThisDrone(&C2V(gDrone_specs)[C2V(gCurrent_selected_drone)]);
    if (BrVector3LengthSquared(&C2V(gDrone_specs)[C2V(gCurrent_selected_drone)].field_0x18) > 0.f) {
        return &C2V(gDrone_specs)[C2V(gCurrent_selected_drone)].field_0x18;
    } else {
        return &C2V(gDefault_drone_direction);
    }
}
C2_HOOK_FUNCTION(0x004526c0, GetCurrentViewDroneDirection)
