#include "audio.h"

#include "3d.h"
#include "resource.h"
#include "s3sound.h"
#include "sample.h"
#include "platform.h"

#include "rec2_macros.h"
#include "rec2_types.h"

#include "c2_ctype.h"
#include "c2_io.h"
#include "c2_sys_stat.h"
#include "c2_stdlib.h"
#include "c2_string.h"

#include <fcntl.h>
#include <stddef.h>

C2_HOOK_VARIABLE_IMPLEMENT(int, gS3_enabled, 0x007a06c0);
C2_HOOK_VARIABLE_IMPLEMENT(tS3_sound_source*, gS3_sound_sources, 0x007a0590);
C2_HOOK_VARIABLE_IMPLEMENT(int, gS3_nsound_sources, 0x007a0584);
C2_HOOK_VARIABLE_IMPLEMENT(tS3_outlet*, gS3_outlets, 0x007a058c);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gS3_CDA_enabled, 0x00673504, 1);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gS3_path_separator, 2, 0x007a0554);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gS3_sound_folder_name, 6, 0x007a0558);
C2_HOOK_VARIABLE_IMPLEMENT(int, gS3_next_outlet_id, 0x007a0588);
C2_HOOK_VARIABLE_IMPLEMENT(int, gS3_noutlets, 0x007a0580);
C2_HOOK_VARIABLE_IMPLEMENT(int, gS3_soundbank_buffer_len, 0x006b2c80);
C2_HOOK_VARIABLE_IMPLEMENT(char*, gS3_soundbank_buffer, 0x006b2c84);
C2_HOOK_VARIABLE_IMPLEMENT(tS3_descriptor*, gS3_root_descriptor, 0x007a0598);
C2_HOOK_VARIABLE_IMPLEMENT(tS3_callbacks, gS3_callbacks, 0x006b2dc8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gS3_effects_enabled, 0x007a0758);
C2_HOOK_VARIABLE_IMPLEMENT(tS3SetEffect_cbfn*, gS3_enable_effect_cbfn, 0x007a0760);
C2_HOOK_VARIABLE_IMPLEMENT(tS3SetEffect_cbfn*, gS3_disable_effect_cbfn, 0x007a0764);
C2_HOOK_VARIABLE_IMPLEMENT(int, gS3_opened_output_devices, 0x006b2c18);
C2_HOOK_VARIABLE_IMPLEMENT(br_uint_32, gS3_last_service_time, 0x007a0568);
C2_HOOK_VARIABLE_IMPLEMENT(tS3_channel*, gS3_unbound_channels, 0x007a056c);
C2_HOOK_VARIABLE_IMPLEMENT(tS3_channel*, gS3_last_unbound_channel, 0x007a059c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gS3_enable_midi, 0x0079feb8);

int (C2_HOOK_FASTCALL * S3Init_original)(const char* pPath, int pLow_memory_mode, const char* pSound_dirname);
int C2_HOOK_FASTCALL S3Init(const char* pPath, int pLow_memory_mode, const char* pSound_dirname) {

#if 0//defined(C2_HOOKS_ENABLED)
    return S3Init_original(pPath, pLow_memory_mode, pSound_path);
#else
    tS3_descriptor* descriptor;

    /* nop_FUN_00565b39(); */

    /* FIXME: this is a struct with 7 members */
    C2V(gS3_noutlets) = 0;
    C2V(gS3_nsound_sources) = 0;
    C2V(gS3_next_outlet_id) = 0;
    C2V(gS3_outlets) = NULL;
    C2V(gS3_sound_sources) = NULL;
    C2V(gS3_descriptors) = NULL;
    C2V(gS3_root_descriptor) = NULL;

    C2_HOOK_BUG_ON(sizeof(C2V(gS3_callbacks)) != 0x10);
    c2_memset(&C2V(gS3_callbacks), 0, sizeof(C2V(gS3_callbacks)));

    S3Disable();
    S3StopMidi();
    S3StopCDA();
    C2V(gS3_effects_enabled) = 0;
    C2V(gS3_enable_effect_cbfn) = NULL;
    C2V(gS3_disable_effect_cbfn) = NULL;
    if (!PDS3Init()) {
        return eS3_error_digi_init;
    }
    C2V(gS3_opened_output_devices) = 1;

    C2_HOOK_BUG_ON(sizeof(tS3_descriptor) != 0x4c);

    descriptor = S3MemAllocate(sizeof(tS3_descriptor), kMem_S3_sentinel);
    if (descriptor == NULL) {
        return eS3_error_memory;
    }
    c2_memset(descriptor, 0, sizeof(tS3_descriptor));
    descriptor->sample_id = 0x261269;
    C2V(gS3_sound_dirname)[0] = '\0';
    C2V(gS3_descriptors) = descriptor;
    C2V(gS3_root_descriptor) = descriptor;
    if (pSound_dirname != NULL) {
        char dirpath[256];

        PDExtractDirectory(dirpath, pPath);
        c2_sprintf(C2V(gS3_sound_dirname), "%s%s", dirpath, pSound_dirname);
    }
    if (S3LoadSoundbank(pPath, pLow_memory_mode) != 0) {
        return eS3_error_soundbank;
    }
    C2V(gS3_last_service_time) = PDGetTotalTime();
    C2V(gS3_unbound_channels) = NULL;
    C2V(gS3_last_unbound_channel) = NULL;
    return eS3_error_none;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005651d0, S3Init, S3Init_original)

