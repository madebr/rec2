#include "password.h"

#include "global.h"
#include "util.h"
#include "resources.h"

#include <windows.h>

int g_parental_lock;
static char g_Password[32];
static char g_Password_config[32];

static void StringNibbleSwap(char* pStr) {
    char c;

    while ((c = *pStr) != '\0') {
        *pStr = (c << 4) | (c >> 4);
        ++pStr;
    }
}

INT_PTR CALLBACK Password1stUseDlgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {

    switch (Msg) {
    case WM_INITDIALOG:
        CenterWindowOnScreen(hWnd);
        ShowWindow(hWnd, SW_SHOW);
        g_parental_lock = -1;
        return 1;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_OK:
            g_parental_lock = 0;
            if (SendDlgItemMessageA(hWnd, IDC_YES, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                g_parental_lock = 1;
            }
            EndDialog(hWnd, 0);
            return 0;
        case IDC_CANCEL:
            g_parental_lock = 0;
            EndDialog(hWnd, 0);
            return 0;
        case IDC_YES:
            EnableWindow(GetDlgItem(hWnd, IDC_OK), 1);
            return 0;
        case IDC_NEVER:
            EnableWindow(GetDlgItem(hWnd, IDC_OK), 1);
            return 0;
        }
    }
    return 0;
}


INT_PTR CALLBACK PasswordSetupDlgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {

    switch (Msg) {
    case WM_CREATE:
        CenterWindowOnScreen(hWnd);
        ShowWindow(hWnd, SW_SHOW);
        g_parental_lock = -1;
        SetFocus(GetDlgItem(hWnd, IDC_PASSWORD));
        return 1;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_OK:
            if (SendDlgItemMessageA(hWnd, IDC_YES, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                g_parental_lock = 1;
            }
            GetDlgItemTextA(hWnd, IDC_PASSWORD, g_Password_config, sizeof(g_Password_config)-1);
            GetDlgItemTextA(hWnd, IDC_PASSWORD2, g_Password, sizeof(g_Password)-1);
            if (strcmp(g_Password_config, g_Password) == 0) {
                g_parental_lock = 1;
                EndDialog(hWnd, 0);
            }
            return 0;
        case IDC_CANCEL:
            g_parental_lock = -1;
            EndDialog(hWnd, 0);
            return 0;
        }
    }
    return 0;
}


INT_PTR CALLBACK ParentalLockDlgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {

    switch (Msg) {
    case WM_CLOSE:
        goto cancel;
    case WM_INITDIALOG:
        CenterWindowOnScreen(hWnd);
        ShowWindow(GetDlgItem(hWnd, IDC_OK), SW_HIDE);
        ShowWindow(GetDlgItem(hWnd, IDC_QUIT), SW_SHOW);
        ShowWindow(hWnd, SW_SHOW);
        g_parental_lock = -1;
        return 1;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_OK:
            if (SendDlgItemMessageA(hWnd, IDC_YES, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                g_parental_lock = 1;
            }
            GetDlgItemTextA(hWnd, IDC_PASSWORD, g_Password_config, sizeof(g_Password_config) - 1);
            if (strlen(g_Password_config) < 2) {
                return 0;
            }
            if (strcmp(g_Password, g_Password_config) != 0) {
                return 0;
            }
            g_parental_lock = 1;
            EndDialog(hWnd, 0);
            return 0;
        case IDC_PASSWORD:
            if (HIWORD(wParam) != 0x300) {
                return 0;
            }
            GetDlgItemTextA(hWnd, IDC_PASSWORD, g_Password_config, sizeof(g_Password_config) - 1);
            if (strlen(g_Password_config) > 1 && strcmp(g_Password, g_Password_config) == 0) {
                ShowWindow(GetDlgItem(hWnd, IDC_OK), SW_SHOW);
                ShowWindow(GetDlgItem(hWnd, IDC_QUIT), SW_HIDE);
                return 0;
            } else {
                ShowWindow(GetDlgItem(hWnd, IDC_OK), SW_HIDE);
                ShowWindow(GetDlgItem(hWnd, IDC_QUIT), SW_SHOW);
                return 0;
            }
        case IDC_QUIT:
            goto cancel;
        default:
            return 0;
        }
    default:
        return 0;
    }
cancel:
    g_blood = 0;
    g_parental_lock = -2;
    EndDialog(hWnd, 0);
    return 0;
}

void SetupPassword(HKEY hKey, HWND hWnd) {

    DialogBoxParamA(g_hInstance,MAKEINTRESOURCE(IDD_PASSWORD1STUSE), hWnd, Password1stUseDlgProc, 0);
    if (g_parental_lock == -1) {
        return;
    }
    if (g_parental_lock == 0) {
        RegSetValueExA(hKey, "Password", 0, REG_DWORD_LITTLE_ENDIAN, (BYTE*)&g_parental_lock, sizeof(g_parental_lock));
        return;
    }
    DialogBoxParamA(g_hInstance, MAKEINTRESOURCE(IDD_PASSWORDSETUP),hWnd, PasswordSetupDlgProc, 0);
    if (g_parental_lock == 1) {
        StringNibbleSwap(g_Password_config);
        RegSetValueExA(hKey, "Password", 0, REG_SZ, (BYTE*)g_Password_config, strlen(g_Password_config));
    }
}

int AskPassword(HWND hWnd) {
    HKEY hKey;
    DWORD size;
    DWORD type;

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\SCI\\Carmageddon2", 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS) {
        RegCreateKeyExA(HKEY_LOCAL_MACHINE, "Software\\SCI\\Carmageddon2", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, NULL);
        SetupPassword(hKey, hWnd);
        RegCloseKey(hKey);
    } else {
        size = sizeof(g_Password);
        if (RegQueryValueExA(hKey, "Password", NULL, &type, (LPBYTE)g_Password, &size) != ERROR_SUCCESS) {
            SetupPassword(hKey, hWnd);
            RegCloseKey(hKey);
            RegCloseKey(hKey);
            return 0;
        }
        if (type == REG_SZ) {
            StringNibbleSwap(g_Password);
            DialogBoxParamA(g_hInstance, MAKEINTRESOURCE(IDD_ENTERPASSWORD), hWnd, ParentalLockDlgProc, 0);
            RegCloseKey(hKey);
            if (g_parental_lock == -1) {
                g_close_requested = kClose_Abort;
                return -1;
            }
        }
    }
    return 0;
}
