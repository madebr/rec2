/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "72-interface-netsync.h"

#include "72-interface.h"


// GLOBAL: CARMA2_HW 0x0061bad0
// GLOBAL: CARMA2_SW 0x00616a00
tFrontend_spec gFrontend_NETSYNC = {
#if 0 // FIXME: enable this
    "NetSync",
    0,
    28,
    NetSync_Infunc,
    NULL,
    NetSync_MenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    0,
    6,
    0,
    {
        { 0xab,     temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x404,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402,    temp,           NULL, 0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xe,      NetSync_Start,  NULL, 1, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x15,     NetSync_Abort,  NULL, 1, 17, 18, 0, 0, 0, 0, 1, 1, },
    }
#else
    0
#endif
};
