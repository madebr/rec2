#include "c2_hooks.h"

#include "win32.h"
#include "win32net.h"

#include "errors.h"
#include "globvars.h"
#include "loading.h"
#include "main.h"

#include "rec2_macros.h"

#include <windows.h>

#include <string.h>



// GLOBAL: CARMA2_HW 0x006acea4
HINSTANCE gHInstance;

// GLOBAL: CARMA2_HW 0x006ace9c
int gNShowCmd;


// GLOBAL: CARMA2_HW 0x006621d0
const char* gCarma2WndClassName = "Carma2MainWndClass";;

// GLOBAL: CARMA2_HW 0x00662270
const char* gRegionDescriminator = "AUDIQUATTRO";

// FUNCTION: CARMA2_HW 0x0051aaa0
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
#if 0//defined(C2_HOOKS_ENABLED)
    return WinMain_original(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
#else
    char* currentArgument;
    WNDCLASSA wndCls;
    LANGID langId;
    const char* args[1];
    gAFE = 0;
    SetDefaultPedFolderNames();
    if (strlen(lpCmdLine) > 0) {
        _strupr(lpCmdLine);
        currentArgument = strtok(lpCmdLine, " \t\n");
        while (1) {
            if (currentArgument == NULL) {
                break;
            }
            if (strcmp(currentArgument, "-NOCUTSCENE") == 0 || strcmp(currentArgument, "-NOCUTSCENES") == 0) {
                gNoCutscenes = 1;
            } else if (strcmp(currentArgument, "-D3D") == 0) {
                gRenderer = "D3D";
            } else if (strcmp(currentArgument, "-D3D_REC2") == 0) {
                gRenderer = "D3D_REC2";
            } else if (strcmp(currentArgument, "-ZOMBIE") == 0) {
                ConfigureZombiePedTexturePath();
            } else if (strcmp(currentArgument, "-BLOOD") == 0) {
                ConfigurePedBloodPaths();
            } else if (strcmp(currentArgument, "-ALIEN") == 0) {
                ConfigurePedAlienPaths();
            } else if (strcmp(currentArgument, "-AFE") == 0) {
                gAFE = 1;
            } else if (strcmp(currentArgument, "-SCALEMOUSE") == 0) {
                gScaleMouse = 1;
            } else {
                dr_dprintf("Unknown argument: \"%s\"", currentArgument);
            }
            currentArgument = strtok(NULL, " \t\n");
        }
    }
    if (FindWindowA(gCarma2WndClassName, NULL) != NULL) {
        ExitProcess(703);
    }
    wndCls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    wndCls.lpfnWndProc = Carma2MainWndProc;
    wndCls.cbClsExtra = 0;
    wndCls.cbWndExtra = 0;
    wndCls.hInstance = hInstance; // gHInstance;
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
    if (strcmp(gRegionDescriminator, "AUDIQUATTRO") == 0 && PRIMARYLANGID(langId) == LANG_JAPANESE) {
        MessageBoxA(NULL, "Sorry, this is the Western European version of Carmageddon II - Carpocalypse Now.\n\nThis version can not run on this machine.\n",
                    "Carmageddon II - Carpocalypse Now", MB_ICONERROR);
        ExitProcess(704);
    }
    gScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    gScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    gHInstance = hInstance;
    gNShowCmd = nShowCmd;
    gPathNetworkIni[0] = '\0';
    if (GetCurrentDirectoryA(REC2_ASIZE(gPathNetworkIni), gPathNetworkIni) != 0) {
        strcat(gPathNetworkIni, "\\");
        strcat(gPathNetworkIni, "NETWORK.INI");
    }

    args[0] = "Carmageddon";

    GameMain(REC2_ASIZE(args), args);
#ifdef REC2_FIX_BUGS
    return 0;
#endif
#endif
}
