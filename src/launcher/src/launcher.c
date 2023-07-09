#include "launcher.h"

#include "global.h"
#include "resources.h"
#include "util.h"

#include <d3d.h>
#include <windows.h>

void Direct3DRefreshDeviceCombobox(void) {
    int found;
    int i;

    ComboboxClearAllOptions(g_Dialog, IDC_DEVICE);
    g_current_d3d_display = SendDlgItemMessageA(g_Dialog, IDC_DRIVER, CB_GETCURSEL, 0, 0);
    found = 0;
    g_current_d3d_device = -1;
    for (i = 0; i < g_DirectDraw_displays[g_current_d3d_display].device_count; i++) {
        if (g_DirectDraw_displays[g_current_d3d_display].devices[i].color_model  == 0) {
            SendDialogMessageFormat(g_Dialog, IDC_DEVICE, CB_ADDSTRING, 0, "%s (Software)", g_DirectDraw_displays[g_current_d3d_display].devices[i].name);
            if (!found) {
                SendDlgItemMessageA(g_Dialog, IDC_DEVICE, CB_SETCURSEL, i, 0);
                g_Flags &= 0xffffffaa;
                g_Flags |= 0x2a;
                g_D3D_prefered_device_is_hardware = 0;
                g_current_d3d_device = i;
            }
        } else {
            SendDialogMessageFormat(g_Dialog, IDC_DEVICE, CB_ADDSTRING, 0, "%s (Hardware)", g_DirectDraw_displays[g_current_d3d_display].devices[i].name);
            SendDlgItemMessageA(g_Dialog, IDC_DEVICE, CB_SETCURSEL, i, 0);
            g_Flags &= 0xffffff9f;
            g_Flags |= 0x1f;
            found = 1;
            g_D3D_prefered_device_is_hardware = 1;
            g_current_d3d_device = i;
            if (g_DirectDraw_displays[g_current_d3d_display].devices[i].zbuffer_not_ok) {
                g_Flags |= 0x3f;
            }
        }
    }
}

void EnableD3DOptions(int enable) {

    EnableWindow(GetDlgItem(g_Dialog, IDC_D3D_OPTIONS), enable);
    EnableWindow(GetDlgItem(g_Dialog, IDC_DRIVER_LABEL), enable);
    EnableWindow(GetDlgItem(g_Dialog, IDC_DEVICE_LABEL), enable);
    EnableWindow(GetDlgItem(g_Dialog, IDC_DRIVER), enable);
    EnableWindow(GetDlgItem(g_Dialog, IDC_DEVICE), enable);
    EnableWindow(GetDlgItem(g_Dialog, IDC_ADVANCED_BUTTON), enable);
}

void ShowDetails(int p_show) {
    static int show = -1;
    static int dialog_width, dialog_height, output_height;

    if (show == -1) {
        RECT window_rect;
        RECT output_rect;
        GetWindowRect(g_Dialog, &window_rect);
        dialog_width = window_rect.right - window_rect.left;
        dialog_height = window_rect.bottom - window_rect.top;
        GetWindowRect(GetDlgItem(g_Dialog, IDC_OUTPUT), &output_rect);
        output_height = output_rect.top - window_rect.top;
    }
    if (p_show != show) {
        show = p_show;
        if (p_show) {
            SetWindowPos(g_Dialog, NULL, 0, 0, dialog_width, dialog_height, SWP_NOZORDER | SWP_NOMOVE);
            ShowWindow(GetDlgItem(g_Dialog, IDC_DETAILS_SHRINK), SW_SHOW);
            ShowWindow(GetDlgItem(g_Dialog, IDC_DETAILS_EXPAND), SW_HIDE);
        } else {
            SetWindowPos(g_Dialog, NULL, 0, 0, dialog_width, output_height, SWP_NOZORDER | SWP_NOMOVE);
            ShowWindow(GetDlgItem(g_Dialog, IDC_DETAILS_EXPAND), SW_SHOW);
            ShowWindow(GetDlgItem(g_Dialog, IDC_DETAILS_SHRINK), SW_HIDE);
        }
    }
}

