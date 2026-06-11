#ifndef C2_HOOKS_H
#define C2_HOOKS_H

#if defined(_MSC_VER)
#ifdef REC2_MATCHING
#define C2_NORETURN
#else
#define C2_NORETURN __declspec(noreturn)
#endif
#define C2_NORETURN_FUNCPTR
#define C2_NAKED __declspec(naked)
#else
#define C2_NORETURN __attribute__ ((__noreturn__))
#define C2_NORETURN_FUNCPTR C2_NORETURN
#define C2_NAKED __attribute__((__naked__))
#endif

#if defined(_MSC_VER) && _MSC_VER < 1300
#define C2_FUNCTION "<unknown>"
#else
#define C2_FUNCTION __FUNCTION__
#endif
extern C2_NORETURN void rec2_error(const char *reason, const char *function, const char *file, int line);
#define NOT_IMPLEMENTED() rec2_error("Not implemented", C2_FUNCTION, __FILE__, __LINE__)
#define UNUSED() rec2_error("Unused", C2_FUNCTION, __FILE__, __LINE__)

#ifdef REC2_MATCHING

#define C2_HOOK_CDECL __cdecl
#define C2_HOOK_FASTCALL __fastcall
#define C2_HOOK_STDCALL __stdcall
#define C2_HOOK_THISCALL __thiscall
#define C2_HOOK_FAKE_THISCALL __fastcall

// FIXME: rewrite to C2_HOOK_STATIC_ASSERT
#define C2_HOOK_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))
#define C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(T, M, O) C2_HOOK_BUG_ON(((uintptr_t)&(((T*)0)->M)) != O)
#define C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(T, M, S) C2_HOOK_BUG_ON(sizeof(((T*)0)->M) != (S))

#define C2_HOOK_ASSERT(condition)
#else

#define C2_HOOK_CDECL
#define C2_HOOK_FASTCALL
#define C2_HOOK_STDCALL
#define C2_HOOK_THISCALL
#define C2_HOOK_FAKE_THISCALL

#define C2_HOOK_ASSERT(condition)
#define C2_HOOK_BUG_ON(condition)
#define C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(T, M, O)
#define C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(T, M, O)
#endif

typedef unsigned char undefined;
typedef unsigned short undefined2;
typedef unsigned int undefined4;

#endif // C2_HOOKS_H
