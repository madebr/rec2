#ifndef C2_PLATFORM_H
#define C2_PLATFORM_H

#include "c2_hooks.h"

#include "rec2_types.h"
#include <s3/s3.h>

extern int gGraf_spec_index;
extern int gNbPixelBits;
extern tGraf_spec gGraf_specs[2];
extern int gMsg_header_strlen;
extern tPD_S3_config gPD_S3_config;
extern int gJoystick_deadzone;
extern void* gHWnd;
extern void* gPDActionReplayBuffer;
extern int gPDActionReplayBufferSize;

extern int PDGetRegisterSourceLocation(char* buffer, int* buffer_size);

extern int C2_HOOK_FASTCALL PDServiceSystem(tU32 pTime_since_last_call);

extern void C2_HOOK_FASTCALL PDInitialiseSystem(void);

extern void C2_HOOK_FASTCALL PDBuildAppPath(char* pThe_path);
extern void C2_HOOK_FASTCALL PDEnterDebugger(const char* pStr);
extern void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str);
extern void C2_HOOK_FASTCALL PDNonFatalError(const char* pThe_str);
extern int C2_HOOK_FASTCALL PDIsWindowInactive(void);
extern char C2_HOOK_FASTCALL PDConvertToASCIILessThan128(char pChar);
extern int C2_HOOK_FASTCALL PDGetKeyboardCharacter(void);
extern void C2_HOOK_FASTCALL PDClearKeyboardBuffer(void);
extern void C2_HOOK_FASTCALL PDSetKeyArray(int* pKeys, int pMark);
extern void C2_HOOK_FASTCALL PDSetFileVariables(void);
extern C2_NORETURN void C2_HOOK_FASTCALL PDShutdownSystem(void);
extern void C2_HOOK_FASTCALL PDUnlockRealBackScreen(void);

extern int C2_HOOK_FASTCALL PDCheckDriveExists2(const char* pThe_path, const char* pFile_name, tU32 pMin_size);

extern int C2_HOOK_FASTCALL PDReadSourceLocation(tPath_name pPath);

extern int C2_HOOK_FASTCALL PDDoWeLeadAnAustereExistance(void);

extern void C2_HOOK_FASTCALL PDInitTimer(void);

extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);

extern int C2_HOOK_FASTCALL PDGetMicroseconds(void);

extern void C2_HOOK_FASTCALL PDScreenBufferSwap(int pRendering_area_only);

extern void C2_HOOK_FASTCALL PDSetPaletteEntries(br_pixelmap* pPalette, int pFirst_colour, int pCount);

extern void C2_HOOK_FASTCALL PDSetPalette(br_pixelmap *pixelmap);

extern void C2_HOOK_FASTCALL PDEnumPath(const char* path, tEnumPathCallback pCallback, void* data);

extern int C2_HOOK_FASTCALL PDmkdir(const char* path);

extern void C2_HOOK_FASTCALL MAMSUnlock(void** pPtr);

extern void C2_HOOK_FASTCALL MAMSLock(void** pPtr);

extern void C2_HOOK_FASTCALL PDForEveryFileRecurse(const char* pThe_path, tPDForEveryFileRecurse_cbfn pAction_routine);

extern void C2_HOOK_FASTCALL PDForEveryFile(const char* pThe_path, tPDForEveryFile_cbfn pAction_routine);

extern void C2_HOOK_FASTCALL PDGetCurrentDate(char* pTimeStr);

extern void C2_HOOK_FASTCALL PDGetCurrentTime(char* pTimeStr);

extern void C2_HOOK_FASTCALL PDFileDelete(const char* pPath, int pIgnore_read_only);

extern int C2_HOOK_FASTCALL PDGetMouseClickPosition(int* pX_coord, int* pY_coord);

extern void C2_HOOK_FASTCALL PDInstallErrorHandlers(void);

extern int C2_HOOK_FASTCALL PDInitScreenVars(int pArgc, const char** pArgv);

extern void C2_HOOK_FASTCALL PDSaveOriginalPalette(void);

extern void C2_HOOK_FASTCALL PDInitScreen(void);

extern void C2_HOOK_FASTCALL PDNetObtainSystemUserName(char* pName, int pMax_length);

extern int C2_HOOK_FASTCALL PDNetShutdown(void);

extern int C2_HOOK_FASTCALL PDNetInitialise(void);

extern void C2_HOOK_FASTCALL PDNetStartProducingJoinList(void);

extern void C2_HOOK_FASTCALL PDNetEndJoinList(void);

extern int C2_HOOK_FASTCALL PDNetGetNextJoinGame(tNet_game_details* pGame, int pIndex);

