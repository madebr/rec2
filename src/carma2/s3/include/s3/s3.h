#ifndef S3_SOUND_H
#define S3_SOUND_H

#include "c2_hooks.h"

// FIXME: alternative definition for br_uint_16/br_uint_32?
#include <brender/brender.h>

#include <stdlib.h> /* uintptr_t */

// External typedefs

typedef float tF32;
typedef int tS3_sound_tag;
typedef int tS3_sound_id;
typedef int tS3_type;
typedef int tS3_repeats;
typedef int tS3_volume;
typedef int tS3_effect_tag;
typedef long tS3_priority;
typedef long tS3_pitch;
typedef long tS3_speed;
typedef struct tS3_channel tS3_channel;
typedef struct tS3_outlet tS3_outlet;
typedef struct tS3_sound_source tS3_sound_source;

/* FIXME: rename to tS3_hardware_info */
typedef struct {
    int free_direct_sound;
    undefined4 field_0x04;
    undefined4 field_0x08;
    undefined4 field_0x0c;
    undefined4 field_0x10;
    undefined4 field_0x14;
    undefined4 field_0x18;
    int independent_pitch;
} tPD_S3_config;

typedef struct {
    int data_len;
    undefined4 field_0x4;
    int nlines;
    char* data;
} tS3_soundbank_read_ctx;

/* FIXME: verify elements of tS3_termination_reason */
typedef enum {
    eS3_tr_natural = 0,
    eS3_tr_stopped = 1,
    eS3_tr_overridden = 2,
} tS3_termination_reason;

typedef enum tS3_error_codes {
    eS3_error_none = 0,
    eS3_error_digi_init = 1,
    eS3_error_memory = 3,
    eS3_error_readfile = 4,
    eS3_error_soundbank = 5,
    eS3_error_bad_id = 6,
    eS3_error_bad_stag = 7,
    eS3_error_channel_alloc = 10,
    eS3_error_function_failed = 12,
} tS3_error_codes;

typedef struct {
    br_uint_8 magic[4];
    br_uint_32 chunk_size; /* remaining size of this chunk */
} tS3_riff_chunk_header;

/* **MUST** match WAVEFORMATEX */
typedef struct {
    br_uint_16 type_format;
    br_uint_16 number_of_channels;
    br_uint_32 sample_rate;
    br_uint_32 avg_bytes_per_second;
    br_uint_16 data_block_sizee;
    br_uint_16 bits_per_sample;
} tS3_wav_chunk_info_header;

typedef struct {
    tS3_riff_chunk_header header;
    tS3_wav_chunk_info_header wav_info_header;
} tS3_riff_fmt_chunk;

typedef struct {
    tS3_riff_chunk_header header;
    br_uint_8 data[];
} tS3_riff_data_chunk;

typedef struct {
    tS3_riff_chunk_header riff_header; /* chunk_size means size of riff file */
    br_uint_8 wave_magic[4];
    tS3_riff_fmt_chunk fmt_chunk;
    tS3_riff_data_chunk data_chunk;
} tS3_wav_file;

typedef struct {
    int sample_size;
    tS3_wav_chunk_info_header* wav_info_header;
    void* samples;
} tS3_wav_info;

// FIXME: add call convention!!
typedef void ( * tS3_outlet_callback)(tS3_outlet*, tS3_sound_tag, tS3_termination_reason);
typedef void ( * tS3_sample_filter)(tS3_effect_tag, tS3_sound_tag);

typedef struct tS3_vector3 {
    tF32 x;
    tF32 y;
    tF32 z;
} tS3_vector3;

typedef struct {
    int sample_size;
    int sample_rate;
    int bytes_per_second;
    int count_channels;
    undefined4 field_0x10;
    undefined4* field_0x14;
} tS3_buffer_desc;

typedef struct tS3_descriptor {
    int sample_id;
    undefined4 type;
    int priority;
    int repeat_rate;
    int min_volume;
    int max_volume;
    int min_pitch;
    int max_pitch;
    int min_speed;
    int max_speed;
    struct tS3_descriptor* prev;
    struct tS3_descriptor* next;
    int flags;
    void* field_0x34;
    int effects_enabled;
    int low_memory_alternative;
    tS3_buffer_desc* buffer_description;
    char* path;
    void* pd_handle;
} tS3_descriptor;

