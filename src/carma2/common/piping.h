#ifndef REC2_PIPING_H
#define REC2_PIPING_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tPipe_smudge_data*, gSmudge_space);
C2_HOOK_VARIABLE_DECLARE(tU8*, gPipe_play_ptr);

void C2_HOOK_FASTCALL DisposePiping(void);

void C2_HOOK_FASTCALL DisposeActionReplay(void);

void C2_HOOK_FASTCALL InitLastDamageArrayEtc(void);

void C2_HOOK_FASTCALL PipeSingleSkidAdjustment(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial);

void C2_HOOK_FASTCALL PipeCarPositions(void);

tU8* C2_HOOK_FASTCALL ARGetPipePlayPtr(void);

#endif //REC2_PIPING_H
