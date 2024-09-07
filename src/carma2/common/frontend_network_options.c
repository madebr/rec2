#include "frontend_network_options.h"

#include "frontend.h"
#include "frontend_network.h"
#include "frontend_quit.h"
#include "platform.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_NETWORK_OPTIONS, 0x00610208, {
    FIXME TODO
});
C2_HOOK_VARIABLE_IMPLEMENT(tNet_game_options, gFrontend_backup_net_options, 0x006864a0);
C2_HOOK_VARIABLE_IMPLEMENT(tStruct_00686508, gUNK_006886e8, 0x006886e8);
C2_HOOK_VARIABLE_IMPLEMENT(tStruct_00686508, gUNK_006883b0, 0x006883b0);

void C2_HOOK_FASTCALL DisplayNetworkOptions(tFrontend_spec* pFrontend) {

    if (C2V(gFrontend_net_options).show_players_on_map) {
        SelectThisItemIn(pFrontend, 1, 2);
    } else {
        SelectThisItemIn(pFrontend, 1, 3);
    }
    if (C2V(gFrontend_net_options).powerup_respawn) {
        SelectThisItemIn(pFrontend, 2, 5);
    } else {
        SelectThisItemIn(pFrontend, 2, 6);
    }
    if (C2V(gFrontend_net_options).open_game) {
        SelectThisItemIn(pFrontend, 3, 8);
    } else {
        SelectThisItemIn(pFrontend, 3, 9);
    }
    if (C2V(gFrontend_net_options).grid_start) {
        SelectThisItemIn(pFrontend, 4, 11);
    } else {
        SelectThisItemIn(pFrontend, 4, 12);
    }
    switch (C2V(gFrontend_net_options).race_sequence_type) {
    case 0:
        SelectThisItemIn(pFrontend, 5, 14);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 5, 15);
        break;
    }
    if (C2V(gFrontend_net_options).random_car_choice) {
        SelectThisItemIn(pFrontend, 6, 17);
    } else {
        SelectThisItemIn(pFrontend, 6, 18);
    }
    switch (C2V(gFrontend_net_options).car_choice) {
    case 0:
        SelectThisItemIn(pFrontend, 7, 20);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 7, 21);
        break;
    case 2:
        SelectThisItemIn(pFrontend, 7, 22);
        break;
    }
    c2_sprintf(pFrontend->items[24].text, "%i", C2V(gFrontend_net_options).starting_credits);
    switch (C2V(gFrontend_game_type)) {
    case eNet_game_type_0:
    case eNet_game_type_2:
    case eNet_game_type_3:
    case eNet_game_type_4:
    case eNet_game_type_6:
        pFrontend->items[25].stringId = 232;
        break;
    case eNet_game_type_1:
        pFrontend->items[25].stringId = 233;
        break;
    case eNet_game_type_5:
        pFrontend->items[25].stringId = 234;
        break;
    case eNet_game_type_foxy:
        pFrontend->items[25].stringId = 235;
        break;
    }
    pFrontend->items[25].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[26].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[30].enabled = kFrontendItemEnabled_enabled;
    pFrontend->items[29].enabled = kFrontendItemEnabled_enabled;
    switch (C2V(gFrontend_game_type)) {
    case eNet_game_type_0:
    case eNet_game_type_2:
    case eNet_game_type_3:
    case eNet_game_type_4:
    case eNet_game_type_6:
        pFrontend->items[25].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[26].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[30].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[29].enabled = kFrontendItemEnabled_disabled;
        c2_strcpy(pFrontend->items[26].text, "----");
        break;
    case eNet_game_type_1:
    case eNet_game_type_5:
        c2_sprintf(pFrontend->items[26].text, "%i", C2V(gFrontend_net_options).starting_target);
        break;
    case eNet_game_type_foxy:
        {
            int seconds = C2V(gFrontend_net_options).starting_target / 1000;
            int minutes = (seconds / 60) % 60;
            int hours = seconds / 3600;
            if (hours == 0) {
                c2_sprintf(pFrontend->items[26].text, "%02i:%02i", minutes, seconds % 60);
            } else {
                c2_sprintf(pFrontend->items[26].text, "%02i:%02i:%02i", hours, minutes, seconds % 60);
            }
        }
        break;
    }
    FuckWithWidths(pFrontend);
}

void C2_HOOK_FASTCALL BackupNetworkOptions(void) {

    C2_HOOK_BUG_ON(sizeof(C2V(gFrontend_net_options)) != 0x30);
    C2_HOOK_BUG_ON(sizeof(C2V(gFrontend_backup_net_options)) != 0x30);
    c2_memcpy(&C2V(gFrontend_backup_net_options), &C2V(gFrontend_net_options), sizeof(C2V(gFrontend_net_options)));
}

