#ifndef REC2_GLOBVRPB_H
#define REC2_GLOBVRPB_H

#include "c2_hooks.h"

#include <brender/br_types.h>
#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tNet_mode, gNet_mode);
C2_HOOK_VARIABLE_DECLARE(int, gNumber_of_net_players);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tNet_game_player_info, gNet_players, 12);
C2_HOOK_VARIABLE_DECLARE(int, gThis_net_player_index);
C2_HOOK_VARIABLE_DECLARE(int, gPending_race);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tCar_detail_info, gCar_details, 60);
C2_HOOK_VARIABLE_DECLARE(int, gINT_0074b780);
C2_HOOK_VARIABLE_DECLARE(int, gStart_race_sent);

#endif // REC2_GLOBVRPB_H