HRESULT CALLBACK D3DDeviceCallback(GUID* guid, char* description, char* name, D3DDEVICEDESC* hal_desc, D3DDEVICEDESC* hel_desc, void* ctx) {
    int device_i;
    int curent_display = g_count_directdraw_displays;

    if (memcmp(&IID_IDirect3DRampDevice, guid, sizeof(GUID)) == 0) {
        return 1;
    }
    if (memcmp(&IID_IDirect3DMMXDevice, guid, sizeof(GUID)) == 0) {
        return 1;
    }
    if (!((hal_desc->dwFlags | hel_desc->dwFlags) & D3DDD_TRICAPS)) {
        return 1;
    }
    if (!hal_desc->dwDeviceZBufferBitDepth && !hel_desc->dwDeviceZBufferBitDepth) {
        return 1;
    }
    device_i = g_DirectDraw_displays[curent_display].device_count;
    g_DirectDraw_displays[curent_display].device_count++;
    strcpy(g_DirectDraw_displays[curent_display].devices[device_i].name, name);
    memcpy(&g_DirectDraw_displays[curent_display].devices[device_i].guid, guid, sizeof(*guid));
    g_DirectDraw_displays[curent_display].devices[device_i].color_model = hal_desc->dcmColorModel;
    if (hal_desc->dcmColorModel == 0) {
        g_DirectDraw_displays[curent_display].devices[device_i].zbuffer_not_ok = 1;
    } else {
        g_DirectDraw_displays[curent_display].devices[device_i].zbuffer_not_ok = !((hal_desc->dpcTriCaps.dwZCmpCaps & D3DPCMPCAPS_ALWAYS) && (hal_desc->dpcTriCaps.dwZCmpCaps & D3DPCMPCAPS_LESS) && (hal_desc->dpcTriCaps.dwZCmpCaps & D3DPCMPCAPS_LESSEQUAL) && (hal_desc->dpcTriCaps.dwZCmpCaps & D3DPCMPCAPS_GREATER));
        if (hal_desc->dpcTriCaps.dwDestBlendCaps != 0 && hal_desc->dpcTriCaps.dwSrcBlendCaps != 0) {
            g_DirectDraw_displays[curent_display].devices[device_i].srcDstBlendsCapsOk = 0;
            return 1;
        }
    }
    g_DirectDraw_displays[curent_display].devices[device_i].srcDstBlendsCapsOk = 1;
    return 1;
}

HRESULT CALLBACK DDrawDisplayModeCallback(DDSURFACEDESC* hal_desc, void* user) {
    int display_mode_idx;

    if (hal_desc->dwWidth >= 320 && hal_desc->dwHeight >= 240 && hal_desc->ddpfPixelFormat.dwRGBBitCount == 16
        && hal_desc->dwWidth <= 1024 && hal_desc->dwHeight * hal_desc->dwWidth < g_DirectDraw_displays[g_count_directdraw_displays].available_memory) {
        display_mode_idx = g_DirectDraw_displays[g_count_directdraw_displays].display_mode_count;
        g_DirectDraw_displays[g_count_directdraw_displays].display_mode_count++;
        memcpy(&g_DirectDraw_displays[g_count_directdraw_displays].display_modes[display_mode_idx], &hal_desc->ddpfPixelFormat, sizeof(hal_desc->ddpfPixelFormat));
        g_DirectDraw_displays[g_count_directdraw_displays].display_modes[display_mode_idx].width = hal_desc->dwWidth;
        g_DirectDraw_displays[g_count_directdraw_displays].display_modes[display_mode_idx].height = hal_desc->dwHeight;
    }
    return 1;
}

