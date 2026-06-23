#include "35-intrface.h"

#include "08-loading1.h"
#include "10-loading2.h"
#include "18-graphics2.h"
#include "19-font.h"
#include "38-flicplay.h"
#include "42-input.h"
#include "69-sound.h"
#include "globvars.h"
#include "platform.h"

// GLOBAL: CARMA2_HW 0x0068c1f0
int gAlways_typing;

// GLOBAL: CARMA2_HW 0x0068c1ec
int gDisabled_count;

// GLOBAL: CARMA2_HW 0x0068c1f8
int gDisabled_choices[10];

// GLOBAL: CARMA2_HW 0x0075b8fc
int gTyping_slot;

// GLOBAL: CARMA2_HW 0x0068c1e8
const tInterface_spec* gSpec;

// GLOBAL: CARMA2_HW 0x0068c220
int gCurrent_mode;

// GLOBAL: CARMA2_HW 0x0068c224
int gCurrent_choice;

// GLOBAL: CARMA2_HW 0x0068c228
tU32 gStart_time;

// GLOBAL: CARMA2_HW 0x0079ecb8
int gMouse_in_use;

// FUNCTION: CARMA2_HW 0x00484620
void C2_HOOK_FASTCALL SetAlwaysTyping(void) {

    gAlways_typing = 1;
}

// FUNCTION: CARMA2_HW 0x00484630
void C2_HOOK_FASTCALL ClearAlwaysTyping(void) {

    gAlways_typing = 0;
}

int C2_HOOK_FASTCALL ChoiceDisabled(int pChoice) {
    int i;

    for (i = 0; i < gDisabled_count; ++i) {
        if (gDisabled_choices[i] == pChoice) {
            return 1;
        }
    }
    return 0;
}

void C2_HOOK_FASTCALL ResetInterfaceTimeout__intrface(void) {

    gStart_time = PDGetTotalTime();
}

// FUNCTION: CARMA2_HW 0x00484dd0
void C2_HOOK_FASTCALL ChangeSelection(const tInterface_spec* pSpec, int* pOld_selection, int* pNew_selection, int pMode, int pSkip_disabled) {

    if (ChoiceDisabled(*pNew_selection)) {
        if (pSkip_disabled) {
            if (*pOld_selection < *pNew_selection) {
                do {
                    *pNew_selection += 1;
                    if (*pNew_selection < pSpec->move_up_min[pMode]) {
                        *pNew_selection = pSpec->move_up_max[pMode];
                    }
                    if (*pNew_selection > pSpec->move_up_max[pMode]) {
                        *pNew_selection = pSpec->move_up_min[pMode];
                    }
                } while (*pNew_selection != *pOld_selection && ChoiceDisabled(*pNew_selection));
            } else {
                do {
                    *pNew_selection -= 1;
                    if (*pNew_selection < pSpec->move_up_min[pMode]) {
                        *pNew_selection = pSpec->move_up_max[pMode];
                    }
                    if (*pNew_selection > pSpec->move_up_max[pMode]) {
                        *pNew_selection = pSpec->move_up_min[pMode];
                    }
                } while (*pNew_selection != *pOld_selection && ChoiceDisabled(*pNew_selection));
            }
        } else {
            *pNew_selection = *pOld_selection;
        }
    }

    if (*pOld_selection != *pNew_selection) {
        *pOld_selection = *pNew_selection;
    }
}

void C2_HOOK_FASTCALL RecopyAreas(const tInterface_spec* pSpec) {

    // empty
}

