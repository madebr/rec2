#include "09-funks.h"

#include "08-loading1.h"
#include "10-loading2.h"
#include "38-flicplay.h"
#include "41-utility.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "70-packfile.h"
#include "globvars.h"
#include "rec2_macros.h"

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x00655b80
const char* gFunk_move_names[7] = {
    "linear", "harmonic", "flash", "controlled", "absolute", "continuous", "texturebits",
};

// GLOBAL: CARMA2_HW 0x00655bb0
const char* gGroove_nature_names[2] = { "constant", "distance" };

// GLOBAL: CARMA2_HW 0x00655bc8
const char* gLollipop_names[3] = { "xlollipop", "ylollipop", "zlollipop" };

// GLOBAL: CARMA2_HW 0x00655bd8
const char* gGroove_path_names[2] = { "straight", "circular" };

// GLOBAL: CARMA2_HW 0x00655be0
const char* gGroove_object_names[4] = { "spin", "rock", "throb", "shear" };

// GLOBAL: CARMA2_HW 0x00688b30
tGroove_funk_binding gGroove_funk_bindings[1440];

// GLOBAL: CARMA2_HW 0x0068b830
tFunk_temp_buffer* gFunk_temp_vertices;

// GLOBAL: CARMA2_HW 0x0068b840
br_scalar gSight_distance_squared;

// GLOBAL: CARMA2_HW 0x0068b844
int gFunkotronics_array_size;

// GLOBAL: CARMA2_HW 0x0068b848
int gGroovidelics_array_size;

// GLOBAL: CARMA2_HW 0x0068b84c
tFunkotronic_spec* gFunkotronics_array;

// GLOBAL: CARMA2_HW 0x0068b850
tGroovidelic_spec* gGroovidelics_array;

// GLOBAL: CARMA2_HW 0x0068b854
br_actor* gGroove_by_proxy_actor;

// CalculateTextureBitsValue

// MapSawToTriangle

// FUNCTION: CARMA2_HW 0x00474880
void C2_HOOK_STDCALL SetSightDistance(br_scalar pYon) {

    gSight_distance_squared = (br_scalar)REC2_SQR(pYon * 1.02);
}

void C2_HOOK_FASTCALL AddFunkGrooveBinding(int pSlot_number, float* pPeriod_address) {
    if (pSlot_number < 0 || pSlot_number >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
        FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
    }
    gGroove_funk_bindings[pSlot_number].v = pPeriod_address;
    gGroove_funk_bindings[pSlot_number].field_0x4 = 0;
    *pPeriod_address = 0.0f;
}

// ControlBoundFunkGroove

// ControlBoundFunkGroovePlus

void C2_HOOK_FASTCALL ShiftBoundGrooveFunks(char* pStart, char* pEnd, ptrdiff_t pDelta) {
    intptr_t start;
    intptr_t end;
    uintptr_t address;
    uintptr_t aligned_delta;
    int i;

    start = (intptr_t)pStart;
    end = (intptr_t)pEnd;
    /* Bound scalar fields move in four-byte steps. */
    aligned_delta = (uintptr_t)pDelta & ~(uintptr_t)3;
    for (i = 0; i < (int)REC2_ASIZE(gGroove_funk_bindings); i++) {
        address = (uintptr_t)gGroove_funk_bindings[i].v;
        if (start <= (intptr_t)address && (intptr_t)address < end) {
            gGroove_funk_bindings[i].v = (float*)(address + aligned_delta);
        }
    }
}

tFunkotronic_spec* C2_HOOK_FASTCALL AddNewFunkotronic(void) {
    tFunkotronic_spec* new_array;
    tFunkotronic_spec* old_array;
    int i;

    for (i = 0; i < gFunkotronics_array_size; i++) {
        if (gFunkotronics_array[i].owner == -999) {
            memset(&gFunkotronics_array[i], 0, sizeof(tFunkotronic_spec));
            return &gFunkotronics_array[i];
        }
    }
    gFunkotronics_array_size += 16;
    new_array = BrMemCalloc(gFunkotronics_array_size, sizeof(tFunkotronic_spec), kMem_funk_spec);
    if (gFunkotronics_array != NULL) {
        memcpy(new_array, gFunkotronics_array, (gFunkotronics_array_size - 16) * sizeof(tFunkotronic_spec));
        old_array = gFunkotronics_array;
        ShiftBoundGrooveFunks((char*)old_array, (char*)&old_array[gFunkotronics_array_size - 16], (ptrdiff_t)((uintptr_t)new_array - (uintptr_t)old_array));
        BrMemFree(old_array);
    }
    gFunkotronics_array = new_array;
    for (i = 0; i < 16; i++) {
        gFunkotronics_array[gFunkotronics_array_size - 16 + i].owner = -999;
    }
    return &gFunkotronics_array[gFunkotronics_array_size - 16];
}

