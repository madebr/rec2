#include "20-skidmark.h"

#include "41-utility.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "globvars.h"
#include "rec2_macros.h"
#include "c2_string.h"

#include "brender/brender.h"

/* The original names occupy writable data; both tables share OILSMEAR. */
// GLOBAL: CARMA2_HW 0x0065fe38
static char gOil_smear_material_name[] = "OILSMEAR.MAT";
// GLOBAL: CARMA2_HW 0x0065fe28
static char gRob_smear_material_name[] = "ROBSMEAR.MAT";
// GLOBAL: CARMA2_HW 0x0065fe18
static char gGib_smear_material_name[] = "GIBSMEAR.MAT";

// GLOBAL: CARMA2_HW 0x0065fe08
char* gBoring_material_names[2] = { gOil_smear_material_name, gRob_smear_material_name };
// GLOBAL: CARMA2_HW 0x0065fe10
char* gMaterial_names[2] = { gOil_smear_material_name, gGib_smear_material_name };
// GLOBAL: CARMA2_HW 0x006a27f0
tSkid gSkids[100];
// GLOBAL: CARMA2_HW 0x006a27e8
int gCurrent_skid;

// FUNCTION: CARMA2_HW 0x004ea2c0
void C2_HOOK_FASTCALL StretchMark(tSkid* pMark, br_vector3* pFrom, br_vector3* pTo, br_scalar pTexture_start, br_scalar pTexture_step) {
    br_vector3 temp;
    br_vector3 average;
    br_scalar len;
    br_scalar length_scale;
    br_scalar texture_scale;
    br_matrix34* mat;
    br_model* model;

    mat = &pMark->actor->t.t.mat;
    BrVector3Sub(&temp, pTo, pFrom);
    len = BrVector3Length(&temp);
    if (!(len >= 0.001f)) {
        BrMatrix34Scale(mat, 0.001f, 1.0f, 0.001f);
        return;
    }

    texture_scale = pTexture_step / len;
    mat->m[1][0] = pMark->normal.v[0];
    mat->m[1][1] = pMark->normal.v[1];
    mat->m[1][2] = pMark->normal.v[2];
    length_scale = len / len;
    mat->m[2][0] = temp.v[1] * pMark->normal.v[2] - temp.v[2] * pMark->normal.v[1];
    mat->m[2][1] = temp.v[2] * pMark->normal.v[0] - temp.v[0] * pMark->normal.v[2];
    mat->m[2][2] = temp.v[0] * pMark->normal.v[1] - temp.v[1] * pMark->normal.v[0];
    mat->m[2][0] *= texture_scale;
    mat->m[2][1] *= texture_scale;
    mat->m[2][2] *= texture_scale;
    mat->m[0][0] = temp.v[0] * length_scale;
    mat->m[0][1] = temp.v[1] * length_scale;
    mat->m[0][2] = temp.v[2] * length_scale;

    average.v[0] = (pFrom->v[0] + pTo->v[0]) * 0.5f;
    average.v[1] = (pFrom->v[1] + pTo->v[1]) * 0.5f;
    average.v[2] = (pFrom->v[2] + pTo->v[2]) * 0.5f;
    pMark->pos.v[0] = average.v[0];
    pMark->pos.v[1] = average.v[1];
    pMark->pos.v[2] = average.v[2];
    mat->m[3][0] = pMark->pos.v[0];
    mat->m[3][1] = pMark->pos.v[1];
    mat->m[3][2] = pMark->pos.v[2];

    model = pMark->actor->model;
    model->vertices[1].map.v[0] = pTexture_start / pTexture_step;
    model->vertices[0].map.v[0] = model->vertices[1].map.v[0];
    model->vertices[3].map.v[0] = (pTexture_start + len) / pTexture_step;
    model->vertices[2].map.v[0] = model->vertices[3].map.v[0];
    BrModelUpdate(model, BR_MODU_VERTEX_MAPPING);
}

// MaterialFromIndex

