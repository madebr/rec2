#include "audio.h"

#include "3d.h"
#include "resource.h"
#include "s3sound.h"
#include "sample.h"
#include "platform.h"

#include "rec2_types.h"

#include "c2_ctype.h"
#include "c2_io.h"
#include "c2_sys_stat.h"
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

int (C2_HOOK_FASTCALL * S3Init_original)(const char* pPath, int pLow_memory_mode, const char* pSound_path);
int C2_HOOK_FASTCALL S3Init(const char* pPath, int pLow_memory_mode, const char* pSound_path) {

#if defined(C2_HOOKS_ENABLED)
    return S3Init_original(pPath, pLow_memory_mode, pSound_path);
#else
#error "Not implemented"
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

int (C2_HOOK_FASTCALL * S3StopChannel_original)(tS3_channel* chan);
int C2_HOOK_FASTCALL S3StopChannel(tS3_channel* chan) {

    C2_HOOK_BUG_ON(sizeof(tS3_channel) != 120);

#if defined(C2_HOOKS_ENABLED)
    return S3StopChannel_original(chan);
#else
    if (chan-> == 0) {
        return eS3_error_bad_stag;
    }
    chan->termination_reason = eS3_tr_stopped;
    if (chan->active) {
        chan->needs_service = 1;
    }
    if (chan->type == eS3_ST_sample) {
        if (chan->sound_source_ptr != NULL) {
            chan->sound_source_ptr->tag = 0;
            chan->sound_source_ptr->channel = 0;
            chan->sound_source_ptr->volume = 0;
        }
        if (S3StopSample(chan) == 0) {
            return eS3_error_function_failed;
        }
    } else if (chan->type == eS3_ST_midi) {
        if (S3StopMIDI(chan) != 0) {
            return eS3_error_function_failed;
        }
    } else if (chan->type == eS3_ST_cda) {
        if (S3StopCDA(chan) != 0) {
            return eS3_error_function_failed;
        }
    }

    if ((chan->descriptor->flags & 2) != 0) {
        S3ReleaseSound(chan->descriptor->id);
    }
    chan->repetitions = 1;
    return 0;
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

int (C2_HOOK_FASTCALL * S3ReleaseOutlet_original)(tS3_outlet* pOutlet);
int C2_HOOK_FASTCALL S3ReleaseOutlet(tS3_outlet* pOutlet) {

#if defined(C2_HOOKS_ENABLED)
    return S3ReleaseOutlet_original(pOutlet);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005654e2, S3ReleaseOutlet, S3ReleaseOutlet_original)

int (C2_HOOK_FASTCALL * S3SetOutletVolume_original)(tS3_outlet* pOutlet, int pVolume);
int C2_HOOK_FASTCALL S3SetOutletVolume(tS3_outlet* pOutlet, int pVolume) {

#if defined(C2_HOOKS_ENABLED)
    return S3SetOutletVolume_original(pOutlet, pVolume);
#else
#error "Not implemented"
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

int (C2_HOOK_FASTCALL * S3StopSound_original)(int pTag);
int C2_HOOK_FASTCALL S3StopSound(int pTag) {

#if defined(C2_HOOKS_ENABLED)
    return S3StopSound_original(pTag);
#else
#error "Not implemented"
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

void C2_HOOK_FASTCALL S3EnableCDA(void) {

    C2V(gS3_CDA_enabled);
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

#if defined(C2_HOOKS_ENABLED)
    return S3SoundBankReadEntry_original(pContext, pDir_name, pLow_memory_mode);
#else
#error "Not implemented"
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
