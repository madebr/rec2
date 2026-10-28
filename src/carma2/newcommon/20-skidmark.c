/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "20-skidmark.h"

#include "41-utility.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "globvars.h"
#include "rec2_macros.h"
#include "c2_string.h"


// GLOBAL: CARMA2_HW 0x0065fe08
// GLOBAL: CARMA2_SW 0x0065a748
char* gBoring_material_names[2] = { "OILSMEAR.MAT", "ROBSMEAR.MAT" };

// GLOBAL: CARMA2_HW 0x0065fe10
// GLOBAL: CARMA2_SW 0x0065a750
char* gMaterial_names[2] = { "OILSMEAR.MAT", "GIBSMEAR.MAT" };

// GLOBAL: CARMA2_HW 0x006a27f0
// GLOBAL: CARMA2_SW 0x0069c848
tSkid gSkids[100];

// GLOBAL: CARMA2_HW 0x006a27e8
int gCurrent_skid;

// StretchMark

// MaterialFromIndex

// FUNCTION: CARMA2_HW 0x004e9be0
// FUNCTION: CARMA2_SW 0x004e59a0
void C2_HOOK_FASTCALL AdjustSkid(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial) {
    gSkids[pSkid_num].actor->t.t.mat = *pMatrix;
    gSkids[pSkid_num].pos = *(br_vector3*)pMatrix->m[3];
    gSkids[pSkid_num].actor->material = pMaterial;
    gSkids[pSkid_num].actor->render_style = BR_RSTYLE_DEFAULT;
}

// FarFromLine2D

// Reflex2D

// FUNCTION: CARMA2_HW 0x004e9c40
// FUNCTION: CARMA2_SW 0x004e5a00
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
// FUNCTION: CARMA2_SW 0x004e5c80
void C2_HOOK_FASTCALL HideSkid(int pSkid_num) {

    gSkids[pSkid_num].actor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x004e9f00
// FUNCTION: CARMA2_SW 0x004e5ca0
void C2_HOOK_FASTCALL HideSkids(void) {
    int skid;

    for (skid = 0; skid < (int)REC2_ASIZE(gSkids); skid++) {
        HideSkid(skid);
    }
}

// SkidLen

// UglyWidthChange

// GenericSkidSection

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004e9f20
// STUB: CARMA2_SW 0x004e5cc0
int C2_HOOK_FASTCALL SkidSection(tS16* pSkid_id, br_vector3* pSkid_start, br_vector3* pSkid_end, br_material* pMaterial, br_vector3* pPos, br_vector3* pNorm, br_vector3* pPrev_pos, br_vector3* pPrev_norm, br_scalar pTexture_start, br_scalar pTexture_step) {

    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004ea490
// FUNCTION: CARMA2_SW 0x004e6230
void C2_HOOK_FASTCALL SkidMark(tCar_spec* pCar_spec, int pWheel_num) {
    br_material* material;
    int skid_mask;
    br_vector3 disp;
    br_scalar dist;
    int on_ground;

    on_ground = pCar_spec->oldd[pWheel_num] < pCar_spec->susp_height[pWheel_num >> 1];
    material = gCurrent_race.material_modifiers[pCar_spec->material_index[pWheel_num]].skid_mark_material;
    if (pCar_spec->oil_remaining[pWheel_num] != 0.0f && on_ground) {
        pCar_spec->new_skidding |= 1 << pWheel_num;
        if (pCar_spec->field_0x16fc[pWheel_num] != NULL) {
            material = pCar_spec->field_0x16fc[pWheel_num];
        } else {
            material = gMaterial[0];
        }
    }
    if (material == NULL) {
        pCar_spec->old_skidding &= ~(1 << pWheel_num);
        return;
    }
    skid_mask = 1 << pWheel_num;
    if (!(pCar_spec->new_skidding & skid_mask) && !(pCar_spec->old_skidding & skid_mask)) {
        return;
    }
    if (pCar_spec->new_skidding & skid_mask) {
        if (skid_mask & pCar_spec->old_skidding) {
            BrVector3Sub(&disp, &pCar_spec->special_start[pWheel_num], &pCar_spec->prev_skid_pos[pWheel_num]);
            dist = BrVector3Length(&disp);
            if (dist < 0.05f) {
                return;
            }
            if (SkidSection(&pCar_spec->old_skid[pWheel_num],
                    &pCar_spec->field_0x160c[pWheel_num],
                    &pCar_spec->field_0x163c[pWheel_num],
                    material,
                    &pCar_spec->special_start[pWheel_num],
                    &pCar_spec->nor[pWheel_num],
                    &pCar_spec->prev_skid_pos[pWheel_num],
                    &pCar_spec->prev_nor[pWheel_num],
                    pCar_spec->total_length[pWheel_num],
                    0.05f)) {
                pCar_spec->old_skidding &= ~skid_mask;
            }
            pCar_spec->total_length[pWheel_num] += dist;
            pCar_spec->oil_remaining[pWheel_num] -= dist;
            if (pCar_spec->oil_remaining[pWheel_num] < 0.0f) {
                pCar_spec->oil_remaining[pWheel_num] = 0.0f;
            }
        } else {
            pCar_spec->old_skidding |= skid_mask;
            pCar_spec->total_length[pWheel_num] = 0.0f;
            pCar_spec->old_skid[pWheel_num] = -1;
        }
    } else {
        pCar_spec->old_skidding &= ~skid_mask;
    }
    pCar_spec->prev_skid_pos[pWheel_num] = pCar_spec->special_start[pWheel_num];
    pCar_spec->prev_nor[pWheel_num] = pCar_spec->nor[pWheel_num];
}

// FUNCTION: CARMA2_HW 0x004ea700
// FUNCTION: CARMA2_SW 0x004e64a0
void C2_HOOK_FASTCALL InitCarSkidStuff(tCar_spec* pCar) {
    int wheel;

    pCar->old_skidding = 0;
    for (wheel = 0; wheel < (int)REC2_ASIZE(pCar->oil_remaining); wheel++) {
        pCar->oil_remaining[wheel] = 0.0f;
    }
}

// SkidsPerFrame

