#include "clip.h"

#include "rend.h"
#include "setup.h"

int ClipFaceToPlane(brp_vertex* vp, brp_vertex* verts_out, int num_in, br_vector4* plane, int cmask) {
    brp_vertex* wp = verts_out;
    int num_out = 0;

    brp_vertex *up;
    br_scalar t, tu, tv;
    int m;
    br_scalar* usp;
    br_scalar* vsp;
    br_scalar* wsp;

    up = &vp[num_in - 1];

    tu = -BR_MAC4(
        plane->v[0], up->comp[C_X],
        plane->v[1], up->comp[C_Y],
        plane->v[2], up->comp[C_Z],
        plane->v[3], up->comp[C_W]);

    for ( ; num_in-- ; up = vp, tu = tv, vp++) {
        tv = -BR_MAC4(
            plane->v[0], vp->comp[C_X],
            plane->v[1], vp->comp[C_Y],
            plane->v[2], vp->comp[C_Z],
            plane->v[3], vp->comp[C_W]);
        if (tv <= 0.f) {
            if (tu <= 0.f) {
                *wp++ = *vp;
                num_out++;
                continue;
            }
            if (tv < 0.f) {
                t = tv / (tv - tu);

                usp = up->comp;
                vsp = vp->comp;
                wsp = wp->comp;

                for (m = cmask ; m != 0 ; m >>= 1, usp++, vsp++, wsp++) {
                    if (m & 1) {
                        *wsp = *vsp + t * (*usp - *vsp);
                    }
                }

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
            t = tu / (tu-tv);
            usp = up->comp;
            vsp = vp->comp;
            wsp = wp->comp;
            for (m = cmask ; m != 0 ; m >>=1, usp++, vsp++, wsp++) {
                if (m & 1) {
                    *wsp = *usp + t * (*vsp - *usp);
                }
            }

            wp->flags = TV_CLIPPED;
            wp++;
            num_out++;
        }
    }

    return num_out;
}

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

#define CLIP_TOGGLE do {                \
        toggle = !toggle;               \
        if (toggle) {                   \
            cp_in = C2V(clip_poly_2);   \
            cp_out = C2V(clip_poly_1);  \
        } else {                        \
            cp_in = C2V(clip_poly_1);   \
            cp_out = C2V(clip_poly_2);  \
        }                               \
    } while (0)

brp_vertex* C2_HOOK_STDCALL FaceClip(br_soft_renderer* self, brp_vertex* clip_in, br_uint_32 mask, br_uint_32 codes, int n, int* n_out) {
    static C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(brp_vertex, clip_poly_1, 16, 0x006b0a60);
    static C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(brp_vertex, clip_poly_2, 16, 0x006b0e60);
    brp_vertex* cp_in = clip_in;
    brp_vertex* cp_out = C2V(clip_poly_1);
    int c;
    br_boolean toggle = 1;

    if (codes & OUTCODE_HITHER) {
        n = ClipFaceToPlus1(cp_in, cp_out, n, C_Z, mask & ~(1 << C_Z));
        if (n < 3) {
            return NULL;
        }
        CLIP_TOGGLE;
    }

    if (codes & OUTCODE_YON) {
        n = ClipFaceToMinus1(cp_in, cp_out, n, C_Z, mask & ~(1 << C_Z));
        if (n < 3) {
            return NULL;
        }
        CLIP_TOGGLE;
    }

    if (codes & OUTCODE_RIGHT) {
        n = ClipFaceToPlus1(cp_in, cp_out, n, C_X, mask & ~(1 << C_X));
        if (n < 3) {
            return NULL;
        }
        CLIP_TOGGLE;
    }

    if (codes & OUTCODE_LEFT) {
        n = ClipFaceToMinus1(cp_in, cp_out, n, C_X,mask & ~(1 << C_X));
        if (n < 3) {
            return NULL;
        }
        CLIP_TOGGLE;
    }

    if (codes & OUTCODE_TOP) {
        n = ClipFaceToPlus1(cp_in, cp_out, n, C_Y, mask & ~(1 << C_Y));
        if (n < 3) {
            return NULL;
        }
        CLIP_TOGGLE;
    }

    if (codes & OUTCODE_BOTTOM) {
        n = ClipFaceToMinus1(cp_in, cp_out, n, C_Y,mask & ~(1 << C_Y));
        if (n < 3) {
            return NULL;
        }
        CLIP_TOGGLE;
    }

    if (C2V(scache).user_clip_active) {
        for (c = 0; c < MAX_STATE_CLIP_PLANES; c++) {

            if (self->state.clip[c].type != BRT_PLANE) {
                continue;
            }

            if (!(codes & (OUTCODE_USER << c))) {
                continue;
            }

            n = ClipFaceToPlane(cp_in, cp_out, n, &self->state.clip[c].plane, mask);
            if (n < 3) {
                return NULL;
            }
            CLIP_TOGGLE;
        }
    }

    *n_out = n;
    return cp_in;
}
C2_HOOK_FUNCTION(0x0054b730, FaceClip)

