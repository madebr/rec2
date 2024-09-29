#include "input.h"

#include "globvars.h"
#include "grafdata.h"
#include "utility.h"

#include "platform.h"

#include "rec2_macros.h"

#include "c2_string.h"

#define NBR_ROLLING_LETTERS 500
#define ROLLING_LETTER_LOOP_RANDOM 96

C2_HOOK_VARIABLE_IMPLEMENT(int, gEdge_trigger_mode, 0x0068c1c4);
C2_HOOK_VARIABLE_IMPLEMENT(tJoy_array, gJoy_array, 0x0074b5c0);
C2_HOOK_VARIABLE_IMPLEMENT(tKey_array, gKey_array, 0x0068bee0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gKey_poll_counter, 0x0068bed4);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tMouse_coord, gCurrent_mouse_position, 0x006571f8, {-1, -1});
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_roll, 0x0068c144);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCurrent_cursor, 0x0068c140);
C2_HOOK_VARIABLE_IMPLEMENT(tRolling_letter*, gRolling_letters, 0x0068be88);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gLetter_x_coords, 15, 0x0068be90);
C2_HOOK_VARIABLE_IMPLEMENT(int, gVisible_length, 0x0068bed0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gLetter_y_coords, 15, 0x0068be48);
C2_HOOK_VARIABLE_IMPLEMENT(int, gThe_length, 0x0068be84);

int (C2_HOOK_FASTCALL * LoadJoystickPreferences_original)(void);
int C2_HOOK_FASTCALL LoadJoystickPreferences(void) {
#if defined(C2_HOOKS_ENABLED)
    int res = LoadJoystickPreferences_original();
    return res;
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c0b0, LoadJoystickPreferences, LoadJoystickPreferences_original)

int (C2_HOOK_FASTCALL * AnyKeyDown_original)(void);
int C2_HOOK_FASTCALL AnyKeyDown(void) {
#if defined(C2_HOOKS_ENABLED)
    int res = AnyKeyDown_original();
    return res;
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00482d70, AnyKeyDown, AnyKeyDown_original)

void (C2_HOOK_FASTCALL * WaitForNoKeys_original)(void);
void C2_HOOK_FASTCALL WaitForNoKeys(void) {
#if defined(C2_HOOKS_ENABLED)
    WaitForNoKeys_original();
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004833b0, WaitForNoKeys, WaitForNoKeys_original)

void C2_HOOK_FASTCALL EdgeTriggerModeOn(void) {
    C2V(gEdge_trigger_mode) = 1;
}
C2_HOOK_FUNCTION(0x00484600, EdgeTriggerModeOn)

void C2_HOOK_FASTCALL EdgeTriggerModeOff(void) {
    C2V(gEdge_trigger_mode) = 0;
}
C2_HOOK_FUNCTION(0x00484610, EdgeTriggerModeOff)

int (C2_HOOK_FASTCALL * PDKeyDown2_original)(int pKey_index, undefined4 pArg2);
int C2_HOOK_FASTCALL PDKeyDown2(int pKey_index, undefined4 pArg2) {
#if defined(C2_HOOKS_ENABLED)
    return PDKeyDown2_original(pKey_index, pArg2);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00482590, PDKeyDown2, PDKeyDown2_original)

