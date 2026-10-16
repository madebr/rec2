#ifndef GUARD_07_STRUCTUR_H
#define GUARD_07_STRUCTUR_H

#include "c2_hooks.h"

extern int gHud_tinted1;
extern int gHud_tinted2;
extern int gHud_tinted3;
extern int gCredits_checkpoint[3];

extern int C2_HOOK_FASTCALL NumberOfOpponentsLeft(void);

// RaceCompleted

extern void C2_HOOK_FASTCALL Checkpoint(int pCheckpoint_index, int pDo_sound);

// IncrementCheckpoint

// IncrementLap

// RayHitFace

// WrongCheckpoint

// CheckCheckpoints

// TotalRepair

// DoLogos

// DoProgOpeningAnimation

// DoProgramDemo

// ChooseOpponent

// SelectOpponents

// PickNetRace

// SwapNetCarsLoad

// SwapNetCarsDispose

// DoGame

// InitialiseProgramState

extern void C2_HOOK_FASTCALL DoProgram(void);

// JumpTheStart

// GoingToInterfaceFromRace

// GoingBackToRaceFromInterface

#endif // GUARD_07_STRUCTUR_H