BOOL FAR PASCAL DDrawDisplayCallback(GUID* p_guid, char* p_driverName, char* p_driverDescription, void* p_user) {
    static LPDIRECTDRAW directdraw;
    static LPDIRECT3D2 direct3d2;
    LPDIRECTDRAW2 directdraw2;
    DDSCAPS caps;
    DWORD total_mem, available_mem;
    int display_i;

    display_i = g_count_directdraw_displays;
    if (display_i >= ARRAY_SIZE(g_DirectDraw_displays)) {
        return 1;
    }
    if (p_guid == NULL) {
        g_DirectDraw_displays[display_i].valid = 0;
    } else {
        g_DirectDraw_displays[display_i].guid = *p_guid;
        g_DirectDraw_displays[display_i].valid = 1;
    }
    strcpy(g_DirectDraw_displays[display_i].name, p_driverName);
    strcpy(g_DirectDraw_displays[display_i].description, p_driverDescription);
    g_DirectDraw_displays[display_i].error = NULL;
    if (g_DirectDrawCreate(p_guid, &directdraw, NULL) < 0) {
        g_DirectDraw_displays[display_i].error = "    Failed: Can't Create";
        g_count_directdraw_displays++;
        return 1;
    }
    if (IDirectDraw_QueryInterface(directdraw, &IID_IDirect3D2, (void**)&direct3d2) != 0) {
        IDirectDraw_Release(directdraw);
        g_DirectDraw_displays[display_i].error = "    Failed: Cant Create D3D interface";
        g_count_directdraw_displays++;
        return 1;
    }
    if (IDirectDraw_QueryInterface(directdraw, &IID_IDirectDraw2, (void**)&directdraw2) != 0) {
        IDirectDraw_Release(directdraw);
        g_DirectDraw_displays[display_i].error = "    Failed: Can't create direct draw 2 interface";
        g_count_directdraw_displays++;
        return 1;
    }
    //LPDDSCAPS lpDDCaps, LPDWORD lpdwTotal, LPDWORD lpdwFree) PURE;
    caps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    if (FAILED(IDirectDraw2_GetAvailableVidMem(directdraw2, &caps, &total_mem, &available_mem))) {
        IDirectDraw_Release(directdraw);
        g_DirectDraw_displays[display_i].error = "    Failed: Can't get available memory";
        g_count_directdraw_displays++;
        return 1;
    }
    g_DirectDraw_displays[display_i].available_memory = available_mem;
    IDirectDraw2_Release(directdraw2);

    if (display_i == 0) {
        DDSURFACEDESC surfaceDesc;
        memset(&surfaceDesc, 0, sizeof(surfaceDesc));
        surfaceDesc.dwSize = sizeof(surfaceDesc);
        if (FAILED(IDirectDraw_GetDisplayMode(directdraw, &surfaceDesc))) {
            IDirectDraw_Release(directdraw);
            g_DirectDraw_displays[display_i].error = "    Failed: Can't Get current display mode";
            g_count_directdraw_displays++;
            return 1;
        }
        g_DirectDraw_displays[display_i].available_memory += surfaceDesc.dwWidth * surfaceDesc.dwHeight * surfaceDesc.ddpfPixelFormat.dwRGBBitCount / 8;
    }

//    memset(&g_ddrawdevice1, 0, sizeof(g_ddrawdevice1));
//    memset(&g_ddrawdevice2, 0, sizeof(g_ddrawdevice2));
//    g_ddrawdevice1.dwSize = sizeof(g_ddrawdevice1);
//    g_ddrawdevice2.dwSize = sizeof(g_ddrawdevice2);
//    g_ddrawdevice1.offset0x4 = 4;
//    g_ddrawdevice1.offset0x8 = 1;
    g_DirectDraw_displays[display_i].device_count = 0;
    g_DirectDraw_displays[display_i].display_mode_count = 0;

    if (IDirect3D2_EnumDevices(direct3d2, D3DDeviceCallback, NULL) != 0) {
        IDirectDraw_Release(directdraw);
        g_DirectDraw_displays[display_i].error = "    Failed: Can't Enum devices";
        g_count_directdraw_displays++;
        return 1;
    }
    if (IDirectDraw_EnumDisplayModes(directdraw, 0, NULL, NULL, DDrawDisplayModeCallback) != 0) {
        IDirectDraw_Release(directdraw);
        g_DirectDraw_displays[display_i].error = "    Failed: Can't Enum Modes";
        g_count_directdraw_displays++;
        return 1;
    }
    g_count_directdraw_displays++;
    IDirectDraw_Release(directdraw);
    return 1;
}

