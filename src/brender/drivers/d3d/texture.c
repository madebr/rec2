#include "texture.h"

#include "backend.h"

void TextureCacheD3DClearEntry(int index, int uid) {
    if (index == -1) {
        return;
    }
    if (C2V(gBuffered_textures)[index].field_0x20 == uid) {
        C2V(gBuffered_textures)[index].field_0x20 = -1;
        C2V(gBuffered_textures)[index].field_0x24 = -1;
    }
}
C2_HOOK_FUNCTION(0x10003b90, TextureCacheD3DClearEntry)
