#ifndef SOFTREND_SUBDIV_H
#define SOFTREND_SUBDIV_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_CDECL averageVerticesOnScreen(br_soft_renderer* renderer, brp_vertex* dest1, brp_vertex* dest2, brp_vertex* dest3, brp_vertex* src1, brp_vertex* src2, brp_vertex* src3);

#endif /* SOFTREND_SUBDIV_H */
