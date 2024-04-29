#include "win32_ssdx.h"

#include "win32.h"

#include "errors.h"
#include "globvars.h"
#include "graphics.h"

#include <ddraw.h>

#include "c2_string.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTSOUND, gDirectSound, 0x006aaa1c);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTSOUND, gPD_S3_direct_sound, 0x006b2d9c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gUse_DirectDraw, 0x006aa9e0);
C2_HOOK_VARIABLE_IMPLEMENT(HWND, gHWnd_SSDX, 0x006aaa08);
C2_HOOK_VARIABLE_IMPLEMENT(int, gEnumerate_DirectX_surfaces, 0x006aa9d8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gPDS3_volume_factors, 256, 0x007a00e0);
C2_HOOK_VARIABLE_IMPLEMENT(tPD_S3_config, gPD_S3_config, 0x007a0080);
C2_HOOK_VARIABLE_IMPLEMENT(MCI_OPEN_PARMS, gPDS3_mci_open_parms, 0x007a0500);
C2_HOOK_VARIABLE_IMPLEMENT(MCI_SET_PARMS, gPDS3_mci_set_parms, 0x0079fec0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPDS3_cda_initialized, 0x006b2d98);
C2_HOOK_VARIABLE_IMPLEMENT(int, gS3_working_directory_initialized, 0x006b2da0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gS3_working_directory, 260, 0x006b2c90);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPDS3_midi_playing, 0x006b2da4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPDS3_cda_media_present, 0x006b2dac);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPDS3_cda_is_playing, 0x006b2db0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPDS3_cda_paused, 0x006b2db4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPDS3_cda_track, 0x006b2db8);
C2_HOOK_VARIABLE_IMPLEMENT(MCI_STATUS_PARMS, gPDS3_cda_status_parms, 0x007a0528);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPDS3_Number_of_tracks, 0x007a00ac);

void C2_HOOK_FASTCALL SSDXLogError(HRESULT hRes) {
#define LOG_CASE_DDERR(V) case V: dr_dprintf("%s (%x)", #V, V); break
    switch (hRes) {
        LOG_CASE_DDERR(DDERR_GENERIC);
        LOG_CASE_DDERR(DDERR_UNSUPPORTED);
        LOG_CASE_DDERR(DDERR_INVALIDPARAMS);
        LOG_CASE_DDERR(DDERR_OUTOFMEMORY);
        LOG_CASE_DDERR(DDERR_INVALIDCAPS);
        LOG_CASE_DDERR(DDERR_INCOMPATIBLEPRIMARY);
        LOG_CASE_DDERR(DDERR_INVALIDOBJECT);
        LOG_CASE_DDERR(DDERR_INVALIDMODE);
        LOG_CASE_DDERR(DDERR_INVALIDRECT);
        LOG_CASE_DDERR(DDERR_INVALIDPIXELFORMAT);
        LOG_CASE_DDERR(DDERR_SURFACEBUSY);
        LOG_CASE_DDERR(DDERR_NOEXCLUSIVEMODE);
        LOG_CASE_DDERR(DDERR_WASSTILLDRAWING);
        LOG_CASE_DDERR(DDERR_SURFACELOST);
        LOG_CASE_DDERR(DDERR_WRONGMODE);
        LOG_CASE_DDERR(DDERR_IMPLICITLYCREATED);
        LOG_CASE_DDERR(DD_OK);
        default: dr_dprintf("UNKNOWN DD ERROR (%x) N.B. Look for this number in 'ddraw.h': %d", hRes, hRes);
    }
#undef LOG_CASE_DDERR
}
C2_HOOK_FUNCTION(0x005002d0, SSDXLogError)

void C2_HOOK_FASTCALL SSDXStart(HWND p_hWnd, int p_DirectDraw, int p_EnumerateDDX, int p_flags) {
    HRESULT hRes;

    hRes = 0;
    dr_dprintf("SSDXStart(): START...");
    if (p_EnumerateDDX) {
        C2V(gEnumerate_DirectX_surfaces) = 1;
        if (p_DirectDraw) {
            C2V(gEnumerate_DirectX_surfaces) = 0;
        }
    } else {
        C2V(gEnumerate_DirectX_surfaces) = 0;
    }
    C2V(gUse_DirectDraw) = p_DirectDraw;
    C2V(gHWnd_SSDX) = p_hWnd;
    if (p_flags & 0x2) {
        dr_dprintf("SSDXStart(): Calling DirectSoundCreate()...");
        hRes = DirectSoundCreate(0, &C2V(gDirectSound), NULL);
    }
    if (hRes != DD_OK) {
        SSDXLogError(hRes);
    }
    dr_dprintf("SSDXStart(): END.");
}
C2_HOOK_FUNCTION(0x00500500, SSDXStart)

