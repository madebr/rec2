#include "globvrpb.h"

C2_HOOK_VARIABLE_IMPLEMENT(tNet_mode, gNet_mode, 0x0068b918);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNumber_of_net_players, 0x0074b7d4);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tNet_game_player_info, gNet_players, 12, 0x0074bcc0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gThis_net_player_index, 0x0074b7d0);
