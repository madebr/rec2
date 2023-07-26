#include "timers.h"

#include "displays.h"
#include "globvars.h"
#include "graphics.h"

#include "platform.h"

#include "brender/brender.h"

#include "c2_string.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gTimers_stack_size, 0x006b7818);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tTimer, gTimers, 19, 0x006b7600);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTimers_frame_count, 0x006aaa4c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTimers_enough_samples, 0x006aaa48);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTimers_max_index, 0x006b7814);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTimers_draw_x, 0x006aaa40);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTimers_draw_y_stride, 0x006aaa44);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTimers_draw_y, 0x006aaa54);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gTimers_frame_start_time, 0x006aaa58);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gTimers_frame_end_time, 0x006aaa50);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gTimers_tolerance, 0x006aaa5c);

#define RGB565_TO_BACKSCREEN_COLOUR(R5, G6, B5) (C2V(gBack_screen)->type == BR_PMT_RGB_565 ? (((R5) << 11) | ((G6) << 5) | (B5)) : (((R5) << 10) | (((G6) >> 1) << 5) | (B5)))

void C2_HOOK_FASTCALL InitTimers(void) {
    int i;
    int j;
    C2V(gTimers_stack_size) = 0;

    C2_HOOK_BUG_ON(sizeof(tTimer) != 28);

    for (i = 0; i < REC2_ASIZE(C2V(gTimers)); i++) {
        for (j = 0; j < REC2_ASIZE(C2V(gTimers)[i].durations); j++) {
            C2V(gTimers)[i].durations[j] = 0;
        }
        C2V(gTimers)[i].longest_duration = 0;
    }

    C2V(gTimers_frame_count) = 0;
    C2V(gTimers_enough_samples) = 0;
    C2V(gTimers_max_index) = REC2_ASIZE(C2V(gTimers)) - 1;
    C2V(gTimers_draw_x) = 34;
    C2V(gTimers_draw_y_stride) = 9;
    C2V(gTimers_draw_y) = C2V(gBack_screen)->height > 250 ? 150 : 0;

#define TIMER_COLOUR_NAME(I, NAME, R5, G6, B5)                                                        \
    do {                                                                                              \
        C2V(gTimers)[(I)].colour = RGB565_TO_BACKSCREEN_COLOUR(R5, G6, B5);                           \
        c2_strncpy(C2V(gTimers)[(I)].identifier, NAME, REC2_ASIZE(C2V(gTimers)[(I)].identifier) - 1); \
    } while (0)

    TIMER_COLOUR_NAME(TIMER_MNG, "MNG", 0x1f, 0x00, 0x00);
    TIMER_COLOUR_NAME(TIMER_ROB, "ROB", 0x00, 0x00, 0x15);
    TIMER_COLOUR_NAME(TIMER_RWL, "RWL", 0x00, 0x3e, 0x00);
    TIMER_COLOUR_NAME(TIMER_RQQ, "R??", 0x1f, 0x00, 0x00);
    TIMER_COLOUR_NAME(TIMER_PHS, "PHS", 0x00, 0x26, 0x1f);
    TIMER_COLOUR_NAME(TIMER_PED, "PED", 0x1f, 0x3e, 0x00);
    TIMER_COLOUR_NAME(TIMER_DRN, "DRN", 0x07, 0x2e, 0x07);
    TIMER_COLOUR_NAME(TIMER_OPP, "OPP", 0x1f, 0x3e, 0x1f);
    TIMER_COLOUR_NAME(TIMER_CRS, "CRS", 0x1f, 0x00, 0x00);
    TIMER_COLOUR_NAME(TIMER_LLR, "LLR", 0x00, 0x26, 0x1f);
    TIMER_COLOUR_NAME(TIMER_CRC, "CRC", 0x00, 0x00, 0x15);
    TIMER_COLOUR_NAME(TIMER_SRC, "SRC", 0x00, 0x3e, 0x00);
    TIMER_COLOUR_NAME(TIMER_PRC, "PRC", 0x1f, 0x00, 0x00);
    TIMER_COLOUR_NAME(TIMER_CHK, "CHK", 0x1f, 0x3e, 0x1f);
    TIMER_COLOUR_NAME(TIMER_MAN, "MAN", 0x1f, 0x3e, 0x00);
    TIMER_COLOUR_NAME(TIMER_GUA, "GUA", 0x00, 0x00, 0x15);
    TIMER_COLOUR_NAME(TIMER_REC, "REC", 0x00, 0x26, 0x1f);
    TIMER_COLOUR_NAME(TIMER_SER, "SER", 0x1f, 0x3e, 0x1f);
    TIMER_COLOUR_NAME(TIMER_OQQ, "O??", 0x06, 0x2a, 0x06);

#undef TIMER_COLOUR_NAME
}
C2_HOOK_FUNCTION(0x00504300, InitTimers)

void C2_HOOK_FASTCALL Timers_StartFrame(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gTimers)); i++) {
        C2V(gTimers)[i].durations[C2V(gTimers)[i].index] = 0;
    }
    C2V(gTimers)[TIMER_OQQ].start_time = PDGetTotalMicroTime();
    C2V(gTimers_frame_start_time) = PDGetTotalMicroTime();
}
C2_HOOK_FUNCTION(0x00504700, Timers_StartFrame)

