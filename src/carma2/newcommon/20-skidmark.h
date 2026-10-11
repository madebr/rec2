#ifndef GUARD_20_SKIDMARK_H
#define GUARD_20_SKIDMARK_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern char* gBoring_material_names[2];
extern char* gMaterial_names[2];
extern br_material* gMaterial[2];
extern tSkid gSkids[100];
extern int gCurrent_skid;

// StretchMark

// MaterialFromIndex

// AdjustSkid

// FarFromLine2D

// Reflex2D

extern void C2_HOOK_FASTCALL InitSkids(void);

extern void C2_HOOK_FASTCALL HideSkid(int pSkid_num);

extern void C2_HOOK_FASTCALL HideSkids(void);

// SkidLen

// UglyWidthChange

// GenericSkidSection

// WheelSkidSection

// SkidMark

// InitCarSkidStuff

// SkidsPerFrame

#endif // GUARD_20_SKIDMARK_H
