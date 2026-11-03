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
#include "63-loading3.h"
#include "globvars.h"
#include "platform.h"

#include <QTML.h>
#include <Movies.h>

#include "c2_string.h"

// 41-utility.h only declares these for REC2_HW; the software build resolves them to
// carma2/common/graphics.c, so the declaration is repeated here for every build.
extern void C2_HOOK_FASTCALL PossibleLock(int pValue);
extern int C2_HOOK_FASTCALL PossibleUnlock(int pValue);

// GLOBAL: CARMA2_HW 0x006a0c24
// GLOBAL: CARMA2_SW 0x0069ac7c
int gQuickTime_initialized;

// GLOBAL: CARMA2_HW 0x006a0c50
// GLOBAL: CARMA2_SW 0x0069aca8
int gNext_replay_record_state;

// GLOBAL: CARMA2_HW 0x006a23b8
// GLOBAL: CARMA2_SW 0x0069c410
int gShow_status_message;

// GLOBAL: CARMA2_HW 0x006a23bc
// GLOBAL: CARMA2_SW 0x0069c414
int gStatus_message_1;

// GLOBAL: CARMA2_HW 0x006a23c0
// GLOBAL: CARMA2_SW 0x0069c418
int gStatus_message_2;

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
// GLOBAL: CARMA2_SW 0x00659690
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
    OSErr err;

    gQuickTime_initialized = 0;
    err = InitializeQTML(0);
    if (err == noErr) {
        err = EnterMovies();
        if (err == noErr) {
            gQuickTime_initialized = 1;
        }
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

// FUNCTION: CARMA2_HW 0x004e2120
// FUNCTION: CARMA2_SW 0x004de4a0
void C2_HOOK_FASTCALL ShowStatusMessage(int pMessage1, int pMessage2) {

    gShow_status_message = 1;
    gStatus_message_1 = pMessage1;
    gStatus_message_2 = pMessage2;
}

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

// GLOBAL: CARMA2_HW 0x006a0c18
struct Rect gQuicktime_rect;

// GLOBAL: CARMA2_HW 0x0065ef2c
int gQuicktime_frame_flags = 1;

// GLOBAL: CARMA2_HW 0x006a0c44
br_uint_16* gQuicktime_frame_pixels;

// GLOBAL: CARMA2_HW 0x006a0c48
br_uint_16* gQuicktime_frame_pixel_conversion_table;

// GLOBAL: CARMA2_HW 0x006736a8
void* gQuickTime_dispatcher;

// GLOBAL: CARMA2_HW 0x006736b4
void* gQuickTime_dispatch;

extern br_pixelmap* gRender_palette;
extern void C2_HOOK_CDECL dr_dprintf(const char* fmt_string, ...);
extern void C2_HOOK_FASTCALL FUN_004e1400(void* pFrame_pixmap_handle, br_pixelmap* pScreen);
extern short C2_HOOK_CDECL FUN_0056d8c0(int pSequence, void* pFrame_pixmap, void* pFrame_rect, int pFlags, void* pCompressed_frame, int* pFrame_size, void* pCodec_info, void* pUnused8);
extern short C2_HOOK_CDECL FUN_0056b440(void* pMedia, void* pCompressed_frame, int pOffset, int pSize, int pDuration, void* pImage_description, int pSample_count, int pSample_flags, int pSample_time);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004e1250
int C2_HOOK_FASTCALL WriteBannerFrame(void) {
    br_pixelmap* pBanner;
    int frame_size;
    char codec_info;
    short result;
    int i;

    if (gQuick_time_banner_number != 0) {
        pBanner = LoadPixelmap(gQuick_time_banner_texture_name);
        if (pBanner != NULL) {
            if (pBanner->width != 320 || pBanner->height != 200) {
                BrFailure("Banner picture must be 320x200");
            }
            *(void**)(*gQuicktime_frame_pixmap_handle) = gQuicktime_frame_pixels;
            *(short*)(*gQuicktime_frame_pixmap_handle + 4) = (gNbPixelBits == 16 ? gCurrent_graf_data->width / 2 : gCurrent_graf_data->width) * 2;
            memset(gQuicktime_frame_pixels, 0, (gNbPixelBits == 16 ? gCurrent_graf_data->width / 2 : gCurrent_graf_data->width) * (gNbPixelBits == 16 ? gCurrent_graf_data->height / 2 : gCurrent_graf_data->height) * 2);
            dr_dprintf("Converting banner. w=%d, h=%d, type=%d", pBanner->width, pBanner->height, pBanner->type);
            FUN_004e1400(gQuicktime_frame_pixmap_handle, pBanner);
            for (i = 0; i < 0x12; i++) {
                result = FUN_0056d8c0(gQuicktime_image_sequence, gQuicktime_frame_pixmap_handle, &gQuicktime_rect, 0xc0, *gQuicktime_compressed_frame_handle, &frame_size, &codec_info, 0);
                if (result != 0) {
                    return result;
                }
                *(int*)(*gQuicktime_image_description_handle + 0x2c) = frame_size;
                result = FUN_0056b440(gQuicktime_media, gQuicktime_compressed_frame_handle, 0, *(int*)(*gQuicktime_image_description_handle + 0x2c), 0x37, gQuicktime_image_description_handle, 1, 0, 0);
                if (result != 0) {
                    return result;
                }
            }
            gQuicktime_frame_flags = 1;
            return 0;
        }
    }
    gQuicktime_frame_flags = 1;
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e1400
void C2_HOOK_FASTCALL FUN_004e1400(void* pFrame_pixmap_handle, br_pixelmap* pScreen) {
    int row;
    int height;
    int destination_offset;
    int col;
    int width;
    int i;
    int pixel_step;
    int top;
    int source_x;
    int source_pixel;
    int dest;

    (void)pFrame_pixmap_handle;
    if (gQuicktime_frame_pixel_conversion_table == NULL) {
        gQuicktime_frame_pixel_conversion_table = BrMemAllocate(0x20000, 0xfd);
        if (gNbPixelBits == 0x10) {
            i = 0;
            do {
                unsigned int value;
                br_uint_8 blue;

                value = ((unsigned int)i >> 1) & 0x7fe0;
                blue = (br_uint_8)i;
                value |= blue & 0x1f;
                i++;
                gQuicktime_frame_pixel_conversion_table[i - 1] = ((value & 0xff) << 8) | ((value >> 8) & 0xff);
            } while (i < 0x10000);
        } else {
            for (i = 0; i < 256; i++) {
                br_uint_32 colour;
                unsigned int value;

                colour = ((br_uint_32*)gRender_palette->pixels)[i];
                value = (((colour >> 19) & 0x1f) << 5) | ((colour >> 11) & 0x1f);
                value = (value << 5) | ((colour >> 3) & 0x1f);
                gQuicktime_frame_pixel_conversion_table[i] = ((value & 0xff) << 8) | ((value >> 8) & 0xff);
            }
        }
    }

    PossibleLock(1);
    if (gNbPixelBits == 0x10) {
        width = gCurrent_graf_data->width / 2;
    } else {
        width = gCurrent_graf_data->width;
    }
    height = gCurrent_graf_data->height;
    if (gNbPixelBits == 0x10) {
        height /= 2;
    }

    if (gNbPixelBits == 0x10) {
        pixel_step = 2;
        if (pScreen->height == 200) {
            top = 20;
            height = 200;
            pixel_step = 1;
        } else {
            top = 0;
        }
        row = 0;
        if (height > 0) {
            destination_offset = top * width * 2;
            do {
                if (width > 0) {
                    dest = destination_offset;
                    source_x = 0;
                    col = width;
                    do {
                        source_pixel = (pixel_step * row * (int)pScreen->row_bytes) / 2 + source_x;
                        *(br_uint_16*)((char*)gQuicktime_frame_pixels + dest) = gQuicktime_frame_pixel_conversion_table[((br_uint_16*)pScreen->pixels)[source_pixel]];
                        dest += 2;
                        source_x += pixel_step;
                        col--;
                    } while (col != 0);
                }
                row++;
                destination_offset += width * 2;
            } while (row < height);
            PossibleUnlock(1);
            return;
        }
    } else {
        row = 0;
        destination_offset = 0;
        do {
            if (width > 0) {
                dest = destination_offset;
                col = 0;
                do {
                    source_pixel = row * (int)pScreen->row_bytes + col;
                    *(br_uint_16*)((char*)gQuicktime_frame_pixels + dest) = gQuicktime_frame_pixel_conversion_table[((br_uint_8*)pScreen->pixels)[source_pixel]];
                    dest += 2;
                    col++;
                } while (col < width);
            }
            row++;
            destination_offset += width * 2;
        } while (row < 200);
    }
    PossibleUnlock(1);
}

// FUNCTION: CARMA2_HW 0x004e1c70
short C2_HOOK_FASTCALL GenerateOneMovieFrame(br_pixelmap* pScreen) {
    int frame_flags;
    int pixmap_width;
    int frame_size;
    char codec_info;
    short result;

    if (gQuicktime_frame_flags != 0) {
        frame_flags = 0xc0;
        gQuicktime_frame_flags = 0;
    } else {
        frame_flags = 0x40;
    }
    PossibleLock(1);
    *(void**)(*gQuicktime_frame_pixmap_handle) = gQuicktime_frame_pixels;
    if (gNbPixelBits == 16) {
        pixmap_width = gCurrent_graf_data->width / 2;
    } else {
        pixmap_width = gCurrent_graf_data->width;
    }
    *(short*)(*gQuicktime_frame_pixmap_handle + 4) = pixmap_width * 2;
    FUN_004e1400(gQuicktime_frame_pixmap_handle, pScreen);
    result = FUN_0056d8c0(gQuicktime_image_sequence, gQuicktime_frame_pixmap_handle,
            &gQuicktime_rect, frame_flags, *gQuicktime_compressed_frame_handle,
            &frame_size, &codec_info, 0);
    if (result != 0) {
        PossibleUnlock(1);
        return result;
    }
    *(int*)(*gQuicktime_image_description_handle + 0x2c) = frame_size;
    result = FUN_0056b440(gQuicktime_media, gQuicktime_compressed_frame_handle, 0,
            *(int*)(*gQuicktime_image_description_handle + 0x2c),
            0x37, gQuicktime_image_description_handle, 1, 0, 0);
    if (result != 0) {
        PossibleUnlock(1);
        return result;
    }
    PossibleUnlock(1);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e1c30
void C2_HOOK_FASTCALL NoOp_004e1c30(void) {
}

// QuickTime dispatcher trampoline (selector 0x3000b).
#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0056d8c0
__declspec(naked) short C2_HOOK_CDECL FUN_0056d8c0(int pSequence, void* pFrame_pixmap, void* pFrame_rect, int pFlags, void* pCompressed_frame, int* pFrame_size, void* pCodec_info, void* pUnused8) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_d8c0
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_d8c0:
        mov     eax, 0x3000b
        jmp     dword ptr [gQuickTime_dispatch]
    }
}
#else
short C2_HOOK_CDECL FUN_0056d8c0(int pSequence, void* pFrame_pixmap, void* pFrame_rect, int pFlags, void* pCompressed_frame, int* pFrame_size, void* pCodec_info, void* pUnused8) {
    return 1;
}
#endif

// QuickTime dispatcher trampoline (selector 0x2007c).
#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0056b440
__declspec(naked) short C2_HOOK_CDECL FUN_0056b440(void* pMedia, void* pCompressed_frame, int pOffset, int pSize, int pDuration, void* pImage_description, int pSample_count, int pSample_flags, int pSample_time) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_b440
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_b440:
        mov     eax, 0x2007c
        jmp     dword ptr [gQuickTime_dispatch]
    }
}
#else
short C2_HOOK_CDECL FUN_0056b440(void* pMedia, void* pCompressed_frame, int pOffset, int pSize, int pDuration, void* pImage_description, int pSample_count, int pSample_flags, int pSample_time) {
    return 1;
}
#endif

