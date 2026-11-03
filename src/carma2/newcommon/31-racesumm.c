/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "31-racesumm.h"

#include "globvars.h"
#include "globvrpb.h"
#include "18-graphics2.h"
#include "44-mainmenu.h"
#include "30-opponent.h"
#include "01-network.h"
#include "35-intrface.h"
#include "platform.h"
#include "69-sound.h"

extern void C2_HOOK_FASTCALL OriginalResetInterfaceTimeout(void);
/* declared here rather than through 72-interface.h / carma2/common/frontend.h: those belong to
   another feature area, and this file must compile in a branch that does not carry them */
extern int C2_HOOK_FASTCALL FRONTEND_Main(tFrontendMenuType pFrontend_type);

// GLOBAL: CARMA2_HW 0x007051f8
int gMoney_pre_race;

// GLOBAL: CARMA2_HW 0x007051e0
int gAPO_pre_race[3];

// GLOBAL: CARMA2_HW 0x007051a0
int gPlayer_lookup_racesumm[14];

// GLOBAL: CARMA2_HW 0x0074a694
int gReceived_game_scores;

// FUNCTION: CARMA2_HW 0x004e33a0
static int C2_HOOK_CDECL SortScores(const void* pArg1, const void* pArg2) {

    return gNet_players[*(int*)pArg2].score - gNet_players[*(int*)pArg1].score;
}

// SortGameScores

// DoNetRaceSummary

// DoEndRaceSummary

// FUNCTION: CARMA2_HW 0x004e33d0
// FUNCTION: CARMA2_SW 0x004df760
int C2_HOOK_FASTCALL AnyBuyableCars(void) {
    int count;
    int i;

    count = GetCarCount(eVehicle_opponent);
    for (i = 0; i < count; i++) {
        if (GetCarSpec(eVehicle_opponent, i) != NULL) {
            return 1;
        }
    }
    return 0;
}

// RecordAPOAndCredits

