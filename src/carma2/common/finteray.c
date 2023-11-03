#include "finteray.h"

#include <brender/brender.h>

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gPling_materials, 0x005964c0, 1);

int C2_HOOK_FASTCALL BadDiv__finteray(br_scalar a, br_scalar b) {

    return fabsf(b) < 1.0f && fabsf(b) * BR_SCALAR_MAX < fabsf(a);
}

void C2_HOOK_FASTCALL MultiRayCheckSingleFace(int pNum_rays, tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt) {
    int i;
    br_scalar t[4];
    br_scalar numerator;
    br_scalar d;
    br_vector3 p[4];
    br_vector3 tv;
    int axis_m;
    int axis_0;
    int axis_1;
    double u0[4];
    double u1;
    double u2;
    double v0[4];
    double v1;
    double v2;
    br_scalar v0i1;
    br_scalar v0i2;
    br_material* this_material;

    C2_HOOK_BUG_ON(sizeof(tFace_ref) != 80);

    this_material = pFace->material;
    d = BrVector3Dot(ray_dir, &pFace->normal);
    for (i = 0; i < pNum_rays; ++i) {
        rt[i] = 100.f;
    }
    if (this_material == NULL || (((this_material->flags & (BR_MATF_ALWAYS_VISIBLE | BR_MATF_TWO_SIDED)) != 0 || d <= 0.f)
        && (this_material == NULL || this_material->identifier == NULL || (this_material->identifier[0] != '!' && this_material->identifier[0] != '>' && this_material->identifier[0] != '?') || !C2V(gPling_materials))
        && fabsf(d) >= 2.384186e-7f)) {
        for (i = 0; i < pNum_rays; i++) {

            BrVector3Sub(&tv, &ray_pos[i], &pFace->v[0]);
            numerator = BrVector3Dot(&pFace->normal, &tv);
            if (BadDiv__finteray(numerator, d)) {
                return;
            }
            if (d > 0.0f) {
                if (-numerator < -0.001f || -numerator > d + 0.003f) {
                    t[i] = 100.0f;
                    continue;
                }
            } else if (numerator < -0.001f || 0.003f - d < numerator) {
                t[i] = 100.0f;
                continue;
            }
            t[i] = -(numerator / d);
            if (t[i] > 1.0f) {
                t[i] = 1.0f;
            }
            BrVector3Scale(&p[i], ray_dir, t[i]);
            BrVector3Accumulate(&p[i], &ray_pos[i]);
        }
        axis_m = fabsf(pFace->normal.v[0]) < fabsf(pFace->normal.v[1]);
        if (fabsf(pFace->normal.v[2]) > fabsf(pFace->normal.v[axis_m])) {
            axis_m = 2;
        }
        if (axis_m == 0) {
            axis_0 = 1;
            axis_1 = 2;
        } else if (axis_m == 1) {
            axis_0 = 0;
            axis_1 = 2;
        } else {
            axis_0 = 0;
            axis_1 = 1;
        }
        v0i1 = pFace->v[0].v[axis_0];
        v0i2 = pFace->v[0].v[axis_1];
        u1 = pFace->v[1].v[axis_0] - v0i1;
        u2 = pFace->v[2].v[axis_0] - v0i1;
        v1 = pFace->v[1].v[axis_1] - v0i2;
        v2 = pFace->v[2].v[axis_1] - v0i2;
        for (i = 0; i < pNum_rays; i++) {
            if (t[i] != 100.0f) {
                br_scalar s1, s2, s3, s4;

                u0[i] = p[i].v[axis_0] - v0i1;
                v0[i] = p[i].v[axis_1] - v0i2;
                s1 = (br_scalar)(u1 * v0[i] - v1 * u0[i]);
                s2 = (br_scalar)(v2 * u1 - v1 * u2);
                if (fabsf(s1) <= 1.0001f * fabsf(s2) && s2 != 0.f) {
                    s1 = s1 / s2;
                    if (s1 >= -0.0001f) {
                        s3 = (br_scalar)(u2 * v0[i] - v2 * u0[i]);
                        s4 = -s2;
                        if (fabsf(s3) <= 1.0001f * fabsf(s4) && s4 != 0.f) {
                            s3 = s3 / s4;
                            if (s3 >= -0.0001f && s1 + s3 <= 1.0001f) {
                                rt[i] = t[i];
                                *normal = pFace->normal;
                                if (d > 0.f) {
                                    BrVector3Negate(normal, &pFace->normal);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
C2_HOOK_FUNCTION(0x0045f0d0, MultiRayCheckSingleFace)

void C2_HOOK_FASTCALL GetNewBoundingBox(br_bounds* b2, br_bounds* b1, br_matrix34* m) {
    br_vector3 a;
    br_vector3 c[3];
    int j;

    BrMatrix34ApplyP(&b2->min, &b1->min, m);
    BrVector3Copy(&b2->max, &b2->min);
    BrVector3Sub(&a, &b1->max, &b1->min);
    for (j = 0; j < 3; j++) {
        BrVector3Scale(&c[j], (br_vector3*)m->m[j], a.v[j]);
    }
    for (j = 0; j < 3; ++j) {
        b2->min.v[j] = (float)(c[2].v[j] < 0.f) * c[2].v[j]
                       + (float)(c[1].v[j] < 0.f) * c[1].v[j]
                       + (float)(c[0].v[j] < 0.f) * c[0].v[j]
                       + b2->min.v[j];
        b2->max.v[j] = (float)(c[0].v[j] > 0.f) * c[0].v[j]
                       + (float)(c[2].v[j] > 0.f) * c[2].v[j]
                       + (float)(c[1].v[j] > 0.f) * c[1].v[j]
                       + b2->max.v[j];
    }
}
C2_HOOK_FUNCTION(0x0045f5b0, GetNewBoundingBox)

int C2_HOOK_FASTCALL PickBoundsTestRay__finteray(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far) {
    int i;
    float s;
    float t;

    for (i = 0; i < 3; i++) {
        if (rd->v[i] >= -2.3841858e-7f) {
            if (rd->v[i] <= 2.3841858e-7f) {
                if (b->max.v[i] < rp->v[i] || rp->v[i] < b->min.v[i]) {
                    return 0;
                }
            } else {
                s = (-1.0f / rd->v[i]) * (rp->v[i] - b->max.v[i]);
                if (s < BR_SCALAR_MIN) {
                    t_far = BR_SCALAR_MIN;
                } else if (s < t_far) {
                    t_far = s;
                }
                t = (-1.0f / rd->v[i]) * (rp->v[i] - b->min.v[i]);
                if (t > BR_SCALAR_MAX) {
                    t_near = BR_SCALAR_MAX;
                } else if (t > t_near) {
                    t_near = t;
                }
            }
        } else {
            s = (-1.0f / rd->v[i]) * (rp->v[i] - b->max.v[i]);
            if (s > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else if (s > t_near) {
                t_near = s;
            }
            t = (-1.0f / rd->v[i]) * (rp->v[i] - b->min.v[i]);
            if (t < BR_SCALAR_MIN) {
                t_far = BR_SCALAR_MIN;
            } else if (t < t_far) {
                t_far = t;
            }
        }
    }
    if (t_far < t_near) {
        return 0;
    }
    *new_t_near = t_near;
    *new_t_far = t_far;
    return 1;
}
C2_HOOK_FUNCTION(0x0045cd90, PickBoundsTestRay__finteray)
