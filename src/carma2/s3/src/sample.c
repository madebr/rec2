#include "sample.h"

tS3_error_codes (C2_HOOK_FASTCALL * S3LoadSample_original)(int pSample_id);
tS3_error_codes C2_HOOK_FASTCALL S3LoadSample(int pSample_id) {

#if defined(C2_HOOKS_ENABLED)
    return S3LoadSample_original(pSample_id);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00568830, S3LoadSample, S3LoadSample_original)
