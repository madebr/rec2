#include "c2_stdlib.h"

#include <stdlib.h>

#if defined(C2_HOOKS_ENABLED)
//#define USE_ORIGINAL_ALLOCATARS
#endif

#if defined(DEBUG_STDLIB)
#define DEBUG_PRINTF(...) C2_HOOK_DEBUGF(##__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

void (C2_HOOK_CDECL * srand_original)(int seed);
void C2_HOOK_CDECL c2_srand(int seed) {
    srand_original(seed);
}
C2_HOOK_FUNCTION_ORIGINAL(0x00576960, c2_srand, srand_original)

int (C2_HOOK_CDECL * rand_original)(void);
int C2_HOOK_CDECL c2_rand(void) {
    return rand_original();
}
C2_HOOK_FUNCTION_ORIGINAL(0x00576970, c2_rand, rand_original)

void C2_HOOK_CDECL c2_abort(void) {
    abort();
}
C2_HOOK_FUNCTION(0x00578aa0, c2_abort)

void* (C2_HOOK_CDECL * c2_malloc_original)(size_t size);
void* C2_HOOK_CDECL c2_malloc(size_t size) {

#if defined(USE_ORIGINAL_ALLOCATARS)
    return c2_malloc_original(size);
#else
    void* result;
    result = malloc(size);
    DEBUG_PRINTF("size=%d => 0x%p", size, result);
    return result;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00576d40, c2_malloc, c2_malloc_original)

void* (C2_HOOK_CDECL * c2_calloc_original)(size_t num, size_t size);
void* C2_HOOK_CDECL c2_calloc(size_t num, size_t size) {

#if defined(USE_ORIGINAL_ALLOCATARS)
    return c2_calloc_original(num, size);
#else
    void* result;
    result = calloc(num, size);
    DEBUG_PRINTF("num=%d, size=%d => 0x%p", num, size, result);
    return result;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00579b20, c2_calloc, c2_calloc_original)

void (C2_HOOK_CDECL *  c2_free_original)(void* ptr);
void C2_HOOK_CDECL c2_free(void* ptr) {

#if defined(USE_ORIGINAL_ALLOCATARS)
    c2_free_original(ptr);
#else
    DEBUG_PRINTF("(0x%p)", ptr);
    free(ptr);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005775f0, c2_free, c2_free_original);

C2_NORETURN void C2_HOOK_CDECL c2_exit(int status) {
    exit(status);
}
C2_HOOK_FUNCTION(0x005784e0, c2_exit)

char* C2_HOOK_CDECL c2_getenv(const char* name) {
    return getenv(name);
}
C2_HOOK_FUNCTION(0x00578ac0, c2_getenv)

void C2_HOOK_CDECL c2_qsort(void *base, size_t nitems, size_t size, int (C2_HOOK_CDECL*compar)(const void *, const void*)) {
    qsort(base, nitems, size, compar);
}

float C2_HOOK_CDECL c2_strtof(const char* str, char** endptr) {
    return strtof(str, endptr);
}
C2_HOOK_FUNCTION(0x00578b50, c2_strtof)

double C2_HOOK_CDECL c2_strtod(const char* str, char** endptr) {

    return strtod(str, endptr);
}

long int C2_HOOK_CDECL c2_strtol(const char* str, char** endptr, int base) {
    return strtol(str, endptr, base);
}
C2_HOOK_FUNCTION(0x00578c10, c2_strtol)

unsigned int C2_HOOK_CDECL c2_strtoul(const char* str, char** endptr, int base) {
    return strtoul(str, endptr, base);
}
C2_HOOK_FUNCTION(0x00578ec0, c2_strtoul)

int C2_HOOK_CDECL c2_atoi(const char *s) {
    return atoi(s);
}
C2_HOOK_FUNCTION(0x00577aa0, c2_atoi)

double C2_HOOK_CDECL c2_atof(const char *s) {

    return atof(s);
}
C2_HOOK_FUNCTION(0x005779a0, c2_atof)
