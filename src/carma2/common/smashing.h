#ifndef REC2_SMASHING_H
#define REC2_SMASHING_H

#include "rec2_types.h"

#include "c2_hooks.h"

#include "c2_stdio.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(tSmash_vertex, gSmash_vertices, 200);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tSmash_quad, gSmash_quads, 50);

void C2_HOOK_FASTCALL InitSmashing(void);

void C2_HOOK_FASTCALL ReadSmashableInitialPosition(FILE* pFile, tSmashable_initial_position_spec*  pInitial_pos);

#endif //REC2_DEPTH_H
