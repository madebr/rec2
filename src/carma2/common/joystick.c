#include "joystick.h"

#include "controls.h"
#include "displays.h"
#include "globvars.h"
#include "input.h"
#include "loading.h"
#include "platform.h"
#include "utility.h"

#include "rec2_macros.h"
#include "rec2_types.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(float, gForce_feedback_upper_limit, 0x00655e58, 2.7f);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(float, Force_feedback_lower_limit, 0x00655e5c, 1.5f);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gJoystick_index, 0x00595f88, -1);
C2_HOOK_VARIABLE_IMPLEMENT(float, gOriginal_joystick_x, 0x00688704);
C2_HOOK_VARIABLE_IMPLEMENT(float, gOriginal_joystick_y, 0x00688708);
C2_HOOK_VARIABLE_IMPLEMENT(int, gOriginal_joystick_fbb, 0x0068870c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gOriginal_joystick_dpad, 0x00688710);
C2_HOOK_VARIABLE_IMPLEMENT(int, gOrig_joystick_index, 0x00688700);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(float, gJoystick_x_steering, 0x00595f90, 1.f);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(float, gJoystick_y_throttle, 0x00595f94, 1.f);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gINT_00596308, 0x00596308, -1);
C2_HOOK_VARIABLE_IMPLEMENT(tHeadup_text_buffer, gJoystick_headup_buffer_0079d8a0, 0x0079d8a0);


void C2_HOOK_FASTCALL SetupFFBValues(void) {
    tPath_name path;
    FILE *f;
    char s[256];

    PathCat(path, C2V(gApplication_path), "FFB.TXT");
    f = DRfopen(path, "rt");
    GetALineAndDontArgue(f, s);
    c2_sscanf(c2_strtok(s, "\t ,/"), "%f", &C2V(gForce_feedback_upper_limit));
    GetALineAndDontArgue(f, s);
    c2_sscanf(c2_strtok(s, "\t ,/"), "%f", &C2V(Force_feedback_lower_limit));
    PFfclose(f);
}

int (C2_HOOK_FASTCALL * PlayExclusiveFFBEffect_original)(const char* pEffect_name, int pArg2);
int C2_HOOK_FASTCALL PlayExclusiveFFBEffect(const char* pEffect_name, int pArg2) {

#if defined(C2_HOOKS_ENABLED)
    return PlayExclusiveFFBEffect_original(pEffect_name, pArg2);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c6b0, PlayExclusiveFFBEffect, PlayExclusiveFFBEffect_original)

void (C2_HOOK_FASTCALL * InitJoysticks_original)(void);
void C2_HOOK_FASTCALL InitJoysticks(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitJoysticks_original();
#else
    PDInitJoysticks();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004575b0, InitJoysticks, InitJoysticks_original)

float (C2_HOOK_FASTCALL * GetJoystickX_original)(void);
float C2_HOOK_FASTCALL GetJoystickX(void) {

#if defined(C2_HOOKS_ENABLED)
    return GetJoystickX_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c8d0, GetJoystickX, GetJoystickX_original)

float (C2_HOOK_FASTCALL * GetJoystickY_original)(void);
float C2_HOOK_FASTCALL GetJoystickY(void) {

#if defined(C2_HOOKS_ENABLED)
    return GetJoystickY_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c8f0, GetJoystickY, GetJoystickY_original)

int (C2_HOOK_FASTCALL * GetJoystickFBBGain_original)(void);
int C2_HOOK_FASTCALL GetJoystickFBBGain(void) {

#if defined(C2_HOOKS_ENABLED)
    return GetJoystickFBBGain_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c7f0, GetJoystickFBBGain, GetJoystickFBBGain_original)

void (C2_HOOK_FASTCALL * SetJoystickX_original)(float pValue);
void C2_HOOK_FASTCALL SetJoystickX(float pValue) {

#if defined(C2_HOOKS_ENABLED)
    SetJoystickX_original(pValue);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c810, SetJoystickX, SetJoystickX_original)

void (C2_HOOK_FASTCALL * SetJoystickY_original)(float pValue);
void C2_HOOK_FASTCALL SetJoystickY(float pValue) {

#if defined(C2_HOOKS_ENABLED)
    SetJoystickY_original(pValue);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c870, SetJoystickY, SetJoystickY_original)

void (C2_HOOK_FASTCALL * SetJoystickFFBGain_original)(int pValue);
void C2_HOOK_FASTCALL SetJoystickFFBGain(int pValue) {

#if defined(C2_HOOKS_ENABLED)
    SetJoystickFFBGain_original(pValue);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c7b0, SetJoystickFFBGain, SetJoystickFFBGain_original)

void (C2_HOOK_FASTCALL * SetJoystickDPadEnabled_original)(int pEnabled);
void C2_HOOK_FASTCALL SetJoystickDPadEnabled(int pEnabled) {

#if defined(C2_HOOKS_ENABLED)
    SetJoystickDPadEnabled_original(pEnabled);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c570, SetJoystickDPadEnabled, SetJoystickDPadEnabled_original)

