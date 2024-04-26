#ifndef S3_SOUND_H
#define S3_SOUND_H

#include "c2_hooks.h"

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
typedef struct tS3_descriptor tS3_descriptor;
typedef struct tS3_outlet tS3_outlet;
typedef struct tS3_sound_source tS3_sound_source;

/* FIXME: verify elements of tS3_termination_reason */
typedef enum {
    eS3_tr_natural = 0,
    eS3_tr_stopped = 1,
    eS3_tr_overridden = 2,
} tS3_termination_reason;

// FIXME: add call convention!!
typedef void ( * tS3_outlet_callback)(tS3_outlet*, tS3_sound_tag, tS3_termination_reason);
typedef void ( * tS3_sample_filter)(tS3_effect_tag, tS3_sound_tag);

typedef struct tS3_vector3 {
    tF32 x;
    tF32 y;
    tF32 z;
} tS3_vector3;

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
    int fieldd_0x50;
    undefined4 field_0x54;
    float field_0x58;
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

int C2_HOOK_FASTCALL S3Init(const char* pPath, int pLow_memory_mode, const char* pSound_path);

void C2_HOOK_FASTCALL S3Enable(void);

void C2_HOOK_FASTCALL S3Disable(void);

tS3_outlet* C2_HOOK_FASTCALL S3CreateOutlet(int pCount_channels_1, int pCount_channels_2);

int C2_HOOK_FASTCALL S3ReleaseOutlet(tS3_outlet* pOutlet);

int C2_HOOK_FASTCALL S3SetOutletVolume(tS3_outlet* pOutlet, int pVolume);

void C2_HOOK_FASTCALL S3StopAllOutletSounds(void);

int C2_HOOK_FASTCALL S3StopOutletSound(tS3_outlet* pOutlet);

int C2_HOOK_FASTCALL S3ReleaseSoundSource(tS3_sound_source* src);

int C2_HOOK_FASTCALL S3StopSound(int pTag);

void C2_HOOK_FASTCALL S3Service(int inside_cockpit, int unk1);

void C2_HOOK_FASTCALL S3UpdateSoundSource(tS3_outlet* outlet, tS3_sound_tag tag, tS3_sound_source* src, float pMax_distance_squared, int pPeriod, tS3_repeats pAmbient_repeats, tS3_volume pVolume, int pPitch, tS3_speed pSpeed);

void C2_HOOK_FASTCALL S3StopAll(void);

int C2_HOOK_STDCALL S3Set3DSoundEnvironment(float a1, float a2, float a3);

#endif
