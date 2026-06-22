#ifndef GUARD_08_LOADING1_H
#define GUARD_08_LOADING1_H

#include "c2_hooks.h"
#include "rec2_types.h"

#include <stdio.h>

extern int gDecode_thing;

extern tU32 C2_HOOK_FASTCALL ReadU32(FILE* pF);

extern float C2_HOOK_FASTCALL ReadF32(FILE* pF);

extern tS32 C2_HOOK_FASTCALL ReadS32(FILE* pF);

extern void C2_HOOK_FASTCALL WriteU32L(FILE* pF, tU32 pNumber);

extern void C2_HOOK_FASTCALL WriteU16L(FILE* pF, tU16 pNumber);

extern void C2_HOOK_FASTCALL WriteU8L(FILE* pF, tU8 pNumber);

extern tU32 C2_HOOK_FASTCALL MemReadU32(char** pPtr);

extern tU16 C2_HOOK_FASTCALL MemReadU16(char** pPtr);

extern tU8 C2_HOOK_FASTCALL MemReadU8(char** pPtr);

extern tS16 C2_HOOK_FASTCALL MemReadS16(char** pPtr);

extern tS8 C2_HOOK_FASTCALL MemReadS8(char** pPtr);

extern void C2_HOOK_FASTCALL MemSkipBytes(char** pPtr, int pBytes_to_skip);

extern int C2_HOOK_FASTCALL GetALineAndInterpretCommand(FILE* pF, const char** pString_list, int pCount);

extern int C2_HOOK_FASTCALL GetAnInt(FILE* pF);

extern float C2_HOOK_FASTCALL GetAFloat(FILE* pF);

void C2_HOOK_FASTCALL GetPairOfFloats(FILE* pF, float* pF1, float* pF2);

extern void C2_HOOK_FASTCALL GetThreeFloats(FILE * pF, float* pF1, float* pF2, float* pF3);

extern void C2_HOOK_FASTCALL GetAVector3(FILE* pF, br_vector3* pV);

extern void C2_HOOK_FASTCALL GetPairOfInts(FILE* pF, int* pF1, int* pF2);

extern void C2_HOOK_FASTCALL GetThreeInts(FILE* pF, int* pF1, int* pF2, int* pF3);

extern void C2_HOOK_FASTCALL GetFourInts(FILE* pF, int* pF1, int* pF2, int* pF3, int* pF4);

extern br_scalar C2_HOOK_FASTCALL GetAScalar(FILE* pF);

extern void GetPairOfScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2);

extern void C2_HOOK_FASTCALL GetThreeScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2, br_scalar* pS3);

extern void C2_HOOK_FASTCALL GetFourScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2, br_scalar* pS3, br_scalar* pS4);

extern void C2_HOOK_FASTCALL GetNScalars(FILE* pF, int n, br_scalar* pS);

extern void C2_HOOK_FASTCALL GetPairOfFloatPercents(FILE* pF, float* pF1, float* pF2);

extern void C2_HOOK_FASTCALL GetThreeFloatPercents(FILE* pF, float* pF1, float* pF2, float* pF3);

extern void C2_HOOK_FASTCALL GetAString(FILE* pF, char* pString);

extern void C2_HOOK_FASTCALL StripCRNL(char* line);

extern void C2_HOOK_CDECL SubsStringJob(char *pFormat, ...);

extern void C2_HOOK_FASTCALL EncodeFile(char* pThe_path);

extern char* C2_HOOK_FASTCALL GetALineWithNoPossibleService(FILE* pF, char* pS);

extern char* C2_HOOK_FASTCALL GetALineAndDontArgue(FILE* pF, char* pS);

extern FILE* OldDRfopen(const char* pFilename, const char* pMode);

extern void C2_HOOK_FASTCALL AllowOpenToFail(void);

extern void C2_HOOK_FASTCALL DoNotAllowOpenToFail(void);

// GetCDPathFromPathsTxtFile

extern FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);

extern void C2_HOOK_FASTCALL UnlockInterfaceStuff(void);

// DisposePhysicsObject

// DisposeCarActModDataCB

// DisposeCar

// DisposeRaceInfo

// DisposeOpponentsCars

extern void C2_HOOK_FASTCALL DisposeRace(void);

// DisposeGameIfNecessary

// DisposeTrack

extern int GetRegisterSourceLocation(char* pSource_location);

#endif // GUARD_08_LOADING1_H
