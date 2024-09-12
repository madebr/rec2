#include "piping.h"

#include "globvars.h"
#include "opponent.h"
#include "utility.h"

#include <brender/brender.h>

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT(tPipe_smudge_data*, gSmudge_space, 0x00694104);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tU8*, gPipe_play_ptr, 0x006768b8, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gPlay_direction, 0x006768c0, 0);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tU8*, gPipe_record_ptr, 0x006768bc, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tU8*, gPipe_buffer_oldest, 0x006768ac, NULL);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(float, gReplay_rate, 0x00676900, 0.f);

void (C2_HOOK_FASTCALL * DisposePiping_original)(void);
void C2_HOOK_FASTCALL DisposePiping(void) {

#if defined(C2_HOOKS_ENABLED)
    DisposePiping_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004024a0, DisposePiping, DisposePiping_original)

void C2_HOOK_FASTCALL DisposeActionReplay(void) {

    DisposePiping();
    if (C2V(gSmudge_space) != NULL) {
        BrMemFree(C2V(gSmudge_space));
        C2V(gSmudge_space) = NULL;
    }
}
C2_HOOK_FUNCTION(0x004c6c60, DisposeActionReplay)

void C2_HOOK_FASTCALL InitLastDamageArrayEtc(void) {
    int i;
    int j;
    int cat;
    int car_count;
    tCar_spec* car;

    C2_HOOK_BUG_ON(REC2_ASIZE(car->frame_start_damage) != 3);

    for (cat = eVehicle_self; cat <= eVehicle_not_really; cat++) {
        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {
            if (cat == eVehicle_self) {
                car = &C2V(gProgram_state).current_car;
            } else {
                car = GetCarSpec(cat, i);
            }
            if (cat != eVehicle_not_really) {
                for (j = 0; j < REC2_ASIZE(car->frame_start_damage); j++) {
                    car->frame_start_damage[j] = 0;
                }
            }
            car->car_ID = (cat << 8) | i;
        }
    }
}
C2_HOOK_FUNCTION(0x004c6580, InitLastDamageArrayEtc)

void (C2_HOOK_FASTCALL * PipeSingleSkidAdjustment_original)(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial);
void C2_HOOK_FASTCALL PipeSingleSkidAdjustment(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial) {

#if defined(C2_HOOKS_ENABLED)
    PipeSingleSkidAdjustment_original(pSkid_num, pMatrix, pMaterial);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c8600, PipeSingleSkidAdjustment, PipeSingleSkidAdjustment_original)

void (C2_HOOK_FASTCALL * PipeCarPositions_original)(void);
void C2_HOOK_FASTCALL PipeCarPositions(void) {

#if defined(C2_HOOKS_ENABLED)
    PipeCarPositions_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004c6640, PipeCarPositions, PipeCarPositions_original)

tU8* C2_HOOK_FASTCALL ARGetPipePlayPtr(void) {

    return C2V(gPipe_play_ptr);
}
C2_HOOK_FUNCTION(0x00402df0, ARGetPipePlayPtr)

int C2_HOOK_FASTCALL ARGetReplayDirection(void) {

    return C2V(gPlay_direction);
}
C2_HOOK_FUNCTION(0x004023b0, ARGetReplayDirection)

int C2_HOOK_FASTCALL ARReplayForwards(void) {

    if (C2V(gPipe_play_ptr) == C2V(gPipe_record_ptr)) {
        return 0;
    }
    if (C2V(gPipe_play_ptr) == C2V(gPipe_buffer_oldest)) {
        return 1;
    }
    if (C2V(gReplay_rate) == 0.f) {
        return ARGetReplayDirection() > 0;
    }
    if (C2V(gReplay_rate) > 0.f) {
        return 1;
    } else {
        return 0;
    }
}
C2_HOOK_FUNCTION(0x00402300, ARReplayForwards)
