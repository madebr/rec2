#include "skidmark.h"

#include "errors.h"
#include "globvars.h"
#include "graphics.h"
#include "loading.h"
#include "piping.h"
#include "temp.h"

#include <brender/brender.h>

#include "rec2_macros.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(char*, gBoring_material_names, 2, 0x0065fe08, {
    "OILSMEAR.MAT",
    "ROBSMEAR.MAT",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(char*, gMaterial_names, 2, 0x0065fe10, {
    "OILSMEAR.MAT",
    "GIBSMEAR.MAT",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_material*, gMaterial, 2, 0x0074cee8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tSkid, gSkids, 100, 0x006a27f0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCurrent_skid, 0x006a27e8);

void (C2_HOOK_FASTCALL * InitSkids_original)(void);
void C2_HOOK_FASTCALL InitSkids(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitSkids_original();
#else

    int skid;
    int mat;
    int sl;
    br_model* square;
    br_pixelmap* pm;
#if defined(REC2_FIX_BUGS)
    char mat_name[32];
#endif
    char* str;

    for (mat = 0; mat < REC2_ASIZE(C2V(gMaterial_names)); mat++) {
        if (C2V(gProgram_state).sausage_eater_mode) {
            str = C2V(gBoring_material_names)[mat];
        } else {
            str = C2V(gMaterial_names)[mat];
        }
        C2V(gMaterial)[mat] = BrMaterialFind(str);
        if (C2V(gMaterial)[mat] == NULL) {
            if (C2V(gProgram_state).sausage_eater_mode) {
                str = C2V(gBoring_material_names)[mat];
            } else {
                str = C2V(gMaterial_names)[mat];
            }

#if defined(REC2_FIX_BUGS)
            // Avoid modification of read-only data by strtok.
            c2_strcpy(mat_name, str);
            str = mat_name;
#endif
            sl = c2_strlen(c2_strtok(str, "."));
            c2_strcpy(str + sl, ".PIX");
            pm = LoadPixelmap(str);
            if (pm == NULL) {
                FatalError(kFatalError_CantLoadPixelmapFile_S, str);
            }
            BrMapAdd(pm);
            c2_strcpy(str + sl, ".MAT");
            C2V(gMaterial)[mat] = LoadMaterial(str);
            if (C2V(gMaterial)[mat] == NULL) {
                BrFatal("C:\\Carma2\\Source\\Common\\Skidmark.c", 215, "Couldn't find %s", C2V(gMaterial_names)[mat]);
            }
            GlorifyMaterial(&C2V(gMaterial)[mat], 1, kRendererShadingType_AmbientOnly);
            BrMaterialAdd(C2V(gMaterial)[mat]);
        }
    }

    C2_HOOK_BUG_ON(sizeof(tSkid) != 28);

    for (skid = 0; skid < REC2_ASIZE(C2V(gSkids)); skid++) {
        C2V(gSkids)[skid].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        BrActorAdd(C2V(gOther_selfs)[3], C2V(gSkids)[skid].actor);
        C2V(gSkids)[skid].actor->render_style = BR_RSTYLE_NONE;
        square = BrModelAllocate(NULL, 4, 2);
        BrVector3Set(&square->vertices[0].p, -0.5f, 0.0f, -0.5f);
        BrVector3Set(&square->vertices[1].p, -0.5f, 0.0f, 0.5f);
        BrVector3Set(&square->vertices[2].p, 0.5f, 0.0f, 0.5f);
        BrVector3Set(&square->vertices[3].p, 0.5f, 0.0f, -0.5f);
        BrVector2Set(&square->vertices[0].map, 0.0f, 0.0f);
        BrVector2Set(&square->vertices[1].map, 0.0f, 1.0f);
        BrVector2Set(&square->vertices[2].map, 1.0f, 1.0f);
        BrVector2Set(&square->vertices[3].map, 1.0f, 0.0f);
        square->faces[0].vertices[0] = 0;
        square->faces[0].vertices[1] = 1;
        square->faces[0].vertices[2] = 2;
        square->faces[0].smoothing = 1;
        square->faces[1].vertices[0] = 0;
        square->faces[1].vertices[1] = 2;
        square->faces[1].vertices[2] = 3;
        square->faces[1].smoothing = 1;
        square->flags |= BR_MODF_KEEP_ORIGINAL;
        BrModelAdd(square);
        C2V(gSkids)[skid].actor->model = square;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e9c40, InitSkids, InitSkids_original)

void C2_HOOK_FASTCALL HideSkid(int pSkid_num) {

    C2V(gSkids)[pSkid_num].actor->render_style = BR_RSTYLE_NONE;
}
C2_HOOK_FUNCTION(0x004e9ee0, HideSkid)

void C2_HOOK_FASTCALL HideSkids(void) {
    int skid;

    for (skid = 0; skid < REC2_ASIZE(C2V(gSkids)); skid++) {
        HideSkid(skid);
    }
}
C2_HOOK_FUNCTION(0x004e9f00, HideSkids)

void (C2_HOOK_FASTCALL * ReadSlick_original)(FILE* pF,tSlick_spec* pSlick_spec);
void C2_HOOK_FASTCALL ReadSlick(FILE* pF, tSlick_spec* pSlick_spec) {

#if 0//defined(C2_HOOKS_ENABLED)
    ReadSlick_original(pF, pSlick_spec);
#else
    float f1, f2, f3;
    char s[256];

    GetAString(pF, s);
    if (c2_strcmp(s, "none") == 0) {
        pSlick_spec->material = NULL;
        return;
    }

    pSlick_spec->material = LoadTemporaryMaterial(s);

    GetThreeFloats(pF, &f1, &f2, &f3);
    pSlick_spec->field_0x0 = (int)f1;
    pSlick_spec->field_0xc = f2;
    pSlick_spec->field_0x10 = f3;
    GetThreeFloats(pF, &f1, &f2, &f3);
    pSlick_spec->field_0x14 = f1;
    pSlick_spec->field_0x4 = f2;
    pSlick_spec->field_0x8 = f3;
    GetPairOfFloats(pF, &f1, &f2);
    pSlick_spec->field_0x20 = f2;
    pSlick_spec->field_0x24 = f3;
    if (pSlick_spec->field_0x24 != 0.f) {
        GetPairOfFloats(pF, &f1, &f2);
        pSlick_spec->field_0x18 = f2;
        pSlick_spec->field_0x1c = f3;
        GetAString(pF, s);
        pSlick_spec->material2 = LoadTemporaryMaterial(s);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ee5e0, ReadSlick, ReadSlick_original)

void C2_HOOK_FASTCALL AdjustSkid(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial) {

    BrMatrix34Copy(&C2V(gSkids)[pSkid_num].actor->t.t.mat, pMatrix);
    BrVector3Copy(&C2V(gSkids)[pSkid_num].pos, (br_vector3*)pMatrix->m[3]);
    C2V(gSkids)[pSkid_num].actor->material = pMaterial;
    C2V(gSkids)[pSkid_num].actor->render_style = BR_RSTYLE_DEFAULT;
}
C2_HOOK_FUNCTION(0x004e9be0, AdjustSkid)

void C2_HOOK_FASTCALL StretchMark(tSkid* pMark, br_vector3* pFrom, br_vector3* pTo, br_scalar pTexture_start, br_scalar pTexture_step) {
    br_vector3 temp;
    br_vector3* rows;
    br_scalar len;
    br_model* model;

    BrVector3Sub(&temp, pTo, pFrom);
    len = BrVector3Length(&temp);
    if (len < 0.001f) {
        BrMatrix34Scale(&pMark->actor->t.t.mat, 0.001f, 0.001f, 0.001f);
        return;
    }

    rows = (br_vector3*)&pMark->actor->t.t.mat.m;
    BrVector3Copy(&rows[1], &pMark->normal);
    BrVector3Cross(&rows[2], &temp, &pMark->normal);
    BrVector3Scale(&rows[2], &rows[2], pTexture_step / len);
    BrVector3Copy(&rows[0], &temp);
    Vector3Average(&pMark->pos, pFrom, pTo);
    BrVector3Copy(&rows[3], &pMark->pos);

    model = pMark->actor->model;
    model->vertices[0].map.v[0] = model->vertices[1].map.v[0] = pTexture_start / pTexture_step;
    model->vertices[2].map.v[0] = model->vertices[3].map.v[0] = (pTexture_start + len) / pTexture_step;
    BrModelUpdate(model, BR_MODU_VERTEX_MAPPING);
}
C2_HOOK_FUNCTION(0x004ea2c0, StretchMark)

int C2_HOOK_FASTCALL SkidSection(tS16* pSkid_id, br_vector3* pSkid_start, br_vector3* pSkid_end, br_material* pMaterial, br_vector3* pPos, br_vector3* pNorm, br_vector3* pPrev_pos, br_vector3* pPrev_norm, br_scalar pTexture_start, br_scalar pTexture_step) {

    if (BrVector3Dot(pNorm, pPrev_norm) < 0.997f || fabsf(BrVector3Dot(pNorm, pPos) - BrVector3Dot(pPrev_pos, pNorm)) > 0.01f) {
        *pSkid_id = -1;
        return 1;
    }

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gSkids)) != 100);

    if (*pSkid_id < REC2_ASIZE(C2V(gSkids)) && C2V(gSkids)[*pSkid_id].actor->material == pMaterial) {
        br_scalar tmp = BrVector3LengthSquared((br_vector3*)C2V(gSkids)[*pSkid_id].actor->t.t.mat.m[2]);
        if (tmp < 2 * (pTexture_step * pTexture_step) && pTexture_step * pTexture_step <= 2 * tmp) {
            if (BrVector3LengthSquared((br_vector3*)C2V(gSkids)[*pSkid_id].actor->t.t.mat.m[0]) <= 0.5f) {
                br_vector3 t1, t2, t3;
                br_vector3 t4, t5;

                BrVector3Sub(&t1, pSkid_end, pSkid_start);
                t1.v[1] = 0.f;
                BrVector3Sub(&t2, pPos, pSkid_end);
                t2.v[1] = 0.f;
                BrVector3Cross(&t3, &t1, &t2);

                BrVector3Sub(&t4, pPos, pPrev_pos);
                t4.v[1] = 0.f;
                BrVector3Sub(&t5, pPrev_pos, pSkid_start);
                t5.v[1] = 0.f;

                if (fabsf(t3.v[1]) <= 0.05f * BrVector3Length(&t1) && BrVector3Dot(&t4, &t5) >= 0.f) {
                    StretchMark(&C2V(gSkids)[*pSkid_id], pSkid_start, pPos, pTexture_start, pTexture_step);
                    PipeSingleSkidAdjustment(*pSkid_id, &C2V(gSkids)[*pSkid_id].actor->t.t.mat, pMaterial);
                    return 0;
                }
            }
        }
    }
    BrVector3Copy(pSkid_start, pPrev_pos);
    BrVector3Copy(pSkid_end, pPos);
    C2V(gSkids)[C2V(gCurrent_skid)].actor->render_style = BR_RSTYLE_DEFAULT;
    C2V(gSkids)[C2V(gCurrent_skid)].actor->material = pMaterial;
    BrVector3Copy(&C2V(gSkids)[C2V(gCurrent_skid)].normal, pNorm);
    StretchMark(&C2V(gSkids)[C2V(gCurrent_skid)], pPrev_pos, pPos, pTexture_start, pTexture_step);
    PipeSingleSkidAdjustment(C2V(gCurrent_skid), &C2V(gSkids)[C2V(gCurrent_skid)].actor->t.t.mat, pMaterial);
    *pSkid_id = C2V(gCurrent_skid);
    C2V(gCurrent_skid) = (C2V(gCurrent_skid) + 1) % REC2_ASIZE(C2V(gSkids));
    return 0;
}
C2_HOOK_FUNCTION(0x004e9f20, SkidSection)
