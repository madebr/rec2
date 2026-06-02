#ifndef GUARD_64_MOVIE_H
#define GUARD_64_MOVIE_H

#include "c2_hooks.h"

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

// InitQuickTimeStuff

// SetQuickTimeDefaults

// MovieRecordButtonPressed

extern void C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void);

// GenerateOneMovieFrame

// MovieRecordFrame

// MovieRecording

// KillStatusMessage

// ShowStatusMessage

// GetMovieWidth

// GetMovieHeight

// TidyMovieARDisplay

// TrapS3SoundOutput

// FreeS3SoundOutput

#endif // GUARD_64_MOVIE_H
