#ifndef S3_SAMPLE_H
#define S3_SAMPLE_H

#include <s3/s3.h>

tS3_buffer_desc* C2_HOOK_FASTCALL S3GetBufferDescription(int pSample_id);

tS3_error_codes C2_HOOK_FASTCALL S3LoadSample(int pSample_id);

#endif /* S3_SAMPLE_H */
