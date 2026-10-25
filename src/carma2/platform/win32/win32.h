#ifndef C2_WIN32_WIN32_H
#define C2_WIN32_WIN32_H

#include "c2_hooks.h"

#include "rec2_types.h"

#include <brender/brender.h>

#include <windows.h>
#include <dinput.h>

extern int gExit_code;

extern void* gHWnd;
extern int gWindowActiveState;  // FIXME: enum (0,1) => background(d3d or glide), 2=> active

extern int gWindowMovingResizing;
extern LPDIRECTINPUTDEVICEA gDirectInputDevice;
#ifdef REC2_HW
extern char* gRenderer;
#endif
extern int gMouseLButtonDown;
extern int gMouseRButtonDown;
extern int gMouseCaptured;
extern POINT gCursorPos_LastClick;
extern POINT gPD_mouse_position;
extern int gCursorPos_LastClick_Valid;
extern int gScaleMouse;
extern int gScreenWidth;
extern int gScreenHeight;
extern FILE *gActionReplayBufferFile;

extern int gKeyboardBufferLength;
extern unsigned char gKeyboardBuffer[20];

extern int gPerformanceCounterInitialized;
extern LARGE_INTEGER gPerformanceCounterStart;
extern LARGE_INTEGER gPerformanceCounterFrequency_s;
extern LARGE_INTEGER gPerformanceCounterFrequency_ms;
extern LARGE_INTEGER gPerformanceCounterFrequency_us;

extern tU32 gTime_app_activated;

extern HINSTANCE gHInstance;

extern char gFatalErrorMessage[512];
extern int gIsFatalError;

extern LRESULT CALLBACK Carma2MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

extern int C2_HOOK_FASTCALL PDFileUnlock(const char* pThe_path);

extern void C2_HOOK_FASTCALL PDRevertPalette(void);

extern void C2_HOOK_FASTCALL PDMouseButtons(int* pLeftButtonDown, int* pRightButtonDown);

extern void C2_HOOK_FASTCALL PDExtractFilename(char* pDest, const char* pPath);

extern void C2_HOOK_FASTCALL PDExtractDirectory(char* pDest, const char* pPath);

extern void C2_HOOK_FASTCALL PDPageInMemory(void* pMemory);

extern void C2_HOOK_FASTCALL PDPageInProcessMemory(void);

extern void C2_HOOK_FASTCALL PDGetCurrentTime(char* pStr);

extern void C2_HOOK_FASTCALL PDGetCurrentDate(char* pStr);

extern void C2_HOOK_FASTCALL PDFileDelete(const char* pPath, int pIgnore_read_only);

extern void C2_HOOK_FASTCALL PDDisposeActionReplayBuffer(void);

extern int C2_HOOK_FASTCALL PDmemicmp(const char* str1, const char* str2, size_t count);

#ifdef REC2_SW
extern void C2_HOOK_FASTCALL SSDXStartDirectDraw(void);

extern void C2_HOOK_FASTCALL SSDXResetSurface(void);

extern void C2_HOOK_FASTCALL PDSwitchToHiresMode(void);

extern void C2_HOOK_FASTCALL PDSwitchToLoresMode(void);

extern void C2_HOOK_FASTCALL FUN_005171c0(void);

extern void C2_HOOK_FASTCALL PDUnlockAttachedSurface(void);

extern void C2_HOOK_FASTCALL PDScreenBufferFlip(void);

extern void C2_HOOK_FASTCALL PDPixelmapRectangleCopy(br_pixelmap* pDest, int pDst_x, int pDst_y, br_pixelmap* pSrc, int pSrc_x, int pSrc_y, int width, int height);

extern void C2_HOOK_FASTCALL DPixelmapLine(br_pixelmap* pDst, int pX1, int pY1, int pX2, int pY2, br_colour pColour);

extern void C2_HOOK_FASTCALL DPixelmapLine2(br_pixelmap* pDst, int pX1, int pY1, int pX2, int pY2, br_colour pColour);

#endif

#endif // C2_WIN32_WIN32_H
