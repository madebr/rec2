#include "skidmark.h"

#include "errors.h"
#include "globvars.h"
#include "graphics.h"
#include "loading.h"
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
            pm = DRLoadPixelmap(str);
            if (pm == NULL) {
                FatalError(kFatalError_CantLoadPixelmapFile_S, str);
            }
            BrMapAdd(pm);
            c2_strcpy(str + sl, ".MAT");
            C2V(gMaterial)[mat] = LoadMaterial(str);
            if (C2V(gMaterial)[mat] == NULL) {
                BrFatal("C:\\Carma2\\Source\\Common\\Skidmark.c", 215, "Couldn't find %s", C2V(gMaterial_names)[mat]);
            }
            AdaptMaterialsForRenderer(&C2V(gMaterial)[mat], 1, kRendererShadingType_AmbientOnly);
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
