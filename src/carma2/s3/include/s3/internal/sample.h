#ifndef S3_SAMPLE_H
#define S3_SAMPLE_H

#include <s3/s3.h>

extern int gS3_low_memory_mode;
extern tS3_error_codes gS3_last_error;
extern char gS3_sound_dirname[256];

tS3_buffer_desc* C2_HOOK_FASTCALL S3GetBufferDescription(int pSample_id);

tS3_error_codes C2_HOOK_FASTCALL S3LoadSample(int pSample_id);

tS3_descriptor* C2_HOOK_FASTCALL S3GetDescriptorByID(int pSample_id);

void C2_HOOK_CDECL s3_debug_disable_sound(void);

void C2_HOOK_CDECL s3_debug_init(void);

void C2_HOOK_CDECL s3_dprintf(const char* pFormat, ...);

void* C2_HOOK_FASTCALL S3BufferWav(const char* pPath, tS3_buffer_desc* pBuffer_desc);

int C2_HOOK_FASTCALL S3PlaySample(tS3_channel* pChannel);

#endif /* S3_SAMPLE_H */