void C2_HOOK_FASTCALL Timers_EndFrame(void) {
    int i;
    int j;
    tU32 total_duration;

    if (C2V(gTimers_stack_size) != 0) {
        PDFatalError("Timers_EndFrame(): Timer stack mismatch.");
    }
    C2V(gTimers)[TIMER_OQQ].durations[C2V(gTimers)[TIMER_OQQ].index] += PDGetTotalMicroTime() - C2V(gTimers)[TIMER_OQQ].start_time;
    C2V(gTimers_frame_end_time) = PDGetTotalMicroTime();
    C2V(gTimers_frame_count)++;
    if (!C2V(gTimers_enough_samples) && C2V(gTimers_frame_count) > 64) {
        C2V(gTimers_enough_samples) = 1;
    }

    total_duration = 0;
    for (i = 0; i < REC2_ASIZE(C2V(gTimers)); i++) {
        C2V(gTimers)[i].total_duration = 0;
        for (j = 0; j < REC2_ASIZE(C2V(gTimers)[i].durations); j++) {
            C2V(gTimers)[i].total_duration += C2V(gTimers)[i].durations[j];
        }
        C2V(gTimers)[i].index = 0;
        total_duration += C2V(gTimers)[i].total_duration;
        if (C2V(gTimers_enough_samples) && C2V(gTimers)[i].total_duration > C2V(gTimers)[i].longest_duration) {
            C2V(gTimers)[i].longest_duration = C2V(gTimers)[i].total_duration;
        }
    }
    C2V(gTimers_tolerance) = abs(total_duration - (C2V(gTimers_frame_end_time) - C2V(gTimers_frame_start_time)));
}
C2_HOOK_FUNCTION(0x00504740, Timers_EndFrame)

void C2_HOOK_FASTCALL Timers_Draw(br_pixelmap* pScreen) {
    int i;
    int j;
    int longest_duration_timer;
    tU32 longest_duration;
    tU32 total_duration;
    float draw_factor;
    tTimer* timer;
    char duration_str[16];
    char tolerance_str[64];

    total_duration = 0;
    for (i = 0; i < REC2_ASIZE(C2V(gTimers)); i++) {
        total_duration += C2V(gTimers)[i].total_duration;
    }

    longest_duration_timer = -1;
    longest_duration = 0;
    draw_factor = (float)(pScreen->width - 2 *  C2V(gTimers_draw_x)) / total_duration;
    for (i = 0; i < REC2_ASIZE(C2V(gTimers)); i++) {
        timer = &C2V(gTimers)[i];
        TransDRPixelmapText(pScreen,
            10,
            C2V(gTimers_draw_y) + i * C2V(gTimers_draw_y_stride) - 3,
            &C2V(gFonts)[1],
            timer->identifier,
            0);
        for (j = 0; j < 3; j++) {
            BrPixelmapLine(pScreen,
                C2V(gTimers_draw_x),
                C2V(gTimers_draw_y) + i * C2V(gTimers_draw_y_stride) + j,
                (int)(C2V(gTimers_draw_x) + timer->total_duration * draw_factor),
                C2V(gTimers_draw_y) + i * C2V(gTimers_draw_y_stride) + j,
                timer->colour);
        }
        c2_sprintf(duration_str, "%i", timer->total_duration);
        TransDRPixelmapText(pScreen,
            (int)(C2V(gTimers_draw_x) + timer->total_duration * draw_factor),
            C2V(gTimers_draw_y) + i * C2V(gTimers_draw_y_stride) - 3,
            &C2V(gFonts)[1],
            duration_str,
            0);
        if (timer->total_duration > longest_duration) {
            longest_duration = timer->total_duration;
            longest_duration_timer = i;
        }
    }
    if (longest_duration_timer != -1) {
        BrPixelmapRectangle(pScreen,
            C2V(gTimers_draw_x) - 2,
            C2V(gTimers_draw_y) + longest_duration_timer * C2V(gTimers_draw_y_stride) - 2,
            (int)(C2V(gTimers)[longest_duration_timer].total_duration * draw_factor - 5.f),
            6,
            RGB565_TO_BACKSCREEN_COLOUR(0x1f, 0x3e, 0x1f));
    }
    BrPixelmapLine(pScreen,
        (int)(C2V(gTimers_draw_x) + 10000.f * draw_factor),
        C2V(gTimers_draw_y) - 1,
        (int)(C2V(gTimers_draw_x) + 10000.f * draw_factor),
        C2V(gTimers_draw_y) + (REC2_ASIZE(C2V(gTimers)) - 1) * C2V(gTimers_draw_y_stride) + 3,
        RGB565_TO_BACKSCREEN_COLOUR(0x1f, 0x00, 0x00));
    c2_sprintf(tolerance_str, "8.38ms, tollerance = %i", C2V(gTimers_tolerance));
    TransDRPixelmapText(pScreen,
        (int)(C2V(gTimers_draw_x) + draw_factor * 10000.f - 13.f),
        C2V(gTimers_draw_y) + C2V(gTimers_draw_y_stride) * (REC2_ASIZE(C2V(gTimers)) - 1) + 7,
        &C2V(gFonts)[1],
        tolerance_str,
        0);
}
C2_HOOK_FUNCTION(0x00504810, Timers_Draw)