int (C2_HOOK_FASTCALL * EitherMouseButtonDown_original)(void);
int C2_HOOK_FASTCALL EitherMouseButtonDown(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    return EitherMouseButtonDown_original();
#else
    tMouse_coord pos;
    tMouse_coord prev_pos = C2V(gCurrent_mouse_position);
    int left;
    int right;

    GetMousePosition(&pos.x, &pos.y);
    if (prev_pos.x == pos.x && prev_pos.y == pos.y) {
        tMouse_coord click_pos;

        int ok = PDGetMouseClickPosition(&click_pos.x, &click_pos.y);
        if (ok && pos.x == click_pos.x && pos.y == click_pos.y) {
            return 2;
        }
    }
    PDMouseButtons(&left, &right);
    if (left || right) {
        return 1;
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004824c0, EitherMouseButtonDown, EitherMouseButtonDown_original)

int C2_HOOK_FASTCALL PDKeyDown(int pKey_index) {
    tKey_down_result result;

    result = PDKeyDown2(pKey_index, 0);
    if (!C2V(gEdge_trigger_mode) || pKey_index <= 10) {
        return result != tKey_down_no;
    }
    return result == tKey_down_yes || result == tKey_down_repeat;
}
C2_HOOK_FUNCTION(0x00482550, PDKeyDown)

void (C2_HOOK_FASTCALL * PollKeys_original)(void);
void C2_HOOK_FASTCALL PollKeys(void) {

#if defined(C2_HOOKS_ENABLED)
    PollKeys_original();
#else

#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00481eb0, PollKeys, PollKeys_original)

tU32* C2_HOOK_FASTCALL KevKeyService(void) {
    static tU32 sum = 0;
    static tU32 code = 0;
    static tU32 code2 = 0;
    static int last_key = -1;
    static int last_single_key = -1;
    static tU32 last_time = 0;
    static tU32 return_val[2];
    tU32 keys;

    return_val[0] = 0;
    return_val[1] = 0;
    keys = C2V(gKeys_pressed);

    if (keys < 0x6B) {
        last_single_key = C2V(gKeys_pressed);
    } else {
        if (keys > 0x6b00) {
            sum = 0;
            code = 0;
            return return_val;
        }
        if ((keys & 0xff) != last_single_key && keys >> 8 != last_single_key) {
            sum = 0;
            code = 0;
            return return_val;
        }
        if (keys >> 8 != last_single_key) {
            sum = 0;
            code = 0;
            return return_val;
        }
        if ((keys & 0xff) == last_single_key) {
            keys = keys >> 8;
        }
        keys = keys & 0xff;
    }

    if (keys != 0 && keys != last_key) {
        sum += keys;
        code += keys << 11;
        code = (code >> 17) + (code << 4);
        code2 = (code2 >> 29) + keys * keys + (code2 << 3);
        last_time = PDGetTotalTime();
    } else if ((tU32)PDGetTotalTime() > last_time + 1000) {
        return_val[0] = (code >> 11) + (sum << 21);
        return_val[1] = code2;
        // printf("final value: code=%lx, code2=%lx\n", return_val[0], return_val[1]);
        code = 0;
        code2 = 0;
        sum = 0;
    }
    last_key = keys;
    return return_val;
}
C2_HOOK_FUNCTION(0x00482f10, KevKeyService)

void C2_HOOK_FASTCALL CyclePollKeys(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gKey_array)); i++) {
        if (C2V(gKey_array)[i] > C2V(gKey_poll_counter)) {
            C2V(gKey_array)[i] = 0;
            if (i > 143) {
                C2V(gJoy_array)[i - 143] = -1; // yes this is a little weird I know...
            }
        }
    }
    C2V(gKey_poll_counter) = 0;
}
C2_HOOK_FUNCTION(0x00482160, CyclePollKeys)

int (C2_HOOK_FASTCALL * KeyIsDown_original)(int pKey_index);
int C2_HOOK_FASTCALL KeyIsDown(int pKey_index) {

#if defined(C2_HOOKS_ENABLED)
    return KeyIsDown_original(pKey_index);
#else
    int i;

    CheckKeysForMouldiness();
    switch (pKey_index) {
        case -2:
            return 1;
        case -1:
            for (i = 0; i < REC2_ASIZE(C2V(gGo_ahead_keys)); i++) {
                if (C2V(gKey_array)[C2V(gGo_ahead_keys)[i]]) {
                    return 1;
                }
            }
            return 0;
        default:
            return C2V(gKey_array)[C2V(gKey_mapping)[pKey_index]];
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00483040, KeyIsDown, KeyIsDown_original)

int C2_HOOK_FASTCALL KeyIsDown2(int pKey_index) {

    return C2V(gKey_array)[C2V(gKey_mapping)[pKey_index]];
}
C2_HOOK_FUNCTION(0x004833a0, KeyIsDown2)

void C2_HOOK_FASTCALL GetMousePosition(int *pX, int *pY) {

    PDGetMousePosition(pX, pY);
    if (*pX < 0) {
        *pX = 0;
    } else if (*pX > C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_width) {
        *pX = C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_width;
    }
    if (*pY < 0) {
        *pY = 0;
    } else if (*pY > C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_height) {
        *pY = C2V(gGraf_specs)[C2V(gGraf_spec_index)].total_height;
    }
    C2V(gCurrent_mouse_position).x = *pX;
    C2V(gCurrent_mouse_position).y = *pY;
}
C2_HOOK_FUNCTION(0x00483c10, GetMousePosition);

int (C2_HOOK_FASTCALL * PDAnyKeyDown_original)(void);
int C2_HOOK_FASTCALL PDAnyKeyDown(void) {

#if defined(C2_HOOKS_ENABLED)
    return PDAnyKeyDown_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00482a00, PDAnyKeyDown, PDAnyKeyDown_original)

void C2_HOOK_FASTCALL InitRollingLetters(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tRolling_letter) != 0x38);
    C2_HOOK_BUG_ON(NBR_ROLLING_LETTERS != 500);
    C2_HOOK_BUG_ON(NBR_ROLLING_LETTERS * sizeof(tRolling_letter) != 28000);

    C2V(gLast_roll) = 0;
    C2V(gCurrent_cursor) = -1;
    C2V(gRolling_letters) = BrMemAllocate(NBR_ROLLING_LETTERS * sizeof(tRolling_letter), kMem_misc);
    for (i = 0; i < NBR_ROLLING_LETTERS; i++) {
        C2V(gRolling_letters)[i].number_of_letters = -1;
    }
}
C2_HOOK_FUNCTION(0x00483c90, InitRollingLetters)

