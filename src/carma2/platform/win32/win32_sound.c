#include "win32_sound.h"

#include "platform.h"
#include "s3/internal/audio.h"
#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x006aaa1c
LPDIRECTSOUND gDirectSound;

// GLOBAL: CARMA2_HW 0x006b2d9c
LPDIRECTSOUND gPD_S3_direct_sound;

// GLOBAL: CARMA2_HW 0x007a00e0
int gPDS3_volume_factors[256];

// GLOBAL: CARMA2_HW 0x007a0080
tPD_S3_config gPD_S3_config;

// GLOBAL: CARMA2_HW 0x007a0500
MCI_OPEN_PARMS gPDS3_mci_open_parms;

// GLOBAL: CARMA2_HW 0x0079fec0
MCI_SET_PARMS gPDS3_mci_set_parms;

// GLOBAL: CARMA2_HW 0x006b2d98
int gPDS3_cda_initialized;

// GLOBAL: CARMA2_HW 0x006b2da0
int gS3_working_directory_initialized;

// GLOBAL: CARMA2_HW 0x006b2c90
char gS3_working_directory[260];

// GLOBAL: CARMA2_HW 0x006b2da4
int gPDS3_midi_playing;

// GLOBAL: CARMA2_HW 0x006b2dac
int gPDS3_cda_media_present;

// GLOBAL: CARMA2_HW 0x006b2db0
int gPDS3_cda_is_playing;

// GLOBAL: CARMA2_HW 0x006b2db4
int gPDS3_cda_paused;

// GLOBAL: CARMA2_HW 0x006b2db8
int gPDS3_cda_track;

// GLOBAL: CARMA2_HW 0x007a0520
MCI_STATUS_PARMS gPDS3_cda_status_parms;

// GLOBAL: CARMA2_HW 0x007a00ac
int gPDS3_Number_of_tracks;

// GLOBAL: CARMA2_HW 0x007a0070
MCI_STATUS_PARMS gPDS3_mci_midi_status_parms;

// GLOBAL: CARMA2_HW 0x006b2dc4
tU32 gPDS3_last_cda_status_update;

// FUNCTION: CARMA2_HW 0x0056992f
int C2_HOOK_FASTCALL PDS3Init(void) {

    C2_HOOK_BUG_ON(sizeof(gPD_S3_config) != 0x20);

    strcpy(gS3_path_separator, "\\");
    strcpy(gS3_sound_folder_name, "SOUND");
    memset(&gPD_S3_config, 0, sizeof(gPD_S3_config));
    if (!PDS3DDXInit()) {
        return 0;
    }
    PDS3InitCDA();
    // nop_FUN_005699a3();
    gPD_S3_config.field_0x04 = 0;
    gPD_S3_config.free_direct_sound = 1;
    gPD_S3_config.independent_pitch = 0;
    return 1;
}

// FUNCTION: CARMA2_HW 0x00569a2c
int C2_HOOK_FASTCALL PDS3DDXInit(void) {
#ifdef DSOUND_ENABLED
    int i;

    if (FAILED(DirectSoundCreate(0, &gPD_S3_direct_sound, NULL))) {
        return 0;
    }
    if (FAILED(IDirectSound_SetCooperativeLevel(gPD_S3_direct_sound, gHWnd, DSSCL_EXCLUSIVE))) {
        return 0;
    }
    for (i = 0; i < (int)REC2_ASIZE(gPDS3_volume_factors); i++) {

        gPDS3_volume_factors[i] = (int)(-(9999.0 - (log10(((double)i + 1.0) * 9999.0 / 256.0) * 9999.0 / 3.0 - 4300.0)));
    }
    S3Enable();
    return 1;
#else
    return 0;
#endif
}

// FUNCTION: CARMA2_HW 0x005699aa
int C2_HOOK_FASTCALL PDS3InitCDA(void) {

    gPDS3_mci_open_parms.lpstrDeviceType = (LPCSTR)(uintptr_t)MCI_DEVTYPE_CD_AUDIO;
    if (mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID, (DWORD_PTR)&gPDS3_mci_open_parms) != 0
         && mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE, (DWORD_PTR)&gPDS3_mci_open_parms) != 0) {

        return 0;
    }
    gPDS3_mci_set_parms.dwTimeFormat = MCI_FORMAT_TMSF;
    mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR)&gPDS3_mci_set_parms);
    S3EnableCDA();
    gPDS3_cda_initialized = 1;
    return 1;
}

