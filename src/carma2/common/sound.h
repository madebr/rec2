#ifndef REC2_SOUND_H
#define REC2_SOUND_H

#include "c2_hooks.h"


C2_HOOK_VARIABLE_DECLARE(int, gCD_fully_installed);

void C2_HOOK_FASTCALL UsePathFileToDetermineIfFullInstallation(void);

#endif // REC2_SOUND_H
