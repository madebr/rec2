#include "sdl3_br_platform.h"

#include <stdio.h>

#include <SDL3/SDL.h>

#ifdef SDL_PLATFORM_WINDOWS
#include <windows.h>
#else
#include <sys/mman.h>
#endif

void PDMapImageSection(void *base, size_t size, br_uint_32 flags) {
#ifdef SDL_PLATFORM_WINDOWS
    DWORD prev_protect;
    DWORD new_protect;
    switch (flags) {
    case kMemory_section_executable:
        new_protect = PAGE_EXECUTE;
        break;
    case kMemory_section_executable | kMemory_section_read:
        new_protect = PAGE_EXECUTE_READ;
        break;
    case kMemory_section_executable | kMemory_section_read | kMemory_section_write:
        new_protect = PAGE_EXECUTE_READWRITE;
        break;
    case kMemory_section_read:
        new_protect = PAGE_READONLY;
        break;
    case kMemory_section_read | kMemory_section_write:
        new_protect = PAGE_READWRITE;
        break;
    default:
        abort();
        break;
    }
    if (VirtualProtect(base, size, new_protect, &prev_protect) == 0) {
        abort();
    }
#else
    int new_protect = 0;
    if (flags & kMemory_section_read) {
        new_protect |= PROT_READ;
    }
    if (flags & kMemory_section_write) {
        new_protect |= PROT_WRITE;
    }
    if (flags & kMemory_section_executable) {
        new_protect |= PROT_EXEC;
    }
    if (mprotect(base, size, new_protect) != 0) {
        abort();
    }
#endif
}
