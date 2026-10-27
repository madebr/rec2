/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "64-movie.h"

#include "16-graphics1.h"
#include "25-grafdata.h"
#include "41-utility.h"
#include "globvars.h"
#include "platform.h"

#include <QTML.h>
#include <Movies.h>

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x006a0c24
// GLOBAL: CARMA2_SW 0x0069ac7c
int gQuickTime_initialized;

// GLOBAL: CARMA2_HW 0x006a0c50
// GLOBAL: CARMA2_SW 0x0069aca8
int gNext_replay_record_state;

// GLOBAL: CARMA2_HW 0x006a23b8
// GLOBAL: CARMA2_SW 0x0069c410
int gShow_status_message;

// GLOBAL: CARMA2_HW 0x00705440
// GLOBAL: CARMA2_SW 0x006fbf00
char gQuick_time_temp_path[256];

// GLOBAL: CARMA2_HW 0x00705240
// GLOBAL: CARMA2_SW 0x006fbd00
char gQuick_time_movie_path_stub[256];

// GLOBAL: CARMA2_HW 0x00705340
// GLOBAL: CARMA2_SW 0x006fbe00
char gQuick_time_banner_texture_name[256];

// GLOBAL: CARMA2_HW 0x0065ee50
char gDefault_quick_time_quality[] = "normal";

// GLOBAL: CARMA2_HW 0x0065ed30
char gDefault_quick_time_compressor[] = "animation";

// Create555BELookupTable

// Convert565BrPixelmapTo555BigEndianPixmap

// SwapPixelmapBytePairs

// WriteBannerFrame

// Allocate555BackScreen

// GetCompressorQualityFromOptions

// GetCompressorTypeFromOptions

// InitMovie

// CreateMovie

// MovieStartNewMovie

// MovieStopRecordingAndSave

// FreeOffQTshite

// FUNCTION: CARMA2_HW 0x004e1700
// FUNCTION: CARMA2_SW 0x004dda00
void C2_HOOK_CDECL InitQuickTimeStuff(void) {

    gQuickTime_initialized = 0;
    if (InitializeQTML(0) == noErr && EnterMovies() == noErr) {//
        gQuickTime_initialized = 1;
    }
    gQuick_time_temp_path[0] = '\0';
    gQuick_time_movie_path_stub[0] = '\0';
    gQuick_time_banner_texture_name[0] = '\0';
}

// FUNCTION: CARMA2_HW 0x004e1740
// FUNCTION: CARMA2_SW 0x004dda40
void C2_HOOK_FASTCALL SetQuickTimeDefaults(void) {

    if (gQuick_time_quality[0] == '\0') {
        strcpy(gQuick_time_quality, gDefault_quick_time_quality);
    }
    if (gQuick_time_compressor[0] == '\0') {
        strcpy(gQuick_time_compressor, gDefault_quick_time_compressor);
    }
    if (gQuick_time_temp_path[0] == '\0') {
        PathCat(gQuick_time_temp_path, gApplication_path, "QTTMP");
    }
    if (gQuick_time_movie_path_stub[0] == '\0') {
        PathCat(gQuick_time_movie_path_stub, gApplication_path, "MOVIE");
    }
    if (gQuick_time_banner_texture_name[0] == '\0') {
        strcpy(gQuick_time_banner_texture_name, "ARBANNER.PIX");
    }
}

// MovieRecordButtonPressed

// STUB: CARMA2_HW 0x004e1a20
// STUB: CARMA2_SW 0x004ddd20
void C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void) {
    NOT_IMPLEMENTED();
}

// GenerateOneMovieFrame

// MovieRecordFrame

// FUNCTION: CARMA2_HW 0x004e2100
// FUNCTION: CARMA2_SW 0x004de480
int C2_HOOK_FASTCALL MovieRecording(void) {
    return gNext_replay_record_state;
}

// FUNCTION: CARMA2_HW 0x004e2110
// FUNCTION: CARMA2_SW 0x004de490
void C2_HOOK_FASTCALL KillStatusMessage(void) {

    gShow_status_message = 0;
}

// ShowStatusMessage

// FUNCTION: CARMA2_HW 0x004e2140
// FUNCTION: CARMA2_SW 0x004de4c0
int C2_HOOK_FASTCALL GetMovieWidth(void) {

    if (gNbPixelBits == 16) {
        return gCurrent_graf_data->width / 2;
    } else {
        return gCurrent_graf_data->width;
    }
}

// FUNCTION: CARMA2_HW 0x004e2160
// FUNCTION: CARMA2_SW 0x004de4e0
int C2_HOOK_FASTCALL GetMovieHeight(void) {

    if (gNbPixelBits == 16) {
        return gCurrent_graf_data->height / 2;
    } else {
        return gCurrent_graf_data->height;
    }
}

// FUNCTION: CARMA2_HW 0x004e2190
// FUNCTION: CARMA2_SW 0x004de510
void C2_HOOK_FASTCALL TidyMovieARDisplay(void) {
    int old_state;

    old_state = gNext_replay_record_state;
    gNext_replay_record_state = 20;
    RenderAFrame(1);
    RenderAFrame(1);
    gNext_replay_record_state = old_state;
}

// TrapS3SoundOutput

// FreeS3SoundOutput

