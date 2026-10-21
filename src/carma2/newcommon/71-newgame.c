#include "71-newgame.h"

#include "08-loading1.h"
#include "10-loading2.h"
#include "41-utility.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrpb.h"

#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x00763960
tNet_game_options gNet_settings[9];

// SetGameTarget

// SetOptions

// ReadNetGameChoices

// PickARandomCar

// RequestCarDetails

// FUNCTION: CARMA2_HW 0x00466370
void C2_HOOK_FASTCALL SetNetAvailability(tNet_game_options* pOptions) {
    int i;

    for (i = 0; i < gNumber_of_racers; i++) {
        switch (gOpponents[i].network_availability) {
        case eNet_avail_never:
            gCar_details[i].ownership = eCar_owner_not_allowed;
            break;
        case eNet_avail_eagle:
            if (pOptions->car_choice == eNet_car_all || pOptions->car_choice == eNet_car_eagle) {
                gCar_details[i].ownership = eCar_owner_none;
            } else {
                gCar_details[i].ownership = eCar_owner_not_allowed;
            }
            break;
        case eNet_avail_all:
            if (pOptions->car_choice == eNet_car_all || pOptions->car_choice == eNet_car_novelty) {
                gCar_details[i].ownership = eCar_owner_none;
            } else {
                gCar_details[i].ownership = eCar_owner_not_allowed;
            }
            break;
        default:
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004663e0
void C2_HOOK_FASTCALL DefaultNetSettings(void) {
    FILE* file;
    tNet_game_options* pOptions;
    tPath_name the_path;
    int i;

    PathCat(the_path, gApplication_path, "NETDEFLT.TXT");
    file = DRfopen(the_path, "rt");
    if (file == NULL) {
        return;
    }
    ReadNetworkSettings(file, &gNet_settings[0]);
    PFrewind(file);
    for (i = 1; i < (int)REC2_ASIZE(gNet_settings); i++) {
        pOptions = &gNet_settings[i];
        ReadNetworkSettings(file, pOptions);
    }
    PFfclose(file);
}

// DisposeJoinableGame

// InitGamesToJoin

// DisposeJoinList

// AddToJoinList

// FillInRaceDescription

