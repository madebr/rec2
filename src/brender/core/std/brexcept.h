#ifndef REC2_BREXCEPT_H
#define REC2_BREXCEPT_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_exception_handler* C2_HOOK_CDECL _BrExceptionBegin(void);

void C2_HOOK_CDECL _BrExceptionEnd(void);

void C2_HOOK_CDECL _BrExceptionThrow(br_int_32 type, void* value);

br_exception C2_HOOK_CDECL _BrExceptionValueFetch(br_exception type, void** evp);

void* C2_HOOK_CDECL _BrExceptionResource(void);

#endif // REC2_BREXCEPT_H
