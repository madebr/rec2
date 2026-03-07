#ifndef REC2_GLOBVRPB_H
#define REC2_GLOBVRPB_H

#include "c2_hooks.h"

#include <brender/br_types.h>
#include "rec2_types.h"

extern tNet_mode gNet_mode;
extern int gNumber_of_net_players;
extern tNet_game_player_info gNet_players[12];
extern int gThis_net_player_index;
extern int gPending_race;
extern tCar_detail_info gCar_details[60];
extern int gINT_0074b780;
extern int gStart_race_sent;
extern int gRendering_mirror;

#endif // REC2_GLOBVRPB_H