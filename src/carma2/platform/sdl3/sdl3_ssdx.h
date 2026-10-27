/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_SDL3_SSDX_H
#define REC2_SDL3_SSDX_H

#include <s3/s3.h>

enum tSSDXFlags {
    SSDX_InitDirectSound = 0x2,
};

extern int SSDXStart(void *phWnd, int pDirectDraw, int pEnumerateDDX, int pflags);

extern void SSDXStop(void);

extern void SSDXRelease(void);

#endif /* REC2_SDL3_SSDX_H */