void C2_HOOK_FASTCALL Joystick_BackupSettings(void) {

    C2V(gOrig_joystick_index) = C2V(gJoystick_index);
    C2V(gOriginal_joystick_x) = GetJoystickX();
    C2V(gOriginal_joystick_y) = GetJoystickY();
    C2V(gOriginal_joystick_fbb) = GetJoystickFBBGain();
    C2V(gOriginal_joystick_dpad) = PDIsJoystickDPadEnabled();
    C2V(gJoystick_index) = C2V(gOrig_joystick_index);
}

void (C2_HOOK_FASTCALL * EnableJoysticks_original)(void);
void C2_HOOK_FASTCALL EnableJoysticks(void) {

#if defined(C2_HOOKS_ENABLED)
    EnableJoysticks_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045bd50, EnableJoysticks, EnableJoysticks_original)

void (C2_HOOK_FASTCALL * DisableJoysticks_original)(void);
void C2_HOOK_FASTCALL DisableJoysticks(void) {

#if defined(C2_HOOKS_ENABLED)
    DisableJoysticks_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045bd70, DisableJoysticks, DisableJoysticks_original)

void (C2_HOOK_FASTCALL * FUN_0045b0a0_original)(tHeadup_text_buffer* pText_buffer);
void C2_HOOK_FASTCALL FUN_0045b0a0(tHeadup_text_buffer* pText_buffer) {

#if defined(C2_HOOKS_ENABLED)
    FUN_0045b0a0_original(pText_buffer);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045b0a0, FUN_0045b0a0, FUN_0045b0a0_original)

int C2_HOOK_FASTCALL FUN_CheckJoystickHeadupButtons(tButtonJoystickInfo** pJoystick_info) {

    if (!PDKeyDown(65)) {
        return 1;
    }
    if (C2V(gINT_00596308) != -1) {
        C2V(gINT_00596308) = -1;
    } else {
        tHeadup_text* text;
        int last_index;
        size_t i;

        *pJoystick_info = PDGetCurrentJoystickData();
        if (*pJoystick_info == NULL) {
            return 0;
        }

        i = 0;
        last_index = -1;
        for (text = C2V(gJoystick_headup_buffer_0079d8a0).texts; text != NULL; text = text->next, i++) {
            if (text == C2V(gJoystick_headup_buffer_0079d8a0).text_last_add) {
                last_index = i;
            }
        }
        for (i = 0; i < (*pJoystick_info)->count_buttons; i++) {
            if ((*pJoystick_info)->buttons[i] == last_index && (*pJoystick_info)->buttons[i] != -1) {
                (*pJoystick_info)->buttons[i] = -1;
            }
        }
    }
    while (PDKeyDown(65)) {
        /* brr */
        /* FIXME: replace with udelay */
    }
    return 1;
}

