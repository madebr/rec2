#include "crush.h"

#include "errors.h"
#include "loading.h"
#include "utility.h"

#include "platform.h"

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

void C2_HOOK_FASTCALL LoadGeneralCrushSettings(tTWTFILE* file) {
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
    C2V(gDistortion_factor) = GetAScalar(file);
    C2V(gMin_crush_force) = GetAScalar(file);
    C2V(gMax_crush_force) = GetAScalar(file);
    C2V(gForce_to_movement_factor) = GetAScalar(file);
    C2V(gMax_crush_dist_sq) = GetAScalar(file);
    C2V(gMin_force_to_split_XZ_per_tonne) = GetAScalar(file);
    C2V(gMin_force_to_split_Y_per_tonne) = GetAScalar(file);
    C2V(gMin_split_damage) = GetAnInt(file);
    C2V(gMax_split_damage) = GetAnInt(file);
    C2V(gChance_of_inverse_buckle) = GetAScalar(file);
    C2V(gFlap_inertia_fudge_biscuit) = GetAScalar(file);
    C2V(gBatty_gravity) = GetAScalar(file);
    C2V(gTorque_to_snap_per_tonne) = GetAScalar(file);
    C2V(gMax_detach_time_ms) = GetAScalar(file);
    C2V(gNormal_force_to_detach) = GetAScalar(file);
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
