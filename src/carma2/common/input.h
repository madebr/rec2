#ifndef REC2_INPUT_H
#define REC2_INPUT_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tJoy_array, gJoy_array);
C2_HOOK_VARIABLE_DECLARE(tKey_array, gKey_array);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gGo_ahead_keys, 3);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gKey_mapping, 77);
C2_HOOK_VARIABLE_DECLARE(int, GEdge_trigger_mode);
C2_HOOK_VARIABLE_DECLARE(tMouse_coord, gCurrent_mouse_position);
C2_HOOK_VARIABLE_DECLARE(tU32, gLast_roll);
C2_HOOK_VARIABLE_DECLARE(int, gCurrent_cursor);
C2_HOOK_VARIABLE_DECLARE(tRolling_letter*, gRolling_letters);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gLetter_x_coords, 15);
C2_HOOK_VARIABLE_DECLARE(int, gVisible_length);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gLetter_y_coords, 15);
C2_HOOK_VARIABLE_DECLARE(int, gThe_length);
C2_HOOK_VARIABLE_DECLARE(tU32, gLast_poll_keys);
C2_HOOK_VARIABLE_DECLARE(int, gINT_006621e4);
C2_HOOK_VARIABLE_DECLARE(int, gINT_006621e8);
C2_HOOK_VARIABLE_DECLARE(int, gINT_006621ec);
C2_HOOK_VARIABLE_DECLARE(int, gINT_006621f0);
C2_HOOK_VARIABLE_DECLARE(int, gJoy1_x);
C2_HOOK_VARIABLE_DECLARE(int, gJoy1_y);
C2_HOOK_VARIABLE_DECLARE(int, gJoy2_x);
C2_HOOK_VARIABLE_DECLARE(int, gJoy2_y);
C2_HOOK_VARIABLE_DECLARE(int, gLast_key_down);
C2_HOOK_VARIABLE_DECLARE(tU32, gLast_key_down_time);
C2_HOOK_VARIABLE_DECLARE(int, gModifiers_down);

int C2_HOOK_FASTCALL LoadJoystickPreferences(void);

int C2_HOOK_FASTCALL AnyKeyDown(void);

void C2_HOOK_FASTCALL WaitForNoKeys(void);

void C2_HOOK_FASTCALL EdgeTriggerModeOn(void);

void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);

int C2_HOOK_FASTCALL AnyModifiersDown(void);

tKey_down_result C2_HOOK_FASTCALL EdgeTriggeryKey(int pKey_index, int pReset);

int C2_HOOK_FASTCALL EitherMouseButtonDown(void);

int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);

void C2_HOOK_FASTCALL PollKeys(void);

tU32* C2_HOOK_FASTCALL KevKeyService(void);

void C2_HOOK_FASTCALL CyclePollKeys(void);

void C2_HOOK_FASTCALL ResetPollKeys(void);

void C2_HOOK_FASTCALL CheckKeysForMouldiness(void);

int C2_HOOK_FASTCALL KeyIsDown(int pKey_index);

int C2_HOOK_FASTCALL KeyIsDownNoMouldiness(int pKey_index);

void C2_HOOK_FASTCALL GetMousePosition(int *pX, int *pY);

int C2_HOOK_FASTCALL PDAnyKeyDown(void);

void C2_HOOK_FASTCALL InitRollingLetters(void);

int C2_HOOK_FASTCALL AddRollingLetter(char pChar, int pX, int pY, tRolling_type rolling_type);

int C2_HOOK_FASTCALL ChangeCharTo(int pSlot_index, int pChar_index, char pNew_char);

void C2_HOOK_FASTCALL RevertTyping(int pSlot_index, char* pRevert_str);

void C2_HOOK_FASTCALL EndRollingLetters(void);

void C2_HOOK_FASTCALL SetJoystickArrays(int* pKeys, int pMark);

#endif // REC2_INPUT_H
