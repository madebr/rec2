#include "brucetrk.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_scalar, gYon_factor, 0x00655e60, 0.25f);

br_scalar C2_HOOK_STDCALL GetYonFactor(void) {

    return C2V(gYon_factor);
}
C2_HOOK_FUNCTION(0x0040dfe0, GetYonFactor)

void C2_HOOK_STDCALL SetYonFactor(br_scalar pNew) {

    C2V(gYon_factor) = pNew;
}
C2_HOOK_FUNCTION(0x0040dff0, SetYonFactor)

C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gMr_blendy, 0x00679264);

void (C2_HOOK_FASTCALL * StripBlendedFaces_original)(br_actor* pActor, br_model* pModel);
void C2_HOOK_FASTCALL StripBlendedFaces(br_actor* pActor, br_model* pModel) {

#if defined(C2_HOOKS_ENABLED)
    StripBlendedFaces_original(pActor, pModel);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040d530, StripBlendedFaces, StripBlendedFaces_original)

intptr_t (C2_HOOK_CDECL * FindNonCarsCB_original)(br_actor* pActor, tTrack_spec* pTrack_spec);
intptr_t C2_HOOK_CDECL FindNonCarsCB(br_actor* pActor, tTrack_spec* pTrack_spec) {

#if defined(C2_HOOKS_ENABLED)
    return FindNonCarsCB_original(pActor, pTrack_spec);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040d1f0, FindNonCarsCB, FindNonCarsCB_original)

void (C2_HOOK_FASTCALL * GetModelTextureArea_original)(br_model* pModel, int* pArea_1, int* pArea_2);
void C2_HOOK_FASTCALL GetModelTextureArea(br_model* pModel, int* pArea_1, int* pArea_2) {
    int i;

    *pArea_1 = 0;
    *pArea_2 = 0;

    for (i = 0; i < V11MODEL(pModel)->ngroups; i++) {
        int j;
        v11group* v11g = &V11MODEL(pModel)->groups[i];

        for (j = 0; j < v11g->nfaces; j++) {
            int k;
            float min_x =  10000.f;
            float max_x = -10000.f;
            float min_y =  10000.f;
            float max_y = -10000.f;
            v11face* v11f = &v11g->faces[j];
            int dx;
            int dy;

            for (k = 0; k < 3; k++) {
                br_uint_16 vertex_i = v11f->vertices[k];

                if (v11g->vertices[vertex_i].map.v[0] < min_x) {
                    min_x = v11g->vertices[vertex_i].map.v[0];
                }
                if (v11g->vertices[vertex_i].map.v[0] > max_x) {
                    max_x = v11g->vertices[vertex_i].map.v[0];
                }
                if (v11g->vertices[vertex_i].map.v[1] < min_y) {
                    min_y = v11g->vertices[vertex_i].map.v[1];
                }
                if (v11g->vertices[vertex_i].map.v[1] > max_y) {
                    max_y = v11g->vertices[vertex_i].map.v[1];
                }
            }
            dy = (int)(max_y - min_y + 0.9f);
            dx = (int)(max_x - min_x + 0.9f);
            *pArea_1 += dx * dy;
            *pArea_2 += dx * dy;
        }
    }
    *pArea_1 += 50;
    *pArea_2 += 50;
    if (*pArea_1 > 250) {
        *pArea_1 = 250;
    }
    if (*pArea_2 > 250) {
        *pArea_2 = 250;
    }

#if C2_HOOKS_ENABLED
    {
        int a1, a2;

        GetModelTextureArea_original(pModel, &a1, &a2);
        C2_HOOK_ASSERT(a1 == *pArea_1);
        C2_HOOK_ASSERT(a2 == *pArea_2);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004f5ae0, GetModelTextureArea, GetModelTextureArea_original)

void C2_HOOK_FASTCALL FixModelPointer(br_model* pModel, br_uint_16 pFlags) {

    C2_HOOK_BUG_ON(sizeof(v11group) != 0x24);

    if (pModel->nvertices != 0 && pModel->nfaces != 0) {
        int i;

        BrModelUpdate(pModel, pFlags);
        for (i = 0; i < V11MODEL(pModel)->ngroups; i++) {
            v11group* v11g;

            v11g = &V11MODEL(pModel)->groups[i];
            (br_material*)v11g->face_colours = pModel->faces[*v11g->face_user].material;
        }
    }
}
C2_HOOK_FUNCTION(0x00515fa0, FixModelPointer)