// FUNCTION: CARMA2_HW 0x004e9be0
void C2_HOOK_FASTCALL AdjustSkid(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial) {
    gSkids[pSkid_num].actor->t.t.mat = *pMatrix;
    gSkids[pSkid_num].pos.v[0] = pMatrix->m[3][0];
    gSkids[pSkid_num].pos.v[1] = pMatrix->m[3][1];
    gSkids[pSkid_num].pos.v[2] = pMatrix->m[3][2];
    gSkids[pSkid_num].actor->material = pMaterial;
    gSkids[pSkid_num].actor->render_style = BR_RSTYLE_DEFAULT;
}

// FarFromLine2D

// Reflex2D

// FUNCTION: CARMA2_HW 0x004e9c40
void C2_HOOK_FASTCALL InitSkids(void) {
    int skid;
    int mat;
    int sl;
    br_model* square;
    br_pixelmap* pm;
    char* str;

    for (mat = 0; mat < (int)REC2_ASIZE(gMaterial_names); mat++) {
        if (gProgram_state.sausage_eater_mode) {
            str = gBoring_material_names[mat];
        } else {
            str = gMaterial_names[mat];
        }
        gMaterial[mat] = BrMaterialFind(str);
        if (gMaterial[mat] == NULL) {
            if (gProgram_state.sausage_eater_mode) {
                str = gBoring_material_names[mat];
            } else {
                str = gMaterial_names[mat];
            }

            str = strtok(str, ".");
            sl = strlen(str);
            strcat(str, ".PIX");
            pm = LoadPixelmap(str);
            if (pm == NULL) {
                FatalError(kFatalError_CantLoadPixelmapFile_S, str);
            }
            BrMapAdd(pm);
            str[sl] = '\0';
            strcat(str, ".MAT");
            gMaterial[mat] = LoadMaterial(str);
            if (gMaterial[mat] == NULL) {
                BrFatal("C:\\Carma2\\Source\\Common\\Skidmark.c", 215, "Couldn't find %s", gMaterial_names[mat]);
            } else {
                GlorifyMaterial(&gMaterial[mat], 1, kRendererShadingType_AmbientOnly);
            }
            BrMaterialAdd(gMaterial[mat]);
        }
    }

    C2_HOOK_BUG_ON(sizeof(tSkid) != 28);

    for (skid = 0; skid < (int)REC2_ASIZE(gSkids); skid++) {
        gSkids[skid].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        BrActorAdd(gOther_selfs[3], gSkids[skid].actor);
        gSkids[skid].actor->render_style = BR_RSTYLE_NONE;
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
        gSkids[skid].actor->model = square;
    }
}

// FUNCTION: CARMA2_HW 0x004e9ee0
void C2_HOOK_FASTCALL HideSkid(int pSkid_num) {

    gSkids[pSkid_num].actor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x004e9f00
void C2_HOOK_FASTCALL HideSkids(void) {
    int skid;

    for (skid = 0; skid < (int)REC2_ASIZE(gSkids); skid++) {
        HideSkid(skid);
    }
}

// SkidLen

// UglyWidthChange

// GenericSkidSection

// WheelSkidSection

// SkidMark

// FUNCTION: CARMA2_HW 0x004ea700
void C2_HOOK_FASTCALL InitCarSkidStuff(tCar_spec* pCar) {
    pCar->old_skidding = 0;
    pCar->oil_remaining[0] = 0.0f;
    pCar->oil_remaining[1] = 0.0f;
    pCar->oil_remaining[2] = 0.0f;
    pCar->oil_remaining[3] = 0.0f;
}

// FUNCTION: CARMA2_HW 0x004ea720
void C2_HOOK_FASTCALL SkidsPerFrame(void) {
    int i;
    tSkid* skid;
    br_actor* actor;

    for (i = 0; i < 100; i++) {
        skid = &gSkids[i];
        actor = skid->actor;
        if (actor->render_style != BR_RSTYLE_NONE) {
            actor->t.t.translate.t.v[0] = skid->pos.v[0];
            actor->t.t.translate.t.v[1] = skid->pos.v[1];
            actor->t.t.translate.t.v[2] = skid->pos.v[2];
        }
    }
}

