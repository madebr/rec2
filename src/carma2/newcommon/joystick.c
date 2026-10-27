/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "joystick.h"

#include "08-loading1.h"
#include "41-utility.h"
#include "globvars.h"
#include "platform.h"

#include "c2_stdio.h"
#include "c2_stdlib.h"
#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x00595f84
int gINT_00595f84 = 7500;

// GLOBAL: CARMA2_HW 0x00595f88
// GLOBAL: CARMA2_SW 0x00590ee8
int gJoystick_index = -1;

// GLOBAL: CARMA2_HW 0x00595f90
// GLOBAL: CARMA2_SW 0x00590ef0
float gJoystick_x_steering = 1.f;

// GLOBAL: CARMA2_HW 0x00595f94
// GLOBAL: CARMA2_SW 0x00590ef4
float gJoystick_y_throttle = 1.f;

// GLOBAL: CARMA2_HW 0x00596310
// GLOBAL: CARMA2_SW 0x00591270
int gINT_00596310 = 50;

// GLOBAL: CARMA2_HW 0x00686184
int gJoystick_preferences_backup_valid;

// GLOBAL: CARMA2_HW 0x0079d8c0
tButtonJoystickInfo gJoystick_preferences_backup;

typedef struct tJoystick_preference_node {
    tButtonJoystickInfo* info;
    struct tJoystick_preference_node* next;
} tJoystick_preference_node;

// FUNCTION: CARMA2_HW 0x0045a0f0
// FUNCTION: CARMA2_SW 0x00459e80
int C2_HOOK_FASTCALL HasCurrentJoystick(void) {
    return gJoystick_index != -1;
}

