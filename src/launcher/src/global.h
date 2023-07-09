#ifndef GLOBAL_H
#define GLOBAL_H

#include "types.h"

#include <d3d.h>
#include <windows.h>


extern char g_DrvGuid[16];
extern int g_DrvGuid_valid;
extern char g_DevGuid[16];
extern int g_Backend;
extern HWND g_hWnd;
extern HWND g_Dialog;
extern int g_blood;
extern HINSTANCE g_hInstance;
extern int g_close_requested;
extern int g_count_directdraw_displays;
extern Direct3DDisplayInfo g_DirectDraw_displays[4];
extern int g_Flags;
extern int g_selected_backend;
extern int g_Detection_state;
extern int g_current_d3d_display;
extern int g_current_d3d_device;
extern void* g_environment;
extern int g_D3D_prefered_device_is_hardware;
extern HRESULT (WINAPI* g_DirectDrawCreate)(GUID*, LPDIRECTDRAW*, IUnknown*);
extern HRESULT (WINAPI* g_DirectDrawEnumerateA)(LPDDENUMCALLBACKA, LPVOID);
extern int g_Config_loaded;
extern DWORD g_Flags_register;
extern int g_Detected_backends;
extern HANDLE g_ddraw_dll;
extern HANDLE g_glide2x_dll;
extern IDirectDraw2* g_DirectDraw2;

#endif /* GLOBAL_H */
