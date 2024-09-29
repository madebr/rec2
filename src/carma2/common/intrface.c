#include "intrface.h"

#include "errors.h"
#include "flicplay.h"
#include "frontend.h"
#include "globvars.h"
#include "graphics.h"
#include "input.h"
#include "loading.h"
#include "platform.h"
#include "sound.h"
#include "utility.h"

#include <brender/brender.h>
#include "rec2_macros.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_item_spec, gDefaultLastInterfaceItem, 0x00604730, FIXME);
C2_HOOK_VARIABLE_IMPLEMENT(int, gAlways_typing, 0x0068c1f0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDisabled_count, 0x0068c1ec);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gDisabled_choices, 10, 0x0068c1f8);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gStart_time, 0x0068c228);
C2_HOOK_VARIABLE_IMPLEMENT(const tInterface_spec*, gSpec, 0x0068c1e8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCurrent_mode, 0x0068c220);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCurrent_choice, 0x0068c224);

void C2_HOOK_FASTCALL OriginalResetInterfaceTimeout(void) {
    C2V(gStart_time) = PDGetTotalTime();
}
C2_HOOK_FUNCTION(0x00484640, OriginalResetInterfaceTimeout)

int (C2_HOOK_FASTCALL * DoInterfaceScreen_original)(const tInterface_spec* pSpec, int pOptions, int pCurrent_choice);
int C2_HOOK_FASTCALL DoInterfaceScreen(const tInterface_spec* pSpec, int pOptions, int pCurrent_choice) {

#if 0//defined(C2_HOOKS_ENABLED)
    return DoInterfaceScreen_original(pSpec, pOptions, pCurrent_choice);
#else
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

    C2_HOOK_BUG_ON(sizeof(tInterface_spec) != 0x12c);

    // Relict from Carmageddon 1, unused here
    C2_HOOK_ASSUME_UNUSED();

    entry_status = C2V(gProgram_state).prog_status;
    C2V(gTyping_slot) = -1;
    EdgeTriggerModeOn();
    C2V(gSpec) = pSpec;
    C2V(gDisabled_count) = 0;
    LoadInterfaceStuff(C2V(gProgram_state).racing);
    C2V(gProgram_state).dont_save_or_load = pSpec->dont_save_or_load;
    for (i = 0; i < pSpec->number_of_button_flics; i++) {
        LoadFlic(pSpec->flicker_on_flics[i].flic_index);
        LoadFlic(pSpec->flicker_off_flics[i].flic_index);
        LoadFlic(pSpec->pushed_flics[i].flic_index);
    }

    InitFlicQueue();
    OriginalResetInterfaceTimeout();
    C2V(gCurrent_choice) = pCurrent_choice;
    C2V(gCurrent_mode) = pSpec->initial_imode;
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
    ChangeSelection(pSpec, &last_choice, &C2V(gCurrent_choice), C2V(gCurrent_mode), 0);
    WaitForNoKeys();
    C2V(gMouse_in_use) = 0;
    last_choice = C2V(gCurrent_choice);
    for (;;) {
        if (last_choice != C2V(gCurrent_choice)) {
            ChangeSelection(pSpec, &last_choice, &C2V(gCurrent_choice), C2V(gCurrent_mode), 1);
        }
        last_choice = C2V(gCurrent_choice);
        PollKeys();
        EdgeTriggerModeOff();
        the_key = PDAnyKeyDown();
        if (the_key != -1 && the_key != 4) {
            C2V(gMouse_in_use) = 0;
            OriginalResetInterfaceTimeout();
        }
        EdgeTriggerModeOn();
        if ((C2V(gTyping_slot) < 0 || C2V(gAlways_typing)) && (PDKeyDown(70) || PDKeyDown(85))) {
            if (pSpec->move_left_delta[C2V(gCurrent_mode)] != 0) {
                C2V(gCurrent_choice) += pSpec->move_left_delta[C2V(gCurrent_mode)];
                if (C2V(gCurrent_choice) < pSpec->move_left_min[C2V(gCurrent_mode)]) {
                    C2V(gCurrent_choice) = pSpec->move_left_max[C2V(gCurrent_mode)];
                }
                if (C2V(gCurrent_choice) > pSpec->move_left_max[C2V(gCurrent_mode)]) {
                    C2V(gCurrent_choice) = pSpec->move_left_min[C2V(gCurrent_mode)];
                }
                DRS3StartSound(C2V(gEffects_outlet), eSoundId_LeftButton);
            }
            if (pSpec->move_left_proc[C2V(gCurrent_mode)] != NULL) {
                defeat_mode_change = pSpec->move_left_proc[C2V(gCurrent_mode)](&C2V(gCurrent_choice), &C2V(gCurrent_mode));
            } else {
                defeat_mode_change = 0;
            }
            if (pSpec->move_left_new_mode[C2V(gCurrent_mode)] >= 0 && !defeat_mode_change) {
                C2V(gCurrent_mode) = pSpec->move_left_new_mode[C2V(gCurrent_mode)];
            }
        }
        if ((C2V(gTyping_slot) < 0 || C2V(gAlways_typing)) && (PDKeyDown(71) || PDKeyDown(87))) {
            if (pSpec->move_right_delta[C2V(gCurrent_mode)] != 0) {
                C2V(gCurrent_choice) += pSpec->move_right_delta[C2V(gCurrent_mode)];
                if (C2V(gCurrent_choice) < pSpec->move_right_min[C2V(gCurrent_mode)]) {
                    C2V(gCurrent_choice) = pSpec->move_right_max[C2V(gCurrent_mode)];
                }
                if (C2V(gCurrent_choice) > pSpec->move_right_max[C2V(gCurrent_mode)]) {
                    C2V(gCurrent_choice) = pSpec->move_right_min[C2V(gCurrent_mode)];
                }
                DRS3StartSound(C2V(gEffects_outlet), eSoundId_LeftButton);
            }
            if (pSpec->move_right_proc[C2V(gCurrent_mode)] != NULL) {
                defeat_mode_change = pSpec->move_right_proc[C2V(gCurrent_mode)](&C2V(gCurrent_choice), &C2V(gCurrent_mode));
            } else {
                defeat_mode_change = 0;
            }
            if (pSpec->move_right_new_mode[C2V(gCurrent_mode)] >= 0 && !defeat_mode_change) {
                C2V(gCurrent_mode) = pSpec->move_right_new_mode[C2V(gCurrent_mode)];
            }
        }
        if (PDKeyDown(72) || PDKeyDown(89)) {
            if (pSpec->move_up_delta[C2V(gCurrent_mode)] != 0) {
                C2V(gCurrent_choice) += pSpec->move_up_delta[C2V(gCurrent_mode)];
                if (C2V(gCurrent_choice) < pSpec->move_up_min[C2V(gCurrent_mode)]) {
                    C2V(gCurrent_choice) = pSpec->move_up_max[C2V(gCurrent_mode)];
                }
                if (C2V(gCurrent_choice) > pSpec->move_up_max[C2V(gCurrent_mode)]) {
                    C2V(gCurrent_choice) = pSpec->move_up_min[C2V(gCurrent_mode)];
                }
                DRS3StartSound(C2V(gEffects_outlet), eSoundId_LeftButton);
            }
            if (pSpec->move_up_proc[C2V(gCurrent_mode)] != NULL) {
                defeat_mode_change = pSpec->move_up_proc[C2V(gCurrent_mode)](&C2V(gCurrent_choice), &C2V(gCurrent_mode));
            } else {
                defeat_mode_change = 0;
            }
            if (pSpec->move_up_new_mode[C2V(gCurrent_mode)] >= 0 && !defeat_mode_change) {
                C2V(gCurrent_mode) = pSpec->move_up_new_mode[C2V(gCurrent_mode)];
            }
        }
        if (PDKeyDown(73) || PDKeyDown(83)) {
            if (pSpec->move_down_delta[C2V(gCurrent_mode)] != 0) {
                C2V(gCurrent_choice) += pSpec->move_down_delta[C2V(gCurrent_mode)];
                if (C2V(gCurrent_choice) < pSpec->move_down_min[C2V(gCurrent_mode)]) {
                    C2V(gCurrent_choice) = pSpec->move_down_max[C2V(gCurrent_mode)];
                }
                if (C2V(gCurrent_choice) > pSpec->move_down_max[C2V(gCurrent_mode)]) {
                    C2V(gCurrent_choice) = pSpec->move_down_min[C2V(gCurrent_mode)];
                }
                DRS3StartSound(C2V(gEffects_outlet), eSoundId_LeftButton);
            }
            if (pSpec->move_down_proc[C2V(gCurrent_mode)] != NULL) {
                defeat_mode_change = pSpec->move_down_proc[C2V(gCurrent_mode)](&C2V(gCurrent_choice), &C2V(gCurrent_mode));
            } else {
                defeat_mode_change = 0;
            }
            if (pSpec->move_down_new_mode[C2V(gCurrent_mode)] >= 0 && !defeat_mode_change) {
                C2V(gCurrent_mode) = pSpec->move_down_new_mode[C2V(gCurrent_mode)];
            }
        }
        if (C2V(gTyping_slot) >= 0 && pSpec->typeable[C2V(gCurrent_mode)] && C2V(gTyping_slot) != C2V(gCurrent_choice) && !C2V(gAlways_typing)) {
            C2V(gCurrent_choice) = C2V(gTyping_slot);
        }
        if (last_choice != C2V(gCurrent_choice)) {
            ChangeSelection(pSpec, &last_choice, &C2V(gCurrent_choice), C2V(gCurrent_mode), 1);
        }
        timed_out = pSpec->time_out && (PDGetTotalTime() >= C2V(gStart_time) + pSpec->time_out);
        RemoveTransientBitmaps(1);
        go_ahead = 1;
        if (pSpec->go_ahead_proc[C2V(gCurrent_mode)]) {
            go_ahead = pSpec->go_ahead_proc[C2V(gCurrent_mode)](&C2V(gCurrent_choice), &C2V(gCurrent_mode));
        }
        if (PDKeyDown(63)) {
            DRS3StartSound(C2V(gEffects_outlet), eSoundId_EscEsc);
            escaped = pSpec->escape_allowed[C2V(gCurrent_mode)];
            if (pSpec->escape_proc[C2V(gCurrent_mode)] != NULL) {
                escaped = pSpec->escape_proc[C2V(gCurrent_mode)](&C2V(gCurrent_choice), &C2V(gCurrent_mode));
            }
        } else {
            escaped = 0;
        }
        if (escaped && C2V(gTyping_slot) >= 0 && !C2V(gAlways_typing)) {
            pSpec->get_original_string(0, C2V(gTyping_slot), the_str, &the_max);
            escaped = 0;
            RevertTyping(C2V(gTyping_slot), the_str);
            C2V(gTyping) = 0;
            C2V(gTyping_slot) = -1;
        }
        if (last_choice != C2V(gCurrent_choice)) {
            ChangeSelection(pSpec, &last_choice, &C2V(gCurrent_choice), C2V(gCurrent_mode), 1);
        }
        if (entry_status != eProg_idling && C2V(gProgram_state).prog_status == eProg_idling) {
            escaped = 1;
        }
        if (pSpec->exit_proc != NULL) {
            if (pSpec->exit_proc(&C2V(gCurrent_choice), &C2V(gCurrent_mode))) {
                break;
            }
        }
        if (go_ahead || timed_out || escaped) {
            break;
        }
    }

    C2V(gTyping) = 0;
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

    if (C2V(gCurrent_choice) == pSpec->escape_code) {
        escaped = 1;
        go_ahead = 0;
    }
    if (escaped) {
        C2V(gCurrent_choice) = pSpec->escape_code;
    }
    if (pSpec->done_proc != NULL) {
        result = (pSpec->done_proc)(C2V(gCurrent_choice), C2V(gCurrent_mode), go_ahead, escaped, timed_out);
    } else {
        result = C2V(gCurrent_choice);
    }
    C2V(gProgram_state).dont_save_or_load = 0;
    EndMouseCursor();
    UnlockInterfaceStuff();
    EdgeTriggerModeOff();
    return result;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004846e0, DoInterfaceScreen, DoInterfaceScreen_original)

