#include "logwrite.h"

#include <stdio.h>

int C2_HOOK_CDECL BrLogWrite(void* buffer, br_size_t s, br_size_t n) {

    return fwrite(buffer, s, n, stderr);
}
