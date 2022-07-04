#ifndef HOOKS_H
#define HOOKS_H

#if defined(__cplusplus)
extern "C" {
#endif

#define HOOK_JOIN(V1, V2) V1##V2

#define HOOK_UNIQUE_NAME_INNER2(NAME, ID) NAME##ID
#define HOOK_UNIQUE_NAME_INNER(NAME, ID) HOOK_UNIQUE_NAME_INNER2(NAME, ID)
#define HOOK_UNIQUE_NAME(NAME) HOOK_UNIQUE_NAME_INNER(HOOK_JOIN(hook_,NAME), __COUNTER__)

#define HOOK_FUNCTION_STARTUP_INNER(NAME, ID)                            \
    void __attribute__ ((constructor)) HOOK_JOIN(NAME, ID)(void)

#define HOOK_FUNCTION_SHUTDOWN_INNER(NAME, ID)                          \
    void __attribute__ ((destructor)) HOOK_JOIN(NAME, ID)(void)

#define HOOK_FUNCTION_STARTUP HOOK_FUNCTION_STARTUP_INNER(hook_startup_, __COUNTER__)
#define HOOK_FUNCTION_SHUTDOWN HOOK_FUNCTION_SHUTDOWN_INNER(hook_shutdown_, __COUNTER__)

void hook_function_register(void **victim, void *detour);
void hook_function_deregister(void **victim, void *detour);

void hook_apply_all(void);
void hook_unapply_all(void);

void hook_print_stats(void);

#define HOOK_FUNCTION_ORIGINAL(ADDRESS, FUNCTION, ORIGINAL)    \
    HOOK_FUNCTION_STARTUP {                                    \
        *(void**)(&ORIGINAL) = (void*)ADDRESS;                 \
        hook_function_register((void**)&ORIGINAL, FUNCTION);   \
    }                                                          \
    HOOK_FUNCTION_SHUTDOWN {                                   \
        hook_function_deregister((void**)&ORIGINAL, FUNCTION); \
    }

#define HOOK_FUNCTION(ADDRESS, FUNCTION)                                      \
    static void* HOOK_JOIN(hook_func_, FUNCTION);                             \
    HOOK_FUNCTION_ORIGINAL(ADDRESS, FUNCTION, HOOK_JOIN(hook_func_, FUNCTION))

#define HOOK_VARIABLE_DECLARE(TYPE, NAME)   \
    extern TYPE* HOOK_JOIN(hookvar_, NAME);

#define HOOK_VARIABLE_IMPLEMENT(TYPE, NAME, ADDRESS)    \
    TYPE* HOOK_JOIN(hookvar_, NAME) = (TYPE*)(ADDRESS);

#define HOOK_VARIABLE(NAME)        \
    (*(HOOK_JOIN(hookvar_, NAME)))


#if defined(__cplusplus)
}
#endif

#endif // HOOKS_H
