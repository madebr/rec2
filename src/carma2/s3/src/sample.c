#include "sample.h"

#include "audio.h"
#include "resource.h"

#include "platform.h"

#include <brender/brender.h>
#include "rec2_types.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT(tS3_descriptor*, gS3_descriptors, 0x007a0594);

tS3_buffer_desc* (C2_HOOK_FASTCALL * S3GetBufferDescription_original)(int pSample_id);
tS3_buffer_desc* C2_HOOK_FASTCALL S3GetBufferDescription(int pSample_id) {

#if 0//defined(C2_HOOKS_ENABLED)
    return S3GetBufferDescription_original(pSample_id);
#else
    tS3_descriptor* descriptor;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_descriptor, buffer_description, 0x40);

    if (!C2V(gS3_enabled)) {
        return NULL;
    }
    descriptor = S3GetDescriptorByID(pSample_id);
    if (descriptor == NULL) {
        return NULL;
    }
    return descriptor->buffer_description;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005687ff, S3GetBufferDescription, S3GetBufferDescription_original)

tS3_error_codes (C2_HOOK_FASTCALL * S3LoadSample_original)(int pSample_id);
tS3_error_codes C2_HOOK_FASTCALL S3LoadSample(int pSample_id) {

#if 0//defined(C2_HOOKS_ENABLED)
    return S3LoadSample_original(pSample_id);
#else
    tS3_descriptor* descriptor;
    tS3_buffer_desc* buffer_description;
    char path[512];

    if (!C2V(gS3_enabled)) {
        return eS3_error_none;
    }
    descriptor = S3GetDescriptorByID(pSample_id);
    if (descriptor == NULL) {
        return eS3_error_bad_id;
    }
    if (descriptor->type != 0) {
        return eS3_error_none;
    }
    if (descriptor->buffer_description != NULL) {
        return eS3_error_none;
    }
    path[0] = '\0';
    c2_strcpy(path, descriptor->path);

    C2_HOOK_BUG_ON(sizeof(tS3_buffer_desc) != 0x18);

    buffer_description = S3MemAllocate(sizeof(tS3_buffer_desc), kMem_S3_sound_header);
    if (buffer_description == NULL) {
        return eS3_error_memory;
    }
    c2_memset(buffer_description, 0, sizeof(tS3_buffer_desc));
    descriptor->pd_handle = PDS3BufferWav(path, buffer_description);
    if (descriptor->pd_handle == NULL) {
        BrFailure("Cound not load sample:%s", path);
    }
    descriptor->effects_enabled = 0;
    descriptor->buffer_description = buffer_description;
    return eS3_error_none;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00568830, S3LoadSample, S3LoadSample_original)

tS3_descriptor* C2_HOOK_FASTCALL S3GetDescriptorByID(int pSample_id) {
    tS3_descriptor *desc;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_descriptor, sample_id, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_descriptor, next, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_descriptor, low_memory_alternative, 0x3c);

    for (desc = C2V(gS3_descriptors); desc != NULL; desc = desc->next) {
        if (desc->sample_id == pSample_id) {
            if (desc->low_memory_alternative >= 0) {
                return S3GetDescriptorByID(desc->low_memory_alternative);
            }
            return desc;
        }
    }
    return NULL;
}
C2_HOOK_FUNCTION(0x00565a3d, S3GetDescriptorByID)

void C2_HOOK_CDECL s3_dprintf(const char* pFormat, ...) {

}
C2_HOOK_FUNCTION(0x00565b46, s3_dprintf);

int (C2_HOOK_FASTCALL * S3CheckWavHeader_original)(tS3_wav_file* pWav_buffer, tS3_wav_chunk_info_header** pWav_format_info_header, void** pSamples, int* pSample_size);
int C2_HOOK_FASTCALL S3CheckWavHeader(tS3_wav_file* pWav_buffer, tS3_wav_chunk_info_header** pWav_format_info_header, void** pSamples, int* pSample_size) {

#if 0//defined(C2_HOOKS_ENABLED)
    return S3CheckWavHeader_original(pWav_buffer, pWav_format_info_header, pSamples, pSample_size);
#else
    void* chunk_start;
    void* file_end;

    if (pWav_format_info_header != NULL) {
        *pWav_format_info_header = NULL;
    }
    if (pSamples != NULL) {
        *pSamples = NULL;
    }
    if (pSample_size != NULL) {
        *pSample_size = 0;
    }
    if (c2_strncmp(pWav_buffer->riff_header.magic, "RIFF", 4) != 0 || c2_strncmp(pWav_buffer->wave_magic, "WAVE", 4) != 0) {
        return 0;
    }
    file_end = (br_uint_8*)pWav_buffer->wave_magic + pWav_buffer->riff_header.chunk_size;
    chunk_start = &pWav_buffer->fmt_chunk;
    while (chunk_start < file_end) {
        tS3_riff_chunk_header* chunk_header = chunk_start;
        if (c2_strncmp(chunk_header->magic, "fmt ", 4) == 0) {
            tS3_riff_fmt_chunk* fmt_chunk = chunk_start;
            if (pWav_format_info_header != NULL && *pWav_format_info_header == NULL) {
                C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tS3_wav_chunk_info_header, bits_per_sample, 0xe);
                if (chunk_header->chunk_size < offsetof(tS3_wav_chunk_info_header, bits_per_sample)) {
                    return 0;
                }
                *pWav_format_info_header = &fmt_chunk->wav_info_header;
                if ((pSamples == NULL || *pSamples != NULL) && (pSample_size == NULL || *pSample_size != 0)) {
                    return 1;
                }
            }
        } else if (c2_strncmp(chunk_header->magic, "data", 4) == 0) {
            tS3_riff_data_chunk* data_chunk = chunk_start;
            if ((pSamples != NULL && *pSamples == NULL) || (pSample_size != NULL && *pSample_size == 0)) {
                if (pSamples != NULL) {
                    *pSamples = data_chunk->data;
                }
                if (pSample_size != NULL) {
                    *pSample_size = data_chunk->header.chunk_size;
                }
                if (pWav_format_info_header == NULL || *pWav_format_info_header != NULL) {
                    return 1;
                }
            }
        }
        chunk_start = (br_uint_8*)&chunk_header[1] + chunk_header->chunk_size;
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00568ee0, S3CheckWavHeader, S3CheckWavHeader_original)
