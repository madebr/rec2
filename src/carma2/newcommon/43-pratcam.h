#ifndef GUARD_43_PRATCAM_H
#define GUARD_43_PRATCAM_H

#include "c2_hooks.h"

extern void C2_HOOK_FASTCALL ChangeAmbientPratcam(int pIndex);

extern void C2_HOOK_FASTCALL PratcamEvent(int pIndex);

extern void C2_HOOK_FASTCALL InitPratcam(void);

extern void C2_HOOK_FASTCALL DisposePratcam(void);

#endif // GUARD_43_PRATCAM_H
