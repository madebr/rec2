#ifndef REC2_PIPING_H
#define REC2_PIPING_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tPipe_smudge_data*, gSmudge_space);

void C2_HOOK_FASTCALL DisposePiping(void);

void C2_HOOK_FASTCALL DisposeReplay(void);

void C2_HOOK_FASTCALL InitLastDamageArrayEtc(void);

#endif //REC2_PIPING_H
