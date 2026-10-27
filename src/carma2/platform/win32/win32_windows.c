/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "win32_windows.h"

#include "win32.h"

#include "40-main.h"
#include "52-errors.h"
#include "70-packfile.h"
#include "platform.h"
#include "win32_ssdx.h"

#include <windows.h>

// FUNCTION: CARMA2_HW 0x0051cad0
// FUNCTION: CARMA2_SW 0x00517d60
void C2_HOOK_CDECL Win32ServiceMessages(void) {
    MSG msg;

#ifdef REC2_SW
    SSDXUnlockAttachedSurface();
#endif
    while (1) {
#ifdef REC2_HW
        if (gWindowActiveState == 1) {
            SetForegroundWindow(gHWnd);
        }
#endif

        if (gWindowActiveState) {
            if (PeekMessageA(&msg, NULL, 0, 0, TRUE) == 0) {
#ifdef REC2_HW
                dr_dprintf("Win32ServiceMessages() - breaking cos PeekMessage() returned 0");
#endif
                break;
            }
        }
        if (!gWindowActiveState) {
            if (GetMessageA(&msg, NULL, 0, 0) == -1) {
#ifdef REC2_HW
                dr_dprintf("Win32ServiceMessages() - breaking cos GetMessage() returned -1");
#endif
                break;
            }
        }
        if (msg.message == WM_QUIT) {
            dr_dprintf("WM_QUIT received.");
#ifdef REC2_HW
            if (gWindowActiveState == 2) {
#else
            if (gWindowActiveState != 0) {
#endif
                dr_dprintf("Active, so lock the surface");
#ifdef REC2_SW
                FUN_005171c0();
#endif
                dr_dprintf("QuitGame being called...");
                QuitGame();
            }
            PDShutdownSystem();
        }
        TranslateMessage(&msg);
#ifdef REC2_HW
        dr_dprintf("Win32ServiceMessages() - dispatching message...");
#endif
        DispatchMessageA(&msg);
    }
#ifdef REC2_HW
    dr_dprintf("Win32ServiceMessages() - END");
#endif
#ifdef REC2_SW
    FUN_005171c0();
#endif
}


// FUNCTION: CARMA2_HW 0x0044c830
// FUNCTION: CARMA2_SW 0x0044c5e0
void C2_HOOK_FASTCALL PDDisposeActionReplayBuffer(void) {
    if (gActionReplayBufferFile != NULL) {
        PFfclose(gActionReplayBufferFile);
        gActionReplayBufferFile = NULL;
    }
}
