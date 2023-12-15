#include "depth.h"

#include "errors.h"
#include "globvars.h"
#include "globvrkm.h"
#include "loading.h"
#include "utility.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gSky_on, 0x00591188, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gDepth_cueing_on, 0x0059118c, 1);

int C2_HOOK_FASTCALL GetSkyTextureOn(void) {

    return C2V(gSky_on);
}
C2_HOOK_FUNCTION(0x00446e00, GetSkyTextureOn)

void (C2_HOOK_FASTCALL * SetSkyTextureOn_original)(int skyTextureOn);
void C2_HOOK_FASTCALL SetSkyTextureOn(int skyTextureOn) {
#if defined(C2_HOOKS_ENABLED)
    SetSkyTextureOn_original(skyTextureOn);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00446e10, SetSkyTextureOn, SetSkyTextureOn_original)

int C2_HOOK_FASTCALL GetDepthCueingOn(void) {

    return C2V(gDepth_cueing_on);
}
C2_HOOK_FUNCTION(0x00446f90, GetDepthCueingOn)

void (C2_HOOK_FASTCALL * SetDepthCueingOn_original)(int pOn);
void C2_HOOK_FASTCALL SetDepthCueingOn(int pOn) {

#if defined(C2_HOOKS_ENABLED)
    SetDepthCueingOn_original(pOn);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00446fa0, SetDepthCueingOn, SetDepthCueingOn_original)

void (C2_HOOK_FASTCALL * ToggleDepthCueingQuietly_original)(void);
void C2_HOOK_FASTCALL ToggleDepthCueingQuietly(void) {

#if defined(C2_HOOKS_ENABLED)
    ToggleDepthCueingQuietly_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00447070, ToggleDepthCueingQuietly, ToggleDepthCueingQuietly_original)

void (C2_HOOK_FASTCALL * ToggleDepthCueing_original)(void);
void C2_HOOK_FASTCALL ToggleDepthCueing(void) {

#if defined(C2_HOOKS_ENABLED)
    ToggleDepthCueing_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00447110, ToggleDepthCueing, ToggleDepthCueing_original)

void C2_HOOK_STDCALL SetYon(br_scalar pYon) {
    int i;
    br_camera* camera_ptr;

    if (pYon < 5.0f) {
        pYon = 5.0f;
    }

    for (i = 0; i < REC2_ASIZE(C2V(gCamera_list)); i++) {
        if (C2V(gCamera_list)[i] != NULL) {
            camera_ptr = C2V(gCamera_list)[i]->type_data;
            camera_ptr->yon_z = pYon;
        }
    }
    C2V(gCamera_yon) = pYon;
}
C2_HOOK_FUNCTION(0x00446b70, SetYon)

br_scalar C2_HOOK_STDCALL GetYon(void) {

    return C2V(gCamera_yon);
}
C2_HOOK_FUNCTION(0x00446bb0, GetYon)

br_model* C2_HOOK_FASTCALL CreateHorizonModel(br_actor* pCamera) {
    tU8 nbands;
    tU8 band;
    tU8 vertex;
    tU8 stripe;
    br_model* model;

    nbands = 21;
    model = BrModelAllocate(NULL, 88, 6 * nbands);
    model->flags |= BR_MODF_KEEP_ORIGINAL;
    for (band = 0; band < nbands; band++) {
        for (stripe = 0; stripe < 3; stripe++) {
            vertex = 6 * band + 2 * stripe;
            model->faces[vertex + 0].vertices[0] = stripe + 4 * band + 0;
            model->faces[vertex + 0].vertices[1] = stripe + 4 * band + 1;
            model->faces[vertex + 0].vertices[2] = stripe + 4 * band + 5;
            model->faces[vertex + 1].vertices[0] = stripe + 4 * band + 0;
            model->faces[vertex + 1].vertices[1] = stripe + 4 * band + 5;
            model->faces[vertex + 1].vertices[2] = stripe + 4 * band + 4;

            model->faces[vertex + 0].smoothing = 1;
            model->faces[vertex + 1].smoothing = 1;

            model->faces[vertex + 0].material = NULL;
            model->faces[vertex + 1].material = NULL;
        }
    }
    return model;
}

int Log2(int pNumber) {
    int i;
    int bits[16];

    bits[0] = 1;
    bits[1] = 2;
    bits[2] = 4;
    bits[3] = 8;
    bits[4] = 16;
    bits[5] = 32;
    bits[6] = 64;
    bits[7] = 128;
    bits[8] = 256;
    bits[9] = 512;
    bits[10] = 1024;
    bits[11] = 2048;
    bits[12] = 4096;
    bits[13] = 0x2000;
    bits[14] = 0x4000;
    bits[15] = 0x8000;
    for (i = REC2_ASIZE(bits) - 1; i >= 0; --i) {
        if (bits[i] & pNumber) {
            return i;
        }
    }
    return 0;
}

void C2_HOOK_FASTCALL LoadDepthTable(char* pName, br_pixelmap** pTable, int* pPower) {
    tPath_name the_path;
    int i;
    int j;
    tU8 temp;

#define PTABLE_PIXEL_AT(X, Y) ((tU8*)((*pTable)->pixels))[(X) + (Y) * (*pTable)->row_bytes]

    PathCat(the_path, C2V(gApplication_path), "SHADETAB");
    PathCat(the_path, the_path, pName);
    *pTable = DRPixelmapLoad(the_path);
    if (*pTable == NULL) {
        FatalError(kFatalError_CannotLoadDepthCueShadeTable);
    }
    *pPower = Log2((*pTable)->height);
    for (i = 0; i < (*pTable)->width; i++) {
        for (j = 0; j < (*pTable)->height / 2; j++) {
            temp = PTABLE_PIXEL_AT(i, j);
            PTABLE_PIXEL_AT(i, j) = PTABLE_PIXEL_AT(i, ((*pTable)->height - j - 1));
            PTABLE_PIXEL_AT(i, ((*pTable)->height - j - 1)) = temp;
        }
    }

#undef PTABLE_PIXEL_AT
}
C2_HOOK_FUNCTION(0x00445860, LoadDepthTable)

void (C2_HOOK_FASTCALL * InitDepthEffects_original)(void);
void C2_HOOK_FASTCALL InitDepthEffects(void) {

#if defined(C2_HOOKS_ENABLED)
    InitDepthEffects_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00445620, InitDepthEffects, InitDepthEffects_original)