int C2_HOOK_FASTCALL PDS3Init(void) {

    C2_HOOK_BUG_ON(sizeof(C2V(gPD_S3_config)) != 0x20);

    c2_strcpy(C2V(gS3_path_separator), "\\");
    c2_strcpy(C2V(gS3_sound_folder_name), "SOUND");
    c2_memset(&C2V(gPD_S3_config), 0, sizeof(C2V(gPD_S3_config)));
    if (!PDS3DDXInit()) {
        return 0;
    }
    PDS3InitCDA();
    // nop_FUN_005699a3();
    C2V(gPD_S3_config).field_0x04 = 0;
    C2V(gPD_S3_config).free_direct_sound = 1;
    C2V(gPD_S3_config).independent_pitch = 0;
    return 1;
}
C2_HOOK_FUNCTION(0x0056992f, PDS3Init)

int C2_HOOK_FASTCALL PDS3DDXInit(void) {
    int i;

    if (FAILED(DirectSoundCreate(0, &C2V(gPD_S3_direct_sound), NULL))) {
        return 0;
    }
    if (FAILED(IDirectSound_SetCooperativeLevel(C2V(gPD_S3_direct_sound), C2V(gHWnd), DSSCL_EXCLUSIVE))) {
        return 0;
    }
    for (i = 0; i < REC2_ASIZE(C2V(gPDS3_volume_factors)); i++) {
        float f;
        f = ((float)i + 1.f) * 999.f / 256.f;
        C2V(gPDS3_volume_factors)[i] = (int)((f * 9999.f / 3.f - 4300.f) - 9999.f);
    }
    S3Enable();
    return 1;
}
C2_HOOK_FUNCTION(0x00569a2c, PDS3DDXInit)

int (C2_HOOK_FASTCALL * PDS3InitCDA_original)(void);
int C2_HOOK_FASTCALL PDS3InitCDA(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    return PDS3InitCDA_original();
#else
    MCIERROR err;
    err = mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE, (DWORD_PTR)&C2V(gPDS3_mci_open_parms));
    if (err != 0) {
        err = mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID, (DWORD_PTR)&C2V(gPDS3_mci_open_parms));
        if (err != 0) {
            return 0;
        }
    }
    C2V(gPDS3_mci_set_parms).dwTimeFormat = MCI_FORMAT_TMSF;
    mciSendCommandA(C2V(gPDS3_mci_open_parms).wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR)&C2V(gPDS3_mci_set_parms));
    S3EnableCDA();
    C2V(gPDS3_cda_initialized) = 1;
    return 1;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005699aa, PDS3InitCDA, PDS3InitCDA_original)

void* C2_HOOK_FASTCALL PDS3CreateSoundBuffer(tS3_wav_info* pWav_info, void* pWav_buffer) {
    LPDIRECTSOUNDBUFFER buffer;
    DSBUFFERDESC buffer_desc;
    void* buffer_data;
    DWORD buffer_data_size;

    C2_HOOK_BUG_ON(sizeof(DSBUFFERDESC) != 0x14);
    c2_memset(&buffer_desc, 0, sizeof(buffer_desc));
    buffer_desc.dwSize = sizeof(DSBUFFERDESC);
    buffer_desc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_STATIC;
    buffer_desc.dwBufferBytes = pWav_info->sample_size;
    buffer_desc.lpwfxFormat = (LPWAVEFORMATEX)pWav_info->wav_info_header;
    if (FAILED(IDirectSound_CreateSoundBuffer(C2V(gPD_S3_direct_sound), &buffer_desc, &buffer, NULL))) {
        return NULL;
    }
    if (FAILED(IDirectSoundBuffer_Lock(buffer, 0, pWav_info->sample_size, &buffer_data, &buffer_data_size, NULL, NULL, 0))) {
        IDirectSoundBuffer_Release(buffer);
        return NULL;
    }
    c2_memmove(buffer_data, pWav_info->samples, buffer_data_size);
    IDirectSoundBuffer_Unlock(buffer, buffer_data, buffer_data_size, NULL, 0);
    return buffer;
}

