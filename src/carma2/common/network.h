#ifndef REC2_NETWORK_H
#define REC2_NETWORK_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gDigits_pix);
C2_HOOK_VARIABLE_DECLARE(tMin_message*, gMin_messages);
C2_HOOK_VARIABLE_DECLARE(tMid_message*, gMid_messages);
C2_HOOK_VARIABLE_DECLARE(tMax_message*, gMax_messages);

void C2_HOOK_FASTCALL InitNetHeadups(void);

void C2_HOOK_FASTCALL DisposeNetHeadups(void);

void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status);

void C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game);

void C2_HOOK_FASTCALL ShutdownNetIfRequired(void);

void C2_HOOK_FASTCALL NetSendHeadupToEverybody(const char* pMessage);

void C2_HOOK_FASTCALL NetSendHeadupToAllPlayers(char* pMessage);

#endif // REC2_NETWORK_H
