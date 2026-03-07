#include "video.h"

#include "globvars.h"
#include "grafdata.h"
#include "graphics.h"
#include "platform.h"
#include "utility.h"

#include <FileTypesAndCreators.h>
#include <GXMath.h>
#include <Movies.h>
#include <QTML.h>
#include <Script.h>
#include <TextUtils.h>

#include "c2_string.h"


// GLOBAL: CARMA2_HW 0x006a0c24
int gQuickTime_initialized;

// GLOBAL: CARMA2_HW 0x006a23b8
int gShow_status_message;

// GLOBAL: CARMA2_HW 0x006a23bc
int gStatus_message_1;

// GLOBAL: CARMA2_HW 0x006a23c0
int gStatus_message_2;

// GLOBAL: CARMA2_HW 0x006a0c50
undefined4 gNext_replay_record_state;

// GLOBAL: CARMA2_HW 0x006a0af0
short gQuicktime_movie_file;

// GLOBAL: CARMA2_HW 0x006a0c30
Movie gQuicktime_movie;

// GLOBAL: CARMA2_HW 0x006a0c34
Handle gQuicktime_frame_pixmap_handle;

// GLOBAL: CARMA2_HW 0x006a0c40
Handle gQuicktime_image_description_handle;

// GLOBAL: CARMA2_HW 0x006a0c3c
Handle gQuicktime_compressed_frame_handle;

// GLOBAL: CARMA2_HW 0x006a0af4
ImageSequence gQuicktime_image_sequence;

// GLOBAL: CARMA2_HW 0x006a0afc
Media gQuicktime_media;

// GLOBAL: CARMA2_HW 0x006a0c10
Track gQuicktime_track;

// GLOBAL: CARMA2_HW 0x006a0c2c
int gQuicktime_movie_counter;

// GLOBAL: CARMA2_HW 0x006a0b08
struct FSSpec gQt_file_spec;

// GLOBAL: CARMA2_HW 0x006a0c18
struct Rect gQuicktime_rect;

// GLOBAL: CARMA2_HW 0x006a0c28
int gRecording_paused;

// GLOBAL: CARMA2_HW 0x006a0c54
tU32 gRecording_time;

// GLOBAL: CARMA2_HW 0x006baa28
int gRecording_mouse_disabled;


// FUNCTION: CARMA2_HW 0x004e1700
void C2_HOOK_CDECL InitQuickTimeStuff(void) {

    gQuickTime_initialized = 0;
    if (InitializeQTML(0) == noErr && EnterMovies() == noErr) {
        gQuickTime_initialized = 1;
    }
    gQuick_time_temp_path[0] = '\0';
    gQuick_time_movie_path_stub[0] = '\0';
    gQuick_time_banner_texture_name[0] = '\0';
}

// FUNCTION: CARMA2_HW 0x004e2110
void C2_HOOK_FASTCALL KillStatusMessage(void) {

    gShow_status_message = 0;
}

// FUNCTION: CARMA2_HW 0x004e1250
void C2_HOOK_FASTCALL WriteBannerFrame(void) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL ShowStatusMessage(int msg1, int msg2) {

    gShow_status_message = 1;
    gStatus_message_1 = msg1;
    gStatus_message_2 = msg2;
}

void C2_HOOK_FASTCALL TidyMovieARDisplay(void) {
    undefined4 state;

    state = gNext_replay_record_state;
    gNext_replay_record_state = 20;
    RenderAFrame(1);
    RenderAFrame(1);
    gNext_replay_record_state = state;
}

void C2_HOOK_FASTCALL FreeOffQTshite(void) {
    tPath_name path;

    if (gQuicktime_movie_file != 0) {
        CloseMovieFile(gQuicktime_movie_file);
    }
    DisposeMovie(gQuicktime_movie);
    PDFileDelete(gQuick_time_temp_path, 1);
    sprintf(path, "%s.#Res", gQuick_time_temp_path);
    PDFileDelete(path, 1);
    if (gQuicktime_frame_pixmap_handle != NULL) {
        DisposeHandle(gQuicktime_frame_pixmap_handle);
        gQuicktime_frame_pixmap_handle = NULL;
    }
    if (gQuicktime_image_description_handle != NULL) {
        DisposeHandle(gQuicktime_image_description_handle);
        gQuicktime_image_description_handle = NULL;
    }
    if (gQuicktime_compressed_frame_handle != NULL) {
        DisposeHandle(gQuicktime_compressed_frame_handle);
        gQuicktime_compressed_frame_handle = NULL;
    }
}

