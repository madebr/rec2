#include "brexcept.h"

#include "core/fw/resource.h"
#include "core/fw/diag.h"

#include "c2_string.h"


// GLOBAL: CARMA2_HW 0x006b0a50
void* exceptionValue;

// GLOBAL: CARMA2_HW 0x006b0a54
br_exception_handler* _BrExceptionHandler;

// FUNCTION: CARMA2_HW 0x0053f510
br_exception_handler* C2_HOOK_CDECL _BrExceptionBegin(void) {
    br_exception_handler* h;

    h = BrResAllocate(NULL, sizeof(br_exception_handler), BR_MEMORY_EXCEPTION_HANDLER);
    h->prev = _BrExceptionHandler;
    _BrExceptionHandler = h;
    return h;
}

// FUNCTION: CARMA2_HW 0x0053f530
void C2_HOOK_CDECL _BrExceptionEnd(void) {
    br_exception_handler* old;

    if (_BrExceptionHandler == NULL) {
        BrFailure("Unhandled exception: %d");
    }
    old = _BrExceptionHandler;
    _BrExceptionHandler = _BrExceptionHandler->prev;
    BrResFree(old);
}

// FUNCTION: CARMA2_HW 0x0053f560
void C2_HOOK_CDECL _BrExceptionThrow(br_int_32 type, void* value) {
    br_exception_handler h;
    br_exception_handler* old;

    if (_BrExceptionHandler == NULL) {
        BrFailure("Unhandled exception: %d");
    }
    memcpy(&h, _BrExceptionHandler, sizeof(br_exception_handler));
    old = _BrExceptionHandler;
    _BrExceptionHandler = _BrExceptionHandler->prev;
    BrResFree(old);
    exceptionValue = value;
    longjmp(h.context, type);
}

// FUNCTION: CARMA2_HW 0x0053f5c0
br_exception C2_HOOK_CDECL _BrExceptionValueFetch(br_exception type, void** evp) {

    if (type != 0 && evp != NULL) {
        *evp = exceptionValue;
    }
    return type;
}

// FUNCTION: CARMA2_HW 0x0053f5e0
void* C2_HOOK_CDECL _BrExceptionResource(void) {

    return _BrExceptionHandler;
}