// FUNCTION: CARMA2_HW 0x0045a300
// FUNCTION: CARMA2_SW 0x0045a090
int C2_HOOK_FASTCALL ReadUntilCharacter(FILE* pF, char* pBuffer, int pBuffer_size, char pLimit_char) {
    int i;
    char s[2];

    strcpy(pBuffer, "");
    s[1] = '\0';
    for (i = 0; (s[0] = fgetc(pF)) != pLimit_char && i < pBuffer_size && !feof(pF); i++) {
        strcat(pBuffer, s);
    }
    if (feof(pF)) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0045b920
int C2_HOOK_FASTCALL WriteJoystickPreferences(FILE* pF, const tButtonJoystickInfo* pInfo) {
    char s[256];
    char joystick_name[256];
    size_t i;

    strcpy(joystick_name, pInfo->productName);
    for (i = 0; i < strlen(joystick_name); i++) {
        if (joystick_name[i] == ' ') {
            joystick_name[i] = '_';
        }
    }
    sprintf(s, "&%s\n", joystick_name);
    fwrite(s, 1, strlen(s), pF);
    sprintf(s, "%d\n", 32);
    fwrite(s, 1, strlen(s), pF);
    for (i = 0; i < 32; i++) {
        sprintf(s, "%d\n", pInfo->buttons[i]);
        fwrite(s, 1, strlen(s), pF);
    }
    sprintf(s, "%d\n", pInfo->field_0xd4);
    fwrite(s, 1, strlen(s), pF);
    sprintf(s, "%.3f, %.3f\n", pInfo->field_0xd8, pInfo->field_0xdc);
    fwrite(s, 1, strlen(s), pF);
    sprintf(s, "%d\n", pInfo->field_0xe0);
    fwrite(s, 1, strlen(s), pF);
    sprintf(s, "%d\n", pInfo->field_0xe4);
    fwrite(s, 1, strlen(s), pF);
    sprintf(s, "%d\n", pInfo->field_0xe8);
    fwrite(s, 1, strlen(s), pF);
    sprintf(s, "\n");
    fwrite(s, 1, strlen(s), pF);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0045b7e0
int C2_HOOK_FASTCALL FindJoystickPreferences(FILE* pF, const char* pJoystick_name) {
    fpos_t position;
    char joystick_name[256];
    char s[256];
    long start;
    size_t i;

    rewind(pF);
    start = ftell(pF);
    fseek(pF, 0, SEEK_END);
    if (ftell(pF) - start < 32) {
        return -1;
    }
    strcpy(joystick_name, pJoystick_name);
    for (i = 0; i < strlen(joystick_name); i++) {
        if (joystick_name[i] == ' ') {
            joystick_name[i] = '_';
        }
    }
    rewind(pF);
    while (!feof(pF)) {
        if (fgetpos(pF, &position) != 0) {
            PDFatalError("getpos error");
        }
        GetAString(pF, s);
        if (strstr(s, joystick_name) != NULL) {
            if (fsetpos(pF, &position) != 0) {
                PDFatalError("setpos error");
            }
            return 1;
        }
    }
    rewind(pF);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045bbc0
int C2_HOOK_FASTCALL ReadJoystickPreferences(FILE* pF, tButtonJoystickInfo* pInfo) {
    char s[256];
    size_t count_buttons;
    size_t i;

    GetAString(pF, s);
    for (i = 0; i < strlen(s); i++) {
        if (s[i] == '_') {
            s[i] = ' ';
        }
    }
    strcpy(pInfo->productName, s + 1);
    count_buttons = GetAnInt(pF);
    for (i = 0; i < 32; i++) {
        if (i < count_buttons) {
            pInfo->buttons[i] = GetAnInt(pF);
        } else {
            pInfo->buttons[i] = -1;
        }
    }
    pInfo->field_0xd4 = GetAnInt(pF);
    GetPairOfScalars(pF, &pInfo->field_0xd8, &pInfo->field_0xdc);
    pInfo->field_0xe0 = GetAnInt(pF);
    pInfo->field_0xe4 = GetAnInt(pF);
    pInfo->field_0xe8 = GetAnInt(pF);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0045bcc0
int C2_HOOK_FASTCALL CountJoystickPreferences(FILE* pF) {
    char s[256];
    int count = 0;
    long start;

    rewind(pF);
    start = ftell(pF);
    fseek(pF, 0, SEEK_END);
    if (ftell(pF) - start < 32) {
        return 0;
    }
    rewind(pF);
    while (!feof(pF)) {
        GetAString(pF, s);
        if (s[0] == '&') {
            count++;
        }
    }
    rewind(pF);
    return count;
}

// FUNCTION: CARMA2_HW 0x0045bd90
int C2_HOOK_FASTCALL SaveJoystickPreferences(void) {
    tPath_name path;
    char error[256];
    int original_index;
    int count;
    int i;
    int result;
    FILE* f;
    tJoystick_preference_node* list;
    tJoystick_preference_node* node;
    tJoystick_preference_node* tail;
    tJoystick_preference_node* match;
    tButtonJoystickInfo* info;

    original_index = gJoystick_index;
    PathCat(path, gApplication_path, "JOYSTICK.TXT");
    f = fopen(path, "rt");
    if (f == NULL) {
        f = fopen(path, "wt");
        if (f != NULL) {
            for (i = 0; i < 16; i++) {
                if (PDIsJoystickAvailable(i)) {
                    gJoystick_index = i;
                    info = PDGetCurrentJoystickData();
                    if (info != NULL) {
                        WriteJoystickPreferences(f, info);
                    }
                }
            }
            fclose(f);
        }
        gJoystick_index = original_index;
        return 1;
    }
    count = CountJoystickPreferences(f);
    if (count == 0) {
        return 1;
    }
    list = malloc(sizeof(*list));
    if (list == NULL) {
        fclose(f);
        gJoystick_index = original_index;
        return 0;
    }
    node = list;
    for (i = 0; i < count; i++) {
        node->next = NULL;
        node->info = malloc(sizeof(*node->info));
        if (node->info != NULL) {
            ReadJoystickPreferences(f, node->info);
        }
        if (i != count - 1) {
            node->next = malloc(sizeof(*node));
            node = node->next;
        }
        tail = node;
    }
    fclose(f);
    for (i = 0; i < 16; i++) {
        if (PDIsJoystickAvailable(i)) {
            gJoystick_index = i;
            info = PDGetCurrentJoystickData();
            if (info == NULL) {
                continue;
            }
            match = NULL;
            for (node = list; node != NULL; node = node->next) {
                if (match != NULL) {
                    break;
                }
                if (strcmp(node->info->productName, GetCurrentJoystickName()) == 0) {
                    match = node;
                }
            }
            if (match != NULL) {
                *match->info = *info;
            } else {
                node = malloc(sizeof(*node));
                if (node == NULL) {
                    return 0;
                }
                tail->next = node;
                tail = node;
                node->info = malloc(sizeof(*node->info));
                if (node->info == NULL) {
                    return 0;
                }
                node->next = NULL;
                *node->info = *info;
            }
        }
    }
    PathCat(path, gApplication_path, "JOYSTICK.TXT");
    f = fopen(path, "wt");
    if (f == NULL) {
        sprintf(error, "Cant open %s", path);
        return 0;
    }
    node = list;
    do {
        if (node->info != NULL) {
            WriteJoystickPreferences(f, node->info);
        }
        tail = node;
        node = node->next;
        if (tail->info != NULL) {
            free(tail->info);
        }
        if (tail != NULL) {
            free(tail);
        }
    } while (node != NULL);
    result = fclose(f);
    gJoystick_index = original_index;
    return result;
}

// FUNCTION: CARMA2_HW 0x0045bd50
void C2_HOOK_FASTCALL EnableJoysticks(void) {
    tButtonJoystickInfo* info;

    PDEnableJoysticks();
    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        info->field_0xe8 = 1;
    }
}

// FUNCTION: CARMA2_HW 0x0045bd70
void C2_HOOK_FASTCALL DisableJoysticks(void) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        info->field_0xe8 = 0;
    }
    PDDisableJoysticks();
}

// FUNCTION: CARMA2_HW 0x0045c0b0
// FUNCTION: CARMA2_SW 0x0045bb30
int C2_HOOK_FASTCALL LoadJoystickPreferences(void) {
    tPath_name path;
    tButtonJoystickInfo* info;
    FILE* f;

    info = PDGetCurrentJoystickData();
    if (info == NULL) {
        return 0;
    }
    PathCat(path, gApplication_path, "JOYSTICK.TXT");
    f = fopen(path, "rt");
    if (f == NULL) {
        return 0;
    }
    if (FindJoystickPreferences(f, GetCurrentJoystickName()) > 0) {
        ReadJoystickPreferences(f, info);
        fclose(f);
    } else {
        fclose(f);
        remove(path);
    }
    gJoystick_x_steering = info->field_0xd8;
    gJoystick_y_throttle = info->field_0xdc;
    gINT_00595f9c = info->field_0xd4;
    gINT_00595f84 = info->field_0xe4;
    gINT_00596310 += 25;
    if (gINT_00596310 > 100) {
        gINT_00596310 = 0;
    }
    PDSetJoystickFFBGain(gINT_00596310);
    if (info->field_0xe8 != 0) {
        EnableJoysticks();
    } else {
        DisableJoysticks();
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x0045c1e0
void C2_HOOK_FASTCALL BackupJoystickPreferences(void) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        gJoystick_preferences_backup = *info;
        gJoystick_preferences_backup_valid = 1;
    }
}

// FUNCTION: CARMA2_HW 0x0045c210
void C2_HOOK_FASTCALL RestoreJoystickPreferences(void) {
    tButtonJoystickInfo* info;

    if (gJoystick_preferences_backup_valid) {
        info = PDGetCurrentJoystickData();
        if (info != NULL) {
            *info = gJoystick_preferences_backup;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045c460
int C2_HOOK_FASTCALL ToggleJoystickThrottle(void) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        info->field_0xd4 = !info->field_0xd4;
        gINT_00595f9c = info->field_0xd4;
        return gINT_00595f9c;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045c570
void C2_HOOK_FASTCALL SetJoystickDPadEnabled(int pEnabled) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        info->field_0xe0 = pEnabled;
    }
}

// FUNCTION: CARMA2_HW 0x0045c780
int C2_HOOK_FASTCALL CycleJoystickFFBGain(void) {

    gINT_00596310 += 25;
    if (gINT_00596310 > 100) {
        gINT_00596310 = 0;
    }
    PDSetJoystickFFBGain(gINT_00596310);
    return gINT_00596310;
}

// FUNCTION: CARMA2_HW 0x0045c7b0
// FUNCTION: CARMA2_SW 0x0045c1f0
void C2_HOOK_FASTCALL SetJoystickFFBGain(int pValue) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        if (pValue < 0) {
            pValue = 0;
        }
        if (pValue > 100) {
            pValue = 100;
        }
        info->field_0xe4 = pValue;
        gINT_00596310 = pValue;
        PDSetJoystickFFBGain(pValue);
    }
}

