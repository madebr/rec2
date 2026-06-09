#include "vector.h"
#include "c2_math.h"


// FUNCTION: CARMA2_HW 0x00534390
void C2_HOOK_CDECL BrVector2Copy(br_vector2* v1, const br_vector2* v2) {

    v1->v[0] = v2->v[0];
    v1->v[1] = v2->v[1];
}

// FUNCTION: CARMA2_HW 0x005343b0
void C2_HOOK_CDECL BrVector2Set(br_vector2* v1, br_scalar s1, br_scalar s2) {

    v1->v[0] = s1;
    v1->v[1] = s2;
}

// FUNCTION: CARMA2_HW 0x005343d0
void C2_HOOK_CDECL BrVector2SetInt(br_vector2* v1, int i1, int i2) {

    v1->v[0] = BR_SCALAR(i1);
    v1->v[1] = BR_SCALAR(i2);
}

// FUNCTION: CARMA2_HW 0x005343f0
void C2_HOOK_CDECL BrVector2SetFloat(br_vector2* v1, float f1, float f2) {

    v1->v[0] = f1;
    v1->v[1] = f2;
}

// FUNCTION: CARMA2_HW 0x00534410
void C2_HOOK_CDECL BrVector2Negate(br_vector2* v1, const br_vector2* v2) {

    v1->v[0] = -v2->v[0];
    v1->v[1] = -v2->v[1];
}

// FUNCTION: CARMA2_HW 0x00534430
void C2_HOOK_CDECL BrVector2Add(br_vector2* v1, const br_vector2* v2, const br_vector2* v3) {

    v1->v[0] = v2->v[0] + v3->v[0];
    v1->v[1] = v2->v[1] + v3->v[1];
}

// FUNCTION: CARMA2_HW 0x00534450
void C2_HOOK_CDECL BrVector2Accumulate(br_vector2* v1, const br_vector2* v2) {

    v1->v[0] += v2->v[0];
    v1->v[1] += v2->v[1];
}

// FUNCTION: CARMA2_HW 0x00534470
void C2_HOOK_CDECL BrVector2Sub(br_vector2* v1, const br_vector2* v2, const br_vector2* v3) {

    v1->v[0] = v2->v[0] - v3->v[0];
    v1->v[1] = v2->v[1] - v3->v[1];
}

// FUNCTION: CARMA2_HW 0x00534490
void C2_HOOK_CDECL BrVector2Scale(br_vector2* v1, const br_vector2* v2, br_scalar s) {

    v1->v[0] = v2->v[0] * s;
    v1->v[1] = v2->v[1] * s;
}

// FUNCTION: CARMA2_HW 0x005344b0
void C2_HOOK_CDECL BrVector2InvScale(br_vector2* v1, const br_vector2* v2, br_scalar s) {

    v1->v[0] = v2->v[0] / s;
    v1->v[1] = v2->v[1] / s;
}

// FUNCTION: CARMA2_HW 0x005344d0
br_scalar C2_HOOK_CDECL BrVector2Dot(br_vector2* v1, const br_vector2* v2) {

    return BR_MAC2(v1->v[0], v2->v[0], v1->v[1], v2->v[1]);
}

// FUNCTION: CARMA2_HW 0x005344f0
br_scalar C2_HOOK_CDECL BrVector2Length(const br_vector2* v1) {

    return (br_scalar)sqrt(BR_MAC2(v1->v[0], v1->v[0], v1->v[1], v1->v[1]));
}

// FUNCTION: CARMA2_HW 0x00534510
br_scalar C2_HOOK_CDECL BrVector2LengthSquared(const br_vector2* v1) {

    return BR_MAC2(v1->v[0], v1->v[0], v1->v[1], v1->v[1]);
}

// FUNCTION: CARMA2_HW 0x00534530
void C2_HOOK_CDECL BrVector3Copy(br_vector3* v1, const br_vector3* v2) {

    v1->v[0] = v2->v[0];
    v1->v[1] = v2->v[1];
    v1->v[2] = v2->v[2];
}

// FUNCTION: CARMA2_HW 0x00534550
void C2_HOOK_CDECL BrVector3Set(br_vector3* v1, br_scalar s1, br_scalar s2, br_scalar s3) {

    v1->v[0] = s1;
    v1->v[1] = s2;
    v1->v[2] = s3;
}

// FUNCTION: CARMA2_HW 0x00534570
void C2_HOOK_CDECL BrVector3SetInt(br_vector3* v1, int i1, int i2, int i3) {

    v1->v[0] = BR_SCALAR(i1);
    v1->v[1] = BR_SCALAR(i2);
    v1->v[2] = BR_SCALAR(i3);
}