int C2_HOOK_FASTCALL AddRollingLetter(char pChar, int pX, int pY, tRolling_type rolling_type) {
    tRolling_letter* let;
    int i;

    for (i = 0; i < NBR_ROLLING_LETTERS; i++) {
        let = &C2V(gRolling_letters)[i];
        if (let->number_of_letters < 0) {
            break;
        }
    }
    if (i == NBR_ROLLING_LETTERS) {
        return -1;
    }
    let->x_coord = pX;
    let->y_coord = pY;
    let->rolling_type = rolling_type;
    switch (rolling_type) {
    case eRT_looping_random:
        let->number_of_letters = 9;
        break;
    case eRT_looping_single:
        let->number_of_letters = 2;
        break;
    default:
        let->number_of_letters = IRandomBetween(3, 9);
        break;
    }

    let->current_offset = (float)(let->number_of_letters * C2V(gCurrent_graf_data)->save_slot_letter_height);
    for (i = (rolling_type != eRT_looping_random ? 1 : 0); i < let->number_of_letters; i++) {
        if (rolling_type == eRT_numeric) {
            /* The (tU8) cast makes sure extended ASCII is positive. */
            let->letters[i] = (tU8)pChar;
        } else {
            let->letters[i] = IRandomBetween('A', 'Z' + 1);
        }
    }
    if (rolling_type != eRT_looping_random) {
        /* The (tU8) cast makes sure extended ASCII is positive. */
        let->letters[0] = (tU8)pChar;
    }

    return 0;
}
C2_HOOK_FUNCTION(0x00483cf0, AddRollingLetter)

int C2_HOOK_FASTCALL ChangeCharTo(int pSlot_index, int pChar_index, char pNew_char) {
    int x_coord;
    int y_coord;
    int i;
    tRolling_letter* let;
    tRolling_type new_type;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, save_slot_letter_height, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, rolling_letter_x_pitch, 0x10);

    if (pChar_index >= C2V(gVisible_length) || pChar_index < 0) {
        return -1;
    }
    y_coord = C2V(gLetter_y_coords)[pSlot_index];
    x_coord = C2V(gLetter_x_coords)[pSlot_index] + pChar_index * C2V(gCurrent_graf_data)->rolling_letter_x_pitch;

    if (pNew_char == ROLLING_LETTER_LOOP_RANDOM) {
        new_type = eRT_looping_random;
    } else if (pNew_char >= '0' && pNew_char <= '9') {
        new_type = eRT_numeric;
    } else {
        new_type = eRT_alpha;
    }

    for (i = 0; i < NBR_ROLLING_LETTERS; i++) {
        let = &C2V(gRolling_letters)[i];
        if (let->number_of_letters >= 0 && x_coord == let->x_coord && y_coord == let->y_coord) {
            break;
        }
    }
    if (i >= NBR_ROLLING_LETTERS) {
        return AddRollingLetter(pNew_char, x_coord, y_coord, new_type);
    }
    if (pNew_char != ROLLING_LETTER_LOOP_RANDOM) {
        /* The (tU8) cast makes sure extended ASCII is positive. */
        let->letters[0] = (tU8)pNew_char;
    }
    if (pNew_char == ' ') {
        let->letters[0] = ' ';
    }
    let->rolling_type = new_type;
    let->current_offset = (float)(C2V(gCurrent_graf_data)->save_slot_letter_height * let->number_of_letters);
    return i;
}
C2_HOOK_FUNCTION(0x00484000, ChangeCharTo)

void C2_HOOK_FASTCALL RevertTyping(int pSlot_index, char* pRevert_str) {
    unsigned i;

    for (i = 0; (int)i < C2V(gThe_length); i++) {
        ChangeCharTo(pSlot_index, i, i >= c2_strlen(pRevert_str) ? ' ' : pRevert_str[i]);
    }
}
C2_HOOK_FUNCTION(0x00484120, RevertTyping)
