#ifndef S3_SAMPLE_H
#define S3_SAMPLE_H

#include <s3/s3.h>

C2_HOOK_VARIABLE_DECLARE(tS3_descriptor*, gS3_descriptors);
C2_HOOK_VARIABLE_DECLARE(int, gS3_low_memory_mode);
C2_HOOK_VARIABLE_DECLARE(tS3_error_codes, gS3_last_error);
C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gS3_sound_dirname, 256);

tS3_buffer_desc* C2_HOOK_FASTCALL S3GetBufferDescription(int pSample_id);

tS3_error_codes C2_HOOK_FASTCALL S3LoadSample(int pSample_id);

tS3_descriptor* C2_HOOK_FASTCALL S3GetDescriptorByID(int pSample_id);

void C2_HOOK_CDECL s3_dprintf(const char* pFormat, ...);

void* C2_HOOK_FASTCALL S3BufferWav(const char* pPath, tS3_buffer_desc* pBuffer_desc);

#endif /* S3_SAMPLE_H */