int CALLBACK D3DOptionsDlgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    switch (Msg) {
        case WM_INITDIALOG:
            CenterWindowOnScreen(hWnd);
            ShowWindow(hWnd, SW_SHOW);
            SendDlgItemMessageA(hWnd, IDC_BILINEAR_FILTERING, BM_SETCHECK, (g_Flags & kBilinearFiltering) ? BST_CHECKED : BST_UNCHECKED, 0);
            SendDlgItemMessageA(hWnd, IDC_ALPHA_BLEEDING, BM_SETCHECK, (g_Flags & kAlphaBleeding) ? BST_CHECKED : BST_UNCHECKED, 0);
            SendDlgItemMessageA(hWnd, IDC_PERSPECTIVE_CORRECT, BM_SETCHECK, (g_Flags & kPerspectiveCorrect) ? BST_CHECKED : BST_UNCHECKED, 0);
            SendDlgItemMessageA(hWnd, IDC_FRAME_BUFFER_DITHERING, BM_SETCHECK, (g_Flags & kFrameBufferDithering) ? BST_CHECKED : BST_UNCHECKED, 0);
            SendDlgItemMessageA(hWnd, IDC_ALTERNATE_FOX_EFFECT, BM_SETCHECK, (g_Flags & kAlternateFoxEffect) ? BST_CHECKED : BST_UNCHECKED, 0);
            return 1;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
            case IDC_OK:
                g_Flags &= 0xffffff80;
                if (SendDlgItemMessageA(hWnd, IDC_BILINEAR_FILTERING, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    g_Flags |= kAlternateFoxEffect;
                }
                if (SendDlgItemMessageA(hWnd, IDC_ALPHA_BLEEDING, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    g_Flags |= kAlphaBleeding;
                }
                if (SendDlgItemMessageA(hWnd, IDC_PERSPECTIVE_CORRECT, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    g_Flags |= kPerspectiveCorrect;
                }
                if (SendDlgItemMessageA(hWnd, IDC_FRAME_BUFFER_DITHERING, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    g_Flags |= kFrameBufferDithering;
                }
                if (SendDlgItemMessageA(hWnd, IDC_ALTERNATE_FOX_EFFECT, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    g_Flags |= kFrameBufferDithering;
                }
                EndDialog(hWnd, 0);
                break;
            case IDC_CANCEL:
                EndDialog(hWnd, 0);
                return 0;
            }
    }
    return 0;
}

int CALLBACK MainWindowDlgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    int d3d_hw_detected;
    int i;

    switch (Msg) {
    default:
        return 0;
    case WM_QUIT:
        DestroyWindow(hWnd);
        g_close_requested = kClose_Abort;
        return 0;
    case WM_INITDIALOG:
        CenterWindowOnScreen(hWnd);
        g_Dialog = hWnd;
        ShowDetails(0);
        if (g_Config_loaded == 1) {
            SendDlgItemMessageA(hWnd, IDC_SAVE_OPTIONS, BM_SETCHECK, (g_Flags & kSaveOptions) ? BST_CHECKED : BST_UNCHECKED, 0);
            if (!(g_Flags_register & kSaveOptions)) {
                ShowWindow(hWnd, SW_SHOW);
            }
        } else {
            ShowWindow(hWnd, SW_SHOW);
        }
        return 1;
    case 0x483:
        g_Detection_state = 0;
        SendDialogMessageFormat(hWnd, IDC_OUTPUT, LB_ADDSTRING, 0, "Detecting...");
        EnableWindow(GetDlgItem(hWnd, IDC_DIRECTDRAW), 0);
        EnableWindow(GetDlgItem(hWnd, IDC_GLIDE), 0);
        EnableWindow(GetDlgItem(hWnd, IDC_DIRECT3D), 0);
        EnableWindow(GetDlgItem(hWnd, IDC_README), 0);
        EnableWindow(GetDlgItem(hWnd, IDC_OK), 0);
        EnableWindow(GetDlgItem(hWnd, IDC_RECHECK), 0);
        PostMessageA(hWnd, 0x464, 0, 0);
        g_Detected_backends = 0;
        return 0;
    case 0x464:
        SendDialogMessageFormat(hWnd, IDC_OUTPUT, LB_ADDSTRING, 0, "DIRECT DRAW:");
        PostMessageA(hWnd, 0x465, 0, 0);
        return 0;
    case 0x465:
        g_ddraw_dll = LoadLibraryA("ddraw.dll");
        if (g_ddraw_dll == NULL) {
            PostMessageA(hWnd, 0x469, 0, 0);
            return 0;
        }
        g_DirectDrawCreate = (HRESULT (WINAPI*)(GUID*, LPDIRECTDRAW*, IUnknown*))GetProcAddress(g_ddraw_dll, "DirectDrawCreate");
        if (g_DirectDrawCreate == NULL) {
            PostMessageA(hWnd, 0x469, 0, 0);
            return 0;
        }
        g_DirectDrawEnumerateA = (HRESULT (WINAPI*)(LPDDENUMCALLBACKA, LPVOID))GetProcAddress(g_ddraw_dll, "DirectDrawEnumerateA");
        if (g_DirectDrawEnumerateA == NULL) {
            PostMessageA(hWnd, 0x469, 0, 0);
            return 0;
        }
        if (CoCreateInstance(&CLSID_DirectDraw, NULL, 7, &IID_IDirectDraw2, (void**)&g_DirectDraw2) < 0) {
            PostMessageA(hWnd, 0x469, 0, 0);
            return 0;
        }
        IDirectDraw2_Initialize(g_DirectDraw2, NULL);
        PostMessage(hWnd, 0x468, 0, 0);
        return 0;
    case 0x468:
        SendDialogMessageFormat(hWnd, IDC_OUTPUT, LB_ADDSTRING, 0, "  Ok");
        EnableWindow(GetDlgItem(hWnd, IDC_DIRECTDRAW), 1);
        SendDlgItemMessageA(hWnd, IDC_DIRECTDRAW, BM_CLICK, 0, 0);
        SendDlgItemMessageA(hWnd, IDC_OUTPUT, LB_ADDSTRING, 0, (LPARAM)"  Direct Draw Ok.");
        g_Detected_backends |= 1 << kBackend_directdraw;
        g_Detection_state = 1;
        PostMessageA(hWnd, 0x46e, 0, 0);
        return 0;
    case 0x469:
        SendDlgItemMessageA(hWnd, IDC_OUTPUT, LB_ADDSTRING, 0, (LPARAM)"  Failed.");
        PostMessageA(hWnd, 0x478, 0, 0);
        return 0;
    case 0x46e:
        g_count_directdraw_displays = 0;
        g_current_d3d_display = 0;
        g_D3D_prefered_device_is_hardware = 0;
        SendDialogMessageFormat(hWnd, IDC_OUTPUT, LB_ADDSTRING, 0, "DIRECT 3D:");
        ComboboxClearAllOptions(hWnd, IDC_DRIVER);
        PostMessageA(hWnd, 0x46f, 0, 0);
        return 0;
    case 0x46f:
        HRESULT WINAPI EnumerateCallback(LPDDENUMCALLBACKA, void *);
        if (g_DirectDrawEnumerateA(DDrawDisplayCallback, NULL) != 0) {
            PostMessageA(hWnd, 0x473, 0, 0);
        } else {
            PostMessageA(hWnd, 0x470, 0, 0);
        }
        return 0;
    case 0x470:
        SendDialogMessageFormat(hWnd, IDC_OUTPUT, LB_ADDSTRING, 0, "  Dev[%d] %s [%s]", g_current_d3d_display + 1,
                                g_DirectDraw_displays[g_current_d3d_display].name,
                                g_DirectDraw_displays[g_current_d3d_display].description);
        SendDialogMessageFormat(hWnd, IDC_DRIVER, CB_ADDSTRING, 0, g_DirectDraw_displays[g_current_d3d_display].name);
        if (g_DirectDraw_displays[g_current_d3d_display].error == NULL) {
            SendDialogMessageFormat(hWnd, IDC_OUTPUT, LB_ADDSTRING, 0, "    Ok. Memory=%dk", g_DirectDraw_displays[g_current_d3d_display].available_memory / 1024);
            EnableWindow(GetDlgItem(hWnd, IDC_DIRECT3D), 1);
            d3d_hw_detected = 0;
            for (i = 0; i < g_DirectDraw_displays[g_current_d3d_display].device_count; i++) {
                if (g_DirectDraw_displays[g_current_d3d_display].devices[i].color_model != 0) {
                    d3d_hw_detected = 1;
                }
            }
            if (d3d_hw_detected) {
                /* Send click to Direct3D combobox item + set selection of direct3d driver
                   combobox */
                SendDlgItemMessageA(hWnd, IDC_DIRECT3D, BM_CLICK, 0, 0);
                SendDlgItemMessageA(hWnd, IDC_DRIVER, CB_SETCURSEL, g_current_d3d_display, 0);
                Direct3DRefreshDeviceCombobox();
            }
            else if (g_Detection_state == 0) {
                SendDlgItemMessageA(hWnd, IDC_DIRECT3D, BM_CLICK, 0, 0);
                SendDlgItemMessageA(hWnd, IDC_DRIVER, CB_SETCURSEL, g_current_d3d_display, 0);
            }
            else {
                SendDlgItemMessageA(hWnd, IDC_DRIVER, CB_SETCURSEL, g_current_d3d_display, 0);
            }
            g_Detection_state = 1;
            g_Detected_backends |= kBackend_direct3d;
        } else {
            SendDialogMessageFormat(hWnd, IDC_OUTPUT, LB_ADDSTRING, 0, g_DirectDraw_displays[g_current_d3d_display].error);
        }
        PostMessageA(hWnd, 0x471, 0, 0);
        return 0;
    case 0x471:
        g_current_d3d_display = g_current_d3d_display + 1;
        if (g_current_d3d_display < g_count_directdraw_displays) {
            PostMessageA(hWnd, 0x470, 0, 0);
            return 0;
        }
        PostMessageA(hWnd, 0x472, 0, 0);
        return 0;
    case 0x472:
        Direct3DRefreshDeviceCombobox();
        FreeLibrary(g_ddraw_dll);
        PostMessageA(hWnd, 0x478, 0, 0);
        return 0;
    case 0x478:
        SendDialogMessageFormat(hWnd, IDC_OUTPUT, LB_ADDSTRING, 0, "3DFX GLIDE:");
        PostMessageA(hWnd, 0x479, 0, 0);
        return 0;
    case 0x479:
        g_glide2x_dll = LoadLibraryA("glide2x.dll");
        if (g_glide2x_dll == NULL) {
            PostMessageA(hWnd, 0x47c, 0, 0);
            return 0;
        }
        PostMessageA(hWnd, 0x47b, 0, 0);
        return 0;
    case 0x47b:
        FreeLibrary(g_glide2x_dll);
        EnableWindow(GetDlgItem(hWnd, IDC_GLIDE), 1);
        SendDlgItemMessageA(hWnd, IDC_GLIDE, BM_CLICK, 0, 0);
        SendDialogMessageFormat(hWnd, IDC_OUTPUT, LB_ADDSTRING, 0, "  3DFX Ok");
        g_Detected_backends |= 1 << kBackend_3dfx;
        g_Detection_state = 1;
        PostMessageA(hWnd, 0x482, 0, 0);
        PostMessageA(g_hWnd, 0x482, 0, 0);
        return 0;
    case 0x47c:
        SendDialogMessageFormat(hWnd, IDC_OUTPUT, LB_ADDSTRING, 0, "  Failed");
        PostMessageA(hWnd, 0x482, 0, 0);
        PostMessageA(g_hWnd, 0x482, 0, 0);
        return 0;
    case 0x482:
        EnableWindow(GetDlgItem(hWnd, IDC_README), 1);
        EnableWindow(GetDlgItem(hWnd, IDC_RECHECK), 1);
        if (g_Detection_state != 1) {
            ShowDetails(1);
            return 0;
        }
        EnableWindow(GetDlgItem(hWnd, IDC_OK), 1);
        SetFocus(GetDlgItem(hWnd, IDC_OK));
        return 0;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        default:
            return 0;
        case IDC_OK:
            DestroyWindow(hWnd);
            g_close_requested = kClose_StartGame;
            return 0;
        case IDC_CANCEL:
            DestroyWindow(hWnd);
            g_close_requested = kClose_Abort;
            return 0;
        case IDC_RECHECK:
            SendDialogMessageFormat(hWnd, IDC_OUTPUT, LB_ADDSTRING, 0, "       ");
            PostMessageA(g_Dialog, 0x483, 0, 0);
            return 0;
        case IDC_DIRECTDRAW:
            g_selected_backend = 1; /* directdraw */
            EnableD3DOptions(0);
            return 0;
        case IDC_GLIDE:
            g_selected_backend = 2; /* 3dfx */
            EnableD3DOptions(0);
            return 0;
        case IDC_DIRECT3D:
            g_selected_backend = 3; /* direct3d */
            EnableD3DOptions(1);
            return 0;
        case IDC_README:
            STARTUPINFOA startup_info;
            PROCESS_INFORMATION process_information;
            memset(&startup_info, 0, sizeof(startup_info));
            startup_info.cb = sizeof(startup_info);
            CreateProcessA(NULL, "notepad readme.txt", NULL, NULL, 0, 0, &g_environment, NULL, &startup_info, &process_information);
            return 0;
        case IDC_DRIVER:
            if (HIWORD(wParam) == 1) {
                Direct3DRefreshDeviceCombobox();
            }
            return 0;
        case IDC_DEVICE:
            if (HIWORD(wParam) != 1) {
                return 0;
            }
            g_current_d3d_device = SendDlgItemMessageA(g_Dialog, IDC_DEVICE, CB_GETCURSEL, 0, 0);
            if (g_DirectDraw_displays[g_current_d3d_display].devices[g_current_d3d_device].color_model == 0) {
                g_Flags &= 0xffffffaa;
                g_Flags |= 0x2a;
            } else if (!g_DirectDraw_displays[g_current_d3d_display].devices[g_current_d3d_device]. zbuffer_not_ok) {
                g_Flags &= 0xffffff9f;
                g_Flags |= 0x1f;
            } else {
                g_Flags &= 0xffffff9f;
                g_Flags |= 0x3f;
            }
            return 0;
        case IDC_ADVANCED_BUTTON:
            DialogBoxParamA(g_hInstance, MAKEINTRESOURCE(IDD_3DOPTIONS), hWnd, &D3DOptionsDlgProc, 0);
            return 0;
        case IDC_DETAILS_EXPAND:
            ShowDetails(1);
            return 0;
        case IDC_DETAILS_SHRINK:
            ShowDetails(0);
            return 0;
        case IDC_SAVE_OPTIONS:
            if (IsDlgButtonChecked(hWnd, IDC_SAVE_OPTIONS) == 1) {
                g_Flags |= kSaveOptions;
            } else {
                g_Flags &= ~kSaveOptions;
            }
            return 0;
        }
    }
}
