#include "10-loading2.h"

// GLOBAL: CARMA2_HW 0x0068c718
const char* gPedTextTxtPath;

// LoadInRegisteeDir

// LoadInRegistees

// LoadBunchOParameters

// LoadBunchOFloatParameters

// GetHithers

// LoadGeneralParameters

// FinishLoadingGeneral

// LoadKeyMapping

// LoadInterfaceStuff

// InitInterfaceLoadState

// AdjustCarCoordinates

// LoadSpeedo

// LoadTacho

// LoadHeadups

// OldReadNonCarMechanicsData

// GetPerpendicular

// ReadNonCarMechanicsData

// ReadPartHeader

// ReadMechanicsShapes

// NewReadMechanicsData

// LoadGear

// AddRefOffset

// GetDamageProgram

// AllocateCarActorData

// AllocateCarModelData

// AddCarModelToList

// LinkCarModel

// LinkCarModelsToActor

// GetSimpleMaterial

// ReadShrapnelMaterials

// SetModelFlags

// FindCarShellModel

// LinkCarActorsToGroovidelicsCB

// LinkCarActorsToGroovidelics

// ReadCarKeywordData

// TrashModelMaterialIdentifiers

// CentreModel

// PivotizeWheel

// SetUpGraphicalWheelStuff

// LoadCar

// LoadHeadupImages

// OpenRaceFile

// LoadRaces

// LoadRaceInfo

// LoadOpponents

// AboutToLoadFirstCar

// LoadOpponentsCars

// LoadMiscStrings

// FillInRaceInfo

// ReadNetworkSettings

// PrintNetOptions

// STUB: CARMA2_HW 0x0048d190
int C2_HOOK_FASTCALL SaveOptions(void) {
    NOT_IMPLEMENTED();
}

// RestoreOptions

// InitFunkGrooveFlags

// FUNCTION: CARMA2_HW 0x0048ea00
void C2_HOOK_FASTCALL SetDefaultTextFileName(void) {
    gPedTextTxtPath = "TEXT.TXT";
}

