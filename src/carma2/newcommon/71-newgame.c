#include "71-newgame.h"

#include "01-network.h"
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

// FUNCTION: CARMA2_HW 0x004662c0
int C2_HOOK_FASTCALL PickARandomCar(void) {
    int i;
    int cars[2 * REC2_ASIZE(gCar_details)];
    int array_size;

    C2_HOOK_BUG_ON(REC2_ASIZE(cars) != 120);

    array_size = 0;
    for (i = 0; i < gNumber_of_racers; i++) {
        if (gCar_details[i].ownership == eCar_owner_none) {
            cars[array_size] = i;
            array_size++;
            if (gOpponents[i].network_availability == eNet_avail_all) {
                cars[array_size] = i;
                array_size++;
            }
        }
    }
    return cars[IRandomBetween(0, array_size - 1)];
}

// RequestCarDetails

// FUNCTION: CARMA2_HW 0x00466330
// FUNCTION: CARMA2_SW 0x004660c0
void C2_HOOK_FASTCALL PollCarDetails(tNet_game_details* pNet_game) {
    tNet_message* message;

    if (gReceived_car_details) {
        gReceived_car_details = 0;
        if (gNet_mode == eNet_mode_none) {
            gNet_mode = eNet_mode_thinking_about_it;
        }
        message = NetBuildGuaranteedMessage(6, 0);
        NetGuaranteedSendMessageToHost(pNet_game, message, NULL);
    }
}

// FUNCTION: CARMA2_HW 0x00466370
// FUNCTION: CARMA2_SW 0x00466100
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
// FUNCTION: CARMA2_SW 0x00466170
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

