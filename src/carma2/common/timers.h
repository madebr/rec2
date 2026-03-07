#ifndef REC2_TIMERS_H
#define REC2_TIMERS_H

#include "rec2_types.h"

#include "c2_hooks.h"

#include "platform.h"

#define TIMER_MNG 0
#define TIMER_ROB 1
#define TIMER_RWL 2
#define TIMER_RQQ 3
#define TIMER_PHS 4
#define TIMER_PED 5
#define TIMER_DRN 6
#define TIMER_OPP 7
#define TIMER_CRS 8
#define TIMER_LLR 9
#define TIMER_CRC 10
#define TIMER_SRC 11
#define TIMER_PRC 12
#define TIMER_CHK 13
#define TIMER_MAN 14
#define TIMER_GUA 15
#define TIMER_REC 16
#define TIMER_SER 17
#define TIMER_OQQ 18

extern int gTimers_stack_size;
extern tTimer gTimers[19];
extern int gTimers_stack[19];
extern int gTimers_frame_count;
extern int gTimers_enough_samples;
extern int gTimers_max_index;
extern int gTimers_draw_x;
extern int gTimers_draw_y_stride;
extern int gTimers_draw_y;
extern tU32 gTimers_frame_start_time;
extern tU32 gTimers_frame_end_time;
extern tU32 gTimers_tolerance;

void C2_HOOK_FASTCALL Timers_Init(void);

void C2_HOOK_FASTCALL Timers_StartFrame(void);

void C2_HOOK_FASTCALL Timers_EndFrame(void);

void C2_HOOK_FASTCALL Timers_Draw(br_pixelmap* pScreen);

static void inline Timers_Push(int pType) {
    int prev_type;

    gTimers_stack[gTimers_stack_size] = pType;
    if (gTimers_stack_size == 0) {
        prev_type = gTimers_max_index;
    } else {
        prev_type = gTimers_stack[gTimers_stack_size - 1];
    }
    gTimers[prev_type].durations[gTimers[prev_type].index] += PDGetMicroseconds() - gTimers[prev_type].start_time;
    gTimers[pType].start_time = PDGetMicroseconds();
    gTimers_stack_size += 1;
}

static void inline Timers_Pop(int pType) {
    int prev_type;

    gTimers[pType].durations[gTimers[pType].index] +=PDGetMicroseconds() - gTimers[pType].start_time;
    if (gTimers_stack_size > 1) {
        prev_type = gTimers_stack_size - 1;
    } else {
        prev_type = gTimers_max_index;
    }
    gTimers[prev_type].start_time = PDGetMicroseconds();
    gTimers_stack_size -= 1;
}

static void inline Timers_Force(int pType) {
    gTimers[gTimers_max_index].durations[gTimers[gTimers_max_index].index] +=
        PDGetMicroseconds() - gTimers[gTimers_max_index].start_time;
    gTimers_max_index = pType;
    gTimers[gTimers_max_index].start_time = PDGetMicroseconds();
}

#endif //REC2_TIMERS_H