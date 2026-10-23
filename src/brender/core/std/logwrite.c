#include "logwrite.h"

#include <stdio.h>

// FUNCTION: CARMA2_HW 0x0053fa90
int C2_HOOK_CDECL BrLogWrite(void* buffer, br_size_t s, br_size_t n) {

    return fwrite(buffer, s, n, stderr);
}
