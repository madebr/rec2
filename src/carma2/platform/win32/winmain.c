#include "c2_hooks.h"

#include "win32.h"
#include "win32net.h"

#include "52-errors.h"
#include "globvars.h"
#include "08-loading1.h"
#include "40-main.h"
#include "61-pedestrn.h"

#include "rec2_macros.h"

#include <windows.h>

#include "c2_string.h"



// GLOBAL: CARMA2_HW 0x006acea4
HINSTANCE gHInstance;

// GLOBAL: CARMA2_HW 0x006ace9c
int gNShowCmd;

// GLOBAL: CARMA2_HW 0x006ad484
int gProgram_result;


// GLOBAL: CARMA2_HW 0x006621d0
const char* gCarma2WndClassName = "Carma2MainWndClass";

// GLOBAL: CARMA2_HW 0x00662270
const char* gRegionDescriminator = "AUDIQUATTRO";

// FUNCTION: CARMA2_HW 0x0051aaa0
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    char* currentArgument;
    WNDCLASSA wndCls;
    int langId;
    const char* args[1];
    int lencwd;

    args[0] = "Carmageddon";
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
    gScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    gScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    gHInstance = hInstance;
    gNShowCmd = nShowCmd;
    gPathNetworkIni[0] = '\0';
    lencwd = GetCurrentDirectoryA(REC2_ASIZE(gPathNetworkIni), gPathNetworkIni);
    if (lencwd != 0 && lencwd == strlen(gPathNetworkIni)) {
        gPathNetworkIniValid = 1;
        strcat(gPathNetworkIni, "\\");
        strcat(gPathNetworkIni, "NETWORK.INI");
    }

    GameMain(REC2_ASIZE(args), args);
    return gProgram_result;
}