void C2_HOOK_FASTCALL LoadMenuSettings(tFrontend_spec* pFrontend) {
    tPath_name path;
    char buffer[256];
    char* str;
    FILE* f;
    int count;
    int version;
    int itemCount;
    int i;
    int j;

    C2_HOOK_BUG_ON(sizeof(tFrontend_spec) != 0xb8c8);
    C2_HOOK_BUG_ON(sizeof(tFrontend_item_spec) != 0x158);
    C2_HOOK_BUG_ON(sizeof(tFrontend_scroller_spec) != 0x34);
    C2_HOOK_BUG_ON(sizeof(tFrontend_radio_spec) != 0x2c);
    C2_HOOK_BUG_ON(offsetof(tFrontend_spec, items) != 0x130);
    C2_HOOK_BUG_ON(offsetof(tFrontend_spec, scrollers) != 0x8794);
    C2_HOOK_BUG_ON(offsetof(tFrontend_spec, radios) != 0x9be8);
    C2_HOOK_BUG_ON(offsetof(tFrontend_spec, levels) != 0xaf24);

    PathCat(path, C2V(gApplication_path), "INTRFACE");
    PathCat(path, path, pFrontend->name);
    c2_strcat(path, ".TXT");

    f = DRfopen(path, "rt");
    if (f == NULL) {
        // Changed 0xb2 (`Mysterious "%" in %`) -> 0x6b (`Can't open %`)
        FatalError(kFatalError_CantOpen_S, path);
        return;
    }
    pFrontend->loaded = 1;
    GetALineAndDontArgue(f, buffer);
    str = c2_strtok(buffer, "\t ,/");
    if (DRStricmp(str, "VERSION") != 0) {
        FatalError(kFatalError_FileMustStartWith_SS, pFrontend->name, "VERSION");
    }
    str = c2_strtok(NULL, "\t ,/");
    count = c2_sscanf(str, "%d", &version);
    if (count == 0 || version != 3) {
        FatalError(kFatalError_CantCopeWithVersionFor_SS, str, pFrontend->name);
    }
    GetAString(f, pFrontend->name);
    itemCount = GetAnInt(f);
    if (itemCount != pFrontend->count_items) {
        BrFailure("Error - Menu item number mismatch ");
    }
    pFrontend->count_groups = GetAnInt(f);
    GetAString(f, pFrontend->backdrop_name);
    for (i = 0; i < pFrontend->count_items; i++) {
        GetAString(f, pFrontend->items[i].text);
        if (DRStricmp(pFrontend->items[i].text, "XXX") == 0) {
            pFrontend->items[i].text[0] = '\0';
        }
        str = pFrontend->items[i].text;
        while (*str != '\0') {
            if (*str == '_') {
                *str = ' ';
            }
            str++;
        }
        GetPairOfInts(f, &pFrontend->items[i].x, &pFrontend->items[i].y);
        pFrontend->items[i].x2 = pFrontend->items[i].x;
        pFrontend->items[i].y2 = pFrontend->items[i].y;
        GetPairOfInts(f, &pFrontend->items[i].width, &pFrontend->items[i].height);
        GetPairOfInts(f, &pFrontend->items[i].unlitFont, &pFrontend->items[i].highFont);
        pFrontend->items[i].map_index = GetAnInt(f);
        pFrontend->items[i].greyedBackdrop = GetAnInt(f);
        pFrontend->items[i].selectable = GetAnInt(f);
        pFrontend->items[i].group = GetAnInt(f);
        pFrontend->items[i].idLevelBar = GetAnInt(f);
        pFrontend->items[i].wrapText = GetAnInt(f);
        pFrontend->items[i].glowDisabled = GetAnInt(f);
    }
    // Copy item 99 (=last index)
    // FIXME: stringid = 0x401 (==> find out function of this item)
    c2_memcpy(&pFrontend->items[REC2_ASIZE(pFrontend->items) - 1], &C2V(gDefaultLastInterfaceItem), sizeof(tFrontend_item_spec));

    pFrontend->count_scrollers = GetAnInt(f);
    for (i = 0; i < pFrontend->count_scrollers; i++) {
        GetPairOfInts(f, &pFrontend->scrollers[i].id, &pFrontend->scrollers[i].count);
        GetPairOfInts(f, &pFrontend->scrollers[i].nbDisplayedAtOnce, &pFrontend->scrollers[i].indexTopItem);
        GetPairOfInts(f, &pFrontend->scrollers[i].indexFirstScrollableItem, &pFrontend->scrollers[i].indexLastScrollableItem);
        pFrontend->scrollers[i].greyedOutBackdrop = GetAnInt(f);
        GetPairOfInts(f, &pFrontend->scrollers[i].x_greyArea, &pFrontend->scrollers[i].y_greyArea);
        GetPairOfInts(f, &pFrontend->scrollers[i].width_greyArea, &pFrontend->scrollers[i].height_greyArea);
        count = pFrontend->scrollers[i].nbDisplayedAtOnce;
        for (j = pFrontend->scrollers[i].indexTopItem; j <= pFrontend->scrollers[i].indexLastScrollableItem; j++) {
            pFrontend->items[j].visible = count > 0;
            count --;
        }
        pFrontend->scrollers[i].indexOfItemAtTop = pFrontend->scrollers[i].indexTopItem;
    }

    pFrontend->count_radios = GetAnInt(f);
    for (i = 0; i < pFrontend->count_radios; i++) {
        GetPairOfInts(f, &pFrontend->radios[i].id, &pFrontend->radios[i].count);
        GetPairOfInts(f, &pFrontend->radios[i].indexFirstItem, &pFrontend->radios[i].indexLastItem);
        GetPairOfInts(f, &pFrontend->radios[i].indexSelected, &pFrontend->radios[i].greyboxRequested);
        GetPairOfInts(f, &pFrontend->radios[i].x_greybox, &pFrontend->radios[i].y_greybox);
        GetPairOfInts(f, &pFrontend->radios[i].width_greybox, &pFrontend->radios[i].height_greybox);
    }

    pFrontend->count_levels = GetAnInt(f);
    for (i = 0; i < pFrontend->count_levels; i++) {
        GetPairOfInts(f, &pFrontend->levels[i].field_0x00, &pFrontend->levels[i].field_0x04);
        GetPairOfInts(f, &pFrontend->levels[i].first_item_id, &pFrontend->levels[i].last_item_id);
    }

    PFfclose(f);
}
C2_HOOK_FUNCTION(0x00466760, LoadMenuSettings)

void C2_HOOK_FASTCALL ClearAlwaysTyping(void) {

    C2V(gAlways_typing) = 0;
}
C2_HOOK_FUNCTION(0x00484630, ClearAlwaysTyping)

int C2_HOOK_FASTCALL ChoiceDisabled(int pChoice) {
    int i;

    for (i = 0; i < C2V(gDisabled_count); ++i) {
        if (C2V(gDisabled_choices)[i] == pChoice) {
            return 1;
        }
    }
    return 0;
}

void C2_HOOK_FASTCALL ChangeSelection(const tInterface_spec* pSpec, int* pOld_selection, int* pNew_selection, int pMode, int pSkip_disabled) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tInterface_spec, move_up_min, 0x7c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tInterface_spec, move_up_max, 0x84);

    if (ChoiceDisabled(*pNew_selection)) {
        if (!pSkip_disabled) {
            *pNew_selection = *pOld_selection;
        } else if (*pOld_selection < *pNew_selection) {
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
    }

    if (*pOld_selection != *pNew_selection) {
        *pOld_selection = *pNew_selection;
    }
}
C2_HOOK_FUNCTION(0x00484dd0, ChangeSelection)
