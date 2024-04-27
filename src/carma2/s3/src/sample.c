#include "sample.h"

C2_HOOK_VARIABLE_IMPLEMENT(tS3_descriptor*, gS3_descriptors, 0x007a0594);

tS3_buffer_desc* (C2_HOOK_FASTCALL * S3GetBufferDescription_original)(int pSample_id);
tS3_buffer_desc* C2_HOOK_FASTCALL S3GetBufferDescription(int pSample_id) {

#if defined(C2_HOOKS_ENABLED)
    return S3GetBufferDescription_original(pSample_id);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005687ff, S3GetBufferDescription, S3GetBufferDescription_original)

tS3_error_codes (C2_HOOK_FASTCALL * S3LoadSample_original)(int pSample_id);
tS3_error_codes C2_HOOK_FASTCALL S3LoadSample(int pSample_id) {

#if defined(C2_HOOKS_ENABLED)
    return S3LoadSample_original(pSample_id);
#else
#error "Not implemented"
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
