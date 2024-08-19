#ifndef REC2_SKIDMARK_H
#define REC2_SKIDMARK_H

#include "rec2_types.h"

#include "c2_hooks.h"
#include "c2_stdio.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(char*, gBoring_material_names, 2);
C2_HOOK_VARIABLE_DECLARE_ARRAY(char*, gMaterial_names, 2);
C2_HOOK_VARIABLE_DECLARE_ARRAY(br_material*, gMaterial, 2);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tSkid, gSkids, 100);
C2_HOOK_VARIABLE_DECLARE(int, gCurrent_skid);

void C2_HOOK_FASTCALL InitSkids(void);

void C2_HOOK_FASTCALL HideSkid(int pSkid_num);

void C2_HOOK_FASTCALL HideSkids(void);

void C2_HOOK_FASTCALL ReadSpillData(FILE *pF,tSlick_spec *pSlick_spec);

void  C2_HOOK_FASTCALL AdjustSkid(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial);

void C2_HOOK_FASTCALL StretchMark(tSkid* pMark, br_vector3* pFrom, br_vector3* pTo, br_scalar pTexture_start, br_scalar pTexture_step);

int C2_HOOK_FASTCALL SkidSection(tS16* pSkid_id, br_vector3* pSkid_start, br_vector3* pSkid_end, br_material* pMaterial, br_vector3* pPos, br_vector3* pNorm, br_vector3* pPrev_pos, br_vector3* pPrev_norm, br_scalar pTexture_start, br_scalar pTexture_step);

#endif // REC2_SKIDMARK_H