void* C2_HOOK_FASTCALL PDS3CreateSoundBuffer(tS3_wav_info* pWav_info, void* pWav_buffer) {
    LPDIRECTSOUNDBUFFER buffer;
    DSBUFFERDESC buffer_desc;
    void* buffer_data;
    DWORD buffer_data_size;

    C2_HOOK_BUG_ON(sizeof(DSBUFFERDESC) != 0x14);
    memset(&buffer_desc, 0, sizeof(buffer_desc));
    buffer_desc.dwSize = sizeof(DSBUFFERDESC);
    buffer_desc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_STATIC;
    buffer_desc.dwBufferBytes = pWav_info->sample_size;
    buffer_desc.lpwfxFormat = (LPWAVEFORMATEX)pWav_info->wav_info_header;
    if (FAILED(IDirectSound_CreateSoundBuffer(gPD_S3_direct_sound, &buffer_desc, &buffer, NULL))) {
        return NULL;
    }
    if (FAILED(IDirectSoundBuffer_Lock(buffer, 0, pWav_info->sample_size, &buffer_data, &buffer_data_size, NULL, NULL, 0))) {
        IDirectSoundBuffer_Release(buffer);
        return NULL;
    }
    memmove(buffer_data, pWav_info->samples, buffer_data_size);
    IDirectSoundBuffer_Unlock(buffer, buffer_data, buffer_data_size, NULL, 0);
    return buffer;
}

// FUNCTION: CARMA2_HW 0x005693d9
void C2_HOOK_FASTCALL PDS3ReleaseSound(tS3_descriptor* pSound_descriptor) {

    LPDIRECTSOUNDBUFFER buffer = pSound_descriptor->pd_handle;
    if (buffer != NULL) {
        IDirectSoundBuffer_Release(buffer);
    }
}

// FUNCTION: CARMA2_HW 0x00569401
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
    if (FAILED(IDirectSoundBuffer_Lock(buffer, 0, sample_size, (void**)&buffer_data, &buffer_data_size, NULL, NULL, 0))) {
        return 0;
    }
    tmp_buffer = BrMemAllocate(sample_size, kMem_S3_Windows_95_create_temp_buffer_space_to_reverse_sample);
    if (tmp_buffer == NULL) {
        IDirectSoundBuffer_Unlock(buffer, buffer_data, buffer_data_size, NULL, 0);
        return 1;
    }
    memmove(tmp_buffer, buffer_data, buffer_data_size);
    for (i = 0; i < buffer_data_size; i++) {
        buffer_data[i] = tmp_buffer[sample_size - 1 - i];
    }
    BrMemFree(tmp_buffer);
    IDirectSoundBuffer_Unlock(buffer, buffer_data, buffer_data_size, NULL, 0);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00569b30
const char* C2_HOOK_FASTCALL PDS3GetWorkingDirectory(void) {

    if (!gS3_working_directory_initialized) {
        DWORD len_res;
        size_t len;

        C2_HOOK_BUG_ON(sizeof(gS3_working_directory) != 0x104);

        gS3_working_directory[0] = '\0';
        len_res = GetCurrentDirectoryA(sizeof(gS3_working_directory), gS3_working_directory);
        len = strlen(gS3_working_directory);
        if (len_res != len) {
            gS3_working_directory[0] = '\0';
        }
        gS3_working_directory_initialized = 1;
    }
    return gS3_working_directory;
}

// FUNCTION: CARMA2_HW 0x00569cb1
int C2_HOOK_FASTCALL PDS3StopMidiChannel(tS3_channel* pChannel) {

    if (gPDS3_midi_playing) {
        gPDS3_midi_playing = 0;
        mciSendCommandA(pChannel->mciDevice, MCI_CLOSE, 0, 0);
    }
    if (pChannel->active) {
        pChannel->needs_service = 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056a0e1
void C2_HOOK_FASTCALL PDS3CheckCDAMedia(tS3_channel* pChannel) {
    gPDS3_cda_status_parms.dwItem = MCI_STATUS_MEDIA_PRESENT;
    mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&gPDS3_cda_status_parms);
    if (!gPDS3_cda_media_present && gPDS3_cda_status_parms.dwReturn) {
        gPDS3_cda_media_present = 1;
        gPDS3_cda_paused = 0;
        gPDS3_cda_track = 0;
        gPDS3_cda_status_parms.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&gPDS3_cda_status_parms);
        gPDS3_Number_of_tracks = MIN(gPDS3_cda_status_parms.dwReturn, 99);
    } else if (gPDS3_cda_media_present && !gPDS3_cda_status_parms.dwReturn) {
        gPDS3_cda_media_present = 0;
        gPDS3_cda_paused = 0;
    }

}

