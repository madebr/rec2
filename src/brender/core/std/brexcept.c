#include "brexcept.h"

#include "core/fw/resource.h"
#include "core/fw/diag.h"

#include <string.h>

C2_HOOK_VARIABLE_IMPLEMENT(void*, exceptionValue, 0x006b0a50);
C2_HOOK_VARIABLE_IMPLEMENT(br_exception_handler*, _BrExceptionHandler, 0x006b0a54);

br_exception_handler* C2_HOOK_CDECL _BrExceptionBegin(void) {
    br_exception_handler* h;

    h = BrResAllocate(NULL, sizeof(br_exception_handler), BR_MEMORY_EXCEPTION_HANDLER);
    h->prev = C2V(_BrExceptionHandler);
    C2V(_BrExceptionHandler) = h;
    return h;
}
C2_HOOK_FUNCTION(0x0053f510, _BrExceptionBegin)

void C2_HOOK_CDECL _BrExceptionEnd(void) {
    br_exception_handler* old;

    if (C2V(_BrExceptionHandler) == NULL) {
        BrFailure("Unhandled exception: %d");
    }
    old = C2V(_BrExceptionHandler);
    C2V(_BrExceptionHandler) = C2V(_BrExceptionHandler)->prev;
    BrResFree(old);
}
C2_HOOK_FUNCTION(0x0053f530, _BrExceptionEnd)

void C2_HOOK_CDECL _BrExceptionThrow(br_int_32 type, void* value) {
    br_exception_handler h;
    br_exception_handler* old;

    if (C2V(_BrExceptionHandler) == NULL) {
        BrFailure("Unhandled exception: %d");
    }
    memcpy(&h, C2V(_BrExceptionHandler), sizeof(br_exception_handler));
    old = C2V(_BrExceptionHandler);
    C2V(_BrExceptionHandler) = C2V(_BrExceptionHandler)->prev;
    BrResFree(old);
    C2V(exceptionValue) = value;
    longjmp(h.context, type);
}
C2_HOOK_FUNCTION(0x0053f560, _BrExceptionThrow)

br_exception C2_HOOK_CDECL _BrExceptionValueFetch(br_exception type, void** evp) {

    if (type != 0 && evp != NULL) {
        *evp = C2V(exceptionValue);
    }
    return type;
}
C2_HOOK_FUNCTION(0x0053f5c0, _BrExceptionValueFetch)

void* C2_HOOK_CDECL _BrExceptionResource(void) {

    return C2V(_BrExceptionHandler);
}
C2_HOOK_FUNCTION(0x0053f5e0, _BrExceptionResource)
