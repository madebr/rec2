#ifndef REC2_SPARK_H
#define REC2_SPARK_H

#include "c2_hooks.h"

void C2_HOOK_FASTCALL SetSmokeOn(int pSmoke_on);

int C2_HOOK_FASTCALL GetSmokeOn(void);

void C2_HOOK_FASTCALL GenerateSmokeShades(void);

#endif //REC2_SPARK_H
