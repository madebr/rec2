#include "texture.h"

#include "backend.h"

// FUNCTION: D3D 0x10003b90
void C2_HOOK_CDECL TextureCacheD3DClearEntry(int index, int uid) {
    if (index == -1) {
        return;
    }
    if (gBuffered_textures[index].field_0x20 == uid) {
        gBuffered_textures[index].field_0x20 = -1;
        gBuffered_textures[index].field_0x24 = -1;
    }
}