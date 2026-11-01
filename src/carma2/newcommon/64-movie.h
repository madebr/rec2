/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_64_MOVIE_H
#define GUARD_64_MOVIE_H

#include "c2_hooks.h"

extern int gNext_replay_record_state;
extern int gShow_status_message;

extern int gShow_status_message;
extern int gStatus_message_1;
extern int gStatus_message_2;

extern char gQuick_time_temp_path[256];
extern char gQuick_time_movie_path_stub[256];
extern char gQuick_time_banner_texture_name[256];

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

extern void C2_HOOK_CDECL InitQuickTimeStuff(void);

extern void C2_HOOK_FASTCALL SetQuickTimeDefaults(void);

// MovieRecordButtonPressed

extern void C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void);

// GenerateOneMovieFrame

// MovieRecordFrame

extern int C2_HOOK_FASTCALL MovieRecording(void);

extern void C2_HOOK_FASTCALL KillStatusMessage(void);

extern void C2_HOOK_FASTCALL ShowStatusMessage(int pMessage1, int pMessage2);

extern int C2_HOOK_FASTCALL GetMovieWidth(void);

extern int C2_HOOK_FASTCALL GetMovieHeight(void);

extern void C2_HOOK_FASTCALL TidyMovieARDisplay(void);

// TrapS3SoundOutput

// FreeS3SoundOutput

#endif // GUARD_64_MOVIE_H