void (C2_HOOK_FASTCALL * MungeJoystickHeadups_original)(void);
void C2_HOOK_FASTCALL MungeJoystickHeadups(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    MungeJoystickHeadups_original();
#else
    tButtonJoystickInfo* joystick_info;
    tHeadup_text* headup_text;
    int i;

    joystick_info = NULL;
    headup_text = NULL;
    if (C2V(gINT_0068b8e8)) {
        tU32 the_time;

        the_time = PDGetTotalTime();
        if (FUN_CheckJoystickHeadupButtons(&joystick_info) && C2V(gINT_00596308) == -1) {

            if (PDKeyDown(64) && C2V(gINT_00596308) == -1) {
                int i;
                tHeadup_text* headup_text2;

                i = 0;
                for (headup_text2 = C2V(gJoystick_headup_buffer_0079d8a0).texts; headup_text2 != NULL; i++, headup_text2 = headup_text2->next) {
                    if (headup_text2 == C2V(gJoystick_headup_buffer_0079d8a0).text_last_add) {
                        C2V(gINT_00596308) = i;
                    }
                }
                while (PDKeyDown(64)) {
                    /* brr */
                    /* FIXME: replace with udelay */
                }
            }
            if (PDKeyDown(66)) {
                if (C2V(gJoystick_headup_buffer_0079d8a0).text_last_add != NULL
                        && C2V(gJoystick_headup_buffer_0079d8a0).text_last_add != C2V(gJoystick_headup_buffer_0079d8a0).texts) {
                    int i;

                    C2V(gJoystick_headup_buffer_0079d8a0).text_last_add = C2V(gJoystick_headup_buffer_0079d8a0).texts;
                    C2V(gJoystick_headup_buffer_0079d8a0).pos_last_add -= 1;
                    for (i = 0; i < C2V(gJoystick_headup_buffer_0079d8a0).pos_last_add; i++) {
                        C2V(gJoystick_headup_buffer_0079d8a0).text_last_add = C2V(gJoystick_headup_buffer_0079d8a0).text_last_add->next;
                    }
                }
                while (PDGetTotalTime() - the_time < 100) {
                    /* brr */
                    /* FIXME: replace with udelay */
                }
            }
            if (PDKeyDown(67)) {
                if (C2V(gJoystick_headup_buffer_0079d8a0).text_last_add != NULL
                        && C2V(gJoystick_headup_buffer_0079d8a0).text_last_add->next != NULL) {
                    C2V(gJoystick_headup_buffer_0079d8a0).pos_last_add += 1;
                    C2V(gJoystick_headup_buffer_0079d8a0).text_last_add = C2V(gJoystick_headup_buffer_0079d8a0).text_last_add->next;
                }
                while (PDGetTotalTime() - the_time < 100) {
                    /* brr */
                    /* FIXME: replace with udelay */
                }
            }
            if (PDKeyDown(68)) {
                int i;
                tHeadup_text* last_text;
                int last_pos;

                last_text = C2V(gJoystick_headup_buffer_0079d8a0).text_last_add;
                last_pos = C2V(gJoystick_headup_buffer_0079d8a0).pos_last_add;

                for (i = 0; i < C2V(gJoystick_headup_buffer_0079d8a0).field_0x10 - 1; i++) {
                    if (last_text != NULL && last_text != C2V(gJoystick_headup_buffer_0079d8a0).texts) {
                        int j;
                        tHeadup_text* headup_text2;

                        last_pos -= 1;
                        C2V(gJoystick_headup_buffer_0079d8a0).text_last_add = C2V(gJoystick_headup_buffer_0079d8a0).texts;
                        headup_text2 = C2V(gJoystick_headup_buffer_0079d8a0).texts;
                        C2V(gJoystick_headup_buffer_0079d8a0).pos_last_add = last_pos;

                        for (j = 0; j < last_pos; j++) {
                            headup_text2 = headup_text2->next;
                            C2V(gJoystick_headup_buffer_0079d8a0).text_last_add = headup_text2;
                        }
                    }
                }
                while (PDKeyDown(68)) {
                    /* brr */
                    /* FIXME: replace with udelay */
                }
            }
            if (PDKeyDown(69)) {
                int i;
                tHeadup_text* headup_text2;

                headup_text2 = C2V(gJoystick_headup_buffer_0079d8a0).text_last_add;

                for (i = 0; i < C2V(gJoystick_headup_buffer_0079d8a0).field_0x10 - 1; i++) {
                    if (headup_text2 != NULL && headup_text2->next != NULL) {
                        C2V(gJoystick_headup_buffer_0079d8a0).pos_last_add += 1;
                        headup_text2 = headup_text2->next;
                        C2V(gJoystick_headup_buffer_0079d8a0).text_last_add = headup_text2;
                    }
                }
                while (PDKeyDown(69)) {
                    /* brr */
                    /* FIXME: replace with udelay */
                }
            }
        }
        FUN_0045b0a0(&C2V(gJoystick_headup_buffer_0079d8a0));
        headup_text = C2V(gJoystick_headup_buffer_0079d8a0).field_0x4;
        joystick_info = PDGetCurrentJoystickData();
        if (joystick_info == NULL) {
            return;
        }
    } else if (C2V(gINT_0068b8e4)) {
        return;
    }

    for (i = 0; i < REC2_ASIZE(C2V(gRace_head_ups)); i++) {

        if (C2V(gINT_0068b8e8) && headup_text != NULL) {
            int font;
            char s[80];

            if (i == C2V(gINT_00596308)) {
                font = -3;
            } else if (headup_text == C2V(gJoystick_headup_buffer_0079d8a0).text_last_add) {
                font = -1;
            } else {
                font = -2;
            }
            ChangeHeadupFont(C2V(gRace_head_ups)[i], font);
            if (i == C2V(gINT_00596308)) {
                c2_sprintf(s, "%s      ???", headup_text->text);
            } else if (joystick_info == NULL) {
                c2_sprintf(s, "%s", headup_text->text);
            } else {
                size_t j;

                c2_sprintf(s, "%s", headup_text->text);
                for (j = 0; j < joystick_info->count_buttons; j++) {
                    if (i == joystick_info->buttons[j]) {
                        c2_sprintf(s, "%s      #%02d", headup_text->text, (int)j);
                    }
                }
            }
            ChangeHeadupText(C2V(gRace_head_ups)[i], s);
            headup_text = headup_text->next;
        } else {
            ChangeHeadupText(C2V(gRace_head_ups)[i], "");
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045b3b0, MungeJoystickHeadups, MungeJoystickHeadups_original)

int C2_HOOK_FASTCALL HasCurrentJoystick(void) {

    return C2V(gJoystick_index) != -1;
}
C2_HOOK_FUNCTION(0x0045a0f0, HasCurrentJoystick)