// FUNCTION: CARMA2_HW 0x0056a093
int C2_HOOK_FASTCALL PDS3StopCDAChannel(tS3_channel* pChannel) {

    PDS3CheckCDAMedia(pChannel);
    if (gPDS3_cda_media_present) {
        gPDS3_cda_paused = 0;
        gPDS3_cda_track = 0;
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STOP, 0, 0);
    }
    gPDS3_cda_is_playing = 0;
    return 0;
}

// FUNCTION: CARMA2_HW 0x005697ef
int C2_HOOK_FASTCALL PDS3StopSampleChannel(tS3_channel* pChannel) {

    if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type && pChannel->descriptor->pd_handle != NULL) {
        LPDIRECTSOUNDBUFFER buffer = pChannel->descriptor->pd_handle;
        IDirectSoundBuffer_Stop(buffer);
        if (gS3_callbacks.on_sample_channel_stop != NULL && pChannel->active) {
            gS3_callbacks.on_sample_channel_stop(pChannel);
        }
    }
    if (pChannel->active) {
        pChannel->needs_service = 1;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00569507
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
                    if (gS3_callbacks.on_sample_channel_set_volume != NULL) {
                        gS3_callbacks.on_sample_channel_set_volume(pChannel, volume);
                    }
                    if (ds_volume != 0 && pChannel->spatial_sound) {
                        int pan = (int)(pChannel->field_0x28 * 9999.f);
                        pan = MAX(pan, -9999);
                        pan = MIN(pan,  9999);
                        if (gS3_callbacks.on_sample_channel_set_pan != NULL) {
                            gS3_callbacks.on_sample_channel_set_pan(pChannel, pChannel->field_0x28);
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

// FUNCTION: CARMA2_HW 0x00568ed0
void C2_HOOK_FASTCALL PDS3UpdateCDAVolume(tS3_channel* pChannel, int pVolume) {

}

// FUNCTION: CARMA2_HW 0x00569ae2
void C2_HOOK_FASTCALL PDS3Stop(void) {

    if (gPD_S3_config.free_direct_sound) {

        IDirectSound_Release(gPD_S3_direct_sound);
        gPD_S3_direct_sound = NULL;
    }
    if (gPDS3_mci_open_parms.wDeviceID != 0) {
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_CLOSE, 0, 0);
    }
}

// FUNCTION: CARMA2_HW 0x00569f1b
int C2_HOOK_FASTCALL PDS3IsMIDIStopped(tS3_channel *pChannel) {

    gPDS3_mci_midi_status_parms.dwItem = MCI_STATUS_MODE;
    mciSendCommandA(pChannel->mciDevice, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&gPDS3_mci_midi_status_parms);
    return gPDS3_mci_midi_status_parms.dwReturn != MCI_MODE_PLAY;
}

// FUNCTION: CARMA2_HW 0x0056a260
int C2_HOOK_FASTCALL PDS3IsCDAPlaying(void) {

    return gS3_CDA_enabled && gPDS3_cda_is_playing && gPDS3_cda_media_present;
}

int C2_HOOK_FASTCALL PDS3IsSamplePlaying(tS3_channel* pChannel) {
    DWORD status;

    if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type) {
        LPDIRECTSOUNDBUFFER buffer;

        buffer = pChannel->descriptor->pd_handle;
        if (buffer != NULL) {
            if (!FAILED(IDirectSoundBuffer_GetStatus(buffer, &status))) {
                if (status & DSBSTATUS_PLAYING) {
                    return 1;
                }
            }
        }
    }
    PDS3StopSampleChannel(pChannel);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056967e
int C2_HOOK_FASTCALL PDS3UpdateChannelFrequency(tS3_channel* pChannel) {

    if (pChannel->type == 0 && pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type) {
        IDirectSoundBuffer* sound_buffer = pChannel->descriptor->pd_handle;
        if (sound_buffer != NULL) {
            IDirectSoundBuffer_SetFrequency(sound_buffer, MIN(pChannel->rate, 100000));
        }
    }
    return 1;
}

int C2_HOOK_FASTCALL PDS3PlaySample(tS3_channel* pChannel) {

    if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type) {
        IDirectSoundBuffer *sound_buffer = pChannel->descriptor->pd_handle;
        if (sound_buffer != NULL) {
            int looping = pChannel->repetitions == 0;
            IDirectSoundBuffer_Play(sound_buffer, 0, 0, looping ? DSBPLAY_LOOPING : 0);
            if (!looping) {
                IDirectSoundBuffer_SetCurrentPosition(sound_buffer, 0);
            }
        }
        PDS3UpdateChannelVolume(pChannel);
        PDS3UpdateChannelFrequency(pChannel);
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00569b93
tS3_error_codes C2_HOOK_FASTCALL PDS3StartMidiChannel(tS3_channel* pChannel) {
    char path[256];
    MCIERROR err;
    MCIDEVICEID device;

    path[0] = '\0';
    strcpy(path, pChannel->descriptor->path);
    strcat(path, ".MID");
    mci_open_params.lpstrElementName = path;
    mci_open_params.lpstrDeviceType = "sequencer";
    err = mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD_PTR)&mci_open_params);
    device = mci_open_params.wDeviceID;
    if (err != 0) {
        return eS3_error_start_song;
    }
    gPDS3_mci_midi_status_parms.dwItem = MCI_VD_STATUS_FORWARD;
    err = mciSendCommandA(device, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&gPDS3_mci_midi_status_parms);
    if (err != 0) {
        mciSendCommandA(device, MCI_CLOSE, 0, 0);
        return eS3_error_start_song;
    }
    err = mciSendCommandA(device, MCI_PLAY, 0, (DWORD_PTR)&mci_play_parms);
    if (err != 0) {
        mciSendCommandA(device, MCI_CLOSE, 0, 0);
        return eS3_error_start_song;
    }
    pChannel->mciDevice = device;
    gPDS3_midi_playing = 1;
    return eS3_error_none;
}

// FUNCTION: CARMA2_HW 0x00569f92
tS3_error_codes C2_HOOK_FASTCALL PDS3PlayCDAChannel(tS3_channel* pChannel) {
    char* endptr;

    PDS3CheckCDAMedia(pChannel);
    if (!gPDS3_cda_media_present) {
        return eS3_error_start_cda;
    }
    gPDS3_cda_paused = 0;
    gPDS3_cda_track = strtoul(pChannel->descriptor->path, &endptr, 10);
    gPDS3_cda_status_parms.dwItem = MCI_STATUS_MODE;
    mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STATUS, MCI_WAIT | MCI_STATUS_ITEM, (DWORD_PTR)&gPDS3_cda_status_parms);
    if (gPDS3_cda_status_parms.dwReturn == MCI_MODE_NOT_READY
            || gPDS3_cda_status_parms.dwReturn == MCI_MODE_OPEN) {
        return eS3_error_start_cda;
    }
    gPDS3_cda_play_parms.dwFrom = gPDS3_cda_track % 256;
    if (gPDS3_cda_track + 1 < gPDS3_Number_of_tracks) {
        gPDS3_cda_play_parms.dwTo = (gPDS3_cda_track + 1) % 256;
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_PLAY, MCI_FROM | MCI_TO, (DWORD_PTR)&gPDS3_cda_play_parms);
    } else {
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_PLAY, MCI_FROM, (DWORD_PTR)&gPDS3_cda_play_parms);
    }
    gPDS3_cda_is_playing = 1;
    return eS3_error_none;
}

// FUNCTION: CARMA2_HW 0x0056a28d
void C2_HOOK_FASTCALL PDS3ServiceCDA(tS32 pDelta_time) {
    tU32 now;
    tS3_outlet* o;
    tS3_channel* c;

    o = gS3_state.outlets;
    for (; o != NULL; o = o->next) {
        c = o->channel_list;
        for (; c != NULL; c = c->next) {
            S3ServiceChannel(c);
            ;
        }

    }

    if (gPDS3_cda_initialized) {
        now = PDGetTotalTime();
        if (now > gPDS3_last_cda_status_update + 3000) {
            gPDS3_last_cda_status_update = now;
            gPDS3_cda_status_parms.dwCallback = (DWORD_PTR)gHWnd;
            gPDS3_cda_status_parms.dwItem = MCI_STATUS_MODE;
            mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_NOTIFY, (DWORD_PTR)&gPDS3_cda_status_parms);
            gPDS3_cda_is_playing = gPDS3_cda_status_parms.dwReturn == MCI_MODE_PLAY;
        }
    }
}
