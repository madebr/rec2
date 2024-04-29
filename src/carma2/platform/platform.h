#ifndef C2_PLATFORM_H
#define C2_PLATFORM_H

#include "c2_hooks.h"

#include "rec2_macros.h"
#include "rec2_types.h"
#include <s3/s3.h>

C2_HOOK_VARIABLE_DECLARE(int, gGraf_spec_index);
C2_HOOK_VARIABLE_DECLARE(int, gNbPixelBits);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tGraf_spec, gGraf_specs, 2);
C2_HOOK_VARIABLE_DECLARE(int, gMsg_header_strlen);
C2_HOOK_VARIABLE_DECLARE(tPD_S3_config, gPD_S3_config);

int GetRegisterSourceLocation(char* buffer, int* buffer_size);

int C2_HOOK_FASTCALL PDServiceSystem(tU32 pTime_since_last_call);

void C2_HOOK_FASTCALL PDInitialiseSystem(void);

void C2_HOOK_FASTCALL PDBuildAppPath(char* pThe_path);
void C2_HOOK_FASTCALL PDEnterDebugger(const char* pStr);
void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str);
void C2_HOOK_FASTCALL PDNonFatalError(char* pThe_str);
int C2_HOOK_FASTCALL PDIsWindowActive(void);
char C2_HOOK_FASTCALL PDConvertToASCIILessThan128(char pChar);
void C2_HOOK_FASTCALL PDSetFileVariables(void);
C2_NORETURN void C2_HOOK_FASTCALL PDShutdownSystem(void);
void C2_HOOK_FASTCALL PDUnlockRealBackScreen(void);

int C2_HOOK_FASTCALL PDCheckDriveExists2(const char* pThe_path, const char* pFile_name, tU32 pMin_size);

int C2_HOOK_FASTCALL PDReadSourceLocation(tPath_name pPath);

int C2_HOOK_FASTCALL PDDoWeLeadAnAustereExistance(void);

void C2_HOOK_FASTCALL PDInitTimer(void);

int C2_HOOK_FASTCALL PDGetTotalTime(void);

int C2_HOOK_FASTCALL PDGetTotalMicroTime(void);

void C2_HOOK_FASTCALL PDScreenBufferSwap(int pRendering_area_only);

void C2_HOOK_FASTCALL PDSetPaletteEntries(br_pixelmap* pPalette, int pFirst_colour, int pCount);

void C2_HOOK_FASTCALL PDSetPalette(br_pixelmap *pixelmap);

void C2_HOOK_FASTCALL PDEnumPath(const char* path, tEnumPathCallback pCallback, void* data);

int C2_HOOK_FASTCALL PDmkdir(const char* path);

void C2_HOOK_FASTCALL MAMSUnlock(void** pPtr);

void C2_HOOK_FASTCALL MAMSLock(void** pPtr);

void C2_HOOK_FASTCALL PDForEveryFileRecurse(const char* pThe_path, tPDForEveryFileRecurse_cbfn pAction_routine);

void C2_HOOK_FASTCALL PDForEveryFile(const char* pThe_path, tPDForEveryFile_cbfn pAction_routine);

void C2_HOOK_FASTCALL PDGetFormattedDate(char* pTimeStr);

void C2_HOOK_FASTCALL PDGetFormattedTime(char* pTimeStr);

int C2_HOOK_FASTCALL PDGetMouseClickPosition(int* pX_coord, int* pY_coord);

void C2_HOOK_FASTCALL PDInstallErrorHandlers(void);

void C2_HOOK_FASTCALL InstallDRFileCalls(void);

int C2_HOOK_FASTCALL PDInitScreenVars(int pArgc, const char** pArgv);

void C2_HOOK_FASTCALL PDSaveOriginalPalette(void);

void C2_HOOK_FASTCALL PDInitScreen(void);

void C2_HOOK_FASTCALL PDNetObtainSystemUserName(char* pName, int pMax_length);

int C2_HOOK_FASTCALL PDNetShutdown(void);

int C2_HOOK_FASTCALL PDNetInitialise(void);

void C2_HOOK_FASTCALL PDNetStartProducingJoinList(void);

void C2_HOOK_FASTCALL PDNetEndJoinList(void);

int C2_HOOK_FASTCALL PDNetGetNextJoinGame(tNet_game_details* pGame, int pIndex);

int C2_HOOK_FASTCALL PDFileUnlock(char* pThe_path);

void C2_HOOK_FASTCALL PDAllocateScreenAndBack(void);

void C2_HOOK_FASTCALL PDGetMousePosition(int *pX, int *pY);

void C2_HOOK_FASTCALL PDRevertPalette(void);

void C2_HOOK_FASTCALL PDMouseButtons(int* pLeftButtonDown, int* pRightButtonDown);

int C2_HOOK_FASTCALL PDS3Init(void);

void C2_HOOK_FASTCALL PDExtractFilename(char* pDest, const char* pPath);

void C2_HOOK_FASTCALL PDExtractDirectory(char* pDest, const char* pPath);

void* C2_HOOK_FASTCALL PDS3CreateSoundBuffer(tS3_wav_info* pWav_info, void* pWav_buffer);

void C2_HOOK_FASTCALL PDS3ReleaseSound(tS3_descriptor* pSound_descriptor);

char* C2_HOOK_FASTCALL PDS3GetWorkingDirectory(void);

int C2_HOOK_FASTCALL PDS3StopMidiChannel(tS3_channel* pChannel);

int C2_HOOK_FASTCALL PDS3StopCDAChannel(tS3_channel* pChannel);

int C2_HOOK_FASTCALL PDS3StopSampleChannel(tS3_channel* pChannel);

#endif // C2_PLATFORM_H