void C2_HOOK_FASTCALL PDS3ReleaseSound(tS3_descriptor* pSound_descriptor) {

    LPDIRECTSOUNDBUFFER buffer = pSound_descriptor->pd_handle;
    if (buffer != NULL) {
        IDirectSoundBuffer_Release(buffer);
    }
}
C2_HOOK_FUNCTION(0x005693d9, PDS3ReleaseSound)

int C2_HOOK_FASTCALL PDReverseAudio(tS3_channel* pChannel) {
    tU8* buffer_data;
    DWORD buffer_data_size;
    DWORD i;
    tU8* tmp_buffer;
    DWORD sample_size;

    LPDIRECTSOUNDBUFFER buffer = pChannel->descriptor->pd_handle;
    if (buffer == NULL) {
        return 1;
    }
    sample_size = pChannel->descriptor->buffer_description->sample_size;
    if (sample_size > 2000000) {
        return 0;
    }
    if (FAILED(IDirectSoundBuffer_Lock(buffer, 0, sample_size, &buffer_data, &buffer_data_size, NULL, NULL, 0))) {
        return 0;
    }
    tmp_buffer = BrMemAllocate(sample_size, kMem_S3_Windows_95_create_temp_buffer_space_to_reverse_sample);
    if (tmp_buffer == NULL) {
        IDirectSoundBuffer_Unlock(buffer, buffer_data, buffer_data_size, NULL, 0);
        return 1;
    }
    c2_memmove(tmp_buffer, buffer_data, buffer_data_size);
    for (i = 0; i < buffer_data_size; i++) {
        buffer_data[i] = tmp_buffer[sample_size - 1 - i];
    }
    BrMemFree(tmp_buffer);
    IDirectSoundBuffer_Unlock(buffer, buffer_data, buffer_data_size, NULL, 0);
    return 0;
}
C2_HOOK_FUNCTION(0x00569401, PDReverseAudio)

char* C2_HOOK_FASTCALL PDS3GetWorkingDirectory(void) {

    if (!C2V(gS3_working_directory_initialized)) {
        DWORD len_res;
        size_t len;

        C2_HOOK_BUG_ON(sizeof(C2V(gS3_working_directory)) != 0x104);

        C2V(gS3_working_directory)[0] = '\0';
        len_res = GetCurrentDirectoryA(sizeof(C2V(gS3_working_directory)), C2V(gS3_working_directory));
        len = c2_strlen(C2V(gS3_working_directory));
        if (len_res != len) {
            C2V(gS3_working_directory)[0] = '\0';
        }
        C2V(gS3_working_directory_initialized) = 1;
    }
    return C2V(gS3_working_directory);
}
C2_HOOK_FUNCTION(0x00569b30, PDS3GetWorkingDirectory)