int C2_HOOK_FASTCALL MovieStopRecordingAndSave(void) {

    if (gNext_replay_record_state == 4) {
        struct FSSpec fs_spec;
        TimeValue duration;
        tPath_name path;

        gNext_replay_record_state = 5;
        ShowStatusMessage(eMiscString_post_processing, eMiscString_please_wait);
        TidyMovieARDisplay();
        WriteBannerFrame();
        gNext_replay_record_state = 0;
        CDSequenceEnd(gQuicktime_image_sequence);
        EndMediaEdits(gQuicktime_media);
        duration = GetMediaDuration(gQuicktime_media);
        if (duration != 0) {
            InsertMediaIntoTrack(gQuicktime_track, 0, 0, duration, IntToFixed(1));
        }
        if (gQuicktime_movie_file != 0) {
            CloseMovieFile(gQuicktime_movie_file);
        }
        sprintf(path, "%s%3.3d.MOV", gQuick_time_movie_path_stub, gQuicktime_movie_counter);
        gQuicktime_movie_counter += 1;
        fs_spec.vRefNum = 0;
        fs_spec.parID = 0;
        strcpy(fs_spec.name, path);
        c2pstr(fs_spec.name);
        FlattenMovieData(gQuicktime_movie, flattenAddMovieToDataFork, &fs_spec, sigMoviePlayer, smSystemScript, createMovieFileDeleteCurFile);
        FreeOffQTshite();
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004e1a20
void C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void) {

    MovieStopRecordingAndSave();
    KillStatusMessage();
}

// FUNCTION: CARMA2_HW 0x004e2140
int C2_HOOK_FASTCALL GetMovieWidth(void) {

    if (gNbPixelBits == 16) {
        return gCurrent_graf_data->width / 2;
    } else {
        return gCurrent_graf_data->width;
    }
}

// FUNCTION: CARMA2_HW 0x004e2160
int C2_HOOK_FASTCALL GetMovieHeight(void) {

    if (gNbPixelBits == 16) {
        return gCurrent_graf_data->height / 2;
    } else {
        return gCurrent_graf_data->height;
    }
}

// FUNCTION: CARMA2_HW 0x004e0f20
int C2_HOOK_FASTCALL InitMovie(void) {

    gQuicktime_rect.top = 0;
    gQuicktime_rect.left = 0;
    gQuicktime_rect.right = GetMovieWidth();
    gQuicktime_rect.bottom = GetMovieHeight();
    if (CreateMovieFile(&gQt_file_spec, sigMoviePlayer, smCurrentScript, createMovieFileDeleteCurFile, &gQuicktime_movie_file, &gQuicktime_movie) == noErr) {
        return 0;
    }
    PathCat(gQuick_time_temp_path, gApplication_path, "QTTMP");
    PathCat(gQuick_time_movie_path_stub, gApplication_path, "MOVIE");
    if (!CreateMovieFile(&gQt_file_spec, sigMoviePlayer, smCurrentScript, createMovieFileDeleteCurFile, &gQuicktime_movie_file, &gQuicktime_movie) == noErr) {
        return 0;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004e0fe0
int C2_HOOK_FASTCALL CreateMovie(void) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e1c70
int C2_HOOK_FASTCALL GenerateOneMovieFrame(void) {

    NOT_IMPLEMENTED();
    return 0;
}

void C2_HOOK_FASTCALL TrapS3SoundOutput(void) {
}

void C2_HOOK_FASTCALL FreeS3SoundOutput(void) {
}

// FUNCTION: CARMA2_HW 0x004e1da0
int C2_HOOK_FASTCALL MovieRecordFrame(br_pixelmap *pScreen, int pFrame_period) {
    tU32 remaining;

    switch (gNext_replay_record_state) {
    case 1:
        gNext_replay_record_state = 2;
        return 0;
    case 2:
        WaitFor(2000);
        if (InitMovie()) {
            gNext_replay_record_state = 10;
            ShowStatusMessage(eMiscString_movie_error, eMiscString_aborting);
            return 0;
        }
        gNext_replay_record_state = 3;
        ShowStatusMessage(eMiscString_initialising_movie, eMiscString_please_wait);
        return 0;
    case 3:
        if (CreateMovie()) {
            gNext_replay_record_state = 10;
            ShowStatusMessage(eMiscString_movie_error, eMiscString_aborting);
            return 0;
        }
        gNext_replay_record_state = 4;
        ShowStatusMessage(eMiscString_recording_movie, 0);
        TrapS3SoundOutput();
        return 0;
    case 4:
        if (!gRecording_paused) {
            remaining = gRecording_time + pFrame_period;
            gRecording_paused = 0;
            for (; remaining > 1000 / 18; remaining -= 1000 / 18) {
                if (GenerateOneMovieFrame() != 0) {
                    gNext_replay_record_state = 10;
                    ShowStatusMessage(eMiscString_movie_error, eMiscString_aborting);
                    return 0;
                }
            }
            gRecording_time = remaining;
            return 0;
        }
        break;
    case 5:
        gRecording_mouse_disabled = 0;
        return 0;
    case 10:
        WaitFor(2000);
        FreeOffQTshite();
        FreeS3SoundOutput();
        return 0;
    }
    return 0;
}
