#ifndef REC2_SKIDMARK_H
#define REC2_SKIDMARK_H

#include "rec2_types.h"

#include "c2_hooks.h"
#include "c2_stdio.h"

void C2_HOOK_FASTCALL InitSkids(void);

void C2_HOOK_FASTCALL HideSkid(int pSkid_num);

void C2_HOOK_FASTCALL HideSkids(void);

void C2_HOOK_FASTCALL ReadSlick(FILE *pF,tSlick_spec *pSlick_spec);

void  C2_HOOK_FASTCALL AdjustSkid(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial);

#endif // REC2_SKIDMARK_H