typedef struct tS3_sound_source {
    tS3_channel* channel;
    int brender_vector;
    void* position_ptr;
    void* velocity_ptr;
    tS3_outlet* bound_outlet;
    tS3_sound_source* prev;
    tS3_sound_source* next;
    int ambient;
    tS3_repeats ambient_repeats;
    int time_since_last_played;
    tS3_sound_id sound_id;
    float max_distance_sq;
    int period;
    tS3_volume volume;
    tS3_pitch pitch;
    tS3_speed speed;
    tS3_sound_tag tag;
} tS3_sound_source;

typedef struct tS3_channel {
    int active;
    tS3_termination_reason termination_reason;
    int needs_service;
    int repetitions;
    int rate;
    int spatial_sound;
    uintptr_t mciDevice; /* MCIDEVICE */
    tS3_sound_tag tag;
    tS3_type type;
    int volume_multiplier;
    float field_0x28;
    tS3_vector3 position;
    tS3_vector3 velocity;
    tS3_vector3 lastpos;
    int source_volume;
    int source_rate;
    float max_distance_squared;
    tS3_outlet* owner_outlet;
    tS3_descriptor* descriptor;
    tS3_channel* next;
    char* type_struct_sample;
    char* type_struct_midi;
    char* type_struct_cda;
    tS3_sound_source* sound_source_ptr;
} tS3_channel;

typedef struct tS3_outlet {
    int id;
    int max_channels;
    int volume;
    int independent_pitch;
    tS3_channel* channel_list;
    tS3_outlet* prev;
    tS3_outlet* next;
    tS3_outlet_callback* callback;
} tS3_outlet;

typedef struct {
    undefined4 field_0x0;
    void (C2_HOOK_FASTCALL * on_sample_channel_stop)(tS3_channel* pChannel);
    int (C2_HOOK_FASTCALL * on_sample_channel_set_volume)(tS3_channel* pChannel, int pVolume);
    int (C2_HOOK_FASTCALL * on_sample_channel_set_pan)(tS3_channel* pChannel, float pPan);
} tS3_callbacks;

typedef void C2_HOOK_FASTCALL tS3SetEffect_cbfn(int pEffect_index, int pSound_tag);

C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gS3_path_separator, 2);
C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gS3_sound_folder_name, 6);
C2_HOOK_VARIABLE_DECLARE(tS3_callbacks, gS3_callbacks);
C2_HOOK_VARIABLE_DECLARE(int, gS3_CDA_enabled);

int C2_HOOK_FASTCALL S3Init(const char* pPath, int pLow_memory_mode, const char* pSound_path);

void C2_HOOK_FASTCALL S3Enable(void);

void C2_HOOK_FASTCALL S3Disable(void);

tS3_outlet* C2_HOOK_FASTCALL S3CreateOutlet(int pCount_channels_1, int pCount_channels_2);

tS3_error_codes C2_HOOK_FASTCALL S3ReleaseOutlet(tS3_outlet* pOutlet);

int C2_HOOK_FASTCALL S3SetOutletVolume(tS3_outlet* pOutlet, int pVolume);

void C2_HOOK_FASTCALL S3StopAllOutletSounds(void);

int C2_HOOK_FASTCALL S3StopOutletSound(tS3_outlet* pOutlet);

int C2_HOOK_FASTCALL S3ReleaseSoundSource(tS3_sound_source* src);

int C2_HOOK_FASTCALL S3StopSound(int pTag);

void C2_HOOK_FASTCALL S3Service(int inside_cockpit, int unk1);

void C2_HOOK_FASTCALL S3UpdateSoundSource(tS3_outlet* outlet, tS3_sound_tag tag, tS3_sound_source* src, float pMax_distance_squared, int pPeriod, tS3_repeats pAmbient_repeats, tS3_volume pVolume, int pPitch, tS3_speed pSpeed);

int C2_HOOK_STDCALL S3Set3DSoundEnvironment(float a1, float a2, float a3);

tS3_buffer_desc* C2_HOOK_FASTCALL S3GetBufferDescription(int pSample_id);

tS3_error_codes C2_HOOK_FASTCALL S3LoadSample(int pSample_id);

void C2_HOOK_FASTCALL S3EnableCDA(void);

int C2_HOOK_FASTCALL PDReverseAudio(tS3_channel* pChannel);

int C2_HOOK_FASTCALL S3DisableSound(void);

int C2_HOOK_FASTCALL S3SoundStillPlaying(int pTag);

int C2_HOOK_FASTCALL S3SetVolume(int pVolume);

#endif