void C2_HOOK_FASTCALL S3Enable(void) {

    C2V(gS3_enabled) = 1;
}
C2_HOOK_FUNCTION(0x00564ce7, S3Enable)

void C2_HOOK_FASTCALL S3Disable(void) {

    S3StopAllOutletSounds();
    C2V(gS3_enabled) = 0;
}
C2_HOOK_FUNCTION(0x00564cf6, S3Disable)

int (C2_HOOK_FASTCALL * S3StopChannel_original)(tS3_channel* pChannel);
int C2_HOOK_FASTCALL S3StopChannel(tS3_channel* pChannel) {

    C2_HOOK_BUG_ON(sizeof(tS3_channel) != 120);

#if 0//defined(C2_HOOKS_ENABLED)
    return S3StopChannel_original(pChannel);
#else
    if (pChannel->tag == 0) {
        return eS3_error_bad_stag;
    }
    pChannel->termination_reason = eS3_tr_stopped;
    if (pChannel->active) {
        pChannel->needs_service = 1;
    }
    if (pChannel->type == 0) {
        if (pChannel->sound_source_ptr != NULL) {
            pChannel->sound_source_ptr->tag = 0;
            pChannel->sound_source_ptr->channel = 0;
            pChannel->sound_source_ptr->volume = 0;
        }
        if (PDS3StopSampleChannel(pChannel) == 0) {
            return eS3_error_function_failed;
        }
    } else if (pChannel->type == 1) {
        if (S3StopMIDIChannel(pChannel) != 0) {
            return eS3_error_function_failed;
        }
    } else if (pChannel->type == 2) {
        if (S3StopCDAChannel(pChannel) != 0) {
            return eS3_error_function_failed;
        }
    }

    if ((pChannel->descriptor->flags & 0x2) != 0) {
        S3ReleaseSound(pChannel->descriptor->sample_id);
    }
    pChannel->repetitions = 1;
    return eS3_error_none;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00564bfd, S3StopChannel, S3StopChannel_original)

int C2_HOOK_FASTCALL S3GetCountChannels(int pCount_channels_1, int pCount_channels_2) {

    return pCount_channels_2;
}
C2_HOOK_FUNCTION(0x005698fc, S3GetCountChannels)

tS3_outlet* (C2_HOOK_FASTCALL * S3CreateOutlet_original)(int pCount_channels_1, int pCount_channels_2);
tS3_outlet* C2_HOOK_FASTCALL S3CreateOutlet(int pCount_channels_1, int pCount_channels_2) {

#if 0//defined(C2_HOOKS_ENABLED)
    return S3CreateOutlet_original(pCount_channels_1, pCount_channels_2);
#else
    tS3_outlet* o;
    int nchannels;
    tS3_outlet* outlet;
    int channels_remaining;

    nchannels = S3GetCountChannels(pCount_channels_1, pCount_channels_2);

    if (nchannels == 0) {
        C2V(gS3_last_error) = eS3_error_channel_alloc;
        return NULL;
    }

    C2_HOOK_BUG_ON(sizeof(tS3_outlet) != 0x20);

    outlet = S3MemAllocate(sizeof(tS3_outlet), kMem_S3_outlet);
    if (outlet == NULL) {
        C2V(gS3_last_error) = eS3_error_memory;
        return NULL;
    }
    c2_memset(outlet, 0, sizeof(tS3_outlet));
    channels_remaining = S3CreateOutletChannels(outlet, nchannels);
    if (channels_remaining == nchannels) {
        S3MemFree(outlet);
        return NULL;
    }

    o = C2V(gS3_outlets);
    if (C2V(gS3_outlets) != NULL) {
        while (o->next != NULL) {
            o = o->next;
        }
        o->next = outlet;
        outlet->prev = o;
    } else {
        C2V(gS3_outlets) = outlet;
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_outlet, volume, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_outlet, max_channels, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_outlet, id, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_outlet, independent_pitch, 0xc);

    outlet->volume = 0xff;
    outlet->max_channels = nchannels - channels_remaining;
    outlet->id = C2V(gS3_next_outlet_id);
    C2V(gS3_next_outlet_id) += 1;
    outlet->independent_pitch = C2V(gPD_S3_config).independent_pitch;
    C2V(gS3_noutlets) += 1;
    return outlet;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005653c7, S3CreateOutlet, S3CreateOutlet_original)

