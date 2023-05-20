#ifndef C2_HOOKS_H
#define C2_HOOKS_H

// Wrapper for functions that are too small, useful for tagging its address
#define C2_NO_HOOK_FUNCTION(ADDRESS, NAME)

#if defined(_MSC_VER)
#define C2_NORETURN __declspec(noreturn)
#define C2_NORETURN_FUNCPTR
#define C2_NAKED __declspec(naked)
#else
#define C2_NORETURN __attribute__ ((__noreturn__))
#define C2_NORETURN_FUNCPTR C2_NORETURN
#define C2_NAKED __attribute__((__naked__))
#endif

#if defined(C2_HOOKS_ENABLED)

#define C2_HOOK_ASSERT(condition) do { if (!(condition)) { extern void C2_HOOK_CDECL c2_abort(void); c2_abort(); } } while (0)
#define C2_HOOK_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

#if defined(_WIN32) && (defined(_M_IX86) || defined(_X86_))
#define C2_HOOK_CDECL __cdecl
#define C2_HOOK_FASTCALL __fastcall
#define C2_HOOK_STDCALL __stdcall
#else
#define C2_HOOK_CDECL
#define C2_HOOK_FASTCALL
#define C2_HOOK_STDCALL
#endif

#include <stdarg.h>

void c2_hook_debugf(const char* function, const char* format, ...);
void c2_hook_vdebugf(const char* function, const char* format, va_list ap);

#include <hooks.h>

#define C2_HOOK_FUNCTION(ADDRESS, FUNCTION) HOOK_FUNCTION(ADDRESS, FUNCTION)
#define C2_HOOK_FUNCTION_ORIGINAL(ADDRESS, FUNCTION, ORIGINAL) HOOK_FUNCTION_ORIGINAL(ADDRESS, FUNCTION, ORIGINAL)

#define C2_HOOK_VARIABLE_DECLARE(TYPE, NAME) HOOK_VARIABLE_DECLARE(TYPE, NAME)
#define C2_HOOK_VARIABLE_DECLARE_ARRAY(TYPE, NAME, COUNT) HOOK_VARIABLE_DECLARE_ARRAY(TYPE, NAME, COUNT)
#define C2_HOOK_VARIABLE_IMPLEMENT(TYPE, NAME, ADDRESS) HOOK_VARIABLE_IMPLEMENT(TYPE, NAME, ADDRESS)
#define C2_HOOK_VARIABLE_IMPLEMENT_INIT(TYPE, NAME, ADDRESS, ...) HOOK_VARIABLE_IMPLEMENT(TYPE, NAME, ADDRESS)
#define C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(TYPE, NAME, COUNT, ADDRESS) HOOK_VARIABLE_IMPLEMENT_ARRAY(TYPE, NAME, COUNT, ADDRESS)
#define C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(TYPE, NAME, COUNT, ADDRESS, ...) HOOK_VARIABLE_IMPLEMENT_ARRAY(TYPE, NAME, COUNT, ADDRESS)
#define C2_HOOK_VARIABLE(NAME) HOOK_VARIABLE(NAME)

#define C2_HOOK_DEBUGF(FMT, ...) c2_hook_debugf(__FUNCTION__, FMT, ##__VA_ARGS__)
#define C2_HOOK_VDEBUGF(FMT, AP) c2_hook_vdebugf(__FUNCTION__, FMT, AP)

#else

#define C2_HOOK_BUG_ON(condition)

#define C2_HOOK_CDECL
#define C2_HOOK_FASTCALL
#define C2_HOOK_STDCALL

#define C2_HOOK_FUNCTION(ADDRESS, FUNCTION)
#define C2_HOOK_FUNCTION_ORIGINAL(ADDRESS, FUNCTION, ORIGINAL)

#define C2_HOOK_VARIABLE_DECLARE(TYPE, NAME) extern TYPE NAME
#define C2_HOOK_VARIABLE_IMPLEMENT(TYPE, NAME, ADDRESS) TYPE NAME
#define C2_HOOK_VARIABLE_IMPLEMENT_INIT(TYPE, NAME, ADDRESS, ...) TYPE NAME = __VA_ARGS__
#define C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(TYPE, NAME, COUNT, ADDRESS) TYPE NAME[COUNT]
#define C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(TYPE, NAME, COUNT, ADDRESS, ...) TYPE NAME[COUNT] = __VA_ARGS__
#define C2_HOOK_VARIABLE(NAME) (NAME)

#define C2_HOOK_DEBUGF(FMT, ...)
#define C2_HOOK_VDEBUGF(FMT, AP)

#endif
#include "hooks.h"

#define C2V(NAME) C2_HOOK_VARIABLE(NAME)

#define C2_HOOK_START() C2_HOOK_DEBUGF("start")
#define C2_HOOK_STARTF(FMT, ...) C2_HOOK_DEBUGF("start" FMT, ##__VA_ARGS__)
#define C2_HOOK_FINISH() C2_HOOK_DEBUGF("finish")

void c2_hook_unused_function(const char* function, const char* format, ...);

#define C2_HOOK_ASSUME_UNUSED()                               \
    C2_HOOK_DEBUGF(__FUNCTION__, "function used while assumed it is not.")

#endif // C2_HOOKS_H
