#ifndef REC2_SMASHING_H
#define REC2_SMASHING_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(tSmash_vertex, gSmash_vertices, 200);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tSmash_quad, gSmash_quads, 50);

void C2_HOOK_FASTCALL InitSmashing(void);

#endif //REC2_DEPTH_H