void C2_HOOK_STDCALL ClippedRenderTriangles(br_soft_renderer* renderer, brp_block* block, brp_vertex* cp_in, int n, br_uint_16 *fp_vertices, br_uint_16 *fp_edges) {
    int i;
    brp_vertex* tvp;

    for (i = 0; i < n; i++) {
        tvp = &cp_in[i];

        if (tvp->flags & (TV_CLIPPED | OUTCODES_ALL)) {
            PROJECT_VERTEX(tvp, tvp->comp[C_X], tvp->comp[C_Y], tvp->comp[C_Z], tvp->comp[C_W]);
            UPDATE_BOUNDS(tvp);
        }
    }

    for (i = 2; i < n; i++) {
        block->render(block, &cp_in[0], &cp_in[i - 1], &cp_in[i], fp_vertices, fp_edges);
    }
}
C2_HOOK_FUNCTION(0x0054bea0, ClippedRenderTriangles)

br_boolean C2_HOOK_STDCALL ClipLineToPlane(brp_vertex* in, brp_vertex* out, br_vector4* plane, int cmask) {
    br_scalar t, tu, tv;
    int m;
    br_scalar* usp;
    br_scalar* vsp;
    br_scalar* wsp;

    tu = -BR_MAC4(
        plane->v[0], in[0].comp[C_X],
        plane->v[1], in[0].comp[C_Y],
        plane->v[2], in[0].comp[C_Z],
        plane->v[3], in[0].comp[C_W]);

    tv = -BR_MAC4(
        plane->v[0], in[1].comp[C_X],
        plane->v[1], in[1].comp[C_Y],
        plane->v[2], in[1].comp[C_Z],
        plane->v[3], in[1].comp[C_W]);

    out[0] = in[0];
    out[1] = in[1];

    if (tu <= 0.f) {
        if (tv <= 0.f) {
            out[1] = in[1];
            return 1;
        }
        t = tu / (tu - tv);
        usp = in[0].comp;
        vsp = in[1].comp;
        wsp = out[1].comp;
        for (m = cmask ; m != 0; m >>= 1, usp++, vsp++, wsp++) {
            if (m & 1) {
                *wsp = *usp + t * (*vsp - *usp);
            }
        }
    } else {
        if (tv > 0.f) {
            return 0;
        }
        t = tv / (tv - tu);
        usp = in[0].comp;
        vsp = in[1].comp;
        wsp = out[0].comp;
        for (m = cmask ; m != 0 ; m >>= 1, usp++, vsp++, wsp++) {
            if (m & 1) {
                *wsp = *vsp + t * (*usp - *vsp);
            }
        }
    }

    return 1;
}
C2_HOOK_FUNCTION(0x0054bfc0, ClipLineToPlane)
