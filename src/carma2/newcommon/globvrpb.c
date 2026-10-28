/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "globvrpb.h"


// GLOBAL: CARMA2_HW 0x0068b918
// GLOBAL: CARMA2_SW 0x00685e60
tNet_mode gNet_mode;

// GLOBAL: CARMA2_HW 0x0074b7d4
// GLOBAL: CARMA2_SW 0x00741e94
int gNumber_of_net_players;

// GLOBAL: CARMA2_HW 0x0074bcc0
// GLOBAL: CARMA2_SW 0x00742380
tNet_game_player_info gNet_players[12];

// GLOBAL: CARMA2_HW 0x0074b7d0
// GLOBAL: CARMA2_SW 0x00741e90
int gThis_net_player_index;

// GLOBAL: CARMA2_HW 0x0074c6d4
// GLOBAL: CARMA2_SW 0x00742d94
int gSynch_race_start;

// GLOBAL: CARMA2_HW 0x0074c6d8
// GLOBAL: CARMA2_SW 0x00742d98
int gPending_race;

// GLOBAL: CARMA2_HW 0x0074b800
// GLOBAL: CARMA2_SW 0x00741ec0
tCar_detail_info gCar_details[60];

// GLOBAL: CARMA2_HW 0x0074b780
int gINT_0074b780;

// GLOBAL: CARMA2_HW 0x0074b77c
// GLOBAL: CARMA2_SW 0x00741e3c
int gReceived_car_details;

// GLOBAL: CARMA2_HW 0x0074c6b8
// GLOBAL: CARMA2_SW 0x00742d78
int gStart_race_sent;

// GLOBAL: CARMA2_HW 0x0074b778
// GLOBAL: CARMA2_SW 0x00741e38
int gRendering_mirror;
