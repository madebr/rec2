#include "sdl3_input.h"

#include "sdl3.h"

#include "globvars.h"

#include <SDL3/SDL.h>

int gASCII_table[151];
int gASCII_shift_table[151];
int gScan_code[SDL_SCANCODE_COUNT];
int gJoystick_deadzone = 8000;

void Win32InitInputDevice(void) {
}

void KeyBegin(void) {
    memset(gASCII_table, 0, sizeof(gASCII_table));
    memset(gASCII_shift_table, 0, sizeof(gASCII_shift_table));

    gASCII_table[11] = '0';
    gASCII_table[12] = '1';
    gASCII_table[13] = '2';
    gASCII_table[14] = '3';
    gASCII_table[15] = '4';
    gASCII_table[16] = '5';
    gASCII_table[17] = '6';
    gASCII_table[18] = '7';
    gASCII_table[19] = '8';
    gASCII_table[20] = '9';
    gASCII_table[21] = 'a';
    gASCII_table[22] = 'b';
    gASCII_table[23] = 'c';
    gASCII_table[24] = 'd';
    gASCII_table[25] = 'e';
    gASCII_table[26] = 'f';
    gASCII_table[27] = 'g';
    gASCII_table[28] = 'h';
    gASCII_table[29] = 'i';
    gASCII_table[30] = 'j';
    gASCII_table[31] = 'k';
    gASCII_table[32] = 'l';
    gASCII_table[33] = 'm';
    gASCII_table[34] = 'n';
    gASCII_table[35] = 'o';
    gASCII_table[36] = 'p';
    gASCII_table[37] = 'q';
    gASCII_table[38] = 'r';
    gASCII_table[39] = 's';
    gASCII_table[40] = 't';
    gASCII_table[41] = 'u';
    gASCII_table[42] = 'v';
    gASCII_table[43] = 'w';
    gASCII_table[44] = 'x';
    gASCII_table[45] = 'y';
    gASCII_table[46] = 'z';
    gASCII_table[48] = '-';
    gASCII_table[49] = '=';
    gASCII_table[50] = 0x7f;
    gASCII_table[51] = '\r';
    gASCII_table[52] = '\r';
    gASCII_table[53] = '\t';
    gASCII_table[58] = '.';
    gASCII_table[59] = ',';
    gASCII_table[63] = 0x1b;
    gASCII_table[106] = ' ';

    memcpy(gASCII_shift_table, gASCII_table, sizeof(gASCII_table));

    gASCII_shift_table[21] = 'A';
    gASCII_shift_table[22] = 'B';
    gASCII_shift_table[23] = 'C';
    gASCII_shift_table[24] = 'D';
    gASCII_shift_table[25] = 'E';
    gASCII_shift_table[26] = 'F';
    gASCII_shift_table[27] = 'G';
    gASCII_shift_table[28] = 'H';
    gASCII_shift_table[29] = 'I';
#if defined(REC2_FIX_BUGS)
    gASCII_shift_table[30] = 'J';
#endif
    gASCII_shift_table[31] = 'K';
    gASCII_shift_table[32] = 'L';
    gASCII_shift_table[33] = 'M';
    gASCII_shift_table[34] = 'N';
    gASCII_shift_table[35] = 'O';
    gASCII_shift_table[36] = 'P';
    gASCII_shift_table[37] = 'Q';
    gASCII_shift_table[38] = 'R';
    gASCII_shift_table[39] = 'S';
    gASCII_shift_table[40] = 'T';
    gASCII_shift_table[41] = 'U';
    gASCII_shift_table[42] = 'V';
    gASCII_shift_table[43] = 'W';
    gASCII_shift_table[44] = 'X';
    gASCII_shift_table[45] = 'Y';
    gASCII_shift_table[46] = 'Z';

    memset(gScan_code, 0, sizeof(gScan_code));

    gScan_code[12] = 0xff;
    gScan_code[13] = 0xff;
    gScan_code[14] = 0xff;
    gScan_code[15] = 0xff;
    gScan_code[16] = SDL_SCANCODE_CAPSLOCK;
    gScan_code[17] = SDL_SCANCODE_RSHIFT;
    gScan_code[18] = SDL_SCANCODE_RALT;
    gScan_code[19] = SDL_SCANCODE_RCTRL;
    gScan_code[20] = SDL_SCANCODE_LSHIFT;
    gScan_code[21] = SDL_SCANCODE_LALT;
    gScan_code[22] = SDL_SCANCODE_LCTRL;
    gScan_code[23] = SDL_SCANCODE_0;
    gScan_code[24] = SDL_SCANCODE_1;
    gScan_code[25] = SDL_SCANCODE_2;
    gScan_code[26] = SDL_SCANCODE_3;
    gScan_code[27] = SDL_SCANCODE_4;
    gScan_code[28] = SDL_SCANCODE_5;
    gScan_code[29] = SDL_SCANCODE_6;
    gScan_code[30] = SDL_SCANCODE_7;
    gScan_code[31] = SDL_SCANCODE_8;
    gScan_code[32] = SDL_SCANCODE_9;
    gScan_code[33] = SDL_SCANCODE_A;
    gScan_code[34] = SDL_SCANCODE_B;
    gScan_code[35] = SDL_SCANCODE_C;
    gScan_code[36] = SDL_SCANCODE_D;
    gScan_code[37] = SDL_SCANCODE_E;
    gScan_code[38] = SDL_SCANCODE_F;
    gScan_code[39] = SDL_SCANCODE_G;
    gScan_code[40] = SDL_SCANCODE_H;
    gScan_code[41] = SDL_SCANCODE_I;
    gScan_code[42] = SDL_SCANCODE_J;
    gScan_code[43] = SDL_SCANCODE_K;
    gScan_code[44] = SDL_SCANCODE_L;
    gScan_code[45] = SDL_SCANCODE_M;
    gScan_code[46] = SDL_SCANCODE_N;
    gScan_code[47] = SDL_SCANCODE_O;
    gScan_code[48] = SDL_SCANCODE_P;
    gScan_code[49] = SDL_SCANCODE_Q;
    gScan_code[50] = SDL_SCANCODE_R;
    gScan_code[51] = SDL_SCANCODE_S;
    gScan_code[52] = SDL_SCANCODE_T;
    gScan_code[53] = SDL_SCANCODE_U;
    gScan_code[54] = SDL_SCANCODE_V;
    gScan_code[55] = SDL_SCANCODE_W;
    gScan_code[56] = SDL_SCANCODE_X;
    gScan_code[57] = SDL_SCANCODE_Y;
    gScan_code[58] = SDL_SCANCODE_Z;
    gScan_code[59] = SDL_SCANCODE_GRAVE;
    gScan_code[60] = SDL_SCANCODE_MINUS;
    gScan_code[61] = SDL_SCANCODE_EQUALS;
    gScan_code[62] = SDL_SCANCODE_BACKSPACE;
    gScan_code[63] = SDL_SCANCODE_RETURN;
    gScan_code[64] = SDL_SCANCODE_KP_ENTER;
    gScan_code[65] = SDL_SCANCODE_TAB;
    gScan_code[66] = SDL_SCANCODE_SLASH;
    gScan_code[67] = SDL_SCANCODE_BACKSLASH;
    gScan_code[68] = SDL_SCANCODE_SEMICOLON;
    gScan_code[69] = SDL_SCANCODE_APOSTROPHE;
    gScan_code[70] = SDL_SCANCODE_PERIOD;
    gScan_code[71] = SDL_SCANCODE_COMMA;
    gScan_code[72] = SDL_SCANCODE_LEFTBRACKET;
    gScan_code[73] = SDL_SCANCODE_RIGHTBRACKET;
    gScan_code[74] = 0;
    gScan_code[75] = SDL_SCANCODE_ESCAPE;
    gScan_code[76] = SDL_SCANCODE_INSERT;
    gScan_code[77] = SDL_SCANCODE_DELETE;
    gScan_code[78] = SDL_SCANCODE_HOME;
    gScan_code[79] = SDL_SCANCODE_END;
    gScan_code[80] = SDL_SCANCODE_PAGEUP;
    gScan_code[81] = SDL_SCANCODE_PAGEDOWN;
    gScan_code[82] = SDL_SCANCODE_LEFT;
    gScan_code[83] = SDL_SCANCODE_RIGHT;
    gScan_code[84] = SDL_SCANCODE_UP;
    gScan_code[85] = SDL_SCANCODE_DOWN;
    gScan_code[86] = SDL_SCANCODE_NUMLOCKCLEAR;
    gScan_code[87] = SDL_SCANCODE_KP_DIVIDE;
    gScan_code[88] = SDL_SCANCODE_KP_MULTIPLY;
    gScan_code[89] = SDL_SCANCODE_KP_MINUS;
    gScan_code[90] = SDL_SCANCODE_KP_PLUS;
    gScan_code[91] = SDL_SCANCODE_KP_DECIMAL;
    gScan_code[92] = 0;
    gScan_code[93] = SDL_SCANCODE_KP_0;
    gScan_code[94] = SDL_SCANCODE_KP_1;
    gScan_code[95] = SDL_SCANCODE_KP_2;
    gScan_code[96] = SDL_SCANCODE_KP_3;
    gScan_code[97] = SDL_SCANCODE_KP_4;
    gScan_code[98] = SDL_SCANCODE_KP_5;
    gScan_code[99] = SDL_SCANCODE_KP_6;
    gScan_code[100] = SDL_SCANCODE_KP_7;
    gScan_code[101] = SDL_SCANCODE_KP_8;
    gScan_code[102] = SDL_SCANCODE_KP_9;
    gScan_code[103] = SDL_SCANCODE_F1;
    gScan_code[104] = SDL_SCANCODE_F2;
    gScan_code[105] = SDL_SCANCODE_F3;
    gScan_code[106] = SDL_SCANCODE_F4;
    gScan_code[107] = SDL_SCANCODE_F5;
    gScan_code[108] = SDL_SCANCODE_F6;
    gScan_code[109] = SDL_SCANCODE_F7;
    gScan_code[110] = SDL_SCANCODE_F8;
    gScan_code[111] = SDL_SCANCODE_F9;
    gScan_code[112] = SDL_SCANCODE_F10;
    gScan_code[113] = SDL_SCANCODE_F11;
    gScan_code[114] = SDL_SCANCODE_F12;
    gScan_code[115] = SDL_SCANCODE_SYSREQ;
    gScan_code[116] = SDL_SCANCODE_SCROLLLOCK;
    gScan_code[117] = 0xff;
    gScan_code[118] = SDL_SCANCODE_SPACE;
}

