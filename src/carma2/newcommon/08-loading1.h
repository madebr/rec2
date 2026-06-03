#ifndef GUARD_08_LOADING1_H
#define GUARD_08_LOADING1_H

#include "c2_hooks.h"
#include "rec2_types.h"

#include <stdio.h>

extern tU32 C2_HOOK_FASTCALL ReadU32(FILE* pF);

// ReadF32

// ReadS32

// WriteU32L

// WriteU16L

// WriteU8L

// MemReadU32

// MemReadU16

// MemReadU8

// MemReadS16

// MemReadS8

// MemSkipBytes

// GetALineAndInterpretCommand

// GetAnInt

// GetAFloat

// GetPairOfFloats

// GetThreeFloats

// GetAVector3

// GetPairOfInts

// GetThreeInts

// GetFourInts

// GetAScalar

// GetPairOfScalars

// GetThreeScalars

// GetFourScalars

// GetNScalars

// GetPairOfFloatPercents

// GetThreeFloatPercents

// GetAString

// StripCR

// SubsStringJob

extern char* C2_HOOK_FASTCALL GetALineWithNoPossibleService(FILE* pF, char* pS);

// GetALineAndDontArgue

extern FILE* OldDRfopen(const char* pFilename, const char* pMode);

extern void C2_HOOK_FASTCALL AllowOpenToFail(void);

extern void C2_HOOK_FASTCALL DoNotAllowOpenToFail(void);

// GetCDPathFromPathsTxtFile

extern FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);

// UnlockInterfaceStuff

// DisposePhysicsObject

// DisposeCarActModDataCB

// DisposeCar

// DisposeRaceInfo

// DisposeOpponentsCars

// DisposeRace

// DisposeGameIfNecessary

// DisposeTrack

extern int GetRegisterSourceLocation(char* pSource_location);

#endif // GUARD_08_LOADING1_H
