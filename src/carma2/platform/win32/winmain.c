#include "c2_hooks.h"

#include "win32.h"

#include "errors.h"
#include "main.h"

#include <windows.h>

#include <string.h>


C2_HOOK_VARIABLE_IMPLEMENT(int, gAFE, 0x0074d3dc);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNoCutscenes, 0x0068b894);

C2_HOOK_VARIABLE_IMPLEMENT(HINSTANCE, gHInstance, 0x006acea4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNShowCmd, 0x006ace9c);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(const char*, gCarma2WndClassName, 0x006621d0, "Carma2MainWndClass");;
C2_HOOK_VARIABLE_IMPLEMENT_INIT(const char*, gRegionDescriminator, 0x00662270, "AUDIQUATTRO");


C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gPathNetworkIni, 240, 0x006b3380);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPathNetworkIniValid, 0x006ad4c4);

C2_HOOK_VARIABLE_IMPLEMENT(char*, gPedsFolder, 0x006a0414);
C2_HOOK_VARIABLE_IMPLEMENT(char*, gPedSoundPath, 0x00684550);
C2_HOOK_VARIABLE_IMPLEMENT(char*, gPedPowerupTxtPath, 0x006a0ad4);
C2_HOOK_VARIABLE_IMPLEMENT(char*, gPedTextTxtPath, 0x0068c718);
C2_HOOK_VARIABLE_IMPLEMENT(char*, gPedTexturePath, 0x0065852c);

void C2_HOOK_FASTCALL ConfigureDefaultPedSoundPath() {
    C2V(gPedSoundPath) = NULL;
}
C2_HOOK_FUNCTION(0x00457570, ConfigureDefaultPedSoundPath);

void C2_HOOK_FASTCALL ConfigureDefaultPedPowerupTxtPath() {
    C2V(gPedPowerupTxtPath) = "POWERUP.TXT";
}
C2_HOOK_FUNCTION(0x004e0c00, ConfigureDefaultPedPowerupTxtPath);

void C2_HOOK_FASTCALL ConfigureDefaultPedTextTxtPath() {
    C2V(gPedTextTxtPath) = "TEXT.TXT";
}
C2_HOOK_FUNCTION(0x0048ea00, ConfigureDefaultPedTextTxtPath);

void C2_HOOK_FASTCALL ConfigureDefaultPedTexturePath() {
    C2V(gPedTexturePath) = "PIXELMAP";
}
C2_HOOK_FUNCTION(0x0048f7f0, ConfigureDefaultPedTexturePath);

void C2_HOOK_FASTCALL ConfigurePedDefaultPaths() {
    C2V(gPedsFolder) = "PEDS";
    ConfigureDefaultPedSoundPath();
    ConfigureDefaultPedPowerupTxtPath();
    ConfigureDefaultPedTextTxtPath();
    ConfigureDefaultPedTexturePath();
}
C2_HOOK_FUNCTION(0x004d6f10, ConfigurePedDefaultPaths);

void C2_HOOK_FASTCALL ConfigureZombiePedSoundPath() {
    C2V(gPedSoundPath) = "ZOMSOUND";
}
C2_HOOK_FUNCTION(0x00457590, ConfigureZombiePedSoundPath);

void C2_HOOK_FASTCALL ConfigureZombiePedPowerupTxtPath() {
    C2V(gPedPowerupTxtPath) = "ZOMPOWERUP.TXT";
}
C2_HOOK_FUNCTION(0x004e0c20, ConfigureZombiePedPowerupTxtPath);

void C2_HOOK_FASTCALL ConfigureZombiePedTextTxtPath() {
    C2V(gPedTextTxtPath) = "ZOMTEXT.TXT";
}
C2_HOOK_FUNCTION(0x0048ea20, ConfigureZombiePedTextTxtPath);

void C2_HOOK_FASTCALL ConfigureZombiePedTexturePath() {
    C2V(gPedTexturePath) = "ZOMPIXELMAP";
}
C2_HOOK_FUNCTION(0x0048f810, ConfigureZombiePedTexturePath);

void C2_HOOK_FASTCALL ConfigurePedZombiePaths() {
    C2V(gPedsFolder) = "ZOMBIES";
    ConfigureZombiePedSoundPath();
    ConfigureZombiePedPowerupTxtPath();
    ConfigureZombiePedTextTxtPath();
    ConfigureZombiePedTexturePath();
}
C2_HOOK_FUNCTION(0x004d6f50, ConfigurePedZombiePaths);

void C2_HOOK_FASTCALL ConfigureBloodPedSoundPath() {
    C2V(gPedSoundPath) = NULL;
}
C2_HOOK_FUNCTION(0x00457580, ConfigureBloodPedSoundPath);

void C2_HOOK_FASTCALL ConfigureBloodPedPowerupTxtPath() {
    C2V(gPedPowerupTxtPath) = "POWERUP.TXT";
}
C2_HOOK_FUNCTION(0x004e0c10, ConfigureBloodPedPowerupTxtPath);

void C2_HOOK_FASTCALL ConfigureBloodPedTextTxtPath() {
    C2V(gPedTextTxtPath) = "TEXT.TXT";
}
C2_HOOK_FUNCTION(0x0048ea10, ConfigureBloodPedTextTxtPath);

void C2_HOOK_FASTCALL ConfigureBloodPedTexturePath() {
    C2V(gPedTexturePath) = "PIXELMAP";
}
C2_HOOK_FUNCTION(0x0048f800, ConfigureBloodPedTexturePath);

void C2_HOOK_FASTCALL ConfigurePedBloodPaths() {
    C2V(gPedsFolder) = "PEDS";
    ConfigureBloodPedSoundPath();
    ConfigureBloodPedPowerupTxtPath();
    ConfigureBloodPedTextTxtPath();
    ConfigureBloodPedTexturePath();
}
C2_HOOK_FUNCTION(0x004d6f30, ConfigurePedBloodPaths);