// FUNCTION: CARMA2_HW 0x00534590
void C2_HOOK_CDECL BrVector3SetFloat(br_vector3* v1, float f1, float f2, float f3) {

    v1->v[0] = BR_SCALAR(f1);
    v1->v[1] = BR_SCALAR(f2);
    v1->v[2] = BR_SCALAR(f3);
}

// FUNCTION: CARMA2_HW 0x005345b0
void C2_HOOK_CDECL BrVector3Negate(br_vector3* v1, const br_vector3* v2) {

    v1->v[0] = -v2->v[0];
    v1->v[1] = -v2->v[1];
    v1->v[2] = -v2->v[2];
}

// FUNCTION: CARMA2_HW 0x005345d0
void C2_HOOK_CDECL BrVector3Add(br_vector3* v1, const br_vector3* v2, const br_vector3* v3) {

    v1->v[0] = v2->v[0] + v3->v[0];
    v1->v[1] = v2->v[1] + v3->v[1];
    v1->v[2] = v2->v[2] + v3->v[2];
}

// FUNCTION: CARMA2_HW 0x00534600
void C2_HOOK_CDECL BrVector3Accumulate(br_vector3* v1, const br_vector3* v2) {

    v1->v[0] += v2->v[0];
    v1->v[1] += v2->v[1];
    v1->v[2] += v2->v[2];
}

// FUNCTION: CARMA2_HW 0x00534630
void C2_HOOK_CDECL BrVector3Sub(br_vector3* v1, const br_vector3* v2, const br_vector3* v3) {

    v1->v[0] = v2->v[0] - v3->v[0];
    v1->v[1] = v2->v[1] - v3->v[1];
    v1->v[2] = v2->v[2] - v3->v[2];
}

// FUNCTION: CARMA2_HW 0x00534660
void C2_HOOK_CDECL BrVector3Scale(br_vector3* v1, const br_vector3* v2, br_scalar s) {

    v1->v[0] = v2->v[0] * s;
    v1->v[1] = v2->v[1] * s;
    v1->v[2] = v2->v[2] * s;
}

// FUNCTION: CARMA2_HW 0x00534690
void C2_HOOK_CDECL BrVector3InvScale(br_vector3* v1, const br_vector3* v2, br_scalar s) {

    v1->v[0] = v2->v[0] / s;
    v1->v[1] = v2->v[1] / s;
    v1->v[2] = v2->v[2] / s;
}

// FUNCTION: CARMA2_HW 0x005346c0
br_scalar C2_HOOK_CDECL BrVector3Dot(br_vector3* v1, const br_vector3* v2) {

    return BR_MAC3(v1->v[0], v2->v[0], v1->v[1], v2->v[1], v1->v[2], v2->v[2]);
}

// FUNCTION: CARMA2_HW 0x005346f0
void C2_HOOK_CDECL BrVector3Cross(br_vector3* v1, const br_vector3* v2, const br_vector3* v3) {

    v1->v[0] = v2->v[1] * v3->v[2] - v2->v[2] * v3->v[1];
    v1->v[1] = v2->v[2] * v3->v[0] - v2->v[0] * v3->v[2];
    v1->v[2] = v2->v[0] * v3->v[1] - v2->v[1] * v3->v[0];
}

// FUNCTION: CARMA2_HW 0x00534740
br_scalar C2_HOOK_CDECL BrVector3Length(const br_vector3* v1) {

    return (br_scalar)sqrt(BR_MAC3(v1->v[0], v1->v[0], v1->v[1], v1->v[1], v1->v[2], v1->v[2]));
}

// FUNCTION: CARMA2_HW 0x00534780
br_scalar C2_HOOK_CDECL BrVector3LengthSquared(const br_vector3* v1) {

    return BR_MAC3(v1->v[0], v1->v[0], v1->v[1], v1->v[1], v1->v[2], v1->v[2]);
}

// FUNCTION: CARMA2_HW 0x005347c0
void C2_HOOK_CDECL BrVector3Normalise(br_vector3* v1, const br_vector3* v2) {
    br_scalar scale;

    scale = BR_LENGTH3(v2->v[0], v2->v[1], v2->v[2]);

    if (scale > BR_SCALAR_EPSILON * 2) {
        v1->v[0] = v2->v[0] / scale;
        v1->v[1] = v2->v[1] / scale;
        v1->v[2] = v2->v[2] / scale;

    } else {
        v1->v[0] = BR_SCALAR(1.0);
        v1->v[1] = BR_SCALAR(0.0);
        v1->v[2] = BR_SCALAR(0.0);
    }
}

