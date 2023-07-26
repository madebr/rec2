#ifndef REC2_TIMERS_H
#define REC2_TIMERS_H

#include "rec2_types.h"

#include "c2_hooks.h"

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
C2_HOOK_VARIABLE_DECLARE(int, gTimers_frame_count);
C2_HOOK_VARIABLE_DECLARE(int, gTimers_enough_samples);
C2_HOOK_VARIABLE_DECLARE(int, gTimers_max_index);
C2_HOOK_VARIABLE_DECLARE(int, gTimers_draw_x);
C2_HOOK_VARIABLE_DECLARE(int, gTimers_draw_y_stride);
C2_HOOK_VARIABLE_DECLARE(int, gTimers_draw_y);
C2_HOOK_VARIABLE_DECLARE(tU32, gTimers_frame_start_time);
C2_HOOK_VARIABLE_DECLARE(tU32, gTimers_frame_end_time);
C2_HOOK_VARIABLE_DECLARE(tU32, gTimers_tolerance);

void C2_HOOK_FASTCALL InitTimers(void);

void C2_HOOK_FASTCALL Timers_StartFrame(void);

void C2_HOOK_FASTCALL Timers_EndFrame(void);

void C2_HOOK_FASTCALL Timers_Draw(br_pixelmap* pScreen);

#endif //REC2_TIMERS_H
