#include "06-apo.h"

#include "08-loading1.h"
#include "15-displays.h"
#include "27-powerup.h"
#include "41-utility.h"
#include "globvars.h"
#include "globvrpb.h"

#include "c2_stdio.h"
#include "c2_string.h"

// MaxOutAPO

// FUNCTION: CARMA2_HW 0x00502e20
// FUNCTION: CARMA2_SW 0x004fe770
void C2_HOOK_FASTCALL ImprovePSPowerup(tCar_spec* pCar, int pIndex) {

    pCar->power_up_levels[pIndex]++;
    NewTextHeadupSlot(4, 0, 3000, -4, GetMiscString(eMiscString_increase_apo_start + pIndex));
}

// FUNCTION: CARMA2_HW 0x00502e70
// FUNCTION: CARMA2_SW 0x004fe7c0
void C2_HOOK_FASTCALL BuyPSPowerup(int pIndex) {
    char message[256];
    char cost[256];

    if (gRace_finished || gProgram_state.current_car.knackered || gWait_for_it) {
        return;
    }
    if (gProgram_state.current_car.power_up_levels[pIndex] < gProgram_state.current_car.power_up_slots[pIndex]) {
        if (gProgram_state.credits < (gNet_mode != eNet_mode_none ? gCost_APO.initial_network[gCurrent_net_game->type] : gCost_APO.initial[gProgram_state.skill_level])) {
            strcpy(message, GetMiscString(eMiscString_cannot_afford_it_credits_S));
            sprintf(cost, "%d", gNet_mode != eNet_mode_none ? gCost_APO.initial_network[gCurrent_net_game->type] : gCost_APO.initial[gProgram_state.skill_level]);
            SubsStringJob(message, cost);
            NewTextHeadupSlot(4, 0, 3000, -4, message);
        } else {
            SpendCredits(gNet_mode != eNet_mode_none ? gCost_APO.initial_network[gCurrent_net_game->type] : gCost_APO.initial[gProgram_state.skill_level]);
            gProgram_state.current_car.power_up_levels[pIndex]++;
            NewTextHeadupSlot(4, 0, 3000, -4, GetMiscString(eMiscString_increase_apo_start + pIndex));
        }
    } else {
        NewTextHeadupSlot(4, 0, 3000, -4, GetMiscString(eMiscString_you_re_already_at_max_you_need_more_slots));
    }
}

// BuyArmour

// BuyPower

// BuyOffense