void CloseDirectInput(void) {
}

void PDSetKeyArray(int *pKeys, int pMark) {
    const bool *keyboard_state;
    int num_keys;
    int i;

    gKeys_pressed = 0;
    Win32ServiceMessages();
    keyboard_state = SDL_GetKeyboardState(&num_keys);
    for (i = 0; i < 151; i++) {
        int scan_code;

        scan_code = gScan_code[12 + i];
        if (scan_code == 0) {
            continue;
        }
        if (scan_code == 0xff) {
            switch (i) {
            case 0:
                if (keyboard_state[gScan_code[20]] || keyboard_state[gScan_code[17]]) {
                    pKeys[0] = pMark;
                } else if (pKeys[0] == pMark) {
                    pKeys[0] = 0;
                }
                break;
            case 1:
                if (keyboard_state[gScan_code[21]] || keyboard_state[gScan_code[18]]) {
                    pKeys[1] = pMark;
                } else if (pKeys[1] == pMark) {
                    pKeys[1] = 0;
                }
                break;
            case 2:
            case 3:
                if (keyboard_state[gScan_code[22]] || keyboard_state[gScan_code[19]]) {
                    pKeys[2] = pMark;
                    pKeys[3] = pMark;
                } else if (pKeys[2] == pMark || pKeys[3] == pMark) {
                    pKeys[2] = 0;
                    pKeys[3] = 0;
                }
                break;
            }
        } else {
            if (keyboard_state[scan_code] ) {
                pKeys[i] = pMark;
                gKeys_pressed = gKeys_pressed << 8 | (i + 1);
            } else if (pKeys[i] == pMark) {
                pKeys[i] = 0;
            }
        }
    }
}

