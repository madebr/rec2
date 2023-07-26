#ifndef REC2_DISPLAYS_H
#define REC2_DISPLAYS_H

#include "c2_hooks.h"

#include "rec2_types.h"

int C2_HOOK_FASTCALL DRTextWidth(const tDR_font* pFont, const char* pText);

void C2_HOOK_FASTCALL TransDRPixelmapText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge);

void C2_HOOK_FASTCALL InitHeadups(void);

int C2_HOOK_FASTCALL MungeHeadupWidth(tHeadup* pHeadup);

void C2_HOOK_FASTCALL KillOldestQueuedHeadup(void);

void C2_HOOK_FASTCALL ClearQueuedHeadups(void);

#endif //REC2_DISPLAYS_H
