#ifndef GUARD_07_STRUCTUR_H
#define GUARD_07_STRUCTUR_H

#include "c2_hooks.h"
#include "rec2_types.h"

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

extern void C2_HOOK_FASTCALL TotalRepair(void);

// DoLogos

// DoProgOpeningAnimation

// DoProgramDemo

// ChooseOpponent

// SelectOpponents

extern int C2_HOOK_FASTCALL PickNetRace(int pCurrent_race, tNet_sequence_type pNet_race_sequence);

// SwapNetCarsLoad

// SwapNetCarsDispose

// DoGame

// InitialiseProgramState

extern void C2_HOOK_FASTCALL DoProgram(void);

// JumpTheStart

// GoingToInterfaceFromRace

// GoingBackToRaceFromInterface

#endif // GUARD_07_STRUCTUR_H