void C2_HOOK_FASTCALL ConfigureAlienPedSoundPath() {
    C2V(gPedSoundPath) = "ALSOUND";
}
C2_HOOK_FUNCTION(0x004575a0, ConfigureAlienPedSoundPath);

void C2_HOOK_FASTCALL ConfigureAlienPedPowerupTxtPath() {
    C2V(gPedPowerupTxtPath) = "ALPOWERUP.TXT";
}
C2_HOOK_FUNCTION(0x004e0c30, ConfigureAlienPedPowerupTxtPath);

void C2_HOOK_FASTCALL ConfigureAlienPedTextTxtPath() {
    C2V(gPedTextTxtPath) = "ALTEXT.TXT";
}
C2_HOOK_FUNCTION(0x0048ea30, ConfigureAlienPedTextTxtPath);

void C2_HOOK_FASTCALL ConfigureAlienPedTexturePath() {
    C2V(gPedTexturePath) = "ALPIXELMAP";
}
C2_HOOK_FUNCTION(0x0048f820, ConfigureAlienPedTexturePath);

void C2_HOOK_FASTCALL ConfigurePedAlienPaths() {
    C2V(gPedsFolder) = "ALIENS";
    ConfigureAlienPedSoundPath();
    ConfigureAlienPedPowerupTxtPath();
    ConfigureAlienPedTextTxtPath();
    ConfigureAlienPedTexturePath();
}
C2_HOOK_FUNCTION(0x004d6f70, ConfigurePedAlienPaths);

int (APIENTRY * WinMain_original)(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
#if 0//defined(C2_HOOKS_ENABLED)
    return WinMain_original(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
#else
    char* currentArgument;

    C2_HOOK_START();
    C2V(gAFE) = 0;
    ConfigurePedDefaultPaths();
    if (strlen(lpCmdLine) > 0) {
        _strupr(lpCmdLine);
        currentArgument = strtok(lpCmdLine, " \t\n");
        while (1) {
            if (currentArgument == NULL) {
                break;
            }
            if (strcmp(currentArgument, "-NOCUTSCENE") == 0 || strcmp(currentArgument, "-NOCUTSCENES") == 0) {
                C2V(gNoCutscenes) = 1;
            } else if (strcmp(currentArgument, "-D3D") == 0) {
                C2V(gRenderer) = "D3D";
            } else if (strcmp(currentArgument, "-ZOMBIE") == 0) {
                ConfigureZombiePedTexturePath();
            } else if (strcmp(currentArgument, "-BLOOD") == 0) {
                ConfigurePedBloodPaths();
            } else if (strcmp(currentArgument, "-ALIEN") == 0) {
                ConfigurePedAlienPaths();
            } else if (strcmp(currentArgument, "-AFE") == 0) {
                C2V(gAFE) = 1;
            } else if (strcmp(currentArgument, "-SCALEMOUSE") == 0) {
                C2V(gScaleMouse) = 1;
            } else {
                dr_dprintf("Unknown argument: \"%s\"", currentArgument);
            }
            currentArgument = strtok(NULL, " \t\n");
        }
    }
    if (FindWindowA(C2V(gCarma2WndClassName), NULL) != NULL) {
        ExitProcess(703);
    }
    WNDCLASSA wndCls;
    wndCls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    wndCls.lpfnWndProc = Carma2MainWndProc;
    wndCls.cbClsExtra = 0;
    wndCls.cbWndExtra = 0;
    wndCls.hInstance = hInstance; // C2V(gHInstance);
    wndCls.hIcon = LoadIconA(NULL, IDI_APPLICATION);
    wndCls.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wndCls.hbrBackground = GetStockObject(BLACK_BRUSH);
    wndCls.lpszMenuName = NULL;
    wndCls.lpszClassName = C2V(gCarma2WndClassName);
    RegisterClassA(&wndCls);
    C2V(gHWnd) = CreateWindowExA(0, C2V(gCarma2WndClassName), "Carmageddon II", WS_POPUP | WS_VISIBLE,
                                 0, 0, 4, 4, NULL, NULL, C2V(gHInstance), NULL);
    UpdateWindow(C2V(gHWnd));
    SetFocus(C2V(gHWnd));
    LANGID langId = GetSystemDefaultLangID();
    if (strcmp(C2V(gRegionDescriminator), "AUDIQUATTRO") == 0 && PRIMARYLANGID(langId) == LANG_JAPANESE) {
        MessageBoxA(NULL, "Sorry, this is the Western European version of Carmageddon II - Carpocalypse Now.\n\nThis version can not run on this machine.\n",
                    "Carmageddon II - Carpocalypse Now", MB_ICONERROR);
        ExitProcess(704);
    }
    C2V(gScreenWidth) = GetSystemMetrics(SM_CXSCREEN);
    C2V(gScreenHeight) = GetSystemMetrics(SM_CYSCREEN);
    C2V(gHInstance) = hInstance;
    C2V(gNShowCmd) = nShowCmd;
    C2V(gPathNetworkIni)[0] = '\0';
    DWORD lenCwd = GetCurrentDirectoryA(240, C2V(gPathNetworkIni)); // FIXME: use C2_ARRAY_SIZE
    if (lenCwd != 0) {
        strcat(C2V(gPathNetworkIni), "\\");
        strcat(C2V(gPathNetworkIni), "NETWORK.INI");
    }

    const char* args[] = { "Carmageddon" };

    GameMain(1, args);
    C2_HOOK_FINISH();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051aaa0, WinMain, WinMain_original);
