#include "05-drmem.h"

#include "brender/brender.h"
#include "52-errors.h"

#include "rec2_macros.h"
#include "rec2_types.h"

// GLOBAL: CARMA2_HW 0x006815c8
br_resource_class gStainless_classes[126];

// SetNonFatalAllocationErrors

// ResetNonFatalAllocationErrors

// FUNCTION: CARMA2_HW 0x0044c820
void C2_HOOK_FASTCALL MAMSInitMem(void) {

}

// FUNCTION: CARMA2_HW 0x0044c850
void C2_HOOK_FASTCALL PrintMemoryDump(int pFlags, char* pTitle) {

}

// DRStdlibAllocate

// DRStdlibFree

// DRStdlibInquire

// Claim4ByteAlignment

// STUB: CARMA2_HW 0x0044c9e0
void C2_HOOK_FASTCALL InstallDRMemCalls(void) {
    NOT_IMPLEMENTED();
}

// MAMSUnlock

// MAMSLock

// FUNCTION: CARMA2_HW 0x0044ca40
void C2_HOOK_FASTCALL CreateStainlessClasses(void) {
    int i;

    for (i = 129; i < 129 + (int)REC2_ASIZE(gStainless_classes); i++) {
        gStainless_classes[i - 129].res_class = i;
        if (!BrResClassAdd(&gStainless_classes[i - 129])) {
            FatalError(kFatalError_OOM_S);
        }
    }
}

// CheckMemory

