#include "c2_malloc.h"

#include "c2_hooks.h"
#include "c2_stdlib.h"

// FIXME: teach SRE project generation scripts about C2_HOOK_FUNCTION_PROPERTIES

// Function declarations for SRE project generation
#if 0

void C2_HOOK_CUSTOMCALL alloca_probe(size_t size, void* stackPtr, void* newStackPtr) {
    c2_abort();
}
C2_HOOK_FUNCTION(0x00575bb0, alloca_probe)
C2_HOOK_FUNCTION_PROPERTIES(alloca_probe, fixup=alloca_probe)
C2_HOOK_FUNCTION_PROPERTIES(alloca_probe, arg[0], storage=eax)
C2_HOOK_FUNCTION_PROPERTIES(alloca_probe, arg[1], storage=stack+0)
C2_HOOK_FUNCTION_PROPERTIES(alloca_probe, arg[2], storage=stack+4)

#endif
