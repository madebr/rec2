#ifndef REC2_SDL3_H
#define REC2_SDL3_H

#include "rec2_types.h"

#include <SDL3/SDL.h>

typedef struct {
    SDL_JoystickID which;
    Uint64 buttons;
    Sint16 axis_motions[SDL_GAMEPAD_AXIS_COUNT];
} tJoystick_info;

extern Uint64 g_PerformanceFrequency;
extern SDL_Window *g_SDL_Window;
extern char gPathNetworkIni[256];
extern int gScaleMouse;
extern tJoystick_info g_Joystick_infos[16];
extern int g_Count_Joystick_infos;

extern void Win32ServiceMessages(void);

extern void PDInitialiseSystem(void);

extern void PDSetFileVariables(void);

extern void PDBuildAppPath(char* pThe_path);

extern void PDFatalError(const char *message);

extern void PDNonFatalError(const char* pThe_str);

extern int PDIsWindowInactive(void);

extern char PDConvertToASCIILessThan128(char pChar);

extern int PDGetKeyboardCharacter(void);

extern void PDClearKeyboardBuffer(void);

C2_NORETURN void PDShutdownSystem(void);

extern void PDLockRealBackScreen(void);

extern void PDUnlockRealBackScreen(void);

extern int PDGetASCIIFromKey(int pKey);

extern int PDGetASCIIFromKey(int pKey);

extern int PDCheckDriveExists2(const char *pThe_path, const char *pFile_name, tU32 pMin_size);

extern int C2_HOOK_FASTCALL PDReadSourceLocation(tPath_name pPath);

extern int PDDoWeLeadAnAustereExistance(void);

extern void PDSetPaletteEntries(br_pixelmap *pPalette, int pFirst_colour, int pCount);

extern void PDSetPalette(br_pixelmap *pixelmap);

extern int PDServiceSystem(tU32 pTime_since_last_call);

extern void PDScreenBufferSwap(int pRendering_area_only);

extern void PDInitTimer(void);

extern tU32 PDGetTotalTime(void);

extern int PDGetMicroseconds(void);

extern void PDEnterDebugger(const char *pStr);

extern int PDFileUnlock(const char *pThe_path);

extern void PDEnumPath(const char* path, tEnumPathCallback pCallback, void* data);

extern int PDmkdir(const char *path);

extern void MAMSUnlock(void **pPtr);

extern void MAMSLock(void **pPtr);

extern void PDForEveryFileRecurse(const char* pThe_path, tPDForEveryFileRecurse_cbfn pAction_routine);

extern void PDForEveryFile(const char *pThe_path, tPDForEveryFile_cbfn pAction_routine);

extern void PDGetCurrentDate(char *pTimeStr);

extern void PDGetCurrentTime(char *pTimeStr);

extern int PDGetMouseClickPosition(int *pX_coord, int *pY_coord);

extern void PDInstallErrorHandlers(void);

extern int PDInitScreenVars(int pArgc, const char **pArgv);

extern void PDSaveOriginalPalette(void);

extern void PDInitScreen(void);

extern void PDAllocateScreenAndBack(void);

extern int GetRegisterSourceLocation(char *buffer, int *buffer_size);

extern void PDGetMousePosition(int *pX, int *pY);

extern void PDRevertPalette(void);

extern void PDMouseButtons(int *pLeftButtonDown, int *pRightButtonDown);

extern void PDExtractFilename(char *pDest, const char *pPath);

extern void PDExtractDirectory(char *pDest, const char *pPath);

extern void PDPageInProcessMemory(void);

#endif /* REC2_SDL3_H */