// QuickTime dispatcher trampoline (selector 0x100001).
#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0056fde0
__declspec(naked) void* C2_HOOK_CDECL FUN_0056fde0(int pWidth, int pHeight, void* pCompressor) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_fde0
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_fde0:
        mov     eax, 0x100001
        jmp     dword ptr [gQuickTime_dispatch]
    }
}
#else
void* C2_HOOK_CDECL FUN_0056fde0(int pWidth, int pHeight, void* pCompressor) {
    return NULL;
}
#endif

// QuickTime dispatcher trampoline (selector 0x20188).
#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0056c640
__declspec(naked) void* C2_HOOK_CDECL FUN_0056c640(void* pMovie, void* pCompressor) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_c640
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_c640:
        mov     eax, 0x20188
        jmp     dword ptr [gQuickTime_dispatch]
    }
}
#else
void* C2_HOOK_CDECL FUN_0056c640(void* pMovie, void* pCompressor) {
    return NULL;
}
#endif

// QuickTime dispatcher trampoline (selector 0x20003).
#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0056a920
__declspec(naked) int C2_HOOK_CDECL FUN_0056a920(void) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_a920
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_a920:
        mov     eax, 0x20003
        jmp     dword ptr [gQuickTime_dispatch]
    }
}
#else
int C2_HOOK_CDECL FUN_0056a920(void) {
    return 0;
}
#endif

