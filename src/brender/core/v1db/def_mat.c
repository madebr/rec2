#include "def_mat.h"

#include "core/fw/resource.h"
#include "core/math/matrix23.h"
#include "core/v1db/dbsetup.h"

br_material* C2_HOOK_CDECL SetupDefaultMaterial(void) {
    static br_material* default_mat;

    default_mat = BrResAllocate(C2V(v1db).res, sizeof(br_material), BR_MEMORY_MATERIAL);
    if (default_mat == NULL) {
        return NULL;
    }
    default_mat->identifier = "default_mat";
    default_mat->colour = BR_COLOUR_RGB(255, 255, 255);
    default_mat->ka = 0.1f;
    default_mat->kd = 0.7f;
    default_mat->ks = 0.0f;
    default_mat->power = 20;
    default_mat->flags = BR_MATF_LIGHT;
    BrMatrix23Identity(&default_mat->map_transform);
    default_mat->index_base = 10;
    default_mat->index_range = 31;
    return default_mat;
}
C2_HOOK_FUNCTION(0x00526940, SetupDefaultMaterial)
