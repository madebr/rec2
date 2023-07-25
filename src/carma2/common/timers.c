#include "timers.h"

#include "globvars.h"

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