// QuickTime dispatcher trampoline (selector 0x2018e).
#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0056c6e0
__declspec(naked) void* C2_HOOK_CDECL FUN_0056c6e0(void* pTrack, int pMedia_type, int pTime_scale, int pLanguage, int pQuality) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_c6e0
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_c6e0:
        mov     eax, 0x2018e
        jmp     dword ptr [gQuickTime_dispatch]
    }
}
#else
void* C2_HOOK_CDECL FUN_0056c6e0(void* pTrack, int pMedia_type, int pTime_scale, int pLanguage, int pQuality) {
    return NULL;
}
#endif

// QuickTime dispatcher trampoline (selector 0x20072).
#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0056b340
__declspec(naked) int C2_HOOK_CDECL FUN_0056b340(void* pMedia) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_b340
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_b340:
        mov     eax, 0x20072
        jmp     dword ptr [gQuickTime_dispatch]
    }
}
#else
int C2_HOOK_CDECL FUN_0056b340(void* pMedia) {
    return 0;
}
#endif

// QuickTime dispatcher trampoline (selector 0x150008).
#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x00570600
__declspec(naked) void* C2_HOOK_CDECL FUN_00570600(int pSize) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_0600
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_0600:
        mov     eax, 0x150008
        jmp     dword ptr [gQuickTime_dispatch]
    }
}
#else
void* C2_HOOK_CDECL FUN_00570600(int pSize) {
    return NULL;
}
#endif

