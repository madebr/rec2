/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_42_INPUT_H
#define GUARD_42_INPUT_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gKey_mapping[77];

extern tJoy_array gJoy_array;
extern tKey_array gKey_array;
extern int gGo_ahead_keys[3];
extern int gEdge_trigger_mode;
extern tMouse_coord gCurrent_mouse_position;
extern tU32 gLast_roll;
extern int gCurrent_cursor;
extern tRolling_letter* gRolling_letters;
extern int gLetter_x_coords[15];
extern int gVisible_length;
extern int gLetter_y_coords[15];
extern int gThe_length;
extern char gCurrent_typing[110];
extern int gCurrent_position;
extern tU32 gLast_poll_keys;
extern int gINT_006621e4;
extern int gINT_006621e8;
extern int gINT_006621ec;
extern int gINT_006621f0;
extern int gJoy1_x;
extern int gJoy1_y;
extern int gJoy2_x;
extern int gJoy2_y;
extern int gLast_key_down;
extern tU32 gLast_key_down_time;
extern int gModifiers_down;
extern int gKey_poll_counter;

extern void C2_HOOK_FASTCALL SetJoystickArrays(int* pKeys, int pMark);

extern void C2_HOOK_FASTCALL PollKeys(void);

extern void C2_HOOK_FASTCALL CyclePollKeys(void);

extern void C2_HOOK_FASTCALL ResetPollKeys(void);

extern void C2_HOOK_FASTCALL CheckKeysForMouldiness(void);

extern int C2_HOOK_FASTCALL EitherMouseButtonDown(void);

// AnyModifiersDown

extern tKey_down_result C2_HOOK_FASTCALL EdgeTriggeryKey(int pKey_index, int pReset);

extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);

// PDKeyDown3

extern int C2_HOOK_FASTCALL PDAnyKeyDown(void);

extern int C2_HOOK_FASTCALL AnyKeyDown(void);

// KevKeyService

extern int C2_HOOK_FASTCALL KeyIsDown(int pKey_index);

extern int C2_HOOK_FASTCALL KeyIsDownNoMouldiness(int pKey_index);

extern void C2_HOOK_FASTCALL WaitForNoKeys(void);

extern void C2_HOOK_FASTCALL GetMousePosition(int *pX, int *pY);

extern void C2_HOOK_FASTCALL InitRollingLetters(void);

extern void C2_HOOK_FASTCALL EndRollingLetters(void);

extern int C2_HOOK_FASTCALL AddRollingLetter(char pChar, int pX, int pY, tRolling_type rolling_type);

extern void C2_HOOK_FASTCALL AddRollingString(char* pStr, int pX, int pY, tRolling_type pRolling_type);

extern void C2_HOOK_FASTCALL BlankSlot(int pIndex, int pName_length, int pVisible_length);

extern int C2_HOOK_FASTCALL ChangeCharTo(int pSlot_index, int pChar_index, char pNew_char);

extern void C2_HOOK_FASTCALL StopTyping(int pSlot_index);

extern void C2_HOOK_FASTCALL RevertTyping(int pSlot_index, char* pRevert_str);

extern void C2_HOOK_FASTCALL StartTyping(int pSlot_index, char* pText, int pVisible_length);

extern void C2_HOOK_FASTCALL SetSlotXY(int pSlot_index, int pX_coord, int pY_coord);

extern void C2_HOOK_FASTCALL GetTypedName(char* pDestn, int pMax_length);

extern void C2_HOOK_FASTCALL KillCursor(int pSlot_index);

extern void C2_HOOK_FASTCALL EdgeTriggerModeOn(void);

extern void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);

#endif // GUARD_42_INPUT_H