tS3_error_codes (C2_HOOK_FASTCALL * S3ReleaseOutlet_original)(tS3_outlet* pOutlet);
tS3_error_codes C2_HOOK_FASTCALL S3ReleaseOutlet(tS3_outlet* pOutlet) {

#if 0//defined(C2_HOOKS_ENABLED)
    return S3ReleaseOutlet_original(pOutlet);
#else

    if (pOutlet == NULL) {
        return eS3_error_bad_id;
    }
    S3UnbindChannels(pOutlet);
    if (pOutlet->prev != NULL) {
        pOutlet->prev->next = pOutlet->next;
    } else {
        C2V(gS3_outlets) = pOutlet->next;
    }
    if (pOutlet->next != NULL) {
        pOutlet->next->prev = pOutlet->prev;
    }
    if (C2V(gS3_noutlets) != 0) {
        C2V(gS3_noutlets) -= 1;
        if (C2V(gS3_noutlets) == 0) {
            C2V(gS3_outlets) = NULL;
        }
    }
    S3MemFree(pOutlet);
    return eS3_error_none;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005654e2, S3ReleaseOutlet, S3ReleaseOutlet_original)

int (C2_HOOK_FASTCALL * S3SetOutletVolume_original)(tS3_outlet* pOutlet, int pVolume);
int C2_HOOK_FASTCALL S3SetOutletVolume(tS3_outlet* pOutlet, int pVolume) {

#if 0//defined(C2_HOOKS_ENABLED)
    return S3SetOutletVolume_original(pOutlet, pVolume);
#else
    int volume;

    if (!C2V(gS3_enabled)) {
        return 0;
    }

    volume = pVolume;
    volume = MIN(volume, 255);
    volume = MAX(volume, 10);
    if (pOutlet != NULL) {
        tS3_channel* channel;

        pOutlet->volume = volume;
        for (channel = pOutlet->channel_list; channel != NULL; channel = channel->next) {
            if (channel->active) {
                PDS3UpdateChannelVolume(channel);
            }
        }
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0056494f, S3SetOutletVolume, S3SetOutletVolume_original)

int C2_HOOK_FASTCALL S3StopOutletSound(tS3_outlet* pOutlet) {
    tS3_channel* c;

    if (!C2V(gS3_enabled)) {
        return 0;
    }
    for (c = pOutlet->channel_list; c != NULL; c = c->next) {
        // FUN_006a280(c);
        if (c->active) {
            c->spatial_sound = 0;
            S3StopChannel(c);
            c->needs_service = 1;
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0056575b, S3StopOutletSound)

int C2_HOOK_FASTCALL S3ReleaseSoundSource(tS3_sound_source* src) {
    tS3_sound_source* prev;
    tS3_sound_source* next;

    C2_HOOK_BUG_ON(sizeof(tS3_sound_source) != 68);

    if (!C2V(gS3_enabled)) {
        return 0;
    }

    if (src == NULL) {
        return 0;
    }
    prev = src->prev;
    next = src->next;
    if (prev != NULL) {
        prev->next = next;
    } else {
        C2V(gS3_sound_sources) = src->next;
    }
    if (next != NULL) {
        next->prev = prev;
    }
    if (C2V(gS3_nsound_sources) != 0) {
        C2V(gS3_nsound_sources)--;
        if (C2V(gS3_nsound_sources) == 0) {
            C2V(gS3_sound_sources) = NULL;
        }
    }
    S3StopSoundSource(src);
    S3MemFree(src);
    return 0;
}
C2_HOOK_FUNCTION(0x0056671e, S3ReleaseSoundSource)

void C2_HOOK_FASTCALL S3StopAllOutletSounds(void) {
    tS3_outlet* o;

    if (!C2V(gS3_enabled)) {
        return;
    }

    for (o = C2V(gS3_outlets); o != NULL; o = o->next) {
        S3StopOutletSound(o);
    }
}
C2_HOOK_FUNCTION(0x005657bd, S3StopAllOutletSounds)

tS3_error_codes (C2_HOOK_FASTCALL * S3StopSound_original)(int pTag);
tS3_error_codes C2_HOOK_FASTCALL S3StopSound(int pTag) {

#if 0//defined(C2_HOOKS_ENABLED)
    return S3StopSound_original(pTag);
#else
    tS3_channel* channel;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_channel, source_volume, 0x50);

    if (!C2V(gS3_enabled)) {
        return eS3_error_none;
    }
    if (pTag == 0) {
        return eS3_error_bad_stag;
    }
    channel = S3GetChannelForTag(pTag);
    if (channel == NULL) {
        return eS3_error_bad_stag;
    }
    channel->termination_reason = 1;
    channel->source_volume = 0;
    if (channel->active) {
        channel->needs_service = 1;
    }
    if (channel->type == 0) {
        if (channel->sound_source_ptr != NULL) {
            channel->sound_source_ptr->tag = 0;
            channel->sound_source_ptr->channel = NULL;
            channel->sound_source_ptr->volume = 0;
        }
        if (PDS3StopSampleChannel(channel) == 0) {
            return eS3_error_function_failed;
        }
    } else if (channel->type == 1) {
        if (S3StopMIDIChannel(channel) != 0) {
            return eS3_error_function_failed;
        }
    } else if (channel->type == 2) {
        if (S3StopCDAChannel(channel) != 0) {
            return eS3_error_function_failed;
        }
    }
    if (channel->descriptor->flags & 0x2) {
        S3ReleaseSound(channel->descriptor->sample_id);
    }
    channel->repetitions = 1;
    return eS3_error_none;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00564ae2, S3StopSound, S3StopSound_original)

void (C2_HOOK_FASTCALL * S3Service_original)(int inside_cockpit, int unk1);
void C2_HOOK_FASTCALL S3Service(int inside_cockpit, int unk1) {

#if defined(C2_HOOKS_ENABLED)
    S3Service_original(inside_cockpit, unk1);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00564358, S3Service, S3Service_original)

void (C2_HOOK_FASTCALL * S3StopAll_original)(void);
void C2_HOOK_FASTCALL S3StopAll(void) {

#if defined(C2_HOOKS_ENABLED)
    S3StopAll_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00455910, S3StopAll, S3StopAll_original)

int C2_HOOK_FASTCALL S3DisableSound(void) {

    S3StopAllOutletSounds();
    S3StopMidi();
    S3StopCDA();
    if (C2V(gS3_enabled)) {
        tS3_descriptor* descriptor;
        tS3_outlet* outlet;

        S3Disable();

        descriptor = C2V(gS3_descriptors);
        while (descriptor != NULL) {
            tS3_descriptor* next = descriptor->next;
            S3ReleaseSound(descriptor->sample_id);
            S3MemFree(descriptor);
            descriptor = next;
        }

        outlet = C2V(gS3_outlets);
        while (outlet != NULL) {
            tS3_outlet* next = outlet->next;
            S3ReleaseOutlet(outlet);
            outlet = next;
        }

        S3FreeUnboundChannels();
    }
    if (C2V(gS3_opened_output_devices)) {
        PDS3Stop();
    }
    /* nop_FUN_00565b34(); */
    return 0;
}
C2_HOOK_FUNCTION(0x0056532d, S3DisableSound)

void C2_HOOK_FASTCALL S3EnableCDA(void) {

    C2V(gS3_CDA_enabled) = 1;
}
C2_HOOK_FUNCTION(0x00565b70, S3EnableCDA)

int C2_HOOK_FASTCALL S3CreateOutletChannels(tS3_outlet* outlet, int pChannel_count) {
    tS3_channel* chan;
    tS3_channel* last_chan;

    last_chan = NULL;
    while (pChannel_count != 0) {
        C2_HOOK_BUG_ON(sizeof(tS3_channel) != 0x78);

        chan = S3MemAllocate(sizeof(tS3_channel), kMem_S3_channel_00593a58);
        if (chan == NULL) {
            return pChannel_count;
        }
        c2_memset(chan, 0, sizeof(tS3_channel));
        chan->owner_outlet = outlet;

        if (!S3CreateTypeStructs(chan)) {
            S3MemFree(chan);
            return pChannel_count;
        }

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_channel, next, 0x64);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_outlet, channel_list, 0x10);

        if (last_chan != NULL) {
            last_chan->next = chan;
        } else {
            outlet->channel_list = chan;
        }
        last_chan = chan;
        pChannel_count--;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00565571, S3CreateOutletChannels)

void* (C2_HOOK_FASTCALL * S3LoadSoundBankFile_original)(const char* pPath);
void* C2_HOOK_FASTCALL S3LoadSoundBankFile(const char* pPath) {

#if 0//defined(C2_HOOKS_ENABLED)
    return S3LoadSoundBankFile_original(pPath);
#else
    size_t bytes_read;
    char* buffer;
    char path[512];

    size_t file_size;

    if (C2V(gS3_low_memory_mode)) {
        FILE* f;

        C2V(gS3_last_error) = eS3_error_readfile;
        c2_sprintf(path, "DATA%sSOUNDS%s%s", C2V(gS3_path_separator), C2V(gS3_path_separator), pPath);
        f = S3_low_memory_fopen(path, "rb");
        if (f == NULL) {
            return NULL;
        }
        file_size = S3GetFileSize(f);
        buffer = S3MemAllocate(file_size + 1, kMem_S3_sample);
        if (buffer == NULL) {
            c2_fclose(f);
            C2V(gS3_last_error) = eS3_error_memory;
            return NULL;
        }
        buffer[file_size] = '\0';
        bytes_read = c2_fread(buffer, 1, file_size, f);
        if (bytes_read != file_size) {
            c2_fclose(f);
            C2V(gS3_last_error) = eS3_error_readfile;
            return NULL;
        }
        C2V(gS3_soundbank_buffer) = buffer;
        C2V(gS3_soundbank_buffer_len) = file_size;
    } else {
        struct _stat32 stat;
        int fd;

        C2_HOOK_BUG_ON(_O_BINARY != 0x8000);
        if (C2V(gS3_sound_dirname)[0] == '\0') {
            fd = c2_open(pPath, _O_BINARY);
        } else {
            char filename[512];

            PDExtractFilename(filename, pPath);
            c2_sprintf(path, "%s%s%s", C2V(gS3_sound_dirname), C2V(gS3_path_separator), filename);
            fd = c2_open(path, _O_BINARY);
        }
        if (fd == -1) {
            C2V(gS3_last_error) = eS3_error_readfile;
            return NULL;
        }
        if (c2_fstat32(fd, &stat) != 0) {
            C2V(gS3_last_error) = eS3_error_readfile;
            return NULL;
        }
        buffer = S3MemAllocate(stat.st_size + 1, kMem_S3_sample);
        if (buffer == NULL) {
            c2_close(fd);
            C2V(gS3_last_error) = eS3_error_memory;
            return NULL;
        }
        buffer[stat.st_size] = '\0';
        bytes_read = c2_read(fd, buffer, stat.st_size);
        if (bytes_read != stat.st_size) {
            c2_close(fd);
            C2V(gS3_last_error) = eS3_error_readfile;
            return NULL;
        }
        C2V(gS3_soundbank_buffer) = buffer;
        C2V(gS3_soundbank_buffer_len) = stat.st_size;
        c2_close(fd);
    }
    return buffer;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00568c2c, S3LoadSoundBankFile, S3LoadSoundBankFile_original)

int C2_HOOK_FASTCALL S3LoadSoundbank(const char* pPath, int pLow_memory_mode) {
    tS3_soundbank_read_ctx read_ctx;
    char soundbank_filename[512];
    void *buffer;
    char dir_name[512];
    const char* cur_dir;

    if (!C2V(gS3_enabled)) {
        return 0;
    }
    if (pLow_memory_mode == 0x123456) {
        C2V(gS3_low_memory_mode) = 1;
        c2_strcpy(soundbank_filename, pPath);
        dir_name[0] = '\0';
        buffer = S3LoadSoundBankFile(soundbank_filename);
        pLow_memory_mode = 0;
    } else {
        dir_name[0] = '\0';
        soundbank_filename[0] = '\0';
        cur_dir = PDS3GetWorkingDirectory();
        c2_strcpy(dir_name, cur_dir);
        c2_strcat(dir_name, C2V(gS3_path_separator));
        c2_strcat(dir_name, "DATA");
        c2_strcat(dir_name, C2V(gS3_path_separator));
        c2_strcat(dir_name, C2V(gS3_sound_folder_name));
        c2_strcat(dir_name, C2V(gS3_path_separator));
        c2_strcpy(soundbank_filename, pPath);
        buffer = S3LoadSoundBankFile(soundbank_filename);
    }
    if (buffer == NULL) {
        return C2V(gS3_last_error);
    }
    read_ctx.nlines = 0;
    read_ctx.data_len = C2V(gS3_soundbank_buffer_len);
    read_ctx.data = buffer;
    S3SoundBankReaderSkipWhitespace(&read_ctx);
    while (S3SoundBankReadEntry(&read_ctx, dir_name, pLow_memory_mode) != 0) {
    }
    S3MemFree(buffer);
    return 0;
}
C2_HOOK_FUNCTION(0x00567fe0, S3LoadSoundbank)

void (C2_HOOK_FASTCALL * S3SoundBankReaderSkipWhitespace_original)(tS3_soundbank_read_ctx* pContext);
void C2_HOOK_FASTCALL S3SoundBankReaderSkipWhitespace(tS3_soundbank_read_ctx* pContext) {

#if 0//defined(C2_HOOKS_ENABLED)
    S3SoundBankReaderSkipWhitespace_original(pContext);
#else

    while (pContext->data_len != 0) {
        if (c2_isalnum(*pContext->data) || *pContext->data == '-') {
            break;
        }
        S3SoundBankReaderSkipToNewline(pContext);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0056818f, S3SoundBankReaderSkipWhitespace, S3SoundBankReaderSkipWhitespace_original)

int (C2_HOOK_FASTCALL * S3SoundBankReadEntry_original)(tS3_soundbank_read_ctx *pContext, const char* pDir_name, int pLow_memory_mode);
int C2_HOOK_FASTCALL S3SoundBankReadEntry(tS3_soundbank_read_ctx *pContext, const char* pDir_name, int pLow_memory_mode) {

#if 0//defined(C2_HOOKS_ENABLED)
    return S3SoundBankReadEntry_original(pContext, pDir_name, pLow_memory_mode);
#else
    tS3_descriptor* descriptor;
    int i;
    int n;
    const char* dir_name;
    double f1, f2;
    int count;
    C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, cda_dir_name, 4, 0x006b2c8c);

    descriptor = S3CreateDescriptor();
    if (descriptor == NULL) {
        return C2V(gS3_last_error);
    }

    /* Sound ID */
    if (c2_sscanf(pContext->data, "%i%n", &descriptor->sample_id, &n) != 1) {
        return 0;
    }
    S3SoundBankReaderAdvance(pContext, n);
    S3SoundBankReaderNextLine(pContext);

    /* Type, Flags */
    if (c2_sscanf(pContext->data, "%i,%i%n", &descriptor->type, &descriptor->flags, &n) != 2) {
        return 0;
    }
    S3SoundBankReaderAdvance(pContext, n);
    S3SoundBankReaderNextLine(pContext);

    /* File name */
    dir_name = pDir_name;
    if (descriptor->type == 2) {
        dir_name = C2V(cda_dir_name);
        C2V(cda_dir_name)[0] = '\0';
    }
    if (!S3SoundBankReaderReadFilename(&descriptor->path, pContext, dir_name)) {
        return 0;
    }
    S3SoundBankReaderNextLine(pContext);

    /* Priority */
    if (c2_sscanf(pContext->data, "%i%n", &descriptor->priority, &n) != 1) {
        return 0;
    }
    S3SoundBankReaderAdvance(pContext, n);
    S3SoundBankReaderNextLine(pContext);

    /* Repeat rate */
    if (c2_sscanf(pContext->data, "%i%n", &descriptor->repeat_rate, &n) != 1) {
        return 0;
    }
    S3SoundBankReaderAdvance(pContext, n);
    S3SoundBankReaderNextLine(pContext);

    /* MinVol, MaxVol */
    if (c2_sscanf(pContext->data, "%i,%i%n", &descriptor->min_volume, &descriptor->max_volume, &n) != 2) {
        return 0;
    }
    S3SoundBankReaderAdvance(pContext, n);
    S3SoundBankReaderNextLine(pContext);

    /* MinPitch, MaxPitch */
    if (c2_sscanf(pContext->data, "%lf,%lf%n", &f1, &f2, &n) != 2) {
        return 0;
    }
    S3SoundBankReaderAdvance(pContext, n);
    S3SoundBankReaderNextLine(pContext);
    if (f1 == 0.) {
        f1 = 1.875;
    }
    if (f2 == 0.f) {
        f2 = 1.875;
    }
    descriptor->min_pitch = (int)ldexp(f1, 16);
    descriptor->max_pitch = (int)ldexp(f2, 16);

    /* MinSpeed, MaxSpeed */
    if (c2_sscanf(pContext->data, "%lf,%lf%n", &f1, &f2, &n) != 2) {
        return 0;
    }
    S3SoundBankReaderAdvance(pContext, n);
    S3SoundBankReaderNextLine(pContext);
    if (f1 == 0.) {
        f1 = 1.875;
    }
    if (f2 == 0.) {
        f2 = 1.875;
    }
    descriptor->min_speed = (int)ldexp(f1, 16);
    descriptor->max_speed = (int)ldexp(f2, 16);

    /* Special FX index */
    if (c2_sscanf(pContext->data, "%i%n", &descriptor->effects_enabled, &n) != 1) {
        return 0;
    }
    S3SoundBankReaderAdvance(pContext, n);
    S3SoundBankReaderNextLine(pContext);

    /* Number of low memory alternatives */
    if (c2_sscanf(pContext->data, "%d%n", &count, &n) != 1) {
        return 0;
    }
    S3SoundBankReaderAdvance(pContext, n);
    S3SoundBankReaderNextLine(pContext);
    descriptor->low_memory_alternative = -1;
    for (i = 0; i < count; i++) {
        int alternative;

        if (c2_sscanf(pContext->data, "%d%n", &alternative, &n) != 1) {
            return 0;
        }
        if (i + 1 == pLow_memory_mode) {
            descriptor->low_memory_alternative = alternative;
        }
        S3SoundBankReaderAdvance(pContext, n);
        S3SoundBankReaderNextLine(pContext);
    }

    if ((descriptor->flags & 0x1) && descriptor->low_memory_alternative == -1) {
        if (descriptor->type == 1) {
            descriptor->buffer_description = NULL;
        } else {
            if (S3LoadSample(descriptor->sample_id) != eS3_error_none) {
                c2_printf("\nSound bank file: couldn't load '%s'\n", descriptor->path);
                pContext->data_len = 1;
                return 0;
            }
        }
    }
    return 1;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0056828c, S3SoundBankReadEntry, S3SoundBankReadEntry_original)

void C2_HOOK_FASTCALL S3SoundBankReaderNextLine(tS3_soundbank_read_ctx* pContext) {

    S3SoundBankReaderSkipToNewline(pContext);
    S3SoundBankReaderSkipWhitespace(pContext);
}
C2_HOOK_FUNCTION(0x005686e9, S3SoundBankReaderNextLine)

void C2_HOOK_FASTCALL S3SoundBankReaderSkipToNewline(tS3_soundbank_read_ctx* pContext) {
    char* newline_ptr;

    newline_ptr = c2_memchr(pContext->data, '\n', pContext->data_len);
    if (newline_ptr != NULL) {
        S3SoundBankReaderAdvance(pContext, newline_ptr + 1 - pContext->data);
        pContext->nlines += 1;
    } else {
        pContext->data_len = 0;
    }
}
C2_HOOK_FUNCTION(0x00568205, S3SoundBankReaderSkipToNewline)

void C2_HOOK_FASTCALL S3SoundBankReaderAdvance(tS3_soundbank_read_ctx* pContext, int pAmount) {

    pContext->data += pAmount;
    pContext->data_len -= pAmount;
}
C2_HOOK_FUNCTION(0x00568260, S3SoundBankReaderAdvance)

tS3_descriptor* C2_HOOK_FASTCALL S3CreateDescriptor(void) {
    tS3_descriptor* descriptor;

    C2_HOOK_BUG_ON(sizeof(tS3_descriptor) != 0x4c);

    descriptor = S3MemAllocate(sizeof(tS3_descriptor), kMem_S3_descriptor);
    if (descriptor == NULL) {
        C2V(gS3_last_error) = eS3_error_memory;
        return NULL;
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_descriptor, prev, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_descriptor, next, 0x2c);

    c2_memset(descriptor, 0, sizeof(tS3_descriptor));
    C2V(gS3_root_descriptor)->next = descriptor;
    descriptor->prev = C2V(gS3_root_descriptor);
    C2V(gS3_root_descriptor) = descriptor;
    return descriptor;
}
C2_HOOK_FUNCTION(0x00565a8a, S3CreateDescriptor)

int C2_HOOK_FASTCALL S3SoundBankReaderReadFilename(char** pPath, tS3_soundbank_read_ctx* pContext, const char* pDir_name) {
    char* data_start;
    unsigned int bytes_read;
    unsigned int dir_name_len;

    data_start = pContext->data;
    dir_name_len = c2_strlen(pDir_name);
    while (pContext->data_len != 0) {
        if (c2_isspace(*pContext->data)) {
            break;
        }
        S3SoundBankReaderAdvance(pContext, 1);
    }
    bytes_read = pContext->data - data_start;
    if (bytes_read == 0) {
        return 0;
    }
    *pPath = S3MemAllocate(bytes_read + dir_name_len + 1, kMem_S3_scan_name);
    if (*pPath == NULL) {
        return 0;
    }
    c2_strcpy(*pPath, pDir_name);
    c2_memcpy(&(*pPath)[dir_name_len], data_start, bytes_read);
    (*pPath)[bytes_read + dir_name_len] = '\0';
    return 1;
}
C2_HOOK_FUNCTION(0x00568704, S3SoundBankReaderReadFilename)

void (C2_HOOK_FASTCALL * S3StopMidi_original)(void);
void C2_HOOK_FASTCALL S3StopMidi(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    S3StopMidi_original();
#else

    S3StopMidiInternal();
    C2V(gS3_enable_midi) = 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0056a41f, S3StopMidi, S3StopMidi_original)


void (C2_HOOK_FASTCALL * S3StopCDA_original)(void);
void C2_HOOK_FASTCALL S3StopCDA(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    S3StopCDA_original();
#else
    S3StopCDAInternal();
    C2V(gS3_CDA_enabled) = 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00565b7f, S3StopCDA, S3StopCDA_original)

void C2_HOOK_FASTCALL S3StopMidiInternal(void) {

    if (C2V(gS3_enable_midi)) {
        tS3_outlet* outlet;
        for (outlet = C2V(gS3_outlets); outlet != NULL; outlet = outlet->next) {
            tS3_channel *channel;

            for (channel = outlet->channel_list; channel != NULL; channel = channel->next) {
                if (channel->type == 1) {
                    S3StopMIDIChannel(channel);
                }
            }
        }
    }
}
C2_HOOK_FUNCTION(0x0056a7bd, S3StopMidiInternal)

int C2_HOOK_FASTCALL S3StopMIDIChannel(tS3_channel* pChannel) {

    if (C2V(gS3_enable_midi) && pChannel->active && pChannel->type == 1) {
        PDS3StopMidiChannel(pChannel);
        pChannel->active = 0;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0056a4cd, S3StopMIDIChannel)

int C2_HOOK_FASTCALL S3StopCDAInternal(void) {

    if (C2V(gS3_CDA_enabled)) {
        tS3_outlet* outlet;

        for (outlet = C2V(gS3_outlets); outlet != NULL; outlet = outlet->next) {
            tS3_channel* channel;
            for (channel = outlet->channel_list; channel != NULL; channel = channel->next) {
                if (channel->type == 2) {
                    PDS3StopCDAChannel(channel);
                }
            }
        }
    }
    return 1;
}
C2_HOOK_FUNCTION(0x00565c22, S3StopCDAInternal)

int C2_HOOK_FASTCALL S3StopCDAChannel(tS3_channel* pChannel) {

    if (!C2V(gS3_CDA_enabled)) {
        return 0;
    }
    return PDS3StopCDAChannel(pChannel);
}
C2_HOOK_FUNCTION(0x00565bfe, S3StopCDAChannel)

int (C2_HOOK_FASTCALL * S3UnbindChannels_original)(tS3_outlet* pOutlet);
int C2_HOOK_FASTCALL S3UnbindChannels(tS3_outlet* pOutlet) {

#if 0//defined(C2_HOOKS_ENABLED)
    return S3UnbindChannels_original(pOutlet);
#else
    tS3_channel* chan;
    tS3_channel* next;

    for (chan = pOutlet->channel_list; chan != NULL; chan = next) {
        if (chan->active) {
            chan->termination_reason = 1;
            S3StopChannel(chan);
            chan->needs_service = 1;
        }
        next = chan->next;
        /* nop1_FUN_0056991f(chan); */
        if (C2V(gS3_unbound_channels) == NULL) {
            C2V(gS3_unbound_channels) = chan;
        } else {
            C2V(gS3_last_unbound_channel)->next = chan;
        }
        C2V(gS3_last_unbound_channel) = chan;

        C2_HOOK_BUG_ON(sizeof(tS3_channel) != 0x78);
        c2_memset(chan, 0, sizeof(tS3_channel));
    }
    pOutlet->channel_list = NULL;
    return 1;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00565607, S3UnbindChannels, S3UnbindChannels_original)

tS3_channel* C2_HOOK_FASTCALL S3GetChannelForTag(int pTag) {
    tS3_outlet* outlet;

    if (pTag == 0) {
        return NULL;
    }
    for (outlet = C2V(gS3_outlets); outlet != NULL; outlet = outlet->next) {
        if (outlet->id == (pTag & 0xff)) {
            tS3_channel* channel;

            for (channel = outlet->channel_list; channel != NULL; channel = channel->next) {
                if (channel->tag == pTag) {
                    return channel;
                }
            }
            return NULL;
        }
    }
    return NULL;
}
C2_HOOK_FUNCTION(0x00565888, S3GetChannelForTag)

tS3_error_codes C2_HOOK_FASTCALL S3ClearBufferOfMidiChannel(int pTag) {

    if (C2V(gS3_enable_midi)) {
        tS3_channel* channel;

        channel = S3GetChannelForTag(pTag);
        if (channel == NULL) {
            return eS3_error_bad_stag;
        }
        if (channel->type == 1) {
            /* nop0_FUN_00569f85(); */
            channel->descriptor->buffer_description = NULL;
        }
    }
    return eS3_error_none;
}
C2_HOOK_FUNCTION(0x0056a818, S3ClearBufferOfMidiChannel)

int C2_HOOK_FASTCALL S3ReleaseSound(int pSound_id) {
    tS3_descriptor* descriptor;

    if (!C2V(gS3_enabled)) {
        return eS3_error_none;
    }

    descriptor = S3GetDescriptorByID(pSound_id);
    if (descriptor == NULL) {
        return eS3_error_bad_id;
    }
    if (descriptor->type == 1) {
        tS3_outlet *outlet;

        for (outlet = C2V(gS3_outlets); outlet != NULL; outlet = outlet->next) {
            tS3_channel *channel;

            for (channel = outlet->channel_list; channel != NULL; channel = channel->next) {
                if (channel->descriptor != NULL && channel->descriptor->sample_id == pSound_id) {
                    S3ClearBufferOfMidiChannel(channel->tag);
                }
            }
        }
    } else if (descriptor->type == 0) {
        tS3_buffer_desc *description;


        description = descriptor->buffer_description;

        if (description == NULL) {
            return 0;
        }
        PDS3ReleaseSound(descriptor);
        if (description->field_0x14 != NULL) {
            S3MemFree(description->field_0x14);
        }
        S3MemFree(description);
        descriptor->buffer_description = NULL;
    }
    return eS3_error_none;
}
C2_HOOK_FUNCTION(0x00568929, S3ReleaseSound)

double C2_HOOK_STDCALL S3FRandomBetween(double pMin, double pMax) {

    return (double)c2_rand() * (pMax - pMin) / 32767. + pMin;
}
C2_HOOK_FUNCTION(0x00566454, S3FRandomBetween)

int C2_HOOK_FASTCALL S3IRandomBetweenLog(int pMin, int pMax, int pDefault) {
    double dbl;

    if (pMin == -1 || pMin >= pMax) {
        return pDefault;
    }
    dbl = S3FRandomBetween(log(pMin), log(pMax));
    return (int)(ldexp(exp(dbl), -16) * (double)pDefault);
}
C2_HOOK_FUNCTION(0x005663dd, S3IRandomBetweenLog)

int C2_HOOK_FASTCALL S3FreeUnboundChannels(void) {
    tS3_channel *channel;

    channel = C2V(gS3_unbound_channels);
    while (channel != NULL) {
        tS3_channel* next;

        next = channel->next;
        S3MemFree(channel);
        channel = next;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x005656b7, S3FreeUnboundChannels)