// QuickTime dispatcher trampoline (selector 0x150006).
#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x005705c0
__declspec(naked) int C2_HOOK_CDECL FUN_005705c0(void) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_05c0
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_05c0:
        mov     eax, 0x150006
        jmp     dword ptr [gQuickTime_dispatch]
    }
}
#else
int C2_HOOK_CDECL FUN_005705c0(void) {
    return 0;
}
#endif

// QuickTime dispatcher trampoline (selector 0x30004).
#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x0056d7e0
__declspec(naked) int C2_HOOK_CDECL FUN_0056d7e0(void* pFrame_pixmap_handle, void* pFrame_rect, int pFlags, int pSpatial_quality, int pCompressor, int pColor_table_id, int* pMax_compression_size) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_d7e0
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_d7e0:
        mov     eax, 0x30004
        jmp     dword ptr [gQuickTime_dispatch]
    }
}
#else
int C2_HOOK_CDECL FUN_0056d7e0(void* pFrame_pixmap_handle, void* pFrame_rect, int pFlags, int pSpatial_quality, int pCompressor, int pColor_table_id, int* pMax_compression_size) {
    return 0;
}
#endif

// QuickTime dispatcher trampoline (selector 0x15002e).
#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x00570a40
__declspec(naked) void C2_HOOK_CDECL FUN_00570a40(void* pHandle) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_0a40
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_0a40:
        mov     eax, 0x15002e
        jmp     dword ptr [gQuickTime_dispatch]
    }
}
#else
void C2_HOOK_CDECL FUN_00570a40(void* pHandle) {
}
#endif

// QuickTime dispatcher trampoline (selector 0x150019).
#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x005707e0
__declspec(naked) void C2_HOOK_CDECL FUN_005707e0(void* pHandle) {
    __asm {
        mov     eax, dword ptr [gQuickTime_dispatcher]
        or      eax, eax
        je      quicktime_trap_07e0
        call    dword ptr [gQuickTime_dispatcher]
    quicktime_trap_07e0:
        mov     eax, 0x150019
        jmp     dword ptr [gQuickTime_dispatch]
    }
}
#else
void C2_HOOK_CDECL FUN_005707e0(void* pHandle) {
}
#endif

// QuickTime dispatcher trampoline (selector 0x3000a).
#ifdef REC2_MATCHING
// FUN_0056d8a0
// FUN_0056a900
// FUN_0056dac0
// FUN_00570ac0

// STUB: CARMA2_HW 0x004e1810
void C2_HOOK_FASTCALL MovieRecordButtonPressed(void) { NOT_IMPLEMENTED(); }

// GLOBAL: CARMA2_HW 0x006baa28
int gRecording_mouse_disabled;
