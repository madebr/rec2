#include "12-menuoptions.h"

#include "16-graphics1.h"
#include "17-world2.h"
#include "33-depth.h"
#include "35-intrface.h"
#include "37-brucetrk.h"
#include "57-drone.h"
#include "61-pedestrn.h"
#include "69-sound.h"
#include "globvars.h"

// SaveOrigKeyMapping

// SaveKeyMapping

// ChangeKeyMapIndex

// FUNCTION: CARMA2_HW 0x004b4090
// FUNCTION: CARMA2_SW 0x004b0200
void C2_HOOK_FASTCALL GetGraphicsSettingsData(tFrontend_spec* pFrontend) {
    float yon_factor;
    float yon;

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

    yon = GetYon();
    switch ((int)((35.f - yon) / 5.f)) {
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
    case eShadow_none:
        SelectThisItemIn(pFrontend, 4, 20);
        break;
    case eShadow_us_only:
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

    if (OtherPerambulatoryInstasificationIsEnablificated()) {
        SelectThisItemIn(pFrontend, 9, 36);
    } else {
        SelectThisItemIn(pFrontend, 9, 37);
    }

    if (AreWeGoingToOutrageAnimalLoversAroundTheGlobe()) {
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

    if (gAuto_load) {
        SelectThisItemIn(pFrontend, 13, 48);
    } else {
        SelectThisItemIn(pFrontend, 13, 49);
    }
    if (gAmbient_sound) {
        SelectThisItemIn(pFrontend, 14, 51);
    } else {
        SelectThisItemIn(pFrontend, 14, 52);
    }
}

// SetGraphicsSettingsData

