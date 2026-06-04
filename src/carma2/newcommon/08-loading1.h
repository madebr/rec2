#ifndef GUARD_08_LOADING1_H
#define GUARD_08_LOADING1_H

#include "c2_hooks.h"
#include "rec2_types.h"

#include <stdio.h>

extern int gDecode_thing;

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

extern int C2_HOOK_FASTCALL GetAnInt(FILE* pF);

extern float C2_HOOK_FASTCALL GetAFloat(FILE* pF);

// GetPairOfFloats

extern void C2_HOOK_FASTCALL GetThreeFloats(FILE * pF, float* pF1, float* pF2, float* pF3);

// GetAVector3

// GetPairOfInts

extern void C2_HOOK_FASTCALL GetThreeInts(FILE* pF, int* pF1, int* pF2, int* pF3);

// GetFourInts

extern br_scalar C2_HOOK_FASTCALL GetAScalar(FILE* pF);

// GetPairOfScalars

// GetThreeScalars

// GetFourScalars

// GetNScalars

// GetPairOfFloatPercents

// GetThreeFloatPercents

extern void C2_HOOK_FASTCALL GetAString(FILE* pF, char* pString);

// StripCR

// SubsStringJob

char* C2_HOOK_FASTCALL GetALineWithNoPossibleService(FILE* pF, char* pS);

char* C2_HOOK_FASTCALL GetALineAndDontArgue(FILE* pF, char* pS);

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
