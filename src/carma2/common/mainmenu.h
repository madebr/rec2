#ifndef REC2_MAINMENU_H
#define REC2_MAINMENU_H

#include "rec2_types.h"

#include "c2_hooks.h"

int C2_HOOK_FASTCALL DoVerifyQuit(int pReplace_background);

void C2_HOOK_FASTCALL DoMainScreen(void);

void C2_HOOK_FASTCALL DoOptionsMenu(void);

#endif //REC2_MAINMENU_H