// FUNCTION: CARMA2_HW 0x004846e0
int C2_HOOK_FASTCALL DoInterfaceScreen(const tInterface_spec* pSpec, int pOptions, int pCurrent_choice) {
    tProg_status entry_status;
    int i;
    int last_choice;
    int escaped;
    int timed_out;
    int go_ahead;
    int result;
    int the_key;
    int the_max;
    int defeat_mode_change;
    char the_str[256];

    entry_status = gProgram_state.prog_status;
    gTyping_slot = -1;
    EdgeTriggerModeOn();
    gSpec = pSpec;
    gDisabled_count = 0;
    LoadInterfaceStuff(gProgram_state.racing);
    gProgram_state.dont_save_or_load = pSpec->dont_save_or_load;
    for (i = 0; i < pSpec->number_of_button_flics; i++) {
        LoadFlic(pSpec->flicker_on_flics[i].flic_index);
        LoadFlic(pSpec->flicker_off_flics[i].flic_index);
        LoadFlic(pSpec->pushed_flics[i].flic_index);
    }

    InitFlicQueue();
    ResetInterfaceTimeout__intrface();
    gCurrent_choice = pCurrent_choice;
    gCurrent_mode = pSpec->initial_imode;
    if (pSpec->font_needed) {
        InitRollingLetters();
        LoadFont(0);
    }
    KillSplashScreen();
    if (pSpec->start_proc1 != NULL) {
        pSpec->start_proc1();
    }
    if (pSpec->start_proc2 != NULL) {
        pSpec->start_proc2();
    }
    last_choice = -1;
    ChangeSelection(pSpec, &last_choice, &gCurrent_choice, gCurrent_mode, 0);
    WaitForNoKeys();
    gMouse_in_use = 0;
    last_choice = gCurrent_choice;
    for (;;) {
        if (last_choice != gCurrent_choice) {
            ChangeSelection(pSpec, &last_choice, &gCurrent_choice, gCurrent_mode, 1);
        }
        last_choice = gCurrent_choice;
        PollKeys();
        EdgeTriggerModeOff();
        the_key = PDAnyKeyDown();
        if (the_key != -1 && the_key != 4) {
            gMouse_in_use = 0;
            ResetInterfaceTimeout__intrface();
        }
        EdgeTriggerModeOn();
        if ((gTyping_slot < 0 || gAlways_typing) && (PDKeyDown(70) || PDKeyDown(85))) {
            if (pSpec->move_left_delta[gCurrent_mode] != 0) {
                gCurrent_choice += pSpec->move_left_delta[gCurrent_mode];
                if (gCurrent_choice < pSpec->move_left_min[gCurrent_mode]) {
                    gCurrent_choice = pSpec->move_left_max[gCurrent_mode];
                }
                if (gCurrent_choice > pSpec->move_left_max[gCurrent_mode]) {
                    gCurrent_choice = pSpec->move_left_min[gCurrent_mode];
                }
                DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
            }
            if (pSpec->move_left_proc[gCurrent_mode] != NULL) {
                defeat_mode_change = pSpec->move_left_proc[gCurrent_mode](&gCurrent_choice, &gCurrent_mode);
            } else {
                defeat_mode_change = 0;
            }
            if (pSpec->move_left_new_mode[gCurrent_mode] >= 0 && !defeat_mode_change) {
                gCurrent_mode = pSpec->move_left_new_mode[gCurrent_mode];
            }
        }
        if ((gTyping_slot < 0 || gAlways_typing) && (PDKeyDown(71) || PDKeyDown(87))) {
            if (pSpec->move_right_delta[gCurrent_mode] != 0) {
                gCurrent_choice += pSpec->move_right_delta[gCurrent_mode];
                if (gCurrent_choice < pSpec->move_right_min[gCurrent_mode]) {
                    gCurrent_choice = pSpec->move_right_max[gCurrent_mode];
                }
                if (gCurrent_choice > pSpec->move_right_max[gCurrent_mode]) {
                    gCurrent_choice = pSpec->move_right_min[gCurrent_mode];
                }
                DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
            }
            if (pSpec->move_right_proc[gCurrent_mode] != NULL) {
                defeat_mode_change = pSpec->move_right_proc[gCurrent_mode](&gCurrent_choice, &gCurrent_mode);
            } else {
                defeat_mode_change = 0;
            }
            if (pSpec->move_right_new_mode[gCurrent_mode] >= 0 && !defeat_mode_change) {
                gCurrent_mode = pSpec->move_right_new_mode[gCurrent_mode];
            }
        }
        if (PDKeyDown(72) || PDKeyDown(89)) {
            if (pSpec->move_up_delta[gCurrent_mode] != 0) {
                gCurrent_choice += pSpec->move_up_delta[gCurrent_mode];
                if (gCurrent_choice < pSpec->move_up_min[gCurrent_mode]) {
                    gCurrent_choice = pSpec->move_up_max[gCurrent_mode];
                }
                if (gCurrent_choice > pSpec->move_up_max[gCurrent_mode]) {
                    gCurrent_choice = pSpec->move_up_min[gCurrent_mode];
                }
                DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
            }
            if (pSpec->move_up_proc[gCurrent_mode] != NULL) {
                defeat_mode_change = pSpec->move_up_proc[gCurrent_mode](&gCurrent_choice, &gCurrent_mode);
            } else {
                defeat_mode_change = 0;
            }
            if (pSpec->move_up_new_mode[gCurrent_mode] >= 0 && !defeat_mode_change) {
                gCurrent_mode = pSpec->move_up_new_mode[gCurrent_mode];
            }
        }
        if (PDKeyDown(73) || PDKeyDown(83)) {
            if (pSpec->move_down_delta[gCurrent_mode] != 0) {
                gCurrent_choice += pSpec->move_down_delta[gCurrent_mode];
                if (gCurrent_choice < pSpec->move_down_min[gCurrent_mode]) {
                    gCurrent_choice = pSpec->move_down_max[gCurrent_mode];
                }
                if (gCurrent_choice > pSpec->move_down_max[gCurrent_mode]) {
                    gCurrent_choice = pSpec->move_down_min[gCurrent_mode];
                }
                DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
            }
            if (pSpec->move_down_proc[gCurrent_mode] != NULL) {
                defeat_mode_change = pSpec->move_down_proc[gCurrent_mode](&gCurrent_choice, &gCurrent_mode);
            } else {
                defeat_mode_change = 0;
            }
            if (pSpec->move_down_new_mode[gCurrent_mode] >= 0 && !defeat_mode_change) {
                gCurrent_mode = pSpec->move_down_new_mode[gCurrent_mode];
            }
        }
        if (gTyping_slot >= 0 && pSpec->typeable[gCurrent_mode] && gTyping_slot != gCurrent_choice && !gAlways_typing) {
            gCurrent_choice = gTyping_slot;
        }
        if (last_choice != gCurrent_choice) {
            ChangeSelection(pSpec, &last_choice, &gCurrent_choice, gCurrent_mode, 1);
        }
        timed_out = pSpec->time_out && (PDGetTotalTime() >= gStart_time + pSpec->time_out);
        RemoveTransientBitmaps(1);
        RecopyAreas(pSpec);
        go_ahead = 1;
        if (pSpec->go_ahead_proc[gCurrent_mode]) {
            go_ahead = pSpec->go_ahead_proc[gCurrent_mode](&gCurrent_choice, &gCurrent_mode);
        }
        if (PDKeyDown(63)) {
            DRS3StartSound(gEffects_outlet, eSoundId_EscEsc);
            escaped = pSpec->escape_allowed[gCurrent_mode];
            if (pSpec->escape_proc[gCurrent_mode] != NULL) {
                escaped = pSpec->escape_proc[gCurrent_mode](&gCurrent_choice, &gCurrent_mode);
            }
        } else {
            escaped = 0;
        }
        if (escaped && gTyping_slot >= 0 && !gAlways_typing) {
            escaped = 0;
            pSpec->get_original_string(0, gTyping_slot, the_str, &the_max);
            RevertTyping(gTyping_slot, the_str);
            gTyping = 0;
            gTyping_slot = -1;
        }
        if (last_choice != gCurrent_choice) {
            ChangeSelection(pSpec, &last_choice, &gCurrent_choice, gCurrent_mode, 1);
        }
        if (entry_status != eProg_idling && gProgram_state.prog_status == eProg_idling) {
            escaped = 1;
        }
        if (pSpec->exit_proc != NULL) {
            if (pSpec->exit_proc(&gCurrent_choice, &gCurrent_mode)) {
                break;
            }
        }
        if (go_ahead || timed_out || escaped) {
            break;
        }
    }

    gTyping = 0;
    if (pSpec->font_needed) {
        EndRollingLetters();
        DisposeFont(0);
    }
    RemoveTransientBitmaps(1);
    FlushFlicQueue();
    for (i = 0; i < pSpec->number_of_button_flics; i++) {
        UnlockFlic(pSpec->flicker_on_flics[i].flic_index);
        UnlockFlic(pSpec->flicker_off_flics[i].flic_index);
        UnlockFlic(pSpec->pushed_flics[i].flic_index);
    }

    if (gCurrent_choice == pSpec->escape_code) {
        escaped = 1;
        go_ahead = 0;
    }
    if (escaped) {
        gCurrent_choice = pSpec->escape_code;
    }
    if (pSpec->done_proc != NULL) {
        result = (pSpec->done_proc)(gCurrent_choice, gCurrent_mode, go_ahead, escaped, timed_out);
    } else {
        result = gCurrent_choice;
    }
    gProgram_state.dont_save_or_load = 0;
    EndMouseCursor();
    UnlockInterfaceStuff();
    EdgeTriggerModeOff();
    return result;
}

// FUNCTION: CARMA2_HW 0x00484f40
int C2_HOOK_FASTCALL WhichItemIsSelectedIn(tFrontend_spec* pFrontend, int pGroup) {
    int i;

    for (i = 0; i < pFrontend->count_items; i++) {
        tFrontend_item_spec* item = &pFrontend->items[i];

        if (item->group == pGroup && item->radioButton_selected) {
            return i;
        }
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x00484f70
void C2_HOOK_FASTCALL SelectThisItemIn(tFrontend_spec* pFrontend, int pGroup, int pValue) {
    int i;

    for (i = pFrontend->radios[pGroup - 1].indexFirstItem; i <= pFrontend->radios[pGroup - 1].indexLastItem; i++) {

        pFrontend->items[i].radioButton_selected = i == pValue;
    }
}
