#ifndef REC2_SKIDMARK_H
#define REC2_SKIDMARK_H

#include "rec2_types.h"

#include "c2_hooks.h"
#include "c2_stdio.h"

void C2_HOOK_FASTCALL InitSkids(void);

void C2_HOOK_FASTCALL ReadSlick(FILE *pF,tSlick_spec *pSlick_spec);

#endif // REC2_SKIDMARK_H
