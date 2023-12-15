#ifndef REC2_PEDESTRN_H
#define REC2_PEDESTRN_H

#include "c2_hooks.h"

#include <brender/brender.h>

void C2_HOOK_FAKE_THISCALL ScaleModelXYZ(br_model* pModel, int pArg2, float pX, float pY, float pZ);

void C2_HOOK_CDECL InitPeds(void);

#endif //REC2_PEDESTRN_H
