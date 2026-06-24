#ifndef GUARD_68_TIMER_H
#define GUARD_68_TIMER_H

#include "c2_hooks.h"
#include "rec2_types.h"

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

extern void C2_HOOK_FASTCALL Timers_Init(void);

// Timers_StartFrame

// Timers_EndFrame

#endif // GUARD_68_TIMER_H
