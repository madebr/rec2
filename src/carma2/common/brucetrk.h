#ifndef REC2_BRUCETRK_H
#define REC2_BRUCETRK_H

#include "c2_hooks.h"
#include <brender/brender.h>

br_scalar C2_HOOK_STDCALL GetYonFactor(void);

void C2_HOOK_STDCALL SetYonFactor(br_scalar pNew);

#endif //REC2_BRUCETRK_H
