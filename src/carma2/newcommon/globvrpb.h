/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_GLOBVRPB_H
#define REC2_GLOBVRPB_H

#include "c2_hooks.h"

#include <brender/br_types.h>
#include "rec2_types.h"

extern tNet_mode gNet_mode;
extern int gNumber_of_net_players;
extern tNet_game_player_info gNet_players[12];
extern int gThis_net_player_index;
extern int gSynch_race_start;
extern int gPending_race;
extern tNet_game_options* gNet_options;
extern tCar_detail_info gCar_details[60];
extern int gINT_0074b780;
extern int gReceived_car_details;
extern int gFrontend_net_initialized;
extern int gStart_race_sent;
extern int gRendering_mirror;

#endif // REC2_GLOBVRPB_H
