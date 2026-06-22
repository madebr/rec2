#include "04-netgame.h"

#include "01-network.h"
#include "63-loading3.h"
#include "globvars.h"

// GLOBAL: CARMA2_HW 0x0075b8f8
br_pixelmap* gDigits_pix;

// SendShapeNumbers

// ClearShapeStatusFlag

// SendNonCar

// HostFillInCarContents

// ClientSendCarData

// DistanceBetweenCars

// CarShapeNeedsSendingToPlayer

// AddShapeStuffToMechanicsMessage

// TimeToSendData

// SendCarData

// ReceivedRecover

// CopyMechanics

// ReceivedShapeNumbers

// FillInSimpleCarPos

// ReceivedSimpleCarPos

// ReceivedMechanics

// ReceivedCopInfo

// SendAllNonCarPositions

// ReceivedNonCarPosition

// ReceivedNonCar

// SignalToStartRace

// SignalToRepositionCar

// SetUpNetCarPositions

// ReinitialiseCar

// RepositionPlayer

// DisableCar

// EnableCar

// DoNetworkHeadups

// SortNetHeadAscending

// SortNetHeadDescending

// ClipName

// CreateBoxes

// DrawScoreBoxes

// DoNetScores2

// DoNetScores

// FUNCTION: CARMA2_HW 0x0049a710
void C2_HOOK_FASTCALL InitNetHeadups(void) {

    gDigits_pix = LoadPixelmap("HDIGITS.PIX");
    if (gDigits_pix != NULL) {
        BrMapAdd(gDigits_pix);
    }
}

// FUNCTION: CARMA2_HW 0x0049a730
void C2_HOOK_FASTCALL DisposeNetHeadups(void) {

    if (gDigits_pix != NULL) {
        BrMapRemove(gDigits_pix);
        BrPixelmapFree(gDigits_pix);
    }
}

// EverybodysLost

// DeclareWinner

// PlayerIsIt

// CheckForVampireWinner

// PlayerIsInfected

// FarEnoughAway

// CarInContactWithFox

// SelectRandomFox

// CalcPlayerScores

// SendPlayerScores

// DoNetGameManagement

// InitialisePlayerScore

// InitPlayers

// UseGeneralScore

// NetSendEnvironmentChanges

// STUB: CARMA2_HW 0x0049b9a0
void C2_HOOK_FASTCALL CheckForNeedyEnvironmentRecipients(void) {
    NOT_IMPLEMENTED();
}

// ReceivedGameplay

// SendGameplay

// SendGameplayToAllPlayers

// SendGameplayToHost

// InitNetGameplayStuff

// FUNCTION: CARMA2_HW 0x0049bd10
void C2_HOOK_FASTCALL DefaultNetName(void) {

    NetObtainSystemUserName(gProgram_state.player_name, 32);
}

// AddPlayerToShapeStatusLists

// RemovePlayerFromShapeStatusLists

