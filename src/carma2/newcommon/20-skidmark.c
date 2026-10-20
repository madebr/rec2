#include "20-skidmark.h"

#include "41-utility.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "globvars.h"
#include "rec2_macros.h"
#include "c2_string.h"


// GLOBAL: CARMA2_HW 0x0065fe08
char* gBoring_material_names[2] = { "OILSMEAR.MAT", "ROBSMEAR.MAT" };

// GLOBAL: CARMA2_HW 0x0065fe10
char* gMaterial_names[2] = { "OILSMEAR.MAT", "GIBSMEAR.MAT" };

// GLOBAL: CARMA2_HW 0x006a27f0
tSkid gSkids[100];

// GLOBAL: CARMA2_HW 0x006a27e8
int gCurrent_skid;

// StretchMark

// MaterialFromIndex

// AdjustSkid

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
    int wheel;

    pCar->old_skidding = 0;
    for (wheel = 0; wheel < (int)REC2_ASIZE(pCar->oil_remaining); wheel++) {
        pCar->oil_remaining[wheel] = 0.0f;
    }
}

// SkidsPerFrame

