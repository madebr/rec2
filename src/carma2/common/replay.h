#ifndef REC2_REPLAY_H
#define REC2_REPLAY_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tActionReplayCameraMode, gAction_replay_camera_mode);

void C2_HOOK_FASTCALL DefaultQuicktimeSettings(void);

#endif //REC2_REPLAY_H
