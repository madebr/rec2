#include "mapping.h"

#include "setup.h"

#define APPLY_UV(du, dv, su, sv) do {                       \
        du = self->state.cache.map_transform.m[0][0] * su   \
            + self->state.cache.map_transform.m[1][0] * sv  \
            + self->state.cache.map_transform.m[2][0];      \
        dv = self->state.cache.map_transform.m[0][1] * su,  \
            + self->state.cache.map_transform.m[1][1] * sv  \
            + self->state.cache.map_transform.m[2][1];      \
    } while (0)

void C2_HOOK_CDECL SurfaceMapEnvironmentLocal(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* normal, br_colour colour, br_scalar* comp) {
    br_vector3 eye;
    br_vector3 r, wr;
    br_scalar d, cu, cv;

    BrVector3Sub(&eye, &C2V(scache).eye_m, p);
    BrVector3Normalise(&eye, &eye);

    d = 2.f * BrVector3Dot(&eye, normal);
    BrVector3Scale(&r, normal, d);
    BrVector3Sub(&r, &r, &eye);

    if (self->state.matrix.view_to_environment_hint != BRT_DONT_CARE) {
        BrMatrix34ApplyV(&wr, &r, &C2V(scache).model_to_environment);
        BrVector3Normalise(&wr, &wr);
    } else {
        wr = r;
    }

    cu = BrFixedToFloat(BR_ATAN2(wr.v[0], -wr.v[2]));

    cv = -wr.v[1] / 2 + 0.5f;

    APPLY_UV(comp[C_U], comp[C_V], cu, cv);
}
C2_HOOK_FUNCTION(0x0054b320, SurfaceMapEnvironmentLocal)
