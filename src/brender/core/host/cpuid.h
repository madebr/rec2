#ifndef C2_CPUID_H
#define C2_CPUID_H

#include "brender/br_types.h"

void C2_HOOK_CDECL CPUInfo(br_token* processor_type, br_uint_32* features);

#endif //C2_CPUID_H
