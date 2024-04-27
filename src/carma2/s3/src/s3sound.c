#include "s3sound.h"

// this function was only called in Carmageddon I DOS build
int C2_HOOK_FASTCALL S3CreateTypeStructs(tS3_channel* chan) {

    return 1;
}
C2_HOOK_FUNCTION(0x0056990f, S3CreateTypeStructs)
