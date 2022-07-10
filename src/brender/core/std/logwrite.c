#include "logwrite.h"

#include "c2_stdio.h"

int C2_HOOK_CDECL BrLogWrite(void* buffer, br_size_t s, br_size_t n) {

    return c2_fwrite(buffer, s, n, c2_stderr);
}