void PDReadJoysticks(void) {
}

int GetDirectInputJoy1X(void) {
    if (g_Count_Joystick_infos < 1) {
        return -1;
    }
    return (Sint32)g_Joystick_infos[0].axis_motions[SDL_GAMEPAD_AXIS_LEFTX] + 0x10000;
}

int GetDirectInputJoy1Y(void) {
    if (g_Count_Joystick_infos < 1) {
        return -1;
    }
    return (Sint32)g_Joystick_infos[0].axis_motions[SDL_GAMEPAD_AXIS_LEFTY] + 0x10000;
}

tU32 PDGetJoy1Button1(void) {
    if (g_Count_Joystick_infos < 1) {
        return 0;
    }
    return !!(g_Joystick_infos[0].buttons & (0x1 << 0));
}

tU32 PDGetJoy1Button2(void) {
    if (g_Count_Joystick_infos < 1) {
        return 0;
    }
    return !!(g_Joystick_infos[0].buttons & (0x1 << 1));
}

tU32 PDGetJoy1Button3(void) {
    if (g_Count_Joystick_infos < 1) {
        return 0;
    }
    return !!(g_Joystick_infos[0].buttons & (0x1 << 2));
}

tU32 PDGetJoy1Button4(void) {
    if (g_Count_Joystick_infos < 1) {
        return 0;
    }
    return !!(g_Joystick_infos[0].buttons & (0x1 << 3));
}

int PDGetJoy1X(void) {
    return GetDirectInputJoy1X();
}

int PDGetJoy1Y(void) {
    return GetDirectInputJoy1Y();
}

tU32 PDGetJoy2Button1(void) {
    if (g_Count_Joystick_infos < 2) {
        return 0;
    }
    return !!(g_Joystick_infos[1].buttons & (0x1 << 0));
}

tU32 PDGetJoy2Button2(void) {
    if (g_Count_Joystick_infos < 2) {
        return 0;
    }
    return !!(g_Joystick_infos[1].buttons & (0x1 << 1));
}

tU32 PDGetJoy2Button3(void) {
    if (g_Count_Joystick_infos < 2) {
        return 0;
    }
    return !!(g_Joystick_infos[1].buttons & (0x1 << 2));
}

tU32 PDGetJoy2Button4(void) {
    if (g_Count_Joystick_infos < 2) {
        return 0;
    }
    return !!(g_Joystick_infos[1].buttons & (0x1 << 3));
}

int PDGetJoy2X(void) {
    if (g_Count_Joystick_infos < 2) {
        return -1;
    }
    return (Sint32)g_Joystick_infos[1].axis_motions[SDL_GAMEPAD_AXIS_LEFTX] + 0x10000;
}

int PDGetJoy2Y(void) {
    if (g_Count_Joystick_infos < 2) {
        return -1;
    }
    return (Sint32)g_Joystick_infos[2].axis_motions[SDL_GAMEPAD_AXIS_LEFTY] + 0x10000;
}

void PDInitJoysticks(void) {
}