int C2_HOOK_FASTCALL NetOptions_InFunc(tFrontend_spec* pFrontend) {

    Generic_Infunc(pFrontend);
    DisplayNetworkOptions(pFrontend);
    BackupNetworkOptions();
    C2V(gUNK_006886e8).field_0x0 = 24;
    C2V(gUNK_006886e8).field_0x4 = 28;
    C2V(gUNK_006886e8).field_0x8 = 27;
    C2V(gUNK_006886e8).next = &C2V(gUNK_006883b0);
    C2V(gUNK_006883b0).field_0x0 = 26;
    C2V(gUNK_006883b0).field_0x4 = 30;
    C2V(gUNK_006883b0).field_0x8 = 29;
    C2V(gUNK_006883b0).next = NULL;
    C2V(gPTR_00686508) = &C2V(gUNK_006886e8);
    return 0;
}
C2_HOOK_FUNCTION(0x00472ed0, NetOptions_InFunc)

int C2_HOOK_FASTCALL NetOptions_Outfunc(tFrontend_spec* pFrontend) {

    return 0;
}
C2_HOOK_FUNCTION(0x00473210, NetOptions_Outfunc)

int C2_HOOK_FASTCALL NetOptions_CreditsRoller(tFrontend_spec* pFrontend) {

    C2V(gFrontend_net_current_roll) = PDGetTotalTime();
    if (C2V(gFrontend_net_current_roll) - C2V(gFrontend_net_last_roll) < 300) {
        return 0;
    }
    if (C2V(gFrontend_selected_item_index) == 28) {
        C2V(gFrontend_net_options).starting_credits += 1000;
    } else if (C2V(gFrontend_selected_item_index) == 27) {
        C2V(gFrontend_net_options).starting_credits -= 1000;
    }
    if (C2V(gFrontend_net_options).starting_credits < 0) {
        C2V(gFrontend_net_options).starting_credits = 0;
    } else if (C2V(gFrontend_net_options).starting_credits > 500000) {
        C2V(gFrontend_net_options).starting_credits = 500000;
    }
    C2V(gFrontend_net_last_roll) = C2V(gFrontend_net_current_roll);
    c2_sprintf(pFrontend->items[24].text, "%i", C2V(gFrontend_net_options).starting_credits);
    FuckWithWidths(pFrontend);
    return 0;
}
C2_HOOK_FUNCTION(0x00473220, NetOptions_CreditsRoller)

int C2_HOOK_FASTCALL NetOptions_TargetRoller(tFrontend_spec* pFrontend) {
    int increment;
    int minimum;
    int maximum;

    C2V(gFrontend_net_current_roll) = PDGetTotalTime();
    if (C2V(gFrontend_net_current_roll) - C2V(gFrontend_net_last_roll) < 300) {
        return 0;
    }
    switch (C2V(gFrontend_game_type)) {
    case eNet_game_type_1:
        increment = 1;
        minimum = 1;
        maximum = 64;
        break;
    case eNet_game_type_5:
        increment = 1;
        minimum = 1;
        maximum = 100;
        break;
    case eNet_game_type_foxy:
        increment = 30 * 1000;
        minimum = 30 * 1000;
        maximum = 2 * 60 * 60 * 1000;
        break;
    default:
        C2V(gFrontend_net_last_roll) = C2V(gFrontend_net_current_roll);
        return 0;
    }
    if (C2V(gFrontend_selected_item_index) == 29) {
        C2V(gFrontend_net_options).starting_target -= increment;
    } else if ((C2V(gFrontend_selected_item_index) == 30)) {
        C2V(gFrontend_net_options).starting_target += increment;
    }
    if (C2V(gFrontend_net_options).starting_target < minimum) {
        C2V(gFrontend_net_options).starting_target = minimum;
    } else if (C2V(gFrontend_net_options).starting_target > maximum) {
        C2V(gFrontend_net_options).starting_target = maximum;
    }
    C2V(gFrontend_net_last_roll) = C2V(gFrontend_net_current_roll);
    switch (C2V(gFrontend_game_type)) {
    case eNet_game_type_0:
    case eNet_game_type_2:
    case eNet_game_type_3:
    case eNet_game_type_4:
    case eNet_game_type_6:
        c2_strcpy(pFrontend->items[26].text, "----");
        break;
    case eNet_game_type_1:
    case eNet_game_type_5:
        c2_sprintf(pFrontend->items[26].text, "%i", C2V(gFrontend_net_options).starting_target);
        break;
    case eNet_game_type_foxy:
        {
            int seconds = C2V(gFrontend_net_options).starting_target / 1000;
            int minutes = (seconds / 60) % 60;
            int hours = seconds / 3600;
            if (hours == 0) {
                c2_sprintf(pFrontend->items[26].text, "%02i:%02i", minutes, seconds % 60);
            } else {
                c2_sprintf(pFrontend->items[26].text, "%02i:%02i:%02i", hours, minutes, seconds % 60);
            }
        }
        break;
    default:
        break;
    }
    FuckWithWidths(pFrontend);
    return 0;
}
C2_HOOK_FUNCTION(0x004732b0, NetOptions_TargetRoller)
