#ifndef REC2_NETWORK_H
#define REC2_NETWORK_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gDigits_pix);

void C2_HOOK_FASTCALL InitNetHeadups(void);

void C2_HOOK_FASTCALL DisposeNetHeadups(void);

void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status);

void C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game);

#endif // REC2_NETWORK_H