// FUNCTION: CARMA2_HW 0x0045c7f0
int C2_HOOK_FASTCALL GetJoystickFBBGain(void) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        return info->field_0xe4;
    }
    return gINT_00596310;
}

// FUNCTION: CARMA2_HW 0x0045c810
// FUNCTION: CARMA2_SW 0x0045c250
void C2_HOOK_FASTCALL SetJoystickX(float pValue) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        info->field_0xd8 = pValue;
        if (info->field_0xd8 > 2.0) {
            info->field_0xd8 = 2.f;
        }
        if (info->field_0xd8 < 0.0) {
            info->field_0xd8 = 0.f;
        }
        gJoystick_x_steering = info->field_0xd8;
    }
}

// FUNCTION: CARMA2_HW 0x0045c870
// FUNCTION: CARMA2_SW 0x0045c2b0
void C2_HOOK_FASTCALL SetJoystickY(float pValue) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        info->field_0xdc = pValue;
        if (info->field_0xdc > 2.0) {
            info->field_0xdc = 2.f;
        }
        if (info->field_0xdc < 0.0) {
            info->field_0xdc = 0.f;
        }
        gJoystick_y_throttle = info->field_0xdc;
    }
}

// FUNCTION: CARMA2_HW 0x0045c8d0
// FUNCTION: CARMA2_SW 0x0045c310
float C2_HOOK_FASTCALL GetJoystickX(void) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        return info->field_0xd8;
    }
    return gJoystick_x_steering;
}

// FUNCTION: CARMA2_HW 0x0045c8f0
// FUNCTION: CARMA2_SW 0x0045c330
float C2_HOOK_FASTCALL GetJoystickY(void) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        return info->field_0xdc;
    }
    return gJoystick_y_throttle;
}
