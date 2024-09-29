#ifndef REC2_INPUT_H
#define REC2_INPUT_H

#include "c2_hooks.h"

#include "rec2_types.h"

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

int C2_HOOK_FASTCALL LoadJoystickPreferences(void);

int C2_HOOK_FASTCALL AnyKeyDown(void);

void C2_HOOK_FASTCALL WaitForNoKeys(void);

void C2_HOOK_FASTCALL EdgeTriggerModeOn(void);

void C2_HOOK_FASTCALL EdgeTriggerModeOff(void);

int C2_HOOK_FASTCALL PDKeyDown2(int pKey_index, undefined4 pArg2);

int C2_HOOK_FASTCALL EitherMouseButtonDown(void);

int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);

void C2_HOOK_FASTCALL PollKeys(void);

tU32* C2_HOOK_FASTCALL KevKeyService(void);

void C2_HOOK_FASTCALL CyclePollKeys(void);

int C2_HOOK_FASTCALL KeyIsDown(int pKey_index);

int C2_HOOK_FASTCALL KeyIsDown2(int pKey_index);

void C2_HOOK_FASTCALL GetMousePosition(int *pX, int *pY);

int C2_HOOK_FASTCALL PDAnyKeyDown(void);

void C2_HOOK_FASTCALL InitRollingLetters(void);

int C2_HOOK_FASTCALL AddRollingLetter(char pChar, int pX, int pY, tRolling_type rolling_type);

int C2_HOOK_FASTCALL ChangeCharTo(int pSlot_index, int pChar_index, char pNew_char);

void C2_HOOK_FASTCALL RevertTyping(int pSlot_index, char* pRevert_str);

#endif // REC2_INPUT_H