int C2_HOOK_FASTCALL PDS3StopMidiChannel(tS3_channel* pChannel) {

    if C2V(gPDS3_midi_playing) {
        C2V(gPDS3_midi_playing) = 0;
        mciSendCommandA(pChannel->mciDevice, MCI_CLOSE, 0, 0);
    }
    if (pChannel->active) {
        pChannel->needs_service = 1;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00569cb1, PDS3StopMidiChannel)

void (C2_HOOK_FASTCALL * PDS3CheckCDAMedia_original)(tS3_channel* pChannel);
void C2_HOOK_FASTCALL PDS3CheckCDAMedia(tS3_channel* pChannel) {

#if 0//defined(C2_HOOKS_ENABLED)
    PDS3CheckCDAMedia_original(pChannel);
#else
    C2V(gPDS3_cda_status_parms).dwItem = MCI_STATUS_MEDIA_PRESENT;
    mciSendCommandA(C2V(gPDS3_mci_open_parms).wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&C2V(gPDS3_cda_status_parms));
    if (!C2V(gPDS3_cda_media_present) && C2V(gPDS3_cda_status_parms).dwReturn) {
        C2V(gPDS3_cda_media_present) = 1;
        C2V(gPDS3_cda_paused) = 0;
        C2V(gPDS3_cda_track) = 0;
        C2V(gPDS3_cda_status_parms).dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
        mciSendCommandA(C2V(gPDS3_mci_open_parms).wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&C2V(gPDS3_cda_status_parms));
        C2V(gPDS3_Number_of_tracks) = MIN(C2V(gPDS3_cda_status_parms).dwReturn, 99);
    } else if (C2V(gPDS3_cda_media_present) && !C2V(gPDS3_cda_status_parms).dwReturn) {
        C2V(gPDS3_cda_media_present) = 0;
        C2V(gPDS3_cda_paused) = 0;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0056a0e1, PDS3CheckCDAMedia, PDS3CheckCDAMedia_original)

int C2_HOOK_FASTCALL PDS3StopCDAChannel(tS3_channel* pChannel) {
    PDS3CheckCDAMedia(pChannel);
    if (C2V(gPDS3_cda_media_present)) {
        C2V(gPDS3_cda_paused) = 0;
        C2V(gPDS3_cda_track) = 0;
        mciSendCommandA(C2V(gPDS3_mci_open_parms).wDeviceID, MCI_STOP, 0, 0);
    }
    C2V(gPDS3_cda_is_playing) = 0;
    return 0;
}
C2_HOOK_FUNCTION(0x0056a093, PDS3StopCDAChannel)

int C2_HOOK_FASTCALL PDS3StopSampleChannel(tS3_channel* pChannel) {

    if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type && pChannel->descriptor->pd_handle != NULL) {
        LPDIRECTSOUNDBUFFER buffer = pChannel->descriptor->pd_handle;
        IDirectSoundBuffer_Stop(buffer);
        if (C2V(gS3_callbacks).on_sample_channel_stop != NULL && pChannel->active) {
            C2V(gS3_callbacks).on_sample_channel_stop(pChannel);
        }
    }
    if (pChannel->active) {
        pChannel->needs_service = 1;
    }
    return 1;
}
C2_HOOK_FUNCTION(0x005697ef, PDS3StopSampleChannel)

int C2_HOOK_FASTCALL PDS3UpdateChannelVolume(tS3_channel* pChannel) {

    if (pChannel->type == 0) {
        if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type) {
            LPDIRECTSOUNDBUFFER buffer = pChannel->descriptor->pd_handle;

            if (buffer != NULL) {
                int volume = (int)(pChannel->owner_outlet->volume / 255.f * pChannel->volume_multiplier * 11.76470588235294f);
                int ds_volume = volume - 3000;
                ds_volume = MAX(ds_volume, -9999);
                ds_volume = MIN(ds_volume, 0);

                if (SUCCEEDED(IDirectSoundBuffer_SetVolume(buffer, ds_volume))) {
                    if (C2V(gS3_callbacks).on_sample_channel_set_volume != NULL) {
                        C2V(gS3_callbacks).on_sample_channel_set_volume(pChannel, volume);
                    }
                    if (ds_volume != 0 && pChannel->spatial_sound) {
                        int pan = (int)(pChannel->field_0x28 * 9999.f);
                        pan = MAX(pan, -9999);
                        pan = MIN(pan,  9999);
                        if (C2V(gS3_callbacks).on_sample_channel_set_pan != NULL) {
                            C2V(gS3_callbacks).on_sample_channel_set_pan(pChannel, pChannel->field_0x28);
                        }
                        IDirectSoundBuffer_SetPan(buffer, pan);
                    }
                }
            }
        }
    } else if (pChannel->type == 1) {

    } else if (pChannel->type == 2) {
        PDS3UpdateCDAVolume(pChannel, pChannel->owner_outlet->volume);
    }
    return 1;
}
C2_HOOK_FUNCTION(0x00569507, PDS3UpdateChannelVolume)

void C2_HOOK_FASTCALL PDS3UpdateCDAVolume(tS3_channel* pChannel, int pVolume) {

}
C2_HOOK_FUNCTION(0x00568ed0, PDS3UpdateCDAVolume)