extern int C2_HOOK_FASTCALL PDNetGetHeaderSize(void);

extern int C2_HOOK_FASTCALL PDNetJoinGame(tNet_game_details* pGame_details, const char* pName);

extern void C2_HOOK_FASTCALL PDNetHostFinishGame(void);

extern void C2_HOOK_FASTCALL PDNetLeaveGame(void);

extern tNet_message* C2_HOOK_FASTCALL PDNetGetNextMessage(tNet_game_details* pDetails, void** gRemote_addr);

extern int C2_HOOK_FASTCALL PDNetSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress);

extern int C2_HOOK_FASTCALL PDFileUnlock(const char* pThe_path);

extern void C2_HOOK_FASTCALL PDAllocateScreenAndBack(void);

extern void C2_HOOK_FASTCALL PDGetMousePosition(int *pX, int *pY);

extern void C2_HOOK_FASTCALL PDRevertPalette(void);

extern void C2_HOOK_FASTCALL PDMouseButtons(int* pLeftButtonDown, int* pRightButtonDown);

extern tU32 C2_HOOK_FASTCALL PDGetJoystickButtonStates(void);

extern void C2_HOOK_FASTCALL PDSetKeysFromJoystick(int *keys);

extern void C2_HOOK_FASTCALL PDReadJoysticks(void);

extern void C2_HOOK_FASTCALL PDInitJoysticks(void);

extern void C2_HOOK_FASTCALL PDPlayFFBEffectIndex(int index);

extern int C2_HOOK_FASTCALL PDFindJoystickEffect(const char* effectName);

extern void C2_HOOK_FASTCALL PDPlayFFBEffect(const char* effectName);

extern tU32 C2_HOOK_FASTCALL PDGetJoy1Button1(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy1Button2(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy1Button3(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy1Button4(void);

extern int C2_HOOK_FASTCALL PDGetJoy1X(void);

extern int C2_HOOK_FASTCALL PDGetJoy1Y(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy2Button1(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy2Button2(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy2Button3(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy2Button4(void);

extern int C2_HOOK_FASTCALL PDGetJoy2X(void);

extern int C2_HOOK_FASTCALL PDGetJoy2Y(void);

extern tButtonJoystickInfo* C2_HOOK_FASTCALL PDGetCurrentJoystickData(void);

extern int C2_HOOK_FASTCALL PDS3Init(void);

extern void C2_HOOK_FASTCALL PDExtractFilename(char* pDest, const char* pPath);

extern void C2_HOOK_FASTCALL PDExtractDirectory(char* pDest, const char* pPath);

extern void* C2_HOOK_FASTCALL PDS3CreateSoundBuffer(tS3_wav_info* pWav_info, void* pWav_buffer);

extern void C2_HOOK_FASTCALL PDS3ReleaseSound(tS3_descriptor* pSound_descriptor);

extern int C2_HOOK_FASTCALL PDS3UpdateChannelFrequency(tS3_channel* pChannel);

extern int C2_HOOK_FASTCALL PDS3PlaySample(tS3_channel* pChannel);

extern tS3_error_codes C2_HOOK_FASTCALL PDS3StartMidiChannel(tS3_channel* pChannel);

extern const char* C2_HOOK_FASTCALL PDS3GetWorkingDirectory(void);

extern int C2_HOOK_FASTCALL PDS3StopMidiChannel(tS3_channel* pChannel);

extern int C2_HOOK_FASTCALL PDS3StopCDAChannel(tS3_channel* pChannel);

extern tS3_error_codes C2_HOOK_FASTCALL PDS3PlayCDAChannel(tS3_channel* pChannel);

extern int C2_HOOK_FASTCALL PDS3StopSampleChannel(tS3_channel* pChannel);

extern int C2_HOOK_FASTCALL PDS3UpdateChannelVolume(tS3_channel* pChannel);

extern void C2_HOOK_FASTCALL PDS3Stop(void);

extern int C2_HOOK_FASTCALL PDS3IsMIDIStopped(tS3_channel* pChannel);

extern int C2_HOOK_FASTCALL PDS3IsCDAPlaying(void);

extern void C2_HOOK_FASTCALL PDS3ServiceCDA(tS32 pDelta_time);

extern int C2_HOOK_FASTCALL PDS3IsSamplePlaying(tS3_channel* pChannel);

extern void C2_HOOK_FASTCALL PDPageInProcessMemory(void);

extern int C2_HOOK_FASTCALL PDIsJoystickDPadEnabled(void);

extern void C2_HOOK_FASTCALL PDReallyAllocateActionReplayBuffer(void);

#endif // C2_PLATFORM_H
