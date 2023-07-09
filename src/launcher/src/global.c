#include "global.h"

#include <windows.h>

char g_DrvGuid[16];
int g_DrvGuid_valid;
char g_DevGuid[16];
int g_Backend;
HWND g_hWnd;
HWND g_Dialog;
int g_blood;
HINSTANCE g_hInstance;
int g_close_requested;
int g_count_directdraw_displays;
Direct3DDisplayInfo g_DirectDraw_displays[4];
int g_Flags;
int g_selected_backend;
int g_Detection_state;
int g_current_d3d_display;
int g_current_d3d_device;
void* g_environment;
int g_D3D_prefered_device_is_hardware;
HRESULT (WINAPI* g_DirectDrawCreate)(GUID*, LPDIRECTDRAW*, IUnknown*);
HRESULT (WINAPI* g_DirectDrawEnumerateA)(LPDDENUMCALLBACKA, LPVOID);
int g_Config_loaded;
DWORD g_Flags_register;
int g_Detected_backends; // FIXME: g_Detected_drivers
HANDLE g_ddraw_dll;
HANDLE g_glide2x_dll;
IDirectDraw2* g_DirectDraw2;
