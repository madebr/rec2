#include "68-timer.h"

#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x006b7818
int gTimers_stack_size;

// GLOBAL: CARMA2_HW 0x006b7600
tTimer gTimers[19];

// GLOBAL: CARMA2_HW 0x006b7814
int gTimers_max_index;

// GLOBAL: CARMA2_HW 0x006aaa40
int gTimers_draw_x;

// GLOBAL: CARMA2_HW 0x006aaa44
int gTimers_draw_y_stride;

// GLOBAL: CARMA2_HW 0x006aaa48
int gTimers_enough_samples;

// GLOBAL: CARMA2_HW 0x006aaa4c
int gTimers_frame_count;

// GLOBAL: CARMA2_HW 0x006aaa50
tU32 gTimers_frame_end_time;

// GLOBAL: CARMA2_HW 0x006aaa54
int gTimers_draw_y;

// GLOBAL: CARMA2_HW 0x006aaa58
tU32 gTimers_frame_start_time;

// GLOBAL: CARMA2_HW 0x006aaa5c
tU32 gTimers_tolerance;

// FUNCTION: CARMA2_HW 0x00504300
void C2_HOOK_FASTCALL Timers_Init(void) {
    int i;
    int j;

    gTimers_stack_size = 0;

    for (i = 0; i < (int)REC2_ASIZE(gTimers); i++) {
        for (j = 0; j < (int)REC2_ASIZE(gTimers[i].durations); j++) {
            gTimers[i].durations[j] = 0;
        }
        gTimers[i].longest_duration = 0;
    }

    gTimers_frame_count = 0;
    gTimers_enough_samples = 0;
    gTimers_max_index = REC2_ASIZE(gTimers) - 1;
    gTimers_draw_x = 34;
    gTimers_draw_y = gBack_screen->height > 250 ? 150 : 0;
    gTimers_draw_y_stride = 9;

#ifdef REC2_FIX_BUGS
#define LEN_IDENTIFIER(I) REC2_ASIZE(gTimers[I].identifier)
#else
#define LEN_IDENTIFIER(I) (REC2_ASIZE(gTimers[I].identifier) - 1)
#endif
#define TIMER_COLOUR_NAME(I, NAME, R5, G6, B5)                          \
    do {                                                                \
        gTimers[(I)].colour = RGB565_TO_BACKSCREEN_COLOUR(R5, G6, B5);  \
        strncpy(gTimers[(I)].identifier, (NAME), LEN_IDENTIFIER(I));    \
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
#undef LEN_IDENTIFIER
}

// FUNCTION: CARMA2_HW 0x00504700
void C2_HOOK_FASTCALL Timers_StartFrame(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gTimers); i++) {
        gTimers[i].durations[gTimers[i].index] = 0;
    }
    gTimers[TIMER_OQQ].start_time = PDGetMicroseconds();
    gTimers_frame_start_time = PDGetMicroseconds();
}

// FUNCTION: CARMA2_HW 0x00504740
void C2_HOOK_FASTCALL Timers_EndFrame(void) {
    int i;
    int j;
    tU32 total_duration;

    if (gTimers_stack_size != 0) {
        PDFatalError("Timers_EndFrame(): Timer stack mismatch.");
    }
    gTimers[TIMER_OQQ].durations[gTimers[TIMER_OQQ].index] += PDGetMicroseconds() - gTimers[TIMER_OQQ].start_time;
    gTimers_frame_end_time = PDGetMicroseconds();
    gTimers_frame_count += 1;
    if (!gTimers_enough_samples && gTimers_frame_count > 64) {
        gTimers_enough_samples = 1;
    }

    total_duration = 0;
    for (i = 0; i < (int)REC2_ASIZE(gTimers); i++) {
        gTimers[i].total_duration = 0;
        for (j = 0; j < (int)REC2_ASIZE(gTimers[i].durations); j++) {
            gTimers[i].total_duration += gTimers[i].durations[j];
        }
        gTimers[i].index = 0;
        total_duration += gTimers[i].total_duration;
        if (gTimers_enough_samples && gTimers[i].total_duration > gTimers[i].longest_duration) {
            gTimers[i].longest_duration = gTimers[i].total_duration;
        }
    }
    gTimers_tolerance = abs(total_duration - (gTimers_frame_end_time - gTimers_frame_start_time));
}

