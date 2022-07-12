#include "brqsort.h"

#include "c2_stdlib.h"

void C2_HOOK_CDECL BrQsort(void* basep, unsigned int nelems, unsigned int size, br_qsort_cbfn comp) {
    c2_qsort(basep, nelems, size, comp);
}
C2_HOOK_FUNCTION(0x00528410, BrQsort)
