#ifndef GUARD_64_MOVIE_H
#define GUARD_64_MOVIE_H

#include "c2_hooks.h"

extern int gNext_replay_record_state;

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

// KillStatusMessage

// ShowStatusMessage

extern int C2_HOOK_FASTCALL GetMovieWidth(void);

extern int C2_HOOK_FASTCALL GetMovieHeight(void);

extern void C2_HOOK_FASTCALL TidyMovieARDisplay(void);

// TrapS3SoundOutput

// FreeS3SoundOutput

#endif // GUARD_64_MOVIE_H
