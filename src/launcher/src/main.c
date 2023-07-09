#include <windows.h>

#include "global.h"
#include "launcher.h"
#include "password.h"
#include "resources.h"

#define CARMA_LAUCNHER_CLASS_NAME "CarmaLauncherClass"

int ApplyStoredSettings(void) {
    int i;
    int index;
    int display_idx;
    int device_idx;
    char* arg;

    if (g_Config_loaded != 1) {
        return 0;
    }
    if (!(g_Detected_backends & (1 << g_Backend))) {
        return 0;
    }
    index = -1;
    display_idx = -1;
    device_idx = -1;
    if (g_Backend == kBackend_direct3d) { /* d3d */\
        for (i = 0; i < g_count_directdraw_displays; i++) {
            if (g_DirectDraw_displays[i].valid) {
                if (memcmp(&g_DirectDraw_displays[i].guid, &g_DrvGuid, sizeof(GUID)) == 0) {
                    index = i;
                }
            } else if (!g_DrvGuid_valid) {
                index = i;
            }
            display_idx = index;
        }
        if (display_idx == -1) {
            return 0;
        }
        for (i = 0; i < g_DirectDraw_displays[display_idx].device_count; i++) {
            if (memcmp(&g_DirectDraw_displays[display_idx].devices[i].guid, g_DevGuid, sizeof(GUID)) == 0) {
                device_idx = i;
            }
        }
        if (device_idx == -1) {
            return 0;
        }
    }
    g_Flags = g_Flags_register;
    SendDlgItemMessageA(g_Dialog, IDC_ALTERNATE_FOX_EFFECT, BM_SETCHECK, BST_UNCHECKED, 0);
    SendDlgItemMessageA(g_Dialog, IDC_DIRECTDRAW, BM_SETCHECK, (g_Backend == kBackend_directdraw) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendDlgItemMessageA(g_Dialog, IDC_GLIDE, BM_SETCHECK, (g_Backend == kBackend_3dfx) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendDlgItemMessageA(g_Dialog, IDC_DIRECT3D, BM_SETCHECK, (g_Backend == kBackend_direct3d) ? BST_CHECKED : BST_UNCHECKED, 0);
    g_selected_backend = g_Backend;
    switch(g_Backend) {
    case kBackend_directdraw:
    case kBackend_3dfx:
        EnableD3DOptions(0);
        break;
    case kBackend_direct3d:
        EnableD3DOptions(1);
        break;
    }
    if (g_selected_backend == kBackend_direct3d) {
        SendDlgItemMessageA(g_Dialog, IDC_DRIVER, CB_SETCURSEL, display_idx, 0);
        Direct3DRefreshDeviceCombobox();
        SendDlgItemMessageA(g_Dialog, IDC_DEVICE, CB_SETCURSEL, device_idx, 0);
        g_Flags = g_Flags_register;
    }
    arg = GetCommandLineA();
    while (*arg != '\0' && *arg != '/') {
        arg++;
    }
    if (strcmp(arg, "/hold") == 0 || strcmp(arg, "/setup") || strcmp(arg, "/config") == 0) {
        g_Flags &= ~kSaveOptions;
        return 0;
    }
    return 1;
}

LRESULT CALLBACK LauncherWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    switch (Msg) {
    case WM_CREATE:
        if (AskPassword(hWnd) != -1) {
            g_Dialog = CreateDialogParamA(g_hInstance, MAKEINTRESOURCE(IDD_MAIN), hWnd, MainWindowDlgProc, 0);
        }
        break;
    case WM_CLOSE:
        g_close_requested = kClose_Abort;
        return 0;
    }
    return DefWindowProcA(hWnd, Msg, wParam, lParam);
}

void ReadRegisterConfiguration(void) {
    LSTATUS success;
    HKEY hKey;
    DWORD size;
    DWORD type;

    success = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\SCI\\Carmageddon2", 0, KEY_ALL_ACCESS, &hKey);
    if (success != ERROR_SUCCESS) { /* This is also in OG */
        size = sizeof(g_DrvGuid);
        success = RegQueryValueExA(hKey, "DrvGuid", NULL, &type, (LPBYTE)g_DrvGuid, &size);
        if (success != ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return;
        }
        if (type == REG_BINARY) {
            g_DrvGuid_valid = 1;
        } else {
            if (type != REG_DWORD) {
                RegCloseKey(hKey);
                return;
            }
            g_DrvGuid_valid = 0;
        }
        size = sizeof(g_DevGuid);
        success = RegQueryValueExA(hKey, "DevGuid" ,NULL, &type, (LPBYTE)g_DevGuid, &size);
        if (success != ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return;
        }
        if (type != REG_BINARY) {
            RegCloseKey(hKey);
            return;
        }
        size = sizeof(g_Flags_register);
        success = RegQueryValueExA(hKey, "Flags", NULL, &type, (LPBYTE)&g_Flags_register, &size);
        if (success != ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return;
        }
        if (type != REG_DWORD_LITTLE_ENDIAN) {
            g_Flags_register = -1;
            RegCloseKey(hKey);
            return;
        }
        size = sizeof(g_Backend);
        success = RegQueryValueExA(hKey, "Driver", NULL, &type, (LPBYTE)&g_Backend, &size);
        if (success != ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return;
        }
        if (type != REG_DWORD_LITTLE_ENDIAN) {
            g_Backend = -1;
            RegCloseKey(hKey);
            return;
        }
        RegCloseKey(hKey);
        g_Config_loaded = 1;
    }
}


int RegisterCreateCarmaLauncherWindow(HINSTANCE p_hInst) {
    WNDCLASSEXA wndcls;
    HWND window;

    g_hInstance = p_hInst;
    wndcls.cbSize = sizeof(wndcls);
    wndcls.style = CS_VREDRAW | CS_HREDRAW;
    wndcls.lpfnWndProc = LauncherWndProc;
    wndcls.cbClsExtra = 0;
    wndcls.cbWndExtra = 0;
    wndcls.hInstance = p_hInst;
    wndcls.hIcon = LoadIconA(p_hInst, "EDITOR_ICON1");
    wndcls.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wndcls.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wndcls.lpszMenuName = NULL;
    wndcls.lpszClassName = CARMA_LAUCNHER_CLASS_NAME;
    wndcls.hIconSm = (HICON)LoadImageA(p_hInst, "EDITOR_ICON1", IMAGE_ICON, 16, 16, 0);
    if (RegisterClassExA(&wndcls) == 0) {
        if (RegisterClassA((WNDCLASSA*)&wndcls.style) == 0) {
            return 0;
        }
    }
    window = CreateWindowExA(WS_EX_APPWINDOW,
        CARMA_LAUCNHER_CLASS_NAME,
        "Carmageddon Launcher",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        0,
        CW_USEDEFAULT,
        0,
        NULL,
        NULL,
        p_hInst,
        NULL);

    if (window == NULL) {
        return 0;
    }
    g_hWnd = window;
    return 1;
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    MSG msg;
    STARTUPINFOA startup_info;
    PROCESS_INFORMATION process_information;
    HWND window;
    HKEY h;
    char* exename;
    char* command;

    g_hInstance = hInst;
    if (CoInitialize(NULL) < 0) {
        return 0;
    }
    window = FindWindowA(CARMA_LAUCNHER_CLASS_NAME, NULL);
    if (window != NULL) {
        SetForegroundWindow(window);
        SetFocus(window);
        return 0;
    }
    ReadRegisterConfiguration();
    RegisterCreateCarmaLauncherWindow(hInst);
    while (PeekMessageA(&msg, NULL, 0, 0, 0) != 0) {
        GetMessageA(&msg, NULL, 0, 0);
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    SetCapture(g_Dialog);
    PostMessageA(g_Dialog, 0x483, 0, 0);

    static int DAT_004137f8;
    while (PeekMessageA(&msg, NULL, 0, 0, 0) != 0) {
        GetMessageA(&msg, NULL, 0, 0);
        TranslateMessage(&msg);
        if (!DAT_004137f8) {
            DAT_004137f8 = 1;
            EnableWindow(GetDlgItem(g_Dialog, 1011), 1);
            EnableWindow(GetDlgItem(g_Dialog, 1006), 1);
        }
        DispatchMessageA(&msg);
    }
    ReleaseCapture();
    if (!ApplyStoredSettings() || g_Detection_state == 0) {
        ShowWindow(g_Dialog, SW_SHOW);
        if (DAT_004137f8 == 1 || g_Detection_state == 0) {
            while (!g_close_requested) {
                while (PeekMessageA(&msg, NULL, 0, 0, 0) != 0) {
                    GetMessageA(&msg, NULL, 0, 0);
                    TranslateMessage(&msg);
                    DispatchMessageA(&msg);
                }
            }
        }
    } else {
        g_close_requested = kClose_StartGame;
    }
    if ((DAT_004137f8 != 0 || g_Detection_state != 1 || g_close_requested != kClose_NotClosed) && g_close_requested != kClose_StartGame) {
        return 0;
    }

    exename = NULL;
    command = NULL;
    if (g_DirectDraw_displays[g_current_d3d_display].devices[g_current_d3d_device].srcDstBlendsCapsOk == 1) {
        if (g_blood == 1) {
            if (g_selected_backend == kBackend_directdraw) {
                exename = "Carma2_SW.exe";
                command = "carma2_sw.exe";
            } else if (g_selected_backend == kBackend_3dfx) {
                exename = "Carma2_HW.exe";
                command = "carma2_hw.exe";
            } else if (g_selected_backend == kBackend_direct3d) {
                exename = "Carma2_HW.exe";
                command = "carma2_hw.exe -d3d -afe -noblend";
                if (!(g_Flags & kAlternateFoxEffect)) {
                    command = "carma2_hw.exe -d3d -noblend";
                }
            }
        } else {
            if (g_selected_backend == kBackend_directdraw) {
                exename = "Carma2_SW.exe";
                command = "carma2_sw.exe -zombie";
            } else if (g_selected_backend == kBackend_3dfx) {
                exename = "Carma2_HW.exe";
                command = "carma2_hw.exe -zombie";
            } else if (g_selected_backend == kBackend_3dfx) {
                exename = "Carma2_HW.exe";
                command = "carma2_hw.exe -d3d -zombie -afe -noblend";
                if (!(g_Flags & kAlternateFoxEffect)) {
                    command = "carma2_hw.exe -d3d -zombie -noblend";
                }
            }
        }
    } else {
        if (g_blood == 1) {
            if (g_selected_backend == kBackend_directdraw) { /* directdraw */
                exename = "Carma2_SW.exe";
                command = "carma2_sw.exe";
            } else if (g_selected_backend == kBackend_3dfx) { /* glide 3dfx */
                exename = "Carma2_HW.exe";
                command = "carma2_hw.exe";
            } else if (g_selected_backend == kBackend_direct3d) { /* d3d */
                exename = "Carma2_HW.exe";
                command = "carma2_hw.exe -d3d -afe";
                if (!(g_Flags & kAlternateFoxEffect)) {
                    command = "carma2_hw.exe -d3d";
                }
            }
        } else {
            if (g_selected_backend == kBackend_directdraw) { /* directdraw */
                exename = "Carma2_SW.exe";
                command = "carma2_sw.exe -zombie";
            } else if (g_selected_backend == kBackend_3dfx) { /* glide 3dfx */
                exename = "Carma2_HW.exe";
                command = "carma2_hw.exe -zombie";
            } else if (g_selected_backend == kBackend_direct3d) { /* d3d */
                exename = "Carma2_HW.exe";
                command = "carma2_hw.exe -d3d -zombie -afe";
                if (!(g_Flags & kAlternateFoxEffect)) {
                    command = "carma2_hw.exe -d3d -zombie";
                }
            }
        }
    }
    if (command == NULL) {
        MessageBoxA(NULL, "Invalid Option", "Carmageddon 2 Launcher", 0x80010);
        return 0;
    }
    RegCreateKeyExA(HKEY_LOCAL_MACHINE, "Software\\SCI\\Carmageddon2", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &h, NULL);
    RegSetValueExA(h, NULL, 0, REG_SZ, (BYTE*)"Carmageddon 2", strlen("Carmageddon 2"));
    RegSetValueExA(h, "Driver", 0, REG_DWORD, (BYTE*)&g_selected_backend, sizeof(g_selected_backend));
    if (g_current_d3d_display == -1) {
        RegDeleteValueA(h, "DrvGuid");
    } else if (g_DirectDraw_displays[g_current_d3d_display].valid == 1) {
        RegSetValueExA(h, "DrvGuid", 0, REG_BINARY, (BYTE*)&g_DirectDraw_displays[g_current_d3d_display].guid, sizeof(GUID));
    } else {
        RegSetValueExA(h, "DrvGuid", 0, REG_DWORD, (BYTE*)&g_DirectDraw_displays[g_current_d3d_display].valid, sizeof(g_DirectDraw_displays[g_current_d3d_display].valid));
    }
    if (g_current_d3d_device == -1) {
        RegDeleteValueA(h, "DevGuid");
    }
    else {
        RegSetValueExA(h, "DevGuid", 0, REG_BINARY, (BYTE*)&g_DirectDraw_displays[g_current_d3d_display].devices[g_current_d3d_device].guid, sizeof(GUID));
    }
    RegSetValueExA(h, "Flags", 0, REG_DWORD, (BYTE*)&g_Flags, sizeof(g_Flags));

    DestroyWindow(g_Dialog);

    memset(&startup_info, 0, sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);
    if (CreateProcessA(NULL, command, NULL, NULL, 0 ,0, &g_environment, NULL, &startup_info, &process_information) != 1) {
        MessageBoxA(NULL, "Can't Find Executable", exename, MB_RIGHT | MB_ICONHAND);
        g_Flags &= ~kSaveOptions;
        RegSetValueExA(h, "Flags", 0, REG_DWORD, (BYTE*)&g_Flags, sizeof(g_Flags));
    }
    RegSetValueExA(h, "ProcessId", 0, REG_DWORD, (BYTE*)&process_information.dwProcessId, sizeof(process_information.dwProcessId));
    RegCloseKey(h);
    return 0;
}
