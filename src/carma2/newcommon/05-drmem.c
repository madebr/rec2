#include "05-drmem.h"

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

// STUB: CARMA2_HW 0x0044ca40
void C2_HOOK_FASTCALL CreateStainlessClasses(void) {
    NOT_IMPLEMENTED();
}

// CheckMemory

