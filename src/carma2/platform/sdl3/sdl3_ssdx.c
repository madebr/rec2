/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "sdl3_ssdx.h"

tPD_S3_config gPD_S3_config;

int PDS3Init(void) {
    return 0;
}

void PDS3Stop(void) {
}

int SSDXStart(void *p_hWnd, int p_DirectDraw, int p_EnumerateDDX, int p_flags) {

    return 0;
}

void SSDXStop(void) {
}

const char *C2_HOOK_FASTCALL PDS3GetWorkingDirectory(void) {
    return ".";
}

int PDS3StopSampleChannel(tS3_channel *pChannel) {
    return 0;
}

void SSDXRelease(void) {
}

int PDS3UpdateChannelVolume(tS3_channel *pChannel) {
    return 0;
}

int PDS3UpdateChannelFrequency(tS3_channel *pChannel) {
    return 0;
}

int PDS3IsCDAPlaying(void) {
    return 0;
}
