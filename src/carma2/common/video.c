#include "video.h"

#include "globvars.h"
#include "grafdata.h"
#include "graphics.h"
#include "platform.h"

#include <FileTypesAndCreators.h>
#include <GXMath.h>
#include <Movies.h>
#include <QTML.h>
#include <Script.h>
#include <TextUtils.h>

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gQuickTime_initialized, 0x006a0c24);
C2_HOOK_VARIABLE_IMPLEMENT(int, gShow_status_message, 0x006a23b8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gStatus_message_1, 0x006a23bc);
C2_HOOK_VARIABLE_IMPLEMENT(int, gStatus_message_2, 0x006a23c0);
C2_HOOK_VARIABLE_IMPLEMENT(undefined4, gNext_replay_record_state, 0x006a0c50);
C2_HOOK_VARIABLE_IMPLEMENT(short, gQuicktime_movie_file, 0x006a0af0);
C2_HOOK_VARIABLE_IMPLEMENT(Movie, gQuicktime_movie, 0x006a0c30);
C2_HOOK_VARIABLE_IMPLEMENT(Handle, gQuicktime_frame_pixmap_handle, 0x006a0c34);
C2_HOOK_VARIABLE_IMPLEMENT(Handle, gQuicktime_image_description_handle, 0x006a0c40);
C2_HOOK_VARIABLE_IMPLEMENT(Handle, gQuicktime_compressed_frame_handle, 0x006a0c3c);
C2_HOOK_VARIABLE_IMPLEMENT(ImageSequence, gQuicktime_image_sequence, 0x006a0af4);
C2_HOOK_VARIABLE_IMPLEMENT(Media, gQuicktime_media, 0x006a0afc);
C2_HOOK_VARIABLE_IMPLEMENT(Track, gQuicktime_track, 0x006a0c10);
C2_HOOK_VARIABLE_IMPLEMENT(int, gQuicktime_movie_counter, 0x006a0c2c);
C2_HOOK_VARIABLE_IMPLEMENT(struct FSSpec, gQt_file_spec, 0x006a0b08);
C2_HOOK_VARIABLE_IMPLEMENT(struct Rect, gQuicktime_rect, 0x006a0c18);

void (C2_HOOK_CDECL * InitQuickTimeStuff_original)(void);
void C2_HOOK_CDECL InitQuickTimeStuff(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    InitQuickTimeStuff_original();
#else
    C2V(gQuickTime_initialized) = 0;
    if (InitializeQTML(0) == noErr && EnterMovies() == noErr) {
        C2V(gQuickTime_initialized) = 1;
    }
    C2V(gQuick_time_temp_path)[0] = '\0';
    C2V(gQuick_time_movie_path_stub)[0] = '\0';
    C2V(gQuick_time_banner_texture_name)[0] = '\0';
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e1700, InitQuickTimeStuff, InitQuickTimeStuff_original)

void C2_HOOK_FASTCALL KillStatusMessage(void) {

    C2V(gShow_status_message) = 0;
}
C2_HOOK_FUNCTION(0x004e2110, KillStatusMessage)

void (C2_HOOK_FASTCALL * WriteBannerFrame_original)(void);
void C2_HOOK_FASTCALL WriteBannerFrame(void) {
#if defined(C2_HOOKS_ENABLED)
    WriteBannerFrame_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e1250, WriteBannerFrame, WriteBannerFrame_original)

void C2_HOOK_FASTCALL ShowStatusMessage(int msg1, int msg2) {

    C2V(gShow_status_message) = 1;
    C2V(gStatus_message_1) = msg1;
    C2V(gStatus_message_2) = msg2;
}

void C2_HOOK_FASTCALL TidyMovieARDisplay(void) {
    undefined4 state;

    state = C2V(gNext_replay_record_state);
    C2V(gNext_replay_record_state) = 20;
    RenderAFrame(1);
    RenderAFrame(1);
    C2V(gNext_replay_record_state) = state;
}

void C2_HOOK_FASTCALL FreeOffQTshite(void) {
    tPath_name path;

    if (C2V(gQuicktime_movie_file) != 0) {
        CloseMovieFile(C2V(gQuicktime_movie_file));
    }
    DisposeMovie(C2V(gQuicktime_movie));
    PDFileDelete(C2V(gQuick_time_temp_path), 1);
    c2_sprintf(path, "%s.#Res", C2V(gQuick_time_temp_path));
    PDFileDelete(path, 1);
    if (C2V(gQuicktime_frame_pixmap_handle) != NULL) {
        DisposeHandle(C2V(gQuicktime_frame_pixmap_handle));
        C2V(gQuicktime_frame_pixmap_handle) = NULL;
    }
    if (C2V(gQuicktime_image_description_handle) != NULL) {
        DisposeHandle(C2V(gQuicktime_image_description_handle));
        C2V(gQuicktime_image_description_handle) = NULL;
    }
    if (C2V(gQuicktime_compressed_frame_handle) != NULL) {
        DisposeHandle(C2V(gQuicktime_compressed_frame_handle));
        C2V(gQuicktime_compressed_frame_handle) = NULL;
    }
}