// FUNCTION: CARMA2_HW 0x00474950
void C2_HOOK_FASTCALL DisposeFunkotronics(int pOwner) {
    tFunkotronic_spec* the_funk;
    int i;

    the_funk = gFunkotronics_array;
    if (the_funk == NULL) {
        return;
    }
    for (i = 0; i < gFunkotronics_array_size; i++, the_funk++) {
        PossibleService();
        if (the_funk->owner == pOwner) {
            the_funk->owner = -999;
            if (the_funk->proximity_array != NULL) {
                BrMemFree(the_funk->proximity_array);
            }
            if (the_funk->texture_animation_type == eTexture_animation_frames
                && the_funk->texture_animation_data.frames_info.mode == eMove_texturebits) {
                BrMemFree(the_funk->texture_animation_data.frames_info.texture_info.data);
            }
            switch (the_funk->texture_animation_type) {
            case eTexture_animation_flic:
                BrMemFree(the_funk->texture_animation_data.flic_info.flic_data);
                EndFlic(&the_funk->texture_animation_data.flic_info.flic_descriptor);
                BrMemFree(the_funk->material->colour_map->pixels);
                the_funk->material->colour_map->pixels = NULL;
                BrPixelmapFree(the_funk->material->colour_map);
                the_funk->material->colour_map = NULL;
                break;
            case eTexture_animation_camera:
                /* Camera disposal shares this counter with the outer traversal. */
                for (i = 0; i < the_funk->texture_animation_data.camera_info.count; i++) {
                    BrActorRemove(the_funk->texture_animation_data.camera_info.actors[i]);
                    BrActorFree(the_funk->texture_animation_data.camera_info.actors[i]);
                }
                break;
            case eTexture_animation_mirror:
                if (the_funk->texture_animation_data.mirror_info.actor != NULL) {
                    BrActorRemove(the_funk->texture_animation_data.mirror_info.actor);
                    BrActorFree(the_funk->texture_animation_data.mirror_info.actor);
                }
                break;
            default:
                break;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004761e0
br_uint_32 C2_HOOK_FASTCALL CalcProximities(br_actor* pActor, br_material* pMat, void* pData) {
    tFunkotronic_spec* the_funk;
    br_face* the_face;
    int i;

    (void)pMat;
    the_funk = pData;
    if (pActor->model != NULL) {
        the_face = pActor->model->faces;
        for (i = 0; i < pActor->model->nfaces; i++, the_face++) {
            if (the_face->material == the_funk->material) {
                the_funk->proximity_count++;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00476230
br_uint_32 C2_HOOK_FASTCALL AddProximities(br_actor* pActor, br_material* pMat, void* pData) {
    tFunkotronic_spec* the_funk;
    br_face* the_face;
    br_vector3 normal;
    int i;
    int j;

    (void)pMat;
    the_funk = pData;
    if (pActor->model != NULL) {
        the_face = pActor->model->faces;
        for (i = 0; i < pActor->model->nfaces; i++, the_face++) {
            if (the_face->material == the_funk->material) {
                for (j = 0; j < 3; j++) {
                    BrVector3Copy(&the_funk->proximity_array[the_funk->proximity_count].v[j],
                        &pActor->model->vertices[the_face->vertices[j]].p);
                    gFunk_temp_vertices[the_funk->proximity_count].vertices[j] =
                        &pActor->model->vertices[the_face->vertices[j]];
                }
                normal.v[0] = the_face->n.v[0];
                normal.v[1] = the_face->n.v[1];
                normal.v[2] = the_face->n.v[2];
                BrVector3Normalise(&the_funk->proximity_array[the_funk->proximity_count].n, &normal);
                the_funk->proximity_array[the_funk->proximity_count].d = -the_face->d;
                the_funk->proximity_count++;
            }
        }
    }
    return 0;
}

br_material* C2_HOOK_FASTCALL TryThisFunkLink(tCar_crush_buffer_entry* pFunk_link, const char* pStr, tFunkotronic_spec* pFunk) {
    int i;

    if (pFunk_link == NULL) {
        return NULL;
    }
    for (i = 0; i < pFunk_link->count_smashables; i++) {
        if (strcmp(pFunk_link->smashables[i].material_name, pStr) == 0) {
            pFunk_link->smashables[i].funk = pFunk - gFunkotronics_array;
            return pFunk_link->smashables[i].funk_material;
        }
    }
    return NULL;
}

br_material* C2_HOOK_FASTCALL FindSmashableMaterial(const char* pStr, tFunkotronic_spec* pFunk, tCar_crush_buffer* pCar_crush_datas) {
    br_material* material;
    int i;

    for (i = 0; i < pCar_crush_datas->count_entries; i++) {
        material = TryThisFunkLink(pCar_crush_datas->entries[i], pStr, pFunk);
        if (material != NULL) {
            return material;
        }
    }
    return NULL;
}

// AddFunkotronics

// FUNCTION: CARMA2_HW 0x00476430
void C2_HOOK_FASTCALL DisposeGroovidelics(int pOwner) {
    tGroovidelic_spec* the_groove;
    int i;

    the_groove = gGroovidelics_array;
    if (the_groove == NULL) {
        return;
    }
    for (i = 0; i < gGroovidelics_array_size; i++, the_groove++) {
        PossibleService();
        if (the_groove->owner == pOwner) {
            the_groove->owner = -999;
        }
    }
}

tGroovidelic_spec* C2_HOOK_FASTCALL AddNewGroovidelic(void) {
    tGroovidelic_spec* new_array;
    tGroovidelic_spec* old_array;
    int i;

    for (i = 0; i < gGroovidelics_array_size; i++) {
        if (gGroovidelics_array[i].owner == -999) {
            memset(&gGroovidelics_array[i], 0, sizeof(tGroovidelic_spec));
            return &gGroovidelics_array[i];
        }
    }
    gGroovidelics_array_size += 16;
    new_array = BrMemCalloc(gGroovidelics_array_size, sizeof(tGroovidelic_spec), kMem_groove_spec);
    if (gGroovidelics_array != NULL) {
        memcpy(new_array, gGroovidelics_array, (gGroovidelics_array_size - 16) * sizeof(tGroovidelic_spec));
        old_array = gGroovidelics_array;
        ShiftBoundGrooveFunks((char*)old_array, (char*)&old_array[gGroovidelics_array_size - 16], (ptrdiff_t)((uintptr_t)new_array - (uintptr_t)old_array));
        BrMemFree(old_array);
    }
    gGroovidelics_array = new_array;
    for (i = 0; i < 16; i++) {
        gGroovidelics_array[gGroovidelics_array_size - 16 + i].owner = -999;
    }
    return &gGroovidelics_array[gGroovidelics_array_size - 16];
}

// FUNCTION: CARMA2_HW 0x00476470
void C2_HOOK_FASTCALL AddGroovidelics(FILE* pF, int pOwner, br_actor* pParent_actor, int pRef_offset, int pAllowed_to_be_absent) {
    int first_time;
    char s[256];
    char texture_name[32];
    char* str;
    tGroovidelic_spec* the_groove;
    tFunk_texturebits* texture_bits;
    br_model* proxy_model;
    br_vector3 p;
    float x_0;
    float x_1;
    float x_2;
    int d_0;
    int d_1;
    int d_2;
    int i;
    int j;

    first_time = 1;
    while (!PFfeof(pF)) {
        PossibleService();
        GetALineAndDontArgue(pF, s);
        if (strcmp(s, "END OF GROOVE") == 0) {
            break;
        }
        if (!first_time) {
            if (strcmp(s, "NEXT GROOVE") != 0) {
                FatalError(kFatalError_ErrorWithinGroovidelicFile);
            }
            GetALineAndDontArgue(pF, s);
        }
        first_time = 0;
        str = strtok(s, "\t ,/");
        the_groove = AddNewGroovidelic();
        the_groove->owner = pOwner;
        the_groove->block_flags = 0;
        the_groove->actor = DRActorFindRecurse(pParent_actor, str);
        if (the_groove->actor == NULL) {
            if (!pAllowed_to_be_absent && !gAusterity_mode) {
                FatalError(kFatalError_CannotFindActorReferencedInGroovidelicFile_S, str);
            }
            if (gGroove_by_proxy_actor == NULL) {
                gGroove_by_proxy_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
                proxy_model = LoadModel("PROXY.DAT");
                gGroove_by_proxy_actor->model = proxy_model;
                BrModelAdd(gGroove_by_proxy_actor->model);
                BrActorAdd(gDont_render_actor, gGroove_by_proxy_actor);
            }
            the_groove->actor = gGroove_by_proxy_actor;
        }
        the_groove->lollipop_mode = GetALineAndInterpretCommand(pF, gLollipop_names, (int)REC2_ASIZE(gLollipop_names));
        the_groove->mode = GetALineAndInterpretCommand(pF, gGroove_nature_names, (int)REC2_ASIZE(gGroove_nature_names));
        the_groove->path_type = GetALineAndInterpretCommand(pF, gGroove_path_names, (int)REC2_ASIZE(gGroove_path_names));
        the_groove->path_interrupt_status = eInterrupt_none;
        the_groove->object_interrupt_status = eInterrupt_none;
        if (the_groove->path_type != eGroove_path_none) {
            the_groove->path_mode = GetALineAndInterpretCommand(pF, gFunk_move_names, (int)REC2_ASIZE(gFunk_move_names));
        }
        switch (the_groove->path_type) {
        case eGroove_path_straight:
            GetThreeFloats(pF, &p.v[0], &p.v[1], &p.v[2]);
            the_groove->path_data.straight_info.centre = p;
            if (!(the_groove->path_data.straight_info.centre.v[0] < 0.0f || the_groove->path_data.straight_info.centre.v[0] > 0.0f
                    || the_groove->path_data.straight_info.centre.v[1] < 0.0f || the_groove->path_data.straight_info.centre.v[1] > 0.0f
                    || the_groove->path_data.straight_info.centre.v[2] < 0.0f || the_groove->path_data.straight_info.centre.v[2] > 0.0f)) {
                the_groove->path_data.straight_info.centre = the_groove->actor->t.t.translate.t;
            }
            switch (the_groove->path_mode) {
            case eMove_controlled:
            case eMove_absolute:
                AddFunkGrooveBinding(pRef_offset + GetAnInt(pF), &the_groove->path_data.straight_info.period.value);
                break;
            case eMove_texturebits:
                texture_bits = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                the_groove->path_data.straight_info.texture_info.data = texture_bits;
                GetAString(pF, texture_name);
                texture_bits->count = (tU8)strlen(texture_name);
                for (i = 0; i < texture_bits->count; i++) {
                    for (j = 0; j < 7; j++) {
                        if (texture_name[i] == "THBVLRF"[j]) {
                            texture_bits->bits[i] = (tU8)j;
                            break;
                        }
                    }
                }
                texture_bits->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAScalar(pF);
                the_groove->path_data.straight_info.period.value = (x_0 < 0.0f || x_0 > 0.0f) ? 1000.0f / x_0 : 0.0f;
                break;
            }
            GetThreeFloats(pF, &the_groove->path_data.straight_info.x_delta, &the_groove->path_data.straight_info.y_delta, &the_groove->path_data.straight_info.z_delta);
            break;
        case eGroove_path_circular:
            GetThreeFloats(pF, &p.v[0], &p.v[1], &p.v[2]);
            the_groove->path_data.circular_info.centre = p;
            /* The circular fallback shares the straight-path centre storage. */
            if (!(the_groove->path_data.straight_info.centre.v[0] < 0.0f || the_groove->path_data.straight_info.centre.v[0] > 0.0f
                    || the_groove->path_data.straight_info.centre.v[1] < 0.0f || the_groove->path_data.straight_info.centre.v[1] > 0.0f
                    || the_groove->path_data.straight_info.centre.v[2] < 0.0f || the_groove->path_data.straight_info.centre.v[2] > 0.0f)) {
                the_groove->path_data.straight_info.centre = the_groove->actor->t.t.translate.t;
            }
            switch (the_groove->path_mode) {
            case eMove_controlled:
            case eMove_absolute:
                AddFunkGrooveBinding(pRef_offset + GetAnInt(pF), &the_groove->path_data.circular_info.period.value);
                break;
            case eMove_texturebits:
                texture_bits = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                the_groove->path_data.circular_info.texture_info.data = texture_bits;
                GetAString(pF, texture_name);
                texture_bits->count = (tU8)strlen(texture_name);
                for (i = 0; i < texture_bits->count; i++) {
                    for (j = 0; j < 7; j++) {
                        if (texture_name[i] == "THBVLRF"[j]) {
                            texture_bits->bits[i] = (tU8)j;
                            break;
                        }
                    }
                }
                texture_bits->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAScalar(pF);
                the_groove->path_data.circular_info.period.value = (x_0 < 0.0f || x_0 > 0.0f) ? 1000.0f / x_0 : 0.0f;
                break;
            }
            the_groove->path_data.circular_info.radius = GetAScalar(pF);
            the_groove->path_data.circular_info.axis = GetALineAndInterpretCommand(pF, gAxis_names, (int)REC2_ASIZE(gAxis_names));
            break;
        default:
            break;
        }
        the_groove->object_type = GetALineAndInterpretCommand(pF, gGroove_object_names, (int)REC2_ASIZE(gGroove_object_names));
        the_groove->object_position = the_groove->actor->t.t.translate.t;
        if (the_groove->object_type != eGroove_object_none) {
            the_groove->object_mode = GetALineAndInterpretCommand(pF, gFunk_move_names, (int)REC2_ASIZE(gFunk_move_names));
        }
        switch (the_groove->object_type) {
        case eGroove_object_spin:
            switch (the_groove->object_mode) {
            case eMove_controlled:
            case eMove_absolute:
                AddFunkGrooveBinding(pRef_offset + GetAnInt(pF), &the_groove->object_data.spin_info.period.value);
                break;
            case eMove_texturebits:
                texture_bits = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                the_groove->object_data.spin_info.texture_info.data = texture_bits;
                GetAString(pF, texture_name);
                texture_bits->count = (tU8)strlen(texture_name);
                for (i = 0; i < texture_bits->count; i++) {
                    for (j = 0; j < 7; j++) {
                        if (texture_name[i] == "THBVLRF"[j]) {
                            texture_bits->bits[i] = (tU8)j;
                            break;
                        }
                    }
                }
                texture_bits->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAScalar(pF);
                the_groove->object_data.spin_info.period.value = (x_0 < 0.0f || x_0 > 0.0f) ? 1000.0f / x_0 : 0.0f;
                break;
            }
            GetThreeFloats(pF, &p.v[0], &p.v[1], &p.v[2]);
            the_groove->object_centre = p;
            the_groove->object_data.spin_info.axis = GetALineAndInterpretCommand(pF, gAxis_names, (int)REC2_ASIZE(gAxis_names));
            break;
        case eGroove_object_rock:
            switch (the_groove->object_mode) {
            case eMove_controlled:
            case eMove_absolute:
                AddFunkGrooveBinding(pRef_offset + GetAnInt(pF), &the_groove->object_data.rock_info.period.value);
                break;
            case eMove_texturebits:
                texture_bits = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                the_groove->object_data.rock_info.texture_info.data = texture_bits;
                GetAString(pF, texture_name);
                texture_bits->count = (tU8)strlen(texture_name);
                for (i = 0; i < texture_bits->count; i++) {
                    for (j = 0; j < 7; j++) {
                        if (texture_name[i] == "THBVLRF"[j]) {
                            texture_bits->bits[i] = (tU8)j;
                            break;
                        }
                    }
                }
                texture_bits->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAScalar(pF);
                the_groove->object_data.rock_info.period.value = (x_0 < 0.0f || x_0 > 0.0f) ? 1000.0f / x_0 : 0.0f;
                break;
            }
            GetThreeFloats(pF, &p.v[0], &p.v[1], &p.v[2]);
            the_groove->object_centre = p;
            the_groove->object_data.rock_info.axis = GetALineAndInterpretCommand(pF, gAxis_names, (int)REC2_ASIZE(gAxis_names));
            the_groove->object_data.rock_info.max_angle = GetAScalar(pF);
            break;
        case eGroove_object_throb:
            if (the_groove->object_mode == eMove_controlled || the_groove->object_mode == eMove_absolute) {
                GetThreeInts(pF, &d_0, &d_1, &d_2);
                if (d_0 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + d_0, &the_groove->object_data.throb_info.x_period.value);
                }
                if (d_1 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + d_1, &the_groove->object_data.throb_info.y_period.value);
                }
                if (d_2 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + d_2, &the_groove->object_data.throb_info.z_period.value);
                }
            } else {
                GetThreeFloats(pF, &x_0, &x_1, &x_2);
                the_groove->object_data.throb_info.x_period.value = (x_0 < 0.0f || x_0 > 0.0f) ? 1000.0f / x_0 : 0.0f;
                the_groove->object_data.throb_info.y_period.value = (x_1 < 0.0f || x_1 > 0.0f) ? 1000.0f / x_1 : 0.0f;
                the_groove->object_data.throb_info.z_period.value = (x_2 < 0.0f || x_2 > 0.0f) ? 1000.0f / x_2 : 0.0f;
            }
            GetThreeFloats(pF, &p.v[0], &p.v[1], &p.v[2]);
            the_groove->object_centre = p;
            GetThreeFloatPercents(pF, &the_groove->object_data.throb_info.x_magnitude, &the_groove->object_data.throb_info.y_magnitude, &the_groove->object_data.throb_info.z_magnitude);
            break;
        case eGroove_object_shear:
            if (the_groove->object_mode == eMove_controlled || the_groove->object_mode == eMove_absolute) {
                GetThreeInts(pF, &d_0, &d_1, &d_2);
                if (d_0 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + d_0, &the_groove->object_data.shear_info.x_period.value);
                }
                if (d_1 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + d_1, &the_groove->object_data.shear_info.y_period.value);
                }
                if (d_2 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + d_2, &the_groove->object_data.shear_info.z_period.value);
                }
            } else {
                GetThreeFloats(pF, &x_0, &x_1, &x_2);
                the_groove->object_data.shear_info.x_period.value = (x_0 < 0.0f || x_0 > 0.0f) ? 1000.0f / x_0 : 0.0f;
                the_groove->object_data.shear_info.y_period.value = (x_1 < 0.0f || x_1 > 0.0f) ? 1000.0f / x_1 : 0.0f;
                the_groove->object_data.shear_info.z_period.value = (x_2 < 0.0f || x_2 > 0.0f) ? 1000.0f / x_2 : 0.0f;
            }
            GetThreeFloats(pF, &p.v[0], &p.v[1], &p.v[2]);
            the_groove->object_centre = p;
            GetThreeFloatPercents(pF, &the_groove->object_data.shear_info.x_magnitude, &the_groove->object_data.shear_info.y_magnitude, &the_groove->object_data.shear_info.z_magnitude);
            break;
        default:
            break;
        }
    }
}

// KillGroovadelic

// KillFunkotronic

// NormaliseDegreeAngle

// FunkThoseTronics

// PathGrooveBastard

// ObjectGrooveBastard

// GrooveThisDelic

// GrooveThoseDelics

// StopGroovidelic

// DisableGroovidelic

// EnableGroovidelic

// SetGrooveInterrupt

// ResetGrooveFlags

// DisableFunkotronic

// EnableFunkotronic

// MasterDisableFunkotronic

// MasterEnableFunkotronic

// AdjustFunkEnable

// FUNCTION: CARMA2_HW 0x0047b360
tGroovidelic_spec* C2_HOOK_FASTCALL ActorsGroove(br_actor* pActor) {
    tGroovidelic_spec* groove;
    int count;
    int i;

    groove = gGroovidelics_array;
    count = gGroovidelics_array_size;
    for (i = 0; i < count; i++, groove++) {
        if (groove->actor == pActor) {
            return groove;
        }
    }
    return NULL;
}

