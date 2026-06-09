#include "input.h"

#include "globvars.h"
#include "grafdata.h"
#include "utility.h"

#include "platform.h"

#include "rec2_macros.h"

#include "c2_string.h"

#define NBR_ROLLING_LETTERS 500
#define ROLLING_LETTER_LOOP_RANDOM 96


// GLOBAL: CARMA2_HW 0x00657200
int gGo_ahead_keys[3] = { 51, 52, 106 };

// GLOBAL: CARMA2_HW 0x0068c1c4
int gEdge_trigger_mode;

// GLOBAL: CARMA2_HW 0x0074b5c0
tJoy_array gJoy_array;

// GLOBAL: CARMA2_HW 0x0068bee0
tKey_array gKey_array;

// GLOBAL: CARMA2_HW 0x0068bed4
int gKey_poll_counter;

// GLOBAL: CARMA2_HW 0x006571f8
tMouse_coord gCurrent_mouse_position = {-1, -1};

// GLOBAL: CARMA2_HW 0x0068c144
tU32 gLast_roll;

// GLOBAL: CARMA2_HW 0x0068c140
int gCurrent_cursor;

// GLOBAL: CARMA2_HW 0x0068be88
tRolling_letter* gRolling_letters;

// GLOBAL: CARMA2_HW 0x0068be90
int gLetter_x_coords[15];

// GLOBAL: CARMA2_HW 0x0068bed0
int gVisible_length;

// GLOBAL: CARMA2_HW 0x0068be48
int gLetter_y_coords[15];

// GLOBAL: CARMA2_HW 0x0068be84
int gThe_length;

// GLOBAL: CARMA2_HW 0x0068c1c8
tU32 gLast_poll_keys;

// GLOBAL: CARMA2_HW 0x006621e4
int gINT_006621e4 = 1;

// GLOBAL: CARMA2_HW 0x006621e8
int gINT_006621e8 = 1;

// GLOBAL: CARMA2_HW 0x006621ec
int gINT_006621ec = 1;

// GLOBAL: CARMA2_HW 0x006621f0
int gINT_006621f0 = 1;

// GLOBAL: CARMA2_HW 0x0068be44
int gJoy1_x;

// GLOBAL: CARMA2_HW 0x0068c13c
int gJoy1_y;

// GLOBAL: CARMA2_HW 0x0068be40
int gJoy2_x;

// GLOBAL: CARMA2_HW 0x0068becc
int gJoy2_y;

// GLOBAL: CARMA2_HW 0x0065720c
int gLast_key_down = -1000;

// GLOBAL: CARMA2_HW 0x0068c1d0
tU32 gLast_key_down_time;

// GLOBAL: CARMA2_HW 0x0068bed8
int gModifiers_down;

// STUB: CARMA2_HW 0x0045c0b0
int C2_HOOK_FASTCALL LoadJoystickPreferences(void) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x00482d70
int C2_HOOK_FASTCALL AnyKeyDown(void) {
    int the_key;

    the_key = PDAnyKeyDown();
    return (the_key != -1 && the_key != 4) || EitherMouseButtonDown();
}

// FUNCTION: CARMA2_HW 0x004833b0
void C2_HOOK_FASTCALL WaitForNoKeys(void) {

    while (AnyKeyDown() || EitherMouseButtonDown()) {
        CheckQuit();
    }
    CheckQuit();
}

// FUNCTION: CARMA2_HW 0x00484600
void C2_HOOK_FASTCALL EdgeTriggerModeOn(void) {

    gEdge_trigger_mode = 1;
}

// FUNCTION: CARMA2_HW 0x00484610
void C2_HOOK_FASTCALL EdgeTriggerModeOff(void) {

    gEdge_trigger_mode = 0;
}

