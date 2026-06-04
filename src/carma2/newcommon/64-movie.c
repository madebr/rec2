#include "64-movie.h"

#include "41-utility.h"
#include "globvars.h"

#include <QTML.h>
#include <Movies.h>

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x006a0c24
int gQuickTime_initialized;

// GLOBAL: CARMA2_HW 0x00705440
char gQuick_time_temp_path[256];

// GLOBAL: CARMA2_HW 0x00705240
char gQuick_time_movie_path_stub[256];

// GLOBAL: CARMA2_HW 0x00705340
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
void C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void) {
    NOT_IMPLEMENTED();
}

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

