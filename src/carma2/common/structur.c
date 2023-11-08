#include "structur.h"

#include "car.h"
#include "cutscene.h"
#include "drmem.h"
#include "errors.h"
#include "frontend.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "graphics.h"
#include "init.h"
#include "loading.h"
#include "loadsave.h"
#include "mainmenu.h"
#include "network.h"
#include "opponent.h"
#include "piping.h"
#include "racestrt.h"
#include "sound.h"
#include "temp.h"
#include "utility.h"

#include "c2_stdlib.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gMirror_on__structur, 0x00660ce0, 1);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMoney_pre_race, 0x007051f8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gAPO_pre_race, 3, 0x007051e0);

void C2_HOOK_FASTCALL StashCreditsAndAPO(void) {

    C2V(gMoney_pre_race) = C2V(gProgram_state).credits;
    C2V(gAPO_pre_race)[0] = C2V(gProgram_state).current_car.power_up_levels[0];
    C2V(gAPO_pre_race)[1] = C2V(gProgram_state).current_car.power_up_levels[1];
    C2V(gAPO_pre_race)[2] = C2V(gProgram_state).current_car.power_up_levels[2];
}
C2_HOOK_FUNCTION(0x004e3410, StashCreditsAndAPO)

void (C2_HOOK_FASTCALL * DoProgram_original)(void);
void C2_HOOK_FASTCALL DoProgram(void) {
#if defined(C2_HOOKS_ENABLED)
    DoProgram_original();
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00503c50, DoProgram, DoProgram_original);

void C2_HOOK_FASTCALL InitialiseProgramState(void) {

    C2V(gProgram_state).mirror_on = C2V(gMirror_on__structur);
    C2V(gProgram_state).prog_status = eProg_intro;
    C2V(gProgram_state).last_slot = 0;
    C2V(gProgram_state).view_type = eVT_Scene;
    C2V(gProgram_state).frank_or_anniness = eFrankie;
    C2V(gProgram_state).racing = 0;
    C2V(gProgram_state).cut_scene = 0;
    C2V(gProgram_state).saving = 0;
    C2V(gProgram_state).loading = 0;
    C2V(gProgram_state).dont_save_or_load = 0;
    C2V(gProgram_state).dont_load = 0;
    C2V(gProgram_state).field_0x2c = 0;
    if (C2V(gAusterity_mode)) {
        C2V(gProgram_state).prat_cam_on = 0;
    } else {
        C2V(gProgram_state).prat_cam_on = C2V(gPratcam_on);
    }
    C2V(gPratcam_on) = C2V(gProgram_state).prat_cam_on;
    C2V(gProgram_state).cockpit_on = C2V(gCockpit_on);
    C2V(gProgram_state).car_name[0] = '\0';
    SetSoundVolumes(0);
}

int C2_HOOK_FASTCALL ChooseOpponent(int pNastiness) {
    int i;
    int count;
    int temp_array[40];

    while (1) {
        count = 0;
        for (i = 0; i < C2V(gNumber_of_racers); i++) {
            if ((pNastiness == 0 || C2V(gOpponents)[i].strength_rating == pNastiness)
                && C2V(gProgram_state).current_car.index != i
                && !C2V(gOpponents)[i].picked
                && C2V(gOpponents)[i].strength_rating < 6
                && C2V(gOpponents)[i].strength_rating > 0) {
                temp_array[count] = i;
                count++;
            }
        }
        if (count != 0) {
            break;
        }
        if (pNastiness < 5) {
            pNastiness++;
        } else {
            pNastiness = 0;
        }
    }
    i = temp_array[IRandomBetween(0, count - 1)];
    C2V(gOpponents)[i].picked = 1;
    return i;
}
C2_HOOK_FUNCTION(0x00503b00, ChooseOpponent)

int C2_HOOK_FASTCALL NumberOfOpponentsLeft(void) {
    int i;
    int car_count;
    int result;
    tCar_spec* the_car;

    result = 0;
    car_count = GetCarCount(eVehicle_opponent);
    for (i = 0; i < car_count; i++) {
        the_car = GetCarSpec(eVehicle_opponent, i);
        if (!the_car->knackered) {
            result++;
        }
    }
    return result;
}

void C2_HOOK_FASTCALL SelectOpponents(tRace_info* pRace_info) {
    int i;

    if (C2V(gNet_mode) != eNet_mode_none) {
        return;
    }

    pRace_info->number_of_racers = pRace_info->race_spec->count_explicit_opponents;
    for (i = 0; i < C2V(gNumber_of_racers); i++) {
        C2V(gOpponents)[i].picked = 0;
    }
    for (i = 0; i < pRace_info->race_spec->count_explicit_opponents; i++) {
        if (pRace_info->race_spec->explicit_opponents[i] >= 0) {
            C2V(gOpponents)[i].picked = 1;
            pRace_info->opponent_list[i].index = pRace_info->race_spec->explicit_opponents[i];
            pRace_info->opponent_list[i].ranking = -(i + 1);
        }
    }
    for (i = 0; i < pRace_info->race_spec->count_explicit_opponents; i++) {
        if (pRace_info->race_spec->explicit_opponents[i] < 0) {
            pRace_info->opponent_list[i].index = ChooseOpponent(-pRace_info->race_spec->explicit_opponents[i]);
            pRace_info->opponent_list[i].ranking = IRandomBetween(0, 99);
        }
    }
}
C2_HOOK_FUNCTION(0x005039e0, SelectOpponents)
