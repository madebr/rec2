#include "crush.h"

#include "errors.h"
#include "loading.h"
#include "platform.h"
#include "utility.h"

#include <brender/brender.h>

#include "rec2_macros.h"

#include <string.h>

C2_HOOK_VARIABLE_IMPLEMENT(float, gDistortion_factor,0x00679698 );
C2_HOOK_VARIABLE_IMPLEMENT(float, gMin_crush_force, 0x006796b8);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMax_crush_force, 0x0067bdec);
C2_HOOK_VARIABLE_IMPLEMENT(float, gForce_to_movement_factor, 0x006796b0);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMax_crush_dist_sq, 0x0067bacc);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMin_force_to_split_XZ_per_tonne, 0x0067bac8);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMin_force_to_split_Y_per_tonne, 0x0067bd5c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMin_split_damage, 0x0067bd64);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMax_split_damage, 0x0067b7d0);
C2_HOOK_VARIABLE_IMPLEMENT(float, gChance_of_inverse_buckle, 0x0067be04);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFlap_inertia_fudge_biscuit, 0x0067b7b4);
C2_HOOK_VARIABLE_IMPLEMENT(float, gBatty_gravity, 0x0067be7c);
C2_HOOK_VARIABLE_IMPLEMENT(float, gTorque_to_snap_per_tonne, 0x0067bdf8);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMax_detach_time_ms, 0x00679550);
C2_HOOK_VARIABLE_IMPLEMENT(float, gNormal_force_to_detach, 0x0067944c);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMin_bend_force, 0x0067bde8);
C2_HOOK_VARIABLE_IMPLEMENT(float, gChance_of_bending, 0x0067be00);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMin_bend_angle, 0x0067a18c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMin_bend_damage, 0x0067b7b0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMax_bend_damage, 0x0067bdf4);

void (C2_HOOK_FASTCALL * InitCrush_original)(void);
void C2_HOOK_FASTCALL InitCrush(void) {
#if defined(C2_HOOKS_ENABLED)
    InitCrush_original();
#else
#error "Not implemented"
#endif

}
C2_HOOK_FUNCTION_ORIGINAL(0x00429fa0, InitCrush, InitCrush_original)

void C2_HOOK_FASTCALL LoadGeneralCrushSettings(FILE* file) {
    char s[256];

    while (1) {
        if (!GetALineAndDontArgue(file, s)) {
            PDFatalError("Can't find start of CRUSH SETTINGS in .TXT file");
        }
        if (strcmp(s, "START OF CRUSH SETTINGS") == 0) {
            break;
        }
    }
    if (GetAnInt(file) != 4) {
        PDFatalError("Wrong version of CRUSH SETTINGS");
    }
    /* CRUSHING */
    C2V(gDistortion_factor) = GetAScalar(file);
    C2V(gMin_crush_force) = GetAScalar(file);
    C2V(gMax_crush_force) = GetAScalar(file);
    C2V(gForce_to_movement_factor) = GetAScalar(file);
    C2V(gMax_crush_dist_sq) = GetAScalar(file);

    /* SPLITTING */
    C2V(gMin_force_to_split_XZ_per_tonne) = GetAScalar(file);
    C2V(gMin_force_to_split_Y_per_tonne) = GetAScalar(file);
    C2V(gMin_split_damage) = GetAnInt(file);
    C2V(gMax_split_damage) = GetAnInt(file);

    /* BUCKLING */
    C2V(gChance_of_inverse_buckle) = GetAScalar(file);

    /* FLAPPING AND JOINT SNAPPING */
    C2V(gFlap_inertia_fudge_biscuit) = GetAScalar(file);
    C2V(gBatty_gravity) = GetAScalar(file);
    C2V(gTorque_to_snap_per_tonne) = GetAScalar(file);

    /* DETACHING */
    C2V(gMax_detach_time_ms) = GetAScalar(file);
    C2V(gNormal_force_to_detach) = GetAScalar(file);

    /* BENDING */
    C2V(gMin_bend_force) = GetAScalar(file);
    C2V(gChance_of_bending) = GetAScalar(file);
    C2V(gMin_bend_angle) = GetAScalar(file);
    C2V(gMin_bend_damage) = GetAnInt(file);
    C2V(gMax_bend_damage) = GetAnInt(file);
    while (1) {
        if (!GetALineAndDontArgue(file, s)) {
            PDFatalError("Can't find end of CRUSH SETTINGS in .TXT file");
        }
        if (strcmp(s, "END OF CRUSH SETTINGS") == 0) {
            break;
        }
    }
}
C2_HOOK_FUNCTION(0x00429bb0, LoadGeneralCrushSettings)

br_scalar C2_HOOK_FASTCALL SquaredDistanceFromLineSegment(br_vector3* pP, br_vector3* pA, br_vector3* pB) {
    br_vector3 v1;
    br_vector3 v2;
    br_scalar f;

    BrVector3Sub(&v1, pB, pA);
    BrVector3Sub(&v2, pP, pA);
    /* FIXME: numerator and denominator are mixed up?! */
    f = BrVector3Dot(&v1, &v1) / BrVector3Dot(&v2, &v1);
    if (f < 0.f) {
        f = 0.f;
    } else if (f > 1.f) {
        f = 1.f;
    }
    BrVector3Scale(&v1, &v1, f);
    BrVector3Sub(&v2, &v2, &v1);
    return BrVector3Dot(&v2, &v2);
}
C2_HOOK_FUNCTION(0x0042c300, SquaredDistanceFromLineSegment)

void C2_HOOK_FASTCALL LoadMinMax(FILE* pF, br_bounds3* pBounds) {
    float x1, x2;
    float y1, y2;
    float z1, z2;

    GetThreeFloats(pF, &x1, &y1, &z1);
    GetThreeFloats(pF, &x2, &y2, &z2);
    pBounds->min.v[0] = MIN(x1, x2);
    pBounds->min.v[1] = MIN(y1, y2);
    pBounds->min.v[2] = MIN(z1, z2);
    pBounds->max.v[0] = MAX(x1, x2);
    pBounds->max.v[1] = MAX(y1, y2);
    pBounds->max.v[2] = MAX(z1, z2);
}
C2_HOOK_FUNCTION(0x004ef460, LoadMinMax)

void (C2_HOOK_FASTCALL * PrepareCarForCrushing_original)(tCar_spec* pCar_spec);
void C2_HOOK_FASTCALL PrepareCarForCrushing(tCar_spec* pCar_spec) {

#if defined(C2_HOOKS_ENABLED)
    PrepareCarForCrushing_original(pCar_spec);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0042aa20, PrepareCarForCrushing, PrepareCarForCrushing_original)
