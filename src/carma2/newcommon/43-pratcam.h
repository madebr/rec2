#ifndef GUARD_43_PRATCAM_H
#define GUARD_43_PRATCAM_H

#include "c2_hooks.h"

// ChangeAmbientPratcam

extern void C2_HOOK_FASTCALL PratcamEvent(int pIndex);

// InitPratcam

extern void C2_HOOK_FASTCALL DisposePratcam(void);

#endif // GUARD_43_PRATCAM_H
