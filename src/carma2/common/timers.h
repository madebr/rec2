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

C2_HOOK_VARIABLE_DECLARE(int, gTimers_stack_size);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tTimer, gTimers, 19);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gTimers_stack, 19);
C2_HOOK_VARIABLE_DECLARE(int, gTimers_frame_count);
C2_HOOK_VARIABLE_DECLARE(int, gTimers_enough_samples);
C2_HOOK_VARIABLE_DECLARE(int, gTimers_max_index);
C2_HOOK_VARIABLE_DECLARE(int, gTimers_draw_x);
C2_HOOK_VARIABLE_DECLARE(int, gTimers_draw_y_stride);
C2_HOOK_VARIABLE_DECLARE(int, gTimers_draw_y);
C2_HOOK_VARIABLE_DECLARE(tU32, gTimers_frame_start_time);
C2_HOOK_VARIABLE_DECLARE(tU32, gTimers_frame_end_time);
C2_HOOK_VARIABLE_DECLARE(tU32, gTimers_tolerance);

void C2_HOOK_FASTCALL Timers_Init(void);

void C2_HOOK_FASTCALL Timers_StartFrame(void);

void C2_HOOK_FASTCALL Timers_EndFrame(void);

void C2_HOOK_FASTCALL Timers_Draw(br_pixelmap* pScreen);

static void inline Timers_Push(int pType) {
    int prev_type;

    C2V(gTimers_stack)[C2V(gTimers_stack_size)] = pType;
    if (C2V(gTimers_stack_size) == 0) {
        prev_type = C2V(gTimers_max_index);
    } else {
        prev_type = C2V(gTimers_stack)[C2V(gTimers_stack_size) - 1];
    }
    C2V(gTimers)[prev_type].durations[C2V(gTimers)[prev_type].index] += PDGetMicroseconds() - C2V(gTimers)[prev_type].start_time;
    C2V(gTimers)[pType].start_time = PDGetMicroseconds();
    C2V(gTimers_stack_size) += 1;
}

static void inline Timers_Pop(int pType) {
    int prev_type;

    C2V(gTimers)[pType].durations[C2V(gTimers)[pType].index] +=PDGetMicroseconds() - C2V(gTimers)[pType].start_time;
    if (C2V(gTimers_stack_size) > 1) {
        prev_type = C2V(gTimers_stack_size) - 1;
    } else {
        prev_type = C2V(gTimers_max_index);
    }
    C2V(gTimers)[prev_type].start_time = PDGetMicroseconds();
    C2V(gTimers_stack_size) -= 1;
}

static void inline Timers_Force(int pType) {
    C2V(gTimers)[C2V(gTimers_max_index)].durations[C2V(gTimers)[C2V(gTimers_max_index)].index] +=
        PDGetMicroseconds() - C2V(gTimers)[C2V(gTimers_max_index)].start_time;
    C2V(gTimers_max_index) = pType;
    C2V(gTimers)[C2V(gTimers_max_index)].start_time = PDGetMicroseconds();
}

#endif //REC2_TIMERS_H
