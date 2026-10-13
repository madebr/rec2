#include "09-funks.h"

#include "08-loading1.h"
#include "10-loading2.h"
#include "16-graphics1.h"
#include "17-world2.h"
#include "23-piping.h"
#include "36-trig.h"
#include "38-flicplay.h"
#include "41-utility.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "70-packfile.h"
#include "globvars.h"
#include "rec2_macros.h"

#include "c2_math.h"
#include "c2_string.h"

#define SAW(T, PERIOD) (fmod((T), (PERIOD)) / (PERIOD))

#define MOVE_FUNK_PARAMETER(DEST, MODE, PERIOD, TEXTUREBITS, AMPLITUDE, FLASH_VALUE) \
    do { \
        switch (MODE) { \
        case eMove_continuous: \
            if ((PERIOD) < 0.0f || (PERIOD) > 0.0f) { \
                (DEST) = (br_scalar)((AMPLITUDE) * SAW(f_the_time, (PERIOD))); \
            } else { \
                (DEST) = 0.0f; \
            } \
            break; \
        case eMove_controlled: \
            (DEST) = (PERIOD) * (AMPLITUDE); \
            break; \
        case eMove_absolute: \
            (DEST) = (PERIOD); \
            break; \
        case eMove_linear: \
            if ((PERIOD) < 0.0f || (PERIOD) > 0.0f) { \
                (DEST) = (AMPLITUDE) * MapSawToTriangle((float)SAW(f_the_time, (PERIOD))); \
            } else { \
                (DEST) = 0.0f; \
            } \
            break; \
        case eMove_flash: \
            if (!(2.0 * fmod(f_the_time, (PERIOD)) <= (PERIOD))) { \
                (DEST) = (FLASH_VALUE); \
            } else { \
                (DEST) = -(FLASH_VALUE); \
            } \
            break; \
        case eMove_texturebits: \
            (DEST) = (br_scalar)CalculateTextureBitsValue(TEXTUREBITS); \
            break; \
        case eMove_harmonic: \
        default: \
            if ((PERIOD) < 0.0f || (PERIOD) > 0.0f) { \
                (DEST) = (br_scalar)((AMPLITUDE) * sin(BrAngleToRadian(BrDegreeToAngle(SAW(f_the_time, (PERIOD)) * 360.0)))); \
            } else { \
                (DEST) = 0.0f; \
            } \
            break; \
        } \
    } while (0)

// GLOBAL: CARMA2_HW 0x00655b50
const char* gFunk_type_names[5] = { "spin", "rock", "throb", "slither", "roll" };

// GLOBAL: CARMA2_HW 0x00655b68
const char* gFunk_anim_names[4] = { "frames", "flic", "camera", "mirror" };

// GLOBAL: CARMA2_HW 0x00655b78
const char* gTime_mode_names[2] = { "approximate", "accurate" };

// GLOBAL: CARMA2_HW 0x00655b80
const char* gFunk_move_names[7] = {
    "linear", "harmonic", "flash", "controlled", "absolute", "continuous", "texturebits",
};

// GLOBAL: CARMA2_HW 0x00655ba0
const char* gFunk_nature_names[4] = { "constant", "distance", "lastlap", "otherlaps" };

// GLOBAL: CARMA2_HW 0x00655bb0
const char* gGroove_nature_names[2] = { "constant", "distance" };

// GLOBAL: CARMA2_HW 0x00655bc8
const char* gLollipop_names[3] = { "xlollipop", "ylollipop", "zlollipop" };

// GLOBAL: CARMA2_HW 0x00655bd8
const char* gGroove_path_names[2] = { "straight", "circular" };

// GLOBAL: CARMA2_HW 0x00655be0
const char* gGroove_object_names[4] = { "spin", "rock", "throb", "shear" };

// GLOBAL: CARMA2_HW 0x00655bf0
const char* gCamera_animation_names[2] = { "static", "tracking" };

// GLOBAL: CARMA2_HW 0x00688b30
tGroove_funk_binding gGroove_funk_bindings[1440];

// GLOBAL: CARMA2_HW 0x0068b830
tFunk_temp_buffer* gFunk_temp_vertices;

// GLOBAL: CARMA2_HW 0x0068b838
tU32 gPrevious_groove_times[2];

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

int C2_HOOK_FASTCALL CalculateTextureBitsValue(tFunk_texturebits* pTexture_bits) {
    tU32 sum_flags;
    tU32 car_flags;
    int count;
    int i;

    sum_flags = 0;
    count = pTexture_bits->count;
    if (count != 0) {
        car_flags = (tU32)pTexture_bits->car->field_0x18cc;
        for (i = 0; i < count; i++) {
            /* Texture flag indices wrap within their 32-bit word. */
            if (car_flags & ((tU32)1 << (pTexture_bits->bits[i] & 31))) {
                sum_flags += (tU32)1 << (i & 31);
            }
        }
    }
    return (int)sum_flags;
}

float C2_HOOK_FASTCALL MapSawToTriangle(float pNumber) {
    if (pNumber >= 0.5f) {
        return 3.0f - pNumber * 4.0f;
    }
    return pNumber * 4.0f - 1.0f;
}

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

// FUNCTION: CARMA2_HW 0x004748a0
void C2_HOOK_FASTCALL ControlBoundFunkGroove(int pSlot_number, float pValue) {
    if (pSlot_number < 0) {
        return;
    }
    if (pSlot_number >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
        FatalError(kFatalError_UsedRefNumOfControlledGroovidelicFunkotronicIsOutOfRange);
    }
    if (gGroove_funk_bindings[pSlot_number].field_0x4 == 0) {
        *gGroove_funk_bindings[pSlot_number].v = pValue;
    }
}

// FUNCTION: CARMA2_HW 0x004748e0
float C2_HOOK_FASTCALL ControlBoundFunkGroovePlus(int pSlot_number, float pValue) {
    float* period;

    if (pSlot_number < 0) {
        return 0.0f;
    }
    if (pSlot_number >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
        FatalError(kFatalError_UsedRefNumOfControlledGroovidelicFunkotronicIsOutOfRange);
    }
    if (gGroove_funk_bindings[pSlot_number].field_0x4 == 0) {
        period = gGroove_funk_bindings[pSlot_number].v;
        *period = (float)fmod((double)*period + (double)pValue, 1.0);
    }
    return *gGroove_funk_bindings[pSlot_number].v;
}

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

