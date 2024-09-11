#include "drone.h"

#include "errors.h"
#include "globvars.h"
#include "globvrpb.h"
#include "loading.h"
#include "physics.h"
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
    if (pDrone_spec->current_state != 1) {
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
#error "Not implemented"
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

#if defined(C2_HOOKS_ENABLED)
    InitDroneCollisionInfo_original(pDrone_spec);
#else
#error "Not implemented"
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

int C2_HOOK_FASTCALL ReallyAddDroneToPHIL(tDrone_spec* pDrone) {

    if (MarkCollisionInfoAsProcessed(&pDrone->collision_info)) {
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

void (C2_HOOK_FASTCALL * ProcessDrones_original)(void);
void C2_HOOK_FASTCALL ProcessDrones(void) {

#if defined(C2_HOOKS_ENABLED)
    ProcessDrones_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004512f0, ProcessDrones, ProcessDrones_original)
