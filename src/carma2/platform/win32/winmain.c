#include "c2_hooks.h"

#include "win32.h"
#include "win32net.h"

#include "52-errors.h"
#include "globvars.h"
#include "08-loading1.h"
#include "40-main.h"
#include "61-pedestrn.h"
#include "platform.h"

#include "rec2_macros.h"

#include <windows.h>

#include "c2_string.h"
#include "win32_ssdx.h"

// GLOBAL: CARMA2_HW 0x006acea4
// GLOBAL: CARMA2_SW 0x006a6644
HINSTANCE gHInstance;

// GLOBAL: CARMA2_HW 0x006ace9c
// GLOBAL: CARMA2_SW 0x006a663c
int gNShowCmd;

// GLOBAL: CARMA2_HW 0x006ad484
// GLOBAL: CARMA2_SW 0x006a6c14
int gProgram_result;


// GLOBAL: CARMA2_HW 0x006621d0
// GLOBAL: CARMA2_SW 0x0065cac0
const char* gCarma2WndClassName = "Carma2MainWndClass";

// GLOBAL: CARMA2_HW 0x00662270
// GLOBAL: CARMA2_SW 0x0065cb58
const char* gRegionDescriminator = "AUDIQUATTRO";

// FUNCTION: CARMA2_HW 0x0051aaa0
// FUNCTION: CARMA2_SW 0x00514580
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    char* currentArgument;
    WNDCLASSA wndCls;
    int langId;
    const char* args[1];
    int lencwd;
#ifdef REC2_SW
    HDC hdc;
#endif

    args[0] = "Carmageddon";
#ifdef REC2_HW
    gAFE = 0;
    SetDefaultPedFolderNames();
    if (strlen(lpCmdLine) > 0) {
        _strupr(lpCmdLine);
        currentArgument = strtok(lpCmdLine, " \t\n");
        if (currentArgument != NULL) {
            while (1) {
                if (strcmp(currentArgument, "-NOCUTSCENE") == 0) {
                    gNoCutscenes = 1;
                } else if (strcmp(currentArgument, "-NOCUTSCENES") == 0) {
                    gNoCutscenes = 1;
                } else if (strcmp(currentArgument, "-D3D") == 0) {
                    gRenderer = "D3D";
                } else if (strcmp(currentArgument, "-ZOMBIE") == 0) {
                    SetZombiePedFolderNames();
                } else if (strcmp(currentArgument, "-BLOOD") == 0) {
                    SetBloodPedFolderNames();
                } else if (strcmp(currentArgument, "-ALIEN") == 0) {
                    SetAlienPedFolderNames();
                } else if (strcmp(currentArgument, "-AFE") == 0) {
                    gAFE = 1;
                } else if (strcmp(currentArgument, "-SCALEMOUSE") == 0) {
                    gScaleMouse = 1;
                }
#ifdef REC2_FIX_BUGS
                else {
                    dr_dprintf("Unknown argument: \"%s\"", currentArgument);
                }
#endif
                currentArgument = strtok(NULL, " \t\n");
                if (currentArgument == NULL) {
                    break;
                }
            }
        }
    }
#endif
    if (FindWindowA(gCarma2WndClassName, NULL) != NULL) {
        ExitProcess(703);
    }
    wndCls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    wndCls.lpfnWndProc = Carma2MainWndProc;
    wndCls.cbClsExtra = 0;
    wndCls.cbWndExtra = 0;
#ifdef REC2_FIX_BUGS
    wndCls.hInstance = hInstance;
#else
    wndCls.hInstance = gHInstance;
#endif
    wndCls.hIcon = LoadIconA(NULL, IDI_APPLICATION);
    wndCls.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wndCls.hbrBackground = GetStockObject(BLACK_BRUSH);
    wndCls.lpszMenuName = NULL;
    wndCls.lpszClassName = gCarma2WndClassName;
    RegisterClassA(&wndCls);
    gHWnd = CreateWindowExA(0, gCarma2WndClassName, "Carmageddon II", WS_POPUP | WS_VISIBLE,
                                 0, 0, 4, 4, NULL, NULL, gHInstance, NULL);
#ifdef REC2_SW
    SSDXWindowPosOrSizeChanged(gHWnd);
    hdc = GetDC(gHWnd);
    GetSystemPaletteEntries(hdc, 0, REC2_ASIZE(gPD_palette_entries), gPD_palette_entries);
    ReleaseDC(gHWnd, hdc);
#endif
    UpdateWindow(gHWnd);
    SetFocus(gHWnd);
    langId = GetSystemDefaultLangID();
    if (gRegionDescriminator[8] == 'T' && gRegionDescriminator[0] == 'A' &&
        gRegionDescriminator[2] == 'D' && gRegionDescriminator[4] == 'Q' &&
        gRegionDescriminator[1] == 'U' && gRegionDescriminator[6] == 'A' &&
        gRegionDescriminator[10] == 'O' && gRegionDescriminator[7] == 'T' &&
        gRegionDescriminator[3] == 'I' && gRegionDescriminator[5] == 'U' &&
        gRegionDescriminator[9] == 'R' && PRIMARYLANGID(langId) == LANG_JAPANESE) {
        MessageBoxA(NULL, "Sorry, this is the Western European version of Carmageddon II - Carpocalypse Now.\n\nThis version can not run on this machine.\n",
                    "Carmageddon II - Carpocalypse Now", MB_ICONERROR);
        ExitProcess(704);
    }

#ifdef REC2_SW
    gAFE = 0;
    gDefault_spec_index = 1;
    SetDefaultPedFolderNames();
    if (strlen(lpCmdLine) > 0) {
        _strupr(lpCmdLine);
        currentArgument = strtok(lpCmdLine, " \t\n");
        if (currentArgument != NULL) {
            while (1) {
                if (strcmp(currentArgument, "-NOCUTSCENE") == 0) {
                    gNoCutscenes = 1;
                } else if (strcmp(currentArgument, "-NOCUTSCENES") == 0) {
                    gNoCutscenes = 1;
                } else if (strcmp(currentArgument, "-ZOMBIE") == 0) {
                    SetZombiePedFolderNames();
                } else if (strcmp(currentArgument, "-BLOOD") == 0) {
                    SetBloodPedFolderNames();
                } else if (strcmp(currentArgument, "-ALIEN") == 0) {
                    SetAlienPedFolderNames();
                } else if (strcmp(currentArgument, "-AFE") == 0) {
                    gAFE = 1;
                }
#ifdef REC2_FIX_BUGS
                else {
                    dr_dprintf("Unknown argument: \"%s\"", currentArgument);
                }
#endif
                currentArgument = strtok(NULL, " \t\n");
                if (currentArgument == NULL) {
                    break;
                }
            }
        }
    }
#endif
#ifdef REC2_HW
    gScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    gScreenHeight = GetSystemMetrics(SM_CYSCREEN);
#endif
    gHInstance = hInstance;
    gNShowCmd = nShowCmd;
    gPathNetworkIni[0] = '\0';
    lencwd = GetCurrentDirectoryA(REC2_ASIZE(gPathNetworkIni), gPathNetworkIni);
    if (lencwd != 0 && strlen(gPathNetworkIni) == lencwd) {
        gPathNetworkIniValid = 1;
        strcat(gPathNetworkIni, "\\");
        strcat(gPathNetworkIni, "NETWORK.INI");
    }

    GameMain(REC2_ASIZE(args), args);
    return gProgram_result;
}
