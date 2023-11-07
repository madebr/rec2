#ifndef S3_SOUND_H
#define S3_SOUND_H

#include "c2_hooks.h"

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


typedef void tS3_sample_filter(tS3_effect_tag, tS3_sound_tag);

typedef struct tS3_vector3 {
    tF32 x;
    tF32 y;
    tF32 z;
} tS3_vector3;


#endif
