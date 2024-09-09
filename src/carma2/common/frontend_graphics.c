#include "frontend_credits.h"

#include "brucetrk.h"
#include "controls.h"
#include "depth.h"
#include "frontend.h"
#include "frontend_quit.h"
#include "globvars.h"
#include "graphics.h"
#include "loading.h"
#include "world.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_GRAPHICS, 0x0063e528, {
    FIXME TODO
});


void C2_HOOK_FASTCALL GetGraphicsSettingsData(tFrontend_spec* pFrontend) {
    float yon_factor;

    switch (GetCarSimplificationLevel()) {
    case 0:
        SelectThisItemIn(pFrontend, 1, 3);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 1, 4);
        break;
    case 2:
        SelectThisItemIn(pFrontend, 1, 5);
        break;
    case 3:
        SelectThisItemIn(pFrontend, 1, 6);
        break;
    }

    switch ((int)((35.f - GetYon()) / 5.f)) {
    case 0:
        SelectThisItemIn(pFrontend, 2, 8);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 2, 9);
        break;
    case 2:
        SelectThisItemIn(pFrontend, 2, 10);
        break;
    default:
        SelectThisItemIn(pFrontend, 2, 11);
        break;
    }

    yon_factor = GetYonFactor();
    if (yon_factor >= 1.f) {
        SelectThisItemIn(pFrontend, 3, 13);
    } else if (yon_factor >= 0.5f) {
        SelectThisItemIn(pFrontend, 3, 14);
    } else if (yon_factor >= 0.25f) {
        SelectThisItemIn(pFrontend, 3, 15);
    } else {
        SelectThisItemIn(pFrontend, 3, 16);
    }

    switch (GetShadowLevel()) {
    default:
        SelectThisItemIn(pFrontend, 4, 18);
        break;
    case 0:
        SelectThisItemIn(pFrontend, 4, 20);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 4, 19);
        break;
    }

    switch (GetSoundDetailLevel()) {
    case 0:
        SelectThisItemIn(pFrontend, 5, 24);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 5, 23);
        break;
    case 2:
        SelectThisItemIn(pFrontend, 5, 22);
        break;
    }

    switch (GetHowMuchBloodAndSnotToSmearAbout()) {
    case 0:
        SelectThisItemIn(pFrontend, 6, 26);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 6, 27);
        break;
    case 2:
        SelectThisItemIn(pFrontend, 6, 28);
        break;
    }

    if (GetSkyTextureOn()) {
        SelectThisItemIn(pFrontend, 7, 30);
    } else {
        SelectThisItemIn(pFrontend, 7, 31);
    }

    if (GetDepthCueingOn()) {
        SelectThisItemIn(pFrontend, 8, 33);
    } else {
        SelectThisItemIn(pFrontend, 8, 34);
    }

    if (GetDronesOn()) {
        SelectThisItemIn(pFrontend, 9, 36);
    } else {
        SelectThisItemIn(pFrontend, 9, 37);
    }

    if (GetAnimalsOn()) {
        SelectThisItemIn(pFrontend, 10, 39);
    } else {
        SelectThisItemIn(pFrontend, 10, 40);
    }

    if (IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense()) {
        SelectThisItemIn(pFrontend, 11, 42);
    } else {
        SelectThisItemIn(pFrontend, 11, 43);
    }

    if (IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells()) {
        SelectThisItemIn(pFrontend, 12, 45);
    } else {
        SelectThisItemIn(pFrontend, 12, 46);
    }

    if (C2V(gAuto_load)) {
        SelectThisItemIn(pFrontend, 13, 48);
    } else {
        SelectThisItemIn(pFrontend, 13, 49);
    }
    if (C2V(gAmbient_sound)) {
        SelectThisItemIn(pFrontend, 14, 51);
    } else {
        SelectThisItemIn(pFrontend, 14, 52);
    }
}
C2_HOOK_FUNCTION(0x004b4090, GetGraphicsSettingsData)

int C2_HOOK_FASTCALL Graphics_Infunc(tFrontend_spec* pFrontend) {

    Generic_Infunc(pFrontend);
    GetGraphicsSettingsData(pFrontend);
    return 1;
}
C2_HOOK_FUNCTION(0x004747e0, Graphics_Infunc)

void C2_HOOK_FASTCALL SetGraphicsSettingsData(tFrontend_spec* pFrontend) {

    switch (WhichItemIsSelectedIn(pFrontend, 1)) {
    case 3:
        SetCarSimplificationLevel(0);
        break;
    case 4:
        SetCarSimplificationLevel(1);
        break;
    case 5:
        SetCarSimplificationLevel(2);
        break;
    case 6:
        SetCarSimplificationLevel(3);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 2)) {
    case 8:
        SetYon(35.f);
        break;
    case 9:
        SetYon(30.f);
        break;
    case 10:
        SetYon(25.f);
        break;
    case 11:
        SetYon(20.f);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 3)) {
    case 13:
        SetYonFactor(1.0f);
        break;
    case 14:
        SetYonFactor(0.5f);
        break;
    case 15:
        SetYonFactor(0.25f);
        break;
    case 16:
        SetYonFactor(0.125f);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 4)) {
    case 18:
        SetShadowLevel(3);
        break;
    case 19:
        SetShadowLevel(1);
        break;
    case 20:
        SetShadowLevel(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 5)) {
    case 22:
        SetSoundDetailLevel(2);
        break;
    case 23:
        SetSoundDetailLevel(1);
        break;
    case 24:
        SetSoundDetailLevel(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 6)) {
    case 26:
        SetGoreLevel(0);
        break;
    case 27:
        SetGoreLevel(1);
        break;
    case 28:
        SetGoreLevel(2);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 7)) {
    case 30:
        SetSkyTextureOn(1);
        break;
    case 31:
        SetSkyTextureOn(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 8)) {
    case 33:
        SetDepthCueingOn(1);
        break;
    case 34:
        SetDepthCueingOn(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 9)) {
    case 36:
        SetDronesOn(1);
        break;
    case 37:
        SetDronesOn(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 10)) {
    case 39:
        SetAnimalsOn(1);
        break;
    case 40:
        SetAnimalsOn(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 11)) {
    case 42:
        SetFlameThrowerOn(1);
        break;
    case 43:
        SetFlameThrowerOn(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 12)) {
    case 45:
        SetMinesOn(1);
        break;
    case 46:
        SetMinesOn(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 13)) {
    case 48:
        C2V(gAuto_load) = 1;
        break;
    case 49:
        C2V(gAuto_load) = 0;
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 14)) {
    case 51:
        C2V(gAmbient_sound) = 1;
        break;
    case 52:
        C2V(gAmbient_sound) = 0;
        break;
    }

    SaveOptions();
}
C2_HOOK_FUNCTION(0x004b42c0, SetGraphicsSettingsData)
