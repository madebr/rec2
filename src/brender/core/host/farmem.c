#include "farmem.h"

#include "c2_stdlib.h"

// Win32 uses protected mode. No need to support segmented memory.


void C2_HOOK_CDECL HostFarBlockWrite(br_uint_32 offset, br_uint_16 sel, void *block, br_uint_32 count) {
    c2_abort();
}
C2_HOOK_FUNCTION(0x005403a0, HostFarBlockWrite)

void C2_HOOK_CDECL HostFarBlockRead(br_uint_32 offset, br_uint_16 sel, void *block, br_uint_32 count) {
    c2_abort();
}
C2_HOOK_FUNCTION(0x005403dc, HostFarBlockRead)

br_uint_32 C2_HOOK_CDECL HostFarStringWrite(br_uint_32 offset, br_uint_16 sel, br_uint_8 *string, br_uint_32 max) {
    c2_abort();
}
C2_HOOK_FUNCTION(0x005403f9, HostFarStringWrite)

br_uint_32 C2_HOOK_CDECL HostFarStringRead(br_uint_32 offset, br_uint_16 sel, br_uint_8 *string, br_uint_32 max) {
    c2_abort();
}
C2_HOOK_FUNCTION(0x00540427, HostFarStringRead)

void C2_HOOK_CDECL HostFarBlockFill(br_uint_32 offset, br_uint_16 sel, br_uint_8 value, br_uint_32 count) {
    c2_abort();
}
C2_HOOK_FUNCTION(0x005403bd, HostFarBlockFill)

void C2_HOOK_CDECL HostFarByteWrite(br_uint_32 offset, br_uint_16 sel, br_uint_8 value) {
    c2_abort();
}
C2_HOOK_FUNCTION(0x00540454, HostFarByteWrite)

void C2_HOOK_CDECL HostFarWordWrite(br_uint_32 offset, br_uint_16 sel, br_uint_16 value) {
    c2_abort();
}
C2_HOOK_FUNCTION(0x0054046f, HostFarWordWrite)

void C2_HOOK_CDECL HostFarDWordWrite(br_uint_32 offset, br_uint_16 sel, br_uint_32 value) {
    c2_abort();
}
C2_HOOK_FUNCTION(0x0054048d, HostFarDWordWrite)

br_uint_8 C2_HOOK_CDECL HostFarByteRead(br_uint_32 offset, br_uint_16 sel) {
    c2_abort();
}
C2_HOOK_FUNCTION(0x00540463, HostFarByteRead)

br_uint_16 C2_HOOK_CDECL HostFarWordRead(br_uint_32 offset, br_uint_16 sel) {
    c2_abort();
}
C2_HOOK_FUNCTION(0x00540480, HostFarWordRead)

br_uint_32 C2_HOOK_CDECL HostFarDWordRead(br_uint_32 offset, br_uint_16 sel) {
    c2_abort();
}
C2_HOOK_FUNCTION(0x0054049c, HostFarDWordRead)