int C2_HOOK_FASTCALL AnyModifiersDown(void) {
    int i;

    for (i = 0; i < 11; i++) {
        if (gKey_array[i] != 0) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00482590
tKey_down_result C2_HOOK_FASTCALL EdgeTriggeryKey(int pKey_index, int pReset) {
    tU32 now;

    if (pReset) {
        gLast_key_down = -1;
        gLast_key_down_time = 0;
        return tKey_down_no;
    }
    CheckKeysForMouldiness();
    if (!gEdge_trigger_mode) {
        return gKey_array[pKey_index];
    }
    if (gKey_array[pKey_index] == 0) {
        if (pKey_index == gLast_key_down) {
            gLast_key_down_time = 0;
            gLast_key_down = -1000;
        }
        return tKey_down_no;
    }
    now = PDGetTotalTime();
    if (pKey_index == gLast_key_down) {
        if (gModifiers_down == AnyModifiersDown()) {
            if (now - gLast_key_down_time < 300) {
                return tKey_down_still;
            } else {
                gLast_key_down_time = now;
                return tKey_down_repeat;
            }
        } else {
            gLast_key_down_time = 0;
            gLast_key_down = -gLast_key_down;
            return tKey_down_no;
        }
    }
    if (pKey_index == -gLast_key_down) {
        return tKey_down_no;
    }
    if (pKey_index < 11) {
        gLast_key_down_time = 0;
        gLast_key_down = -1000;
        return tKey_down_yes;
    }
    gLast_key_down = pKey_index;
    gLast_key_down_time = now;
    gModifiers_down = AnyModifiersDown();
    return tKey_down_yes;
}

// FUNCTION: CARMA2_HW 0x004824c0
int C2_HOOK_FASTCALL EitherMouseButtonDown(void) {
    tMouse_coord pos;
    tMouse_coord prev_pos = gCurrent_mouse_position;
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
}

// FUNCTION: CARMA2_HW 0x00482550
int C2_HOOK_FASTCALL PDKeyDown(int pKey_index) {
    tKey_down_result result;

    result = EdgeTriggeryKey(pKey_index, 0);
    if (!gEdge_trigger_mode || pKey_index <= 10) {
        return result != tKey_down_no;
    }
    return result == tKey_down_yes || result == tKey_down_repeat;
}

// FUNCTION: CARMA2_HW 0x00481eb0
void C2_HOOK_FASTCALL PollKeys(void) {

    gKey_poll_counter += 1;
    PDSetKeyArray(gKey_array, gKey_poll_counter);
    SetJoystickArrays(gKey_array, gKey_poll_counter);
    gLast_poll_keys = PDGetTotalTime();
}

// FUNCTION: CARMA2_HW 0x00482f10
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
    keys = gKeys_pressed;

    if (keys < 0x6B) {
        last_single_key = gKeys_pressed;
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

// FUNCTION: CARMA2_HW 0x004821a0
void C2_HOOK_FASTCALL ResetPollKeys(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gKey_array) != 151);
    C2_HOOK_BUG_ON(REC2_ASIZE(gJoy_array) != 8);

    for (i = 0; i < REC2_ASIZE(gKey_array); i++) {
        gKey_array[i] = 0;
    }
    for (i = 0; i < REC2_ASIZE(gJoy_array); i++) {
        gJoy_array[i] = -1;
    }
}

// FUNCTION: CARMA2_HW 0x00482160
void C2_HOOK_FASTCALL CyclePollKeys(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(gKey_array); i++) {
        if (gKey_array[i] > gKey_poll_counter) {
            gKey_array[i] = 0;
            if (i > 143) {
                gJoy_array[i - 143] = -1; // yes this is a little weird I know...
            }
        }
    }
    gKey_poll_counter = 0;
}

// FUNCTION: CARMA2_HW 0x004821c0
void C2_HOOK_FASTCALL CheckKeysForMouldiness(void) {

    if (PDGetTotalTime() - gLast_poll_keys > 500) {
        ResetPollKeys();
        CyclePollKeys();
        PollKeys();
    }
}

// FUNCTION: CARMA2_HW 0x00483040
int C2_HOOK_FASTCALL KeyIsDown(int pKey_index) {
    int i;

    CheckKeysForMouldiness();
    switch (pKey_index) {
    case -2:
        return 1;
    case -1:
        for (i = 0; i < REC2_ASIZE(gGo_ahead_keys); i++) {
            if (gKey_array[gGo_ahead_keys[i]]) {
                return 1;
            }
        }
        return 0;
    default:
        return gKey_array[gKey_mapping[pKey_index]];
    }
}

// FUNCTION: CARMA2_HW 0x004833a0
int C2_HOOK_FASTCALL KeyIsDownNoMouldiness(int pKey_index) {

    return gKey_array[gKey_mapping[pKey_index]];
}

