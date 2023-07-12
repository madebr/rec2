#ifndef HOOKS_H
#define HOOKS_H

#if defined(__cplusplus)
extern "C" {
#endif

#define HOOK_JOIN(V1, V2) V1##V2
#define HOOK_VALUE(V) V
#define HOOK_STRINGIFY(A) #A

#if defined(_MSC_VER)
#pragma section(".CRT$XCZ", read)

#define HOOK_STARTUP_INNER(NAME, ID)                                                                       \
    static void __cdecl HOOK_JOIN(NAME, ID)(void);                                                                  \
    __declspec(dllexport) __declspec(allocate(".CRT$XCZ")) void (__cdecl *   HOOK_JOIN(NAME, ID)##_)(void); \
    void (__cdecl * HOOK_JOIN(NAME, ID)##_)(void) = HOOK_JOIN(NAME, ID); \
    static void __cdecl HOOK_JOIN(NAME, ID)(void)

#define HOOK_SHUTDOWN_INNER(NAME, ID)                  \
    static void HOOK_JOIN(NAME, ID)(void)
#else
#define HOOK_STARTUP_INNER(NAME, ID)                            \
    static void __attribute__ ((constructor)) HOOK_JOIN(NAME, ID)(void)

#define HOOK_SHUTDOWN_INNER(NAME, ID)                          \
    static void __attribute__ ((destructor)) HOOK_JOIN(NAME, ID)(void)
#endif

#define HOOK_FUNCTION_STARTUP(NAME) HOOK_STARTUP_INNER(hook_startup_,  NAME)
#define HOOK_FUNCTION_SHUTDOWN(NAME) HOOK_SHUTDOWN_INNER(hook_shutdown_, NAME)

void hook_register(void (*function)(void));
void hook_function_register(void **victim, void *detour);
void hook_function_deregister(void **victim, void *detour);

void hook_run_functions(void);
void hook_apply_all(void);
void hook_unapply_all(void);
void hook_check(void);

void hook_print_stats(void);

#define HOOK_STARTUP(FUNCTION)        \
    HOOK_FUNCTION_STARTUP(FUNCTION) { \
        hook_register(FUNCTION);      \
    }

#define HOOK_FUNCTION_ORIGINAL(ADDRESS, FUNCTION, ORIGINAL)           \
    HOOK_FUNCTION_STARTUP(FUNCTION) {                                 \
        *(void**)(&ORIGINAL) = (void*)ADDRESS;                        \
        hook_function_register((void**)&ORIGINAL, (void*)FUNCTION);   \
    }                                                                 \
    HOOK_FUNCTION_SHUTDOWN(FUNCTION) {                                \
        hook_function_deregister((void**)&ORIGINAL, (void*)FUNCTION); \
    }

#define HOOK_FUNCTION(ADDRESS, FUNCTION)                                      \
    static void* HOOK_JOIN(hook_func_, FUNCTION);                             \
    HOOK_FUNCTION_ORIGINAL(ADDRESS, FUNCTION, HOOK_JOIN(hook_func_, FUNCTION))

#define HOOK_VARIABLE_DECLARE(TYPE, NAME)   \
    extern TYPE* const HOOK_JOIN(hookvar_, NAME)

#define HOOK_VARIABLE_IMPLEMENT(TYPE, NAME, ADDRESS)    \
    TYPE* const HOOK_JOIN(hookvar_, NAME) = (TYPE*)(ADDRESS)

#define HOOK_VARIABLE_DECLARE_ARRAY(TYPE, NAME, COUNT)   \
    extern TYPE (*const HOOK_JOIN(hookvar_, NAME))[COUNT]

#define HOOK_VARIABLE_IMPLEMENT_ARRAY(TYPE, NAME, COUNT, ADDRESS)    \
    TYPE (* const HOOK_JOIN(hookvar_, NAME))[COUNT] = (TYPE (*)[COUNT])(ADDRESS)

#define HOOK_VARIABLE(NAME)        \
    (*(HOOK_JOIN(hookvar_, NAME)))


#if defined(__cplusplus)
}
#endif

#endif // HOOKS_H
