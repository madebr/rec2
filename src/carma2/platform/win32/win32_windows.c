#include "win32_windows.h"

#include "win32.h"

#include "40-main.h"
#include "52-errors.h"
#include "70-packfile.h"
#include "platform.h"

#include <windows.h>

// FUNCTION: CARMA2_HW 0x0051cad0
void C2_HOOK_CDECL SDL3ServiceMessages(void) {
    MSG msg;

    dr_dprintf("Win32ServiceMessages() - START");
    while (1) {
        if (gWindowActiveState == 1) {
            SetForegroundWindow(gHWnd);
        }

        if (gWindowActiveState) {
            if (PeekMessageA(&msg, NULL, 0, 0, 1) == 0) {
                dr_dprintf("Win32ServiceMessages() - breaking cos PeekMessage() returned 0");
                break;
            }
        }
        if (!gWindowActiveState) {
            if (GetMessageA(&msg, NULL, 0, 0) == -1) {
                dr_dprintf("Win32ServiceMessages() - breaking cos GetMessage() returned -1");
                break;
            }
        }
        if (msg.message == WM_QUIT) {
            dr_dprintf("WM_QUIT received.");
            if (gWindowActiveState == 2) {
                dr_dprintf("Active, so lock the surface");
                dr_dprintf("QuitGame being called...");
                QuitGame();
            }
            PDShutdownSystem();
        }
        TranslateMessage(&msg);
        dr_dprintf("Win32ServiceMessages() - dispatching message...");
        DispatchMessageA(&msg);
    }
    dr_dprintf("Win32ServiceMessages() - END");
}


// FUNCTION: CARMA2_HW 0x0044c830
void C2_HOOK_FASTCALL PDDisposeActionReplayBuffer(void) {
    if (gActionReplayBufferFile != NULL) {
        PFfclose(gActionReplayBufferFile);
        gActionReplayBufferFile = NULL;
    }
}
