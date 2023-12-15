#include "pedestrn.h"

#include "rec2_macros.h"

void C2_HOOK_FAKE_THISCALL ScaleModelXYZ(br_model* pModel, int pArg2, float pX, float pY, float pZ) {
    int i;

    REC2_THISCALL_UNUSED(pArg2);

    for (i = 0; i < pModel->nvertices; i++) {
        pModel->vertices[i].p.v[0] *= pX;
        pModel->vertices[i].p.v[1] *= pY;
        pModel->vertices[i].p.v[2] *= pZ;
    }
    BrModelUpdate(pModel, BR_MODU_NORMALS);
}
C2_HOOK_FUNCTION(0x00404290, ScaleModelXYZ)

void C2_HOOK_FAKE_THISCALL ScaleModel(br_model* pModel, int pArg2, float pScale) {
    int i;

    REC2_THISCALL_UNUSED(pArg2);

    for (i = 0; i < pModel->nvertices; i++) {
        pModel->vertices[i].p.v[0] *= pScale;
        pModel->vertices[i].p.v[1] *= pScale;
        pModel->vertices[i].p.v[2] *= pScale;
    }
    BrModelUpdate(pModel, BR_MODU_NORMALS);
}
C2_HOOK_FUNCTION(0x004042f0, ScaleModel)

void (C2_HOOK_CDECL * InitPeds_original)(void);
void C2_HOOK_CDECL InitPeds(void) {
#if defined(C2_HOOKS_ENABLED)
    InitPeds_original();
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004cadc0, InitPeds, InitPeds_original)
