#ifndef REC2_BRQSORT_H
#define REC2_BRQSORT_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void C2_HOOK_CDECL BrQsort(void* basep, unsigned int nelems, unsigned int size, br_qsort_cbfn comp);

#endif // REC2_BRQSORT_H
