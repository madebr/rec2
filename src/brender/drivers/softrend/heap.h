#ifndef SOFREND_HEAP_H
#define SOFREND_HEAP_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_CDECL OpHeapAddTriangle(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2);

#endif /* SOFREND_HEAP_H */
