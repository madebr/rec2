#include "09-funks.h"

#include "52-errors.h"
#include "rec2_macros.h"

#include "c2_string.h"

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

// STUB: CARMA2_HW 0x00474950
void C2_HOOK_FASTCALL DisposeFunkotronics(int pOwner) {
    NOT_IMPLEMENTED();
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

// TryThisFunkLink

// FindSmashableMaterial

// AddFunkotronics

// STUB: CARMA2_HW 0x00476430
void C2_HOOK_FASTCALL DisposeGroovidelics(int pOwner) {
    NOT_IMPLEMENTED();
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

// AddGroovidelics

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