// FUNCTION: CARMA2_HW 0x00474ac0
void C2_HOOK_FASTCALL AddFunkotronics(FILE* pF, int pOwner, int pRef_offset, tCar_crush_buffer* pCar_crush_datas) {
    int first_time;
    char s[256];
    tFunkotronic_spec* the_funk;
    char* str;
    float x_0;
    float x_1;
    int g0;
    int g1;
    float speed1;
    float speed2;
    int i;
    float a_min;
    float d_min;
    float s_min;
    float a_max;
    float d_max;
    float s_max;
    float yon_factor;
    float fov_factor;
    tFunk_texturebits* texture_bits;
    char texture_string[32];
    int j;

    gFunk_temp_vertices = BrMemAllocate(500 * sizeof(tFunk_temp_buffer), BR_MEMORY_APPLICATION);
    first_time = 1;
    while (!PFfeof(pF)) {
        PossibleService();
        GetALineAndDontArgue(pF, s);
        if (strcmp(s, "END OF FUNK") == 0) {
            break;
        }
        if (!first_time) {
            if (strcmp(s, "NEXT FUNK") != 0) {
                FatalError(kFatalError_FunkotronicFile);
            } else {
                GetALineAndDontArgue(pF, s);
            }
        }
        first_time = 0;
        str = strtok(s, "\t ,/");

        the_funk = AddNewFunkotronic();
        the_funk->owner = pOwner;
        if (pCar_crush_datas != NULL) {
            the_funk->material = FindSmashableMaterial(str, the_funk, pCar_crush_datas);
        }
        if (pCar_crush_datas == NULL || the_funk->material == NULL) {
            the_funk->material = BrMaterialFind(str);
            if (the_funk->material == NULL) {
                FatalError(kFatalError_CannotFindMainMaterialInFunkotronicFile_S, str);
            }
        }
        str = strtok(NULL, "\t ,/");
        if (str != NULL && strcmp(str, "multiple") == 0 && the_funk->material->identifier != NULL) {
            BrResFree(the_funk->material->identifier);
            the_funk->material->identifier = NULL;
        }
        the_funk->mode = GetALineAndInterpretCommand(pF, gFunk_nature_names, REC2_ASIZE(gFunk_nature_names));
        the_funk->proximity_count = 0;
        the_funk->proximity_array = NULL;
        if (the_funk->mode == eFunk_mode_distance) {
            DRActorEnumRecurseWithMat(gUniverse_actor, NULL, CalcProximities, the_funk);
            the_funk->proximity_array = BrMemAllocate(the_funk->proximity_count * sizeof(tFunk_proximity), kMem_funk_prox_array);
            the_funk->proximity_count = 0;
            DRActorEnumRecurseWithMat(gUniverse_actor, NULL, AddProximities, the_funk);
        }
        the_funk->matrix_mod_type = GetALineAndInterpretCommand(pF, gFunk_type_names, REC2_ASIZE(gFunk_type_names));
        the_funk->flags &= ~0x1;
        if (the_funk->matrix_mod_type != eMatrix_mod_none) {
            the_funk->matrix_mode = GetALineAndInterpretCommand(pF, gFunk_move_names, REC2_ASIZE(gFunk_move_names));
        }
        switch (the_funk->matrix_mod_type) {
        case eMatrix_mod_spin:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                g0 = GetAnInt(pF);
                AddFunkGrooveBinding(pRef_offset + g0, &the_funk->matrix_mod_data.spin_info.period);
                break;
            case eMove_texturebits:
                texture_bits = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                the_funk->matrix_mod_data.texture_info.data = texture_bits;
                GetAString(pF, texture_string);
                texture_bits->count = (tU8)strlen(texture_string);
                for (i = 0; i < texture_bits->count; i++) {
                    for (j = 0; j < 7; j++) {
                        if (texture_string[i] == "THBVLRF"[j]) {
                            texture_bits->bits[i] = (tU8)j;
                            break;
                        }
                    }
                }
                texture_bits->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAScalar(pF);
                the_funk->matrix_mod_data.rock_info.period = (x_0 < 0.0f || x_0 > 0.0f) ? (float)(1000.0 / x_0) : 0.0f;
                break;
            }
            break;
        case eMatrix_mod_rock:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                g0 = GetAnInt(pF);
                AddFunkGrooveBinding(pRef_offset + g0, &the_funk->matrix_mod_data.rock_info.period);
                break;
            case eMove_texturebits:
                texture_bits = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                the_funk->matrix_mod_data.texture_info.data = texture_bits;
                GetAString(pF, texture_string);
                texture_bits->count = (tU8)strlen(texture_string);
                for (i = 0; i < texture_bits->count; i++) {
                    for (j = 0; j < 7; j++) {
                        if (texture_string[i] == "THBVLRF"[j]) {
                            texture_bits->bits[i] = (tU8)j;
                            break;
                        }
                    }
                }
                texture_bits->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAScalar(pF);
                the_funk->matrix_mod_data.rock_info.period = (x_0 < 0.0f || x_0 > 0.0f) ? (float)(1000.0 / x_0) : 0.0f;
                break;
            }
            the_funk->matrix_mod_data.rock_info.rock_angle = GetAScalar(pF);
            GetPairOfFloats(pF, &x_0, &x_1);
            the_funk->matrix_mod_data.rock_info.x_centre = x_0 / 100.0f;
            the_funk->matrix_mod_data.rock_info.y_centre = x_1 / 100.0f;
            break;
        case eMatrix_mod_throb:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                GetPairOfInts(pF, &g0, &g1);
                if (g0 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + g0, &the_funk->matrix_mod_data.throb_info.x_period);
                }
                if (g1 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + g1, &the_funk->matrix_mod_data.throb_info.y_period);
                }
                break;
            default:
                GetPairOfFloats(pF, &speed1, &speed2);
                the_funk->matrix_mod_data.throb_info.x_period = (speed1 < 0.0f || speed1 > 0.0f) ? (float)(1000.0 / speed1) : 0.0f;
                the_funk->matrix_mod_data.throb_info.y_period = (speed2 < 0.0f || speed2 > 0.0f) ? (float)(1000.0 / speed2) : 0.0f;
                break;
            }
            GetPairOfFloatPercents(pF,
                &the_funk->matrix_mod_data.throb_info.x_magnitude,
                &the_funk->matrix_mod_data.throb_info.y_magnitude);
            GetPairOfFloats(pF, &x_0, &x_1);
            the_funk->matrix_mod_data.throb_info.x_centre = x_0 / 100.0f;
            the_funk->matrix_mod_data.throb_info.y_centre = x_1 / 100.0f;
            if (the_funk->matrix_mode != eMove_controlled) {
                if (!(the_funk->matrix_mod_data.throb_info.x_period < 0.0f || the_funk->matrix_mod_data.throb_info.x_period > 0.0f)) {
                    the_funk->matrix_mod_data.throb_info.x_period = 1.0f;
                    the_funk->matrix_mod_data.throb_info.x_magnitude = 0.0f;
                }
                if (!(the_funk->matrix_mod_data.throb_info.y_period < 0.0f || the_funk->matrix_mod_data.throb_info.y_period > 0.0f)) {
                    the_funk->matrix_mod_data.throb_info.y_period = 1.0f;
                    the_funk->matrix_mod_data.throb_info.y_magnitude = 0.0f;
                }
            }
            break;
        case eMatrix_mod_slither:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                GetPairOfInts(pF, &g0, &g1);
                if (g0 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + g0, &the_funk->matrix_mod_data.slither_info.x_period);
                }
                if (g1 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + g1, &the_funk->matrix_mod_data.slither_info.y_period);
                }
                break;
            default:
                GetPairOfFloats(pF, &speed1, &speed2);
                the_funk->matrix_mod_data.slither_info.x_period = (speed1 < 0.0f || speed1 > 0.0f) ? (float)(1000.0 / speed1) : 0.0f;
                the_funk->matrix_mod_data.slither_info.y_period = (speed2 < 0.0f || speed2 > 0.0f) ? (float)(1000.0 / speed2) : 0.0f;
                break;
            }
            GetPairOfFloatPercents(pF,
                &the_funk->matrix_mod_data.slither_info.x_magnitude,
                &the_funk->matrix_mod_data.slither_info.y_magnitude);
            if (the_funk->matrix_mode != eMove_controlled) {
                if (!(the_funk->matrix_mod_data.slither_info.x_period < 0.0f || the_funk->matrix_mod_data.slither_info.x_period > 0.0f)) {
                    the_funk->matrix_mod_data.slither_info.x_period = 1.0f;
                    the_funk->matrix_mod_data.slither_info.x_magnitude = 0.0f;
                }
                if (!(the_funk->matrix_mod_data.slither_info.y_period < 0.0f || the_funk->matrix_mod_data.slither_info.y_period > 0.0f)) {
                    the_funk->matrix_mod_data.slither_info.y_period = 1.0f;
                    the_funk->matrix_mod_data.slither_info.y_magnitude = 0.0f;
                }
            }
            break;
        case eMatrix_mod_roll:
            switch (the_funk->matrix_mode) {
            case eMove_controlled:
            case eMove_absolute:
                GetPairOfInts(pF, &g0, &g1);
                if (g0 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + g0, &the_funk->matrix_mod_data.roll_info.x_period);
                }
                if (g1 >= 0) {
                    AddFunkGrooveBinding(pRef_offset + g1, &the_funk->matrix_mod_data.roll_info.y_period);
                }
                break;
            default:
                GetPairOfFloats(pF, &speed1, &speed2);
                the_funk->matrix_mod_data.roll_info.x_period = (speed1 < 0.0f || speed1 > 0.0f) ? (float)(1000.0 / speed1) : 0.0f;
                the_funk->matrix_mod_data.roll_info.y_period = (speed2 < 0.0f || speed2 > 0.0f) ? (float)(1000.0 / speed2) : 0.0f;
                break;
            }
            break;
        default:
            break;
        }
        the_funk->lighting_animation_type = GetALineAndInterpretCommand(pF, gFunk_move_names, REC2_ASIZE(gFunk_move_names));
        if (the_funk->lighting_animation_type != eMove_none) {
            switch (the_funk->lighting_animation_type) {
            case eMove_controlled:
            case eMove_absolute:
                g0 = GetAnInt(pF);
                AddFunkGrooveBinding(pRef_offset + g0, &the_funk->lighting_animation_data.controlled.period);
                break;
            case eMove_texturebits:
                texture_bits = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                the_funk->lighting_animation_data.texture_info.data = texture_bits;
                GetAString(pF, texture_string);
                texture_bits->count = (tU8)strlen(texture_string);
                for (i = 0; i < texture_bits->count; i++) {
                    for (j = 0; j < 7; j++) {
                        if (texture_string[i] == "THBVLRF"[j]) {
                            texture_bits->bits[i] = (tU8)j;
                            break;
                        }
                    }
                }
                texture_bits->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAScalar(pF);
                the_funk->lighting_animation_data.rock_info.period = (x_0 < 0.0f || x_0 > 0.0f) ? (float)(1000.0 / x_0) : 0.0f;
                break;
            }
            GetThreeFloatPercents(pF, &a_min, &d_min, &s_min);
            GetThreeFloatPercents(pF, &a_max, &d_max, &s_max);
            the_funk->ambient_base = (a_min + a_max) / 2.0f;
            the_funk->direct_base = (d_min + d_max) / 2.0f;
            the_funk->specular_base = (s_min + s_max) / 2.0f;
            the_funk->ambient_delta = (a_max - a_min) / 2.0f;
            the_funk->direct_delta = (d_max - d_min) / 2.0f;
            the_funk->specular_delta = (s_max - s_min) / 2.0f;
        }
        the_funk->texture_animation_type = GetALineAndInterpretCommand(pF, gFunk_anim_names, REC2_ASIZE(gFunk_anim_names));
        if (the_funk->texture_animation_type != eTexture_animation_none
                && the_funk->texture_animation_type != eTexture_animation_mirror
                && the_funk->texture_animation_type != eTexture_animation_camera) {
            the_funk->time_mode = GetALineAndInterpretCommand(pF, gTime_mode_names, REC2_ASIZE(gTime_mode_names));
        }
        if (the_funk->texture_animation_type == eTexture_animation_flic && gAusterity_mode) {
            the_funk->texture_animation_type = eTexture_animation_none;
            GetALineAndDontArgue(pF, s);
        }
        the_funk->last_frame = 0.0f;
        switch (the_funk->texture_animation_type) {
        case eTexture_animation_frames:
            the_funk->texture_animation_data.frames_info.mode = GetALineAndInterpretCommand(pF, gFunk_move_names, REC2_ASIZE(gFunk_move_names));
            switch (the_funk->texture_animation_data.frames_info.mode) {
            case eMove_controlled:
            case eMove_absolute:
                g0 = GetAnInt(pF);
                AddFunkGrooveBinding(pRef_offset + g0, &the_funk->texture_animation_data.frames_info.controlled.period);
                break;
            case eMove_texturebits:
                texture_bits = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);
                the_funk->texture_animation_data.frames_info.texture_info.data = texture_bits;
                GetAString(pF, texture_string);
                texture_bits->count = (tU8)strlen(texture_string);
                for (i = 0; i < texture_bits->count; i++) {
                    for (j = 0; j < 7; j++) {
                        if (texture_string[i] == "THBVLRF"[j]) {
                            texture_bits->bits[i] = (tU8)j;
                            break;
                        }
                    }
                }
                texture_bits->car = gCurrent_car_spec;
                break;
            default:
                x_0 = GetAScalar(pF);
                the_funk->texture_animation_data.frames_info.rock_info.period = (x_0 < 0.0f || x_0 > 0.0f) ? (float)(1000.0 / x_0) : 0.0f;
                break;
            }
            the_funk->texture_animation_data.frames_info.texture_count = (int)GetAScalar(pF);
            the_funk->texture_animation_data.frames_info.current_frame = 0;
            the_funk->texture_animation_data.frames_info.has_matrix = 0;
            for (i = 0; i < the_funk->texture_animation_data.frames_info.texture_count; i++) {
                GetALineAndDontArgue(pF, s);
                str = strtok(s, "\t ,/");
                the_funk->texture_animation_data.frames_info.textures[i] = BrMapFind(str);
                if (the_funk->texture_animation_data.frames_info.textures[i] == NULL) {
                    FatalError(kFatalError_CannotFindAnimationFramePixelmapReferencedInFunkotronicFile);
                }
                BrMatrix23Identity(&the_funk->texture_animation_data.frames_info.mat[i]);
                str = strtok(NULL, "\t ,/");
                if (str != NULL && strlen(str) != 0) {
                    int size_x;
                    int pos_x;
                    int size_y;
                    int pos_y;

                    the_funk->texture_animation_data.frames_info.has_matrix = 1;
                    sscanf(str, "%d", &size_x);
                    str = strtok(NULL, "\t ,/");
                    sscanf(str, "%d", &pos_x);
                    str = strtok(NULL, "\t ,/");
                    sscanf(str, "%d", &size_y);
                    str = strtok(NULL, "\t ,/");
                    sscanf(str, "%d", &pos_y);
                    the_funk->texture_animation_data.frames_info.mat[i].m[0][0] = 1.f / (float)size_x;
                    the_funk->texture_animation_data.frames_info.mat[i].m[1][1] = 1.f / (float)size_y;
                    the_funk->texture_animation_data.frames_info.mat[i].m[2][0] = (float)pos_x / (float)size_x;
                    the_funk->texture_animation_data.frames_info.mat[i].m[2][1] = (float)pos_y / (float)size_y;
                }
            }
            break;
        case eTexture_animation_flic:
            GetAString(pF, s);
            the_funk->texture_animation_data.flic_info.flic_data = NULL;
            if (LoadFlicData(s, &the_funk->texture_animation_data.flic_info.flic_data, &the_funk->texture_animation_data.flic_info.flic_data_length)) {
                char *the_pixels;
                br_pixelmap *the_pixelmap;

                the_funk->texture_animation_data.flic_info.flic_descriptor.data_start = NULL;
                StartFlic(s, -1, &the_funk->texture_animation_data.flic_info.flic_descriptor, the_funk->texture_animation_data.flic_info.flic_data_length, (tS8*)the_funk->texture_animation_data.flic_info.flic_data, NULL, 0, 0, 0);
                the_funk->last_frame = 0.0f;
                the_pixels = BrMemAllocate(
                    the_funk->texture_animation_data.flic_info.flic_descriptor.height
                        * ((the_funk->texture_animation_data.flic_info.flic_descriptor.width + 3) & ~3),
                        kMem_video_pixels);
                if (gScreen->row_bytes < 0) {
                    BrFatal("C:\\Carma2\\Source\\Common\\Funkgroo.c", 732,
                            "Bruce bug at line %d, file C:\\Carma2\\Source\\Common\\Funkgroo.c", 732);
                }
                the_pixelmap = DRPixelmapAllocate(gScreen->type,
                    (br_uint_16)the_funk->texture_animation_data.flic_info.flic_descriptor.width,
                    (br_uint_16)the_funk->texture_animation_data.flic_info.flic_descriptor.height,
                    the_pixels,
                    0);
                AssertFlicPixelmap(&the_funk->texture_animation_data.flic_info.flic_descriptor, the_pixelmap);
                the_funk->material->colour_map = the_pixelmap;
                the_funk->material->user = the_pixelmap;
                BrMaterialUpdate(the_funk->material, BR_MATU_ALL);
            } else {
                the_funk->texture_animation_type = eTexture_animation_none;
            }
            break;
        case eTexture_animation_camera:
            the_funk->texture_animation_data.camera_info.mode = GetALineAndInterpretCommand(pF, gCamera_animation_names, REC2_ASIZE(gCamera_animation_names));
            GetPairOfFloats(pF, &yon_factor, &fov_factor);
            the_funk->texture_animation_data.camera_info.field_0x60 = GetAnInt(pF);
            the_funk->texture_animation_data.camera_info.count = GetAnInt(pF);
            for (i = 0; i < the_funk->texture_animation_data.camera_info.count; i++) {
                br_actor* actor;
                br_camera* camera;
                br_camera* global_camera;
                br_vector3 cam_pos;

                actor = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
                the_funk->texture_animation_data.camera_info.actors[i] = actor;
                if (actor == NULL) {
                    FatalError(kFatalError_CannotAllocateCamera);
                }
                camera = actor->type_data;
                global_camera = gCamera->type_data;
                camera->type = BR_CAMERA_PERSPECTIVE_FOV;
                camera->field_of_view = (br_angle)(int)(fov_factor * global_camera->field_of_view);
                camera->hither_z = global_camera->hither_z;
                camera->yon_z = yon_factor * global_camera->yon_z;
                actor = BrActorAdd(gUniverse_actor, actor);
                if (actor == NULL) {
                    FatalError(kFatalError_CannotAllocateCamera);
                }
                GetThreeFloats(pF, &cam_pos.v[0], &cam_pos.v[1], &cam_pos.v[2]);
                if (the_funk->texture_animation_data.camera_info.mode == eFunk_camera_static) {
                    br_euler e;
                    float angle1;
                    float angle2;

                    GetPairOfFloats(pF, &angle1, &angle2);
                    e.order = 0x12;
                    e.a = BrDegreeToAngle(angle1);
                    e.b = BrDegreeToAngle(360.0f - angle2);
                    e.c = 0;
                    BrEulerToMatrix34(&actor->t.t.mat, &e);
                }
                actor->t.t.mat.m[3][0] = cam_pos.v[0];
                actor->t.t.mat.m[3][1] = cam_pos.v[1];
                actor->t.t.mat.m[3][2] = cam_pos.v[2];
            }
            break;
        case eTexture_animation_mirror:
            if (the_funk->proximity_count == 0) {
                the_funk->texture_animation_data.mirror_info.actor = NULL;
                GetALineAndDontArgue(pF, s);
                GetALineAndDontArgue(pF, s);
            } else {
                br_actor* actor;
                br_camera* camera;
                br_camera* global_camera;
                br_vector3 n;
                br_matrix34 mat;

                global_camera = gCamera->type_data;
                yon_factor = GetAScalar(pF);
                the_funk->texture_animation_data.mirror_info.yon = yon_factor * global_camera->yon_z;
                the_funk->texture_animation_data.mirror_info.field_0x5c = GetAnInt(pF);
                actor = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
                the_funk->texture_animation_data.mirror_info.actor = actor;
                if (actor == NULL) {
                    FatalError(kFatalError_CannotAllocateCamera);
                }
                camera = actor->type_data;
                camera->type = BR_CAMERA_PERSPECTIVE_FOV;
                camera->field_of_view = global_camera->field_of_view;
                camera->hither_z = global_camera->hither_z;
                camera->yon_z = 2.0f * global_camera->yon_z;
                actor = BrActorAdd(gUniverse_actor, actor);
                if (actor == NULL) {
                    FatalError(kFatalError_CannotAllocateCamera);
                }
                n = the_funk->proximity_array->n;
                the_funk->texture_animation_data.mirror_info.mat.m[0][0] = 1.0f - 2.0f * n.v[0] * n.v[0];
                the_funk->texture_animation_data.mirror_info.mat.m[0][1] = 2.0f * n.v[0] * n.v[1];
                the_funk->texture_animation_data.mirror_info.mat.m[0][2] = 2.0f * n.v[0] * n.v[2];
                the_funk->texture_animation_data.mirror_info.mat.m[1][0] = 2.0f * n.v[1] * n.v[0];
                the_funk->texture_animation_data.mirror_info.mat.m[1][1] = 1.0f - 2.0f * n.v[1] * n.v[1];
                the_funk->texture_animation_data.mirror_info.mat.m[1][2] = 2.0f * n.v[1] * n.v[2];
                the_funk->texture_animation_data.mirror_info.mat.m[2][0] = 2.0f * n.v[2] * n.v[0];
                the_funk->texture_animation_data.mirror_info.mat.m[2][1] = 2.0f * n.v[2] * n.v[1];
                the_funk->texture_animation_data.mirror_info.mat.m[2][2] = 1.0f - 2.0f * n.v[2] * n.v[2];
                the_funk->texture_animation_data.mirror_info.mat.m[3][0] = -2.0f * n.v[0] * the_funk->proximity_array->d;
                the_funk->texture_animation_data.mirror_info.mat.m[3][1] = -2.0f * n.v[1] * the_funk->proximity_array->d;
                the_funk->texture_animation_data.mirror_info.mat.m[3][2] = -2.0f * n.v[2] * the_funk->proximity_array->d;
                mat.m[0][0] = the_funk->proximity_array->v[1].v[0] - the_funk->proximity_array->v[0].v[0];
                mat.m[0][1] = the_funk->proximity_array->v[1].v[1] - the_funk->proximity_array->v[0].v[1];
                mat.m[0][2] = the_funk->proximity_array->v[1].v[2] - the_funk->proximity_array->v[0].v[2];
                mat.m[1][0] = the_funk->proximity_array->n.v[0];
                mat.m[1][1] = the_funk->proximity_array->n.v[1];
                mat.m[1][2] = the_funk->proximity_array->n.v[2];
                mat.m[2][0] = mat.m[0][1] * mat.m[1][2] - mat.m[0][2] * mat.m[1][1];
                mat.m[2][1] = mat.m[0][2] * mat.m[1][0] - mat.m[0][0] * mat.m[1][2];
                mat.m[2][2] = mat.m[0][0] * mat.m[1][1] - mat.m[0][1] * mat.m[1][0];
                // This basis has no initialized translation.
                BrVector3SetFloat(&the_funk->texture_animation_data.mirror_info.bounds.min, (float)0x7fffffff, (float)0x7fffffff, (float)0x7fffffff);
                BrVector3SetFloat(&the_funk->texture_animation_data.mirror_info.bounds.max, (float)-0x7fffffff, (float)-0x7fffffff, (float)-0x7fffffff);
                for (i = 0; i < the_funk->proximity_count; i++) {
                    int j;

                    for (j = 0; j < 3; j++) {
                        int k;

                        BrMatrix34ApplyP(&gFunk_temp_vertices[i].points[j],
                            &the_funk->proximity_array->v[j],
                            &mat);
                        for (k = 0; k < 3; k++) {
                            if (!(gFunk_temp_vertices[i].points[j].v[k] >= the_funk->texture_animation_data.mirror_info.bounds.min.v[k])) {
                                the_funk->texture_animation_data.mirror_info.bounds.min.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                            if (the_funk->texture_animation_data.mirror_info.bounds.max.v[k] < gFunk_temp_vertices[i].points[j].v[k]) {
                                the_funk->texture_animation_data.mirror_info.bounds.max.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                        }
                    }
                }
                for (i = 0; i < the_funk->proximity_count; i++) {
                    int j;

                    for (j = 0; j < 3; j++) {
                        int k;

                        gFunk_temp_vertices[i].vertices[j]->map.v[0] =
                            (gFunk_temp_vertices[i].points[j].v[0] - the_funk->texture_animation_data.mirror_info.bounds.min.v[0])
                                / (the_funk->texture_animation_data.mirror_info.bounds.max.v[0] - the_funk->texture_animation_data.mirror_info.bounds.min.v[0]);
                        gFunk_temp_vertices[i].vertices[j]->map.v[1] =
                            (gFunk_temp_vertices[i].points[j].v[2] - the_funk->texture_animation_data.mirror_info.bounds.min.v[2])
                                / (the_funk->texture_animation_data.mirror_info.bounds.max.v[2] - the_funk->texture_animation_data.mirror_info.bounds.min.v[0]);
                        for (k = 0; k < 3; k++) {
                            if (!(gFunk_temp_vertices[i].points[j].v[k] >= the_funk->texture_animation_data.mirror_info.bounds.min.v[k])) {
                                the_funk->texture_animation_data.mirror_info.bounds.min.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                            if (the_funk->texture_animation_data.mirror_info.bounds.max.v[k] < gFunk_temp_vertices[i].points[j].v[k]) {
                                the_funk->texture_animation_data.mirror_info.bounds.max.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                        }
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    BrMemFree(gFunk_temp_vertices);
}

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

// FUNCTION: CARMA2_HW 0x00477160
void C2_HOOK_FASTCALL KillGroovadelic(int pOwner) {
    tGroovidelic_spec* groove;
    int i;

    if (gGroovidelics_array == NULL) {
        return;
    }
    groove = gGroovidelics_array;
    for (i = 0; i < gGroovidelics_array_size; i++, groove++) {
        if (groove->owner == pOwner
                && groove->path_mode != eMove_controlled && groove->path_mode != eMove_absolute
                && groove->object_mode != eMove_controlled && groove->object_mode != eMove_absolute) {
            groove->owner = -999;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004771c0
void C2_HOOK_FASTCALL KillFunkotronic(int pOwner) {
    tFunkotronic_spec* funk;
    int i;

    if (gFunkotronics_array == NULL) {
        return;
    }
    funk = gFunkotronics_array;
    for (i = 0; i < gFunkotronics_array_size; i++, funk++) {
        if (funk->owner == pOwner
                && funk->matrix_mode != eMove_controlled && funk->matrix_mode != eMove_absolute
                && funk->lighting_animation_type != eMove_controlled && funk->lighting_animation_type != eMove_absolute
                && ((funk->texture_animation_data.frames_info.mode != eMove_controlled
                        && funk->texture_animation_data.frames_info.mode != eMove_texturebits)
                    || funk->texture_animation_type != eTexture_animation_frames)) {
            funk->owner = -999;
        }
    }
}

br_scalar C2_HOOK_FASTCALL NormaliseDegreeAngle(br_scalar pAngle) {
    while (!(pAngle >= 0.0f)) {
        pAngle += 360.0f;
    }
    return pAngle;
}

// FunkThoseTronics

// FUNCTION: CARMA2_HW 0x00478e30
void C2_HOOK_FASTCALL PathGrooveBastard(tGroovidelic_spec* pGroove, tU32 pTime, br_matrix34* pMat, int pInterrupt_it) {
    br_scalar pos;
    br_scalar* delta;
    double f_the_time;
    int i;
    int move;
    int use_cos;
    br_angle angle;

    f_the_time = (double)pTime;
    switch (pGroove->path_type) {
    case eGroove_path_straight:
        for (i = 0; i < 3; i++) {
            switch (i) {
            case 0:
                delta = &pGroove->path_data.straight_info.x_delta;
                break;
            case 1:
                delta = &pGroove->path_data.straight_info.y_delta;
                break;
            default:
                delta = &pGroove->path_data.straight_info.z_delta;
                break;
            }
            if (!(*delta < 0.0f || *delta > 0.0f)) {
                continue;
            }
            MOVE_FUNK_PARAMETER(pos, pGroove->path_mode, pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data, *delta, -*delta);
            pos += pGroove->path_data.straight_info.centre.v[i];
            move = 0;
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos >= pMat->m[3][i]) {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                }
                move = 1;
            } else if (pGroove->path_interrupt_status == eInterrupt_none) {
                move = 1;
            } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                if (!(pos >= pGroove->path_resumption_value)) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    move = 1;
                }
            } else if (pos > pGroove->path_resumption_value) {
                pGroove->path_interrupt_status = eInterrupt_none;
                move = 1;
            }
            if (move) {
                BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.straight_info.centre);
                pMat->m[3][i] = pos;
            }
        }
        BrVector3Copy(&pGroove->object_position, &pGroove->actor->t.t.translate.t);
        break;
    case eGroove_path_circular:
        BrVector3Copy(&pGroove->actor->t.t.translate.t, &pGroove->path_data.circular_info.centre);
        for (i = 0; i < 3; i++) {
            if (i == 0) {
                if (pGroove->path_data.circular_info.axis == eAxis_y) {
                    use_cos = 1;
                } else if (pGroove->path_data.circular_info.axis == eAxis_z) {
                    use_cos = 0;
                } else {
                    continue;
                }
            } else if (i == 1) {
                if (pGroove->path_data.circular_info.axis == eAxis_x) {
                    use_cos = 0;
                } else if (pGroove->path_data.circular_info.axis == eAxis_z) {
                    use_cos = 1;
                } else {
                    continue;
                }
            } else {
                if (pGroove->path_data.circular_info.axis == eAxis_x) {
                    use_cos = 1;
                } else if (pGroove->path_data.circular_info.axis == eAxis_y) {
                    use_cos = 0;
                } else {
                    continue;
                }
            }
            if (pGroove->path_data.circular_info.period.value < 0.0f || pGroove->path_data.circular_info.period.value > 0.0f) {
                angle = BrDegreeToAngle(SAW(f_the_time, pGroove->path_data.circular_info.period.value) * 360.0);
                if (use_cos) {
                    pos = (br_scalar)(cos(BrAngleToRadian(angle)) * pGroove->path_data.circular_info.radius);
                } else {
                    pos = (br_scalar)(sin(BrAngleToRadian(angle)) * pGroove->path_data.circular_info.radius);
                }
            } else {
                pos = 0.0f;
            }
            pMat->m[3][i] = pGroove->path_data.circular_info.centre.v[i] + pos;
        }
        BrVector3Copy(&pGroove->object_position, &pGroove->actor->t.t.translate.t);
        break;
    default:
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00479890
void C2_HOOK_FASTCALL ObjectGrooveBastard(tGroovidelic_spec* pGroove, tU32 pTime, br_matrix34* pMat, int pInterrupt_it) {
    int rock_it;
    br_scalar x_size;
    br_scalar y_size;
    br_scalar z_size;
    br_scalar pos;
    br_bounds* bounds;
    br_scalar x_extent;
    br_scalar y_extent;
    br_scalar z_extent;
    double f_the_time = (double)pTime;

    switch (pGroove->object_type) {
    case eGroove_object_spin:
        switch (pGroove->object_data.spin_info.axis) {
        case eAxis_x:
            MOVE_FUNK_PARAMETER(pos, pGroove->object_mode, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data, 360.f, -360.f);
            DRMatrix34PostRotateX(pMat, BrDegreeToAngle(pos));
            break;
        case eAxis_y:
            MOVE_FUNK_PARAMETER(pos, pGroove->object_mode, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data, 360.f, -360.f);
            DRMatrix34PostRotateY(pMat, BrDegreeToAngle(pos));
            break;
        case eAxis_z:
            MOVE_FUNK_PARAMETER(pos, pGroove->object_mode, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data, 360.f, -360.f);
            DRMatrix34PostRotateZ(pMat, BrDegreeToAngle(pos));
            break;
        }
        break;
    case eGroove_object_rock:
        MOVE_FUNK_PARAMETER(pos, pGroove->object_mode, pGroove->object_data.rock_info.period.value, pGroove->object_data.rock_info.texture_info.data, pGroove->object_data.rock_info.max_angle, pGroove->object_data.rock_info.max_angle);
        rock_it = 1;
        if (pInterrupt_it) {
            pGroove->object_resumption_value = pos;
            if (pos >= pGroove->object_data.rock_info.current_angle) {
                pGroove->object_interrupt_status = eInterrupt_greater_than;
            } else {
                pGroove->object_interrupt_status = eInterrupt_less_than;
            }
        } else {
            if (pGroove->object_interrupt_status == eInterrupt_less_than) {
                if (pos >= pGroove->object_resumption_value || gAction_replay_mode) {
                    rock_it = 0;
                } else {
                    pGroove->object_interrupt_status = eInterrupt_none;
                }
            } else if (pGroove->object_interrupt_status == eInterrupt_greater_than) {
                if (!(pos > pGroove->object_resumption_value) || gAction_replay_mode) {
                    rock_it = 0;
                } else {
                    pGroove->object_interrupt_status = eInterrupt_none;
                }
            }
        }
        if (rock_it) {
            pGroove->object_data.rock_info.current_angle = pos;
        }

        switch (pGroove->object_data.rock_info.axis) {
        case eAxis_x:
            DRMatrix34PostRotateX(pMat, BrDegreeToAngle(pGroove->object_data.rock_info.current_angle));
            break;
        case eAxis_y:
            DRMatrix34PostRotateY(pMat, BrDegreeToAngle(pGroove->object_data.rock_info.current_angle));
            break;
        case eAxis_z:
            DRMatrix34PostRotateZ(pMat, BrDegreeToAngle(pGroove->object_data.rock_info.current_angle));
            break;
        }
        break;
    case eGroove_object_throb:
        MOVE_FUNK_PARAMETER(z_size, pGroove->object_mode, pGroove->object_data.throb_info.z_period.value, pGroove->object_data.throb_info.z_texture_info.data, pGroove->object_data.throb_info.z_magnitude, -pGroove->object_data.throb_info.z_magnitude);
        MOVE_FUNK_PARAMETER(y_size, pGroove->object_mode, pGroove->object_data.throb_info.y_period.value, pGroove->object_data.throb_info.y_texture_info.data, pGroove->object_data.throb_info.y_magnitude, -pGroove->object_data.throb_info.y_magnitude);
        MOVE_FUNK_PARAMETER(x_size, pGroove->object_mode, pGroove->object_data.throb_info.x_period.value, pGroove->object_data.throb_info.x_texture_info.data, pGroove->object_data.throb_info.x_magnitude, -pGroove->object_data.throb_info.x_magnitude);

        BrMatrix34PostScale(pMat, x_size + 1.f, y_size + 1.f, z_size + 1.f);
        /* FALLTHROUGH */
    case eGroove_object_shear:
        bounds = &pGroove->actor->model->bounds;
        x_extent = bounds->max.v[0] - bounds->min.v[0];
        y_extent = bounds->max.v[1] - bounds->min.v[1];
        z_extent = bounds->max.v[2] - bounds->min.v[2];
        if (!(pGroove->object_data.shear_info.x_magnitude < 0.0f || pGroove->object_data.shear_info.x_magnitude > 0.0f)) {
            MOVE_FUNK_PARAMETER(z_size, pGroove->object_mode, pGroove->object_data.shear_info.z_period.value, pGroove->object_data.shear_info.z_texture_info.data, pGroove->object_data.shear_info.z_magnitude, -pGroove->object_data.shear_info.z_magnitude);
            MOVE_FUNK_PARAMETER(y_size, pGroove->object_mode, pGroove->object_data.shear_info.y_period.value, pGroove->object_data.shear_info.y_texture_info.data, pGroove->object_data.shear_info.y_magnitude, -pGroove->object_data.shear_info.y_magnitude);
            BrMatrix34PostShearX(pMat, y_size / y_extent, z_size / z_extent);
        } else if (!(pGroove->object_data.shear_info.y_magnitude < 0.0f || pGroove->object_data.shear_info.y_magnitude > 0.0f)) {
            MOVE_FUNK_PARAMETER(z_size, pGroove->object_mode, pGroove->object_data.shear_info.z_period.value, pGroove->object_data.shear_info.z_texture_info.data, pGroove->object_data.shear_info.z_magnitude, -pGroove->object_data.shear_info.z_magnitude);
            MOVE_FUNK_PARAMETER(x_size, pGroove->object_mode, pGroove->object_data.shear_info.x_period.value, pGroove->object_data.shear_info.x_texture_info.data, pGroove->object_data.shear_info.x_magnitude, -pGroove->object_data.shear_info.x_magnitude);
            BrMatrix34PostShearY(pMat, x_size / x_extent, z_size / z_extent);
        } else {
            MOVE_FUNK_PARAMETER(y_size, pGroove->object_mode, pGroove->object_data.shear_info.y_period.value, pGroove->object_data.shear_info.y_texture_info.data, pGroove->object_data.shear_info.y_magnitude, -pGroove->object_data.shear_info.y_magnitude);
            MOVE_FUNK_PARAMETER(x_size, pGroove->object_mode, pGroove->object_data.shear_info.x_period.value, pGroove->object_data.shear_info.x_texture_info.data, pGroove->object_data.shear_info.x_magnitude, -pGroove->object_data.shear_info.x_magnitude);
            BrMatrix34PostShearZ(pMat, x_size / x_extent, y_size / y_extent);
        }
        break;
    default:
        return;
    }
}

// FUNCTION: CARMA2_HW 0x00478c80
void C2_HOOK_FASTCALL GrooveThisDelic(tGroovidelic_spec* pGroove, tU32 pTime, int pInterrupt_it) {
    br_actor* the_actor;
    br_vector3 actor_pos;
    br_matrix34* the_mat;
    tInterrupt_status old_path_interrupt;
    tInterrupt_status old_object_interrupt;

    old_path_interrupt = pGroove->path_interrupt_status;
    old_object_interrupt = pGroove->object_interrupt_status;
    the_actor = pGroove->actor;
    pGroove->done_this_frame = 1;
    CalcActorGlobalPos(&actor_pos, the_actor);
    if (PointOutOfSight(&actor_pos, 0, pGroove->mode == eGroove_mode_distance ? gYon_squared : 36.f)) {
        return;
    }

    the_mat = &the_actor->t.t.mat;
    if ((!gAction_replay_mode
        || !ARReplayIsReallyPaused()
        || pGroove->path_mode == eMove_controlled
        || pGroove->path_mode == eMove_absolute) && !(pGroove->block_flags & 0x1)) {
        PathGrooveBastard(pGroove, pTime, the_mat, pInterrupt_it);
    }
    if (((pGroove->object_type != eGroove_object_none || pGroove->lollipop_mode != eLollipop_none)
            && (!gAction_replay_mode
                    || !ARReplayIsReallyPaused()
                    || pGroove->object_mode == eMove_controlled
                    || pGroove->object_mode == eMove_absolute)) && !(pGroove->block_flags & 0x2)) {
        the_mat->m[0][0] = 1.0f;
        the_mat->m[0][1] = 0.0f;
        the_mat->m[0][2] = 0.0f;
        the_mat->m[1][0] = 0.0f;
        the_mat->m[1][1] = 1.0f;
        the_mat->m[1][2] = 0.0f;
        the_mat->m[2][0] = 0.0f;
        the_mat->m[2][1] = 0.0f;
        the_mat->m[2][2] = 1.0f;
        the_mat->m[3][0] = -pGroove->object_centre.v[0];
        the_mat->m[3][1] = -pGroove->object_centre.v[1];
        the_mat->m[3][2] = -pGroove->object_centre.v[2];
        ObjectGrooveBastard(pGroove, pTime, the_mat, pInterrupt_it);
        the_mat->m[3][0] = pGroove->object_position.v[0] + the_mat->m[3][0] + pGroove->object_centre.v[0];
        the_mat->m[3][1] = pGroove->object_position.v[1] + the_mat->m[3][1] + pGroove->object_centre.v[1];
        the_mat->m[3][2] = pGroove->object_position.v[2] + the_mat->m[3][2] + pGroove->object_centre.v[2];
        if (pGroove->lollipop_mode != eLollipop_none) {
            LollipopizeActor(pGroove->actor, &gCamera_to_world, pGroove->lollipop_mode);
        }
    }
    if (pGroove->path_interrupt_status != old_path_interrupt || pGroove->object_interrupt_status != old_object_interrupt) {
        PipeSingleGrooveStop(
            pGroove - gGroovidelics_array,
            the_mat,
            pGroove->path_interrupt_status,
            pGroove->object_interrupt_status,
            pGroove->path_resumption_value,
            pGroove->object_resumption_value);
    }
}

// FUNCTION: CARMA2_HW 0x00478c00
void C2_HOOK_FASTCALL GrooveThoseDelics(void) {
    tGroovidelic_spec* the_groove;
    tU32 time;
    int i;

    if (gGroovidelics_array != NULL) {
        time = GetTotalTime();
        gPrevious_groove_times[1] = gPrevious_groove_times[0];
        the_groove = gGroovidelics_array;
        gPrevious_groove_times[0] = time;
        for (i = 0; i < gGroovidelics_array_size; i++, the_groove++) {
            if (the_groove->owner != -999 && !the_groove->done_this_frame) {
                GrooveThisDelic(the_groove, time, 0);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047b0e0
void C2_HOOK_FASTCALL StopGroovidelic(br_actor* pActor) {
    tGroovidelic_spec* the_groove;
    int count;
    int i;

    the_groove = gGroovidelics_array;
    count = gGroovidelics_array_size;
    for (i = 0; i < count; i++, the_groove++) {
        if (the_groove->actor == pActor) {
            if (the_groove->path_interrupt_status == eInterrupt_none
                && the_groove->object_interrupt_status == eInterrupt_none) {
                GrooveThisDelic(the_groove, gPrevious_groove_times[1], 1);
            }
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047b130
void C2_HOOK_FASTCALL DisableGroovidelic(br_actor* pActor, tU32 pBlock_flags) {
    tGroovidelic_spec* the_groove;
    int i;

    the_groove = gGroovidelics_array;
    for (i = 0; i < gGroovidelics_array_size; i++, the_groove++) {
        if (the_groove->actor == pActor) {
            the_groove->block_flags |= pBlock_flags;
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047b160
void C2_HOOK_FASTCALL EnableGroovidelic(br_actor* pActor, tU32 pBlock_flags) {
    tGroovidelic_spec* the_groove;
    int i;

    the_groove = gGroovidelics_array;
    for (i = 0; i < gGroovidelics_array_size; i++, the_groove++) {
        if (the_groove->actor == pActor) {
            the_groove->block_flags &= ~pBlock_flags;
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047b1a0
void C2_HOOK_FASTCALL SetGrooveInterrupt(int pIndex, const br_matrix34* pMatrix, tInterrupt_status pPath_interrupt_status, tInterrupt_status pObject_interrupt_status, float pPath_resumption_value, float pObject_resumption_value) {
    tGroovidelic_spec* the_groove;
    br_actor* actor;

    the_groove = &gGroovidelics_array[pIndex];
    actor = the_groove->actor;
    the_groove->path_interrupt_status = pPath_interrupt_status;
    the_groove->object_interrupt_status = pObject_interrupt_status;
    the_groove->path_resumption_value = pPath_resumption_value;
    the_groove->object_resumption_value = pObject_resumption_value;
    BrMatrix34Copy(&actor->t.t.mat, pMatrix);
}

// FUNCTION: CARMA2_HW 0x0047b1e0
void C2_HOOK_FASTCALL ResetGrooveFlags(void) {
    tGroovidelic_spec* the_groove;
    int i;

    the_groove = gGroovidelics_array;
    for (i = 0; i < gGroovidelics_array_size; i++, the_groove++) {
        the_groove->done_this_frame = 0;
    }
}

// FUNCTION: CARMA2_HW 0x0047b250
void C2_HOOK_FASTCALL DisableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags |= 1u;
    PipeSingleFunkEnable(pIndex, 0);
}

// FUNCTION: CARMA2_HW 0x0047b280
void C2_HOOK_FASTCALL EnableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags &= ~1u;
    PipeSingleFunkEnable(pIndex, 1);
}

// FUNCTION: CARMA2_HW 0x0047b2b0
void C2_HOOK_FASTCALL MasterDisableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags |= 2u;
}

// FUNCTION: CARMA2_HW 0x0047b2e0
void C2_HOOK_FASTCALL MasterEnableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags &= ~2u;
}

// FUNCTION: CARMA2_HW 0x0047b310
void C2_HOOK_FASTCALL AdjustFunkEnable(int pIndex, tS16 pFlags) {
    if (pFlags != 0) {
        EnableFunkotronic(pIndex);
    } else {
        DisableFunkotronic(pIndex);
    }
}

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

