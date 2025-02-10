#include "win32_br_platform.h"

#include <stdio.h>

#include <windows.h>

void C2_HOOK_FASTCALL PDMapImageSection(void *base, size_t size, br_uint_32 flags) {
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
}