int C2_HOOK_FASTCALL MovieStopRecordingAndSave(void) {

    if (C2V(gNext_replay_record_state) == 4) {
        struct FSSpec fs_spec;
        TimeValue duration;
        tPath_name path;

        C2V(gNext_replay_record_state) = 5;
        ShowStatusMessage(eMiscString_post_processing, eMiscString_please_wait);
        TidyMovieARDisplay();
        WriteBannerFrame();
        C2V(gNext_replay_record_state) = 0;
        CDSequenceEnd(C2V(gQuicktime_image_sequence));
        EndMediaEdits(C2V(gQuicktime_media));
        duration = GetMediaDuration(C2V(gQuicktime_media));
        if (duration != 0) {
            InsertMediaIntoTrack(C2V(gQuicktime_track), 0, 0, duration, IntToFixed(1));
        }
        if (C2V(gQuicktime_movie_file) != 0) {
            CloseMovieFile(C2V(gQuicktime_movie_file));
        }
        c2_sprintf(path, "%s%3.3d.MOV", C2V(gQuick_time_movie_path_stub), C2V(gQuicktime_movie_counter));
        C2V(gQuicktime_movie_counter) += 1;
        fs_spec.vRefNum = 0;
        fs_spec.parID = 0;
        c2_strcpy(fs_spec.name, path);
        c2pstr(fs_spec.name);
        FlattenMovieData(C2V(gQuicktime_movie), flattenAddMovieToDataFork, &fs_spec, sigMoviePlayer, smSystemScript, createMovieFileDeleteCurFile);
        FreeOffQTshite();
    }
    return 1;
}

void (C2_HOOK_FASTCALL * MovieStopRecordingIfNecessary_original)(void);
void C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    ActionReplayFinishRecording_original();
#else
    MovieStopRecordingAndSave();
    KillStatusMessage();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e1a20, MovieStopRecordingIfNecessary, MovieStopRecordingIfNecessary_original)

int C2_HOOK_FASTCALL GetMovieWidth(void) {

    if (C2V(gNbPixelBits) == 16) {
        return C2V(gCurrent_graf_data)->width / 2;
    } else {
        return C2V(gCurrent_graf_data)->width;
    }
}
C2_HOOK_FUNCTION(0x004e2140, GetMovieWidth)

int C2_HOOK_FASTCALL GetMovieHeight(void) {

    if (C2V(gNbPixelBits) == 16) {
        return C2V(gCurrent_graf_data)->height / 2;
    } else {
        return C2V(gCurrent_graf_data)->height;
    }
}
C2_HOOK_FUNCTION(0x004e2160, GetMovieHeight)

int C2_HOOK_FASTCALL InitMovie(void) {

    C2V(gQuicktime_rect).top = 0;
    C2V(gQuicktime_rect).left = 0;
    C2V(gQuicktime_rect).right = GetMovieWidth();
    C2V(gQuicktime_rect).bottom = GetMovieHeight();
    if (CreateMovieFile(&C2V(gQt_file_spec), sigMoviePlayer, smCurrentScript, createMovieFileDeleteCurFile, &C2V(gQuicktime_movie_file), &C2V(gQuicktime_movie)) == noErr) {
        return 0;
    }
    PathCat(C2V(gQuick_time_temp_path), C2V(gApplication_path), "QTTMP");
    PathCat(C2V(gQuick_time_movie_path_stub), C2V(gApplication_path), "MOVIE");
    if (!CreateMovieFile(&C2V(gQt_file_spec), sigMoviePlayer, smCurrentScript, createMovieFileDeleteCurFile, &C2V(gQuicktime_movie_file), &C2V(gQuicktime_movie)) == noErr) {
        return 0;
    }
    return 1;
}
C2_HOOK_FUNCTION(0x004e0f20, InitMovie)

int (C2_HOOK_FASTCALL * CreateMovie_original)(void);
int C2_HOOK_FASTCALL CreateMovie(void) {

#if defined(C2_HOOKS_ENABLED)
    return CreateMovie_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e0fe0, CreateMovie, CreateMovie_original)

int (C2_HOOK_FASTCALL * GenerateOneMovieFrame_original)(void);
int C2_HOOK_FASTCALL GenerateOneMovieFrame(void) {

#if defined(C2_HOOKS_ENABLED)
    return GenerateOneMovieFrame_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e1c70, GenerateOneMovieFrame, GenerateOneMovieFrame_original)
