#include "clip.h"

int C2_HOOK_STDCALL ClipFaceToPlus1(brp_vertex* vp, brp_vertex* verts_out, int num_in, int axis, int cmask) {
    brp_vertex* wp = verts_out;
    int num_out = 0;
    brp_vertex* up;
    br_scalar t, tu, tv;
    int m;
    br_scalar* usp;
    br_scalar* vsp;
    br_scalar* wsp;

    up = &vp[num_in - 1];
    tu = up->comp[axis] - up->comp[C_W];

    for ( ; num_in != 0 ; num_in--, up = vp, tu = tv, vp++) {

        tv = vp->comp[axis] - vp->comp[C_W];

        if (tv <= 0.f) {
            if (tu <= 0.f) {
                *wp++ = *vp;
                num_out++;
                continue;
            }

            if (tv < 0.f) {
                t = tv /(tv - tu);
                usp = up->comp;
                vsp = vp->comp;
                wsp = wp->comp;
                for (m = cmask ; m != 0; m >>= 1, usp++, vsp++, wsp++) {
                    if (m & 1) {
                        *wsp = *vsp + t * (*usp - *vsp);
                    }
                }

                wp->comp[axis] = wp->comp[C_W];
                wp->flags = TV_CLIPPED;
                wp++;
                num_out++;
            }
            *wp++ = *vp;
            num_out++;

        } else {
            if (tu >= 0.f) {
                continue;
            }

            t = tu / (tu - tv);

            usp = up->comp;
            vsp = vp->comp;
            wsp = wp->comp;

            for (m = cmask ; m != 0 ; m >>= 1, usp++, vsp++, wsp++) {
                if (m & 1) {
                    *wsp = *usp + t * (*vsp - *usp);
                }
            }

            wp->comp[axis] = wp->comp[C_W];
            wp->flags = TV_CLIPPED;
            wp++;
            num_out++;
        }
    }

    return num_out;
}
C2_HOOK_FUNCTION(0x0054bb70, ClipFaceToPlus1)

int C2_HOOK_STDCALL ClipFaceToMinus1(brp_vertex* vp, brp_vertex* verts_out, int num_in, int axis, int cmask) {
    brp_vertex* wp = verts_out;
    int num_out = 0;
    brp_vertex* up;
    br_scalar t, tu, tv;
    int m;
    br_scalar *usp;
    br_scalar* vsp;
    br_scalar* wsp;

    up = &vp[num_in - 1];
    tu = -up->comp[axis] - up->comp[C_W];

    for ( ; num_in != 0 ; num_in--, up = vp, tu = tv, vp++) {

        tv = - vp->comp[axis] - vp->comp[C_W];

        if (tv <= 0.f) {
            if (tu <= 0.f) {
                *wp++ = *vp;
                num_out++;
                continue;
            }
            if (tv < 0.f) {
                t = tv /(tv-tu);
                usp = up->comp;
                vsp = vp->comp;
                wsp = wp->comp;

                for (m = cmask ; m != 0; m >>= 1, usp++, vsp++, wsp++) {
                    if (m & 1) {
                        *wsp = *vsp + t * (*usp - *vsp);
                    }
                }

                wp->comp[axis] = -wp->comp[C_W] + 2 * BR_SCALAR_EPSILON;
                wp->flags = TV_CLIPPED;
                wp++;
                num_out++;
            }
            *wp++ = *vp;
            num_out++;
        } else {
            if (tu >= 0.f) {
                continue;
            }
            t = tu /(tu-tv);
            usp = up->comp;
            vsp = vp->comp;
            wsp = wp->comp;
            for (m = cmask ; m != 0; m >>= 1, usp++, vsp++, wsp++) {
                if (m & 1) {
                    *wsp = *usp + t * (*vsp - *usp);
                }
            }

            wp->comp[axis] = -wp->comp[C_W] + 2 * BR_SCALAR_EPSILON;
            wp->flags = TV_CLIPPED;
            wp++;
            num_out++;
        }
    }

    return num_out;
}
C2_HOOK_FUNCTION(0x0054bd00, ClipFaceToMinus1)