// FUNCTION: CARMA2_HW 0x00534840
void C2_HOOK_CDECL BrVector3NormaliseLP(br_vector3* v1, const br_vector3* v2) {
    br_scalar scale;

    scale = 1.f / (br_scalar)sqrt(BR_MAC3(v2->v[0], v2->v[0], v2->v[1], v2->v[1], v2->v[2], v2->v[2]));
    if (scale != 0.f) {
        v1->v[0] = v2->v[0] * scale;
        v1->v[1] = v2->v[1] * scale;
        v1->v[2] = v2->v[2] * scale;
    }
}

// FUNCTION: CARMA2_HW 0x005348b0
br_scalar C2_HOOK_CDECL BrVector4Dot(const br_vector4* v1, const br_vector4* v2) {

    return BR_MAC4(v1->v[0], v2->v[0], v1->v[1], v2->v[1], v1->v[2], v2->v[2], v1->v[3], v2->v[3]);
}

// FUNCTION: CARMA2_HW 0x005348e0
void C2_HOOK_CDECL BrVector4Copy(br_vector4* v1, const br_vector4* v2) {

    v1->v[0] = v2->v[0];
    v1->v[1] = v2->v[1];
    v1->v[2] = v2->v[2];
    v1->v[3] = v2->v[3];
}

// FUNCTION: CARMA2_HW 0x00534900
br_scalar C2_HOOK_CDECL BrFVector2Dot(const br_fvector2* v1, const br_vector2* v2) {

    return BR_MAC2(v1->v[0], v2->v[0], v1->v[1], v2->v[1]);
}

// FUNCTION: CARMA2_HW 0x00534920
void C2_HOOK_CDECL BrFVector3Copy(br_fvector3* v1, const br_vector3* v2) {

    v1->v[0] = v2->v[0];
    v1->v[1] = v2->v[1];
    v1->v[2] = v2->v[2];
}

// FUNCTION: CARMA2_HW 0x00534940
void C2_HOOK_CDECL BrVector3ScaleF(br_vector3* v1, const br_fvector3* v2, br_scalar s) {

    v1->v[0] = v2->v[0] * s;
    v1->v[1] = v2->v[1] * s;
    v1->v[2] = v2->v[2] * s;
}

// FUNCTION: CARMA2_HW 0x00534970
br_scalar C2_HOOK_CDECL BrFVector3Dot(const br_fvector3* v1, const br_vector3* v2) {

    return BR_MAC3(v1->v[0], v2->v[0], v1->v[1], v2->v[1], v1->v[2], v2->v[2]);
}

// FUNCTION: CARMA2_HW 0x005349a0
void C2_HOOK_CDECL BrFVector3Normalise(br_fvector3* v1, const br_vector3* v2) {
    br_scalar scale;

    scale = (br_scalar)sqrt(BR_MAC3(v2->v[0], v2->v[0], v2->v[1], v2->v[1], v2->v[2], v2->v[2]));
    if (scale != 0.f) {
        scale = 1.f / scale;
        v1->v[0] = v2->v[0] * scale;
        v1->v[1] = v2->v[1] * scale;
        v1->v[2] = v2->v[2] * scale;
    } else {
        v1->v[0] = 1.f;
        v1->v[1] = 0.f;
        v1->v[2] = 0.f;
    }
}

// FUNCTION: CARMA2_HW 0x00534a20
void C2_HOOK_CDECL BrFVector3NormaliseLP(br_fvector3* v1, const br_vector3* v2) {
    br_scalar scale;

    scale = 1.f / (br_scalar)sqrt(BR_MAC3(v2->v[0], v2->v[0], v2->v[1], v2->v[1], v2->v[2], v2->v[2]));
    v1->v[0] = v2->v[0] * scale;
    v1->v[1] = v2->v[1] * scale;
    v1->v[2] = v2->v[2] * scale;
}

// FUNCTION: CARMA2_HW 0x00534a80
void C2_HOOK_CDECL BrVector2Normalise(br_vector2* v1, const br_vector2* v2) {
    br_scalar scale;

    scale = (br_scalar)sqrt(BR_MAC2(v2->v[0], v2->v[0], v2->v[1], v2->v[1]));
    if (scale > 2.3841858e-7f) {
        scale = 1.f / scale;
        v1->v[0] = v2->v[0] * scale;
        v1->v[1] = v2->v[1] * scale;

    } else {
        v1->v[0] = 1.f;
        v1->v[1] = 0.f;
    }
}