// FUNCTION: CARMA2_HW 0x00483c10
void C2_HOOK_FASTCALL GetMousePosition(int *pX, int *pY) {

    PDGetMousePosition(pX, pY);
    if (*pX < 0) {
        *pX = 0;
    } else if (*pX > gGraf_specs[gGraf_spec_index].total_width) {
        *pX = gGraf_specs[gGraf_spec_index].total_width;
    }
    if (*pY < 0) {
        *pY = 0;
    } else if (*pY > gGraf_specs[gGraf_spec_index].total_height) {
        *pY = gGraf_specs[gGraf_spec_index].total_height;
    }
    gCurrent_mouse_position.x = *pX;
    gCurrent_mouse_position.y = *pY;
}

// FUNCTION: CARMA2_HW 0x00482a00
int C2_HOOK_FASTCALL PDAnyKeyDown(void) {
    int i;

    CheckKeysForMouldiness();

#ifndef REC2_MATCHING
    C2_HOOK_BUG_ON(142 < REC2_ASIZE(gKey_array));
#endif

    CheckKeysForMouldiness();
    for (i = 142; i >= 0; --i) {
        if (gKey_array[i] && i != 4) {
            if (!gEdge_trigger_mode) {
                return i;
            }
            switch (EdgeTriggeryKey(i, 0)) {
                case tKey_down_no:
                case tKey_down_still:
                    return -1;
                case tKey_down_yes:
                case tKey_down_repeat:
                    return i;
            }
        }
    }
    if (gEdge_trigger_mode) {
        EdgeTriggeryKey(0, 1);
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x00483c90
void C2_HOOK_FASTCALL InitRollingLetters(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tRolling_letter) != 0x38);
    C2_HOOK_BUG_ON(NBR_ROLLING_LETTERS != 500);
    C2_HOOK_BUG_ON(NBR_ROLLING_LETTERS * sizeof(tRolling_letter) != 28000);

    gLast_roll = 0;
    gCurrent_cursor = -1;
    gRolling_letters = BrMemAllocate(NBR_ROLLING_LETTERS * sizeof(tRolling_letter), kMem_misc);
    for (i = 0; i < NBR_ROLLING_LETTERS; i++) {
        gRolling_letters[i].number_of_letters = -1;
    }
}

// FUNCTION: CARMA2_HW 0x00483cf0
int C2_HOOK_FASTCALL AddRollingLetter(char pChar, int pX, int pY, tRolling_type rolling_type) {
    tRolling_letter* let;
    int i;

    for (i = 0; i < NBR_ROLLING_LETTERS; i++) {
        let = &gRolling_letters[i];
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

    let->current_offset = (float)(let->number_of_letters * gCurrent_graf_data->save_slot_letter_height);
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

// FUNCTION: CARMA2_HW 0x00484000
int C2_HOOK_FASTCALL ChangeCharTo(int pSlot_index, int pChar_index, char pNew_char) {
    int x_coord;
    int y_coord;
    int i;
    tRolling_letter* let;
    tRolling_type new_type;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, save_slot_letter_height, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, rolling_letter_x_pitch, 0x10);

    if (pChar_index >= gVisible_length || pChar_index < 0) {
        return -1;
    }
    y_coord = gLetter_y_coords[pSlot_index];
    x_coord = gLetter_x_coords[pSlot_index] + pChar_index * gCurrent_graf_data->rolling_letter_x_pitch;

    if (pNew_char == ROLLING_LETTER_LOOP_RANDOM) {
        new_type = eRT_looping_random;
    } else if (pNew_char >= '0' && pNew_char <= '9') {
        new_type = eRT_numeric;
    } else {
        new_type = eRT_alpha;
    }

    for (i = 0; i < NBR_ROLLING_LETTERS; i++) {
        let = &gRolling_letters[i];
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
    let->current_offset = (float)(gCurrent_graf_data->save_slot_letter_height * let->number_of_letters);
    return i;
}

// FUNCTION: CARMA2_HW 0x00484120
void C2_HOOK_FASTCALL RevertTyping(int pSlot_index, char* pRevert_str) {
    unsigned i;

    for (i = 0; (int)i < gThe_length; i++) {
        ChangeCharTo(pSlot_index, i, i >= strlen(pRevert_str) ? ' ' : pRevert_str[i]);
    }
}

// FUNCTION: CARMA2_HW 0x00483ce0
void C2_HOOK_FASTCALL EndRollingLetters(void) {

    BrMemFree(gRolling_letters);
}

// FUNCTION: CARMA2_HW 0x00482770
void C2_HOOK_FASTCALL SetJoystickArrays(int* pKeys, int pMark) {
    int i;

    for (i = 0; i < 44; i++) {
        pKeys[107 + i] = 0;
    }
    PDReadJoysticks();
    for (i = 0; i < 8; i++) {
        gJoy_array[i] = -1;
    }
    for (i = 0; i < 36; i++) {
        pKeys[107 + i] = 0;
    }
    if (gINT_006621e4 || gINT_006621e8) {
        if (PDGetJoy1Button1()) {
            pKeys[107] = pMark;
        }
        if (PDGetJoy1Button2()) {
            pKeys[108] = pMark;
        }
        if (PDGetJoy1Button3()) {
            pKeys[109] = pMark;
        }
        if (PDGetJoy1Button4()) {
            pKeys[110] = pMark;
        }
    }
    if (gINT_006621ec || gINT_006621f0) {
        if (PDGetJoy2Button1()) {
            pKeys[139] = pMark;
        }
        if (PDGetJoy2Button2()) {
            pKeys[140] = pMark;
        }
        if (PDGetJoy2Button3()) {
            pKeys[141] = pMark;
        }
        if (PDGetJoy2Button4()) {
            pKeys[142] = pMark;
        }
    }
    if (gINT_006621e4) {
        int new_joy1_x = PDGetJoy1X();
        if (new_joy1_x != -1) {
            if (new_joy1_x < 0x8000) {
                if (new_joy1_x < 0x4000 && gJoy1_x >= 0x4000) {
                    pKeys[143] = pMark;
                }
                gJoy_array[0] = 2 * (0x8000 - new_joy1_x);
            } else {
                if (new_joy1_x > 0xc000 && gJoy1_x <= 0xc000) {
                    pKeys[144] = pMark;
                }
                gJoy_array[1] = 2 * (new_joy1_x - 0x8000);
            }
        }
        gJoy1_x = new_joy1_x;
    }
    if (gINT_006621e8) {
        int new_joy1_y = PDGetJoy1Y();
        if (new_joy1_y != -1) {
            if (new_joy1_y < 0x8000) {
                if (new_joy1_y < 0x4000 && gJoy1_y >= 0x4000) {
                    pKeys[145] = pMark;
                }
                gJoy_array[2] = 2 * (0x8000 - new_joy1_y);
            }
            else {
                if (new_joy1_y > 0xc000 && gJoy1_y <= 0xc000) {
                    pKeys[146] = pMark;
                }
                gJoy_array[3] = 2 * (new_joy1_y - 0x8000);
            }
        }
        gJoy1_y = new_joy1_y;
    }
    if (gINT_006621ec) {
        int new_joy2_x = PDGetJoy2X();
        if (new_joy2_x != -1) {
            if (new_joy2_x < 0x8000) {
                if (new_joy2_x < 0x4000 && gJoy2_x >= 0x4000) {
                    pKeys[147] = pMark;
                }
                gJoy_array[4] = 2 * (0x8000 - new_joy2_x);
            } else {
                if (new_joy2_x > 0xc000 && gJoy2_x <= 0xc000) {
                    pKeys[148] = pMark;
                }
                gJoy_array[5] = 2 * (new_joy2_x - 0x8000);
            }
        }
        gJoy2_x = new_joy2_x;
    }
    if (gINT_006621f0) {
        int new_joy2_y = PDGetJoy2Y();
        if (new_joy2_y != -1) {
            if (new_joy2_y < 0x8000) {
                if (new_joy2_y < 0x4000 && gJoy2_y >= 0x4000) {
                    pKeys[149] = pMark;
                }
                gJoy_array[6] = 2 * (0x8000 - new_joy2_y);
            } else {
                if (new_joy2_y > gJoystick_deadzone + 0x8000) {
                    if (new_joy2_y > 0xc000 && gJoy2_y <= 0xc000) {
                        pKeys[150] = pMark;
                    }
                    gJoy_array[7] = 2 * (new_joy2_y - 0x8000);
                }
            }
        }
        gJoy2_y = new_joy2_y;
    }
}
