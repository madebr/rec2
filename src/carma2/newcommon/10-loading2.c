#include "10-loading2.h"

// GLOBAL: CARMA2_HW 0x0068c718
const char* gPedTextTxtPath;

// LoadInRegisteeDir

// STUB: CARMA2_HW 0x00486e10
void C2_HOOK_FASTCALL LoadInRegistees(void) {
    NOT_IMPLEMENTED();
}

// LoadBunchOParameters

// LoadBunchOFloatParameters

// GetHithers

// STUB: CARMA2_HW 0x00486ef0
void C2_HOOK_FASTCALL LoadGeneralParameters(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00487dc0
void C2_HOOK_FASTCALL FinishLoadingGeneral(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00487e10
void C2_HOOK_FASTCALL LoadKeyMapping(void) {
    NOT_IMPLEMENTED();
}

// LoadInterfaceStuff

// FUNCTION: CARMA2_HW 0x00487eb0
void C2_HOOK_FASTCALL InitInterfaceLoadState(void) {

}

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

// STUB: CARMA2_HW 0x0048c150
void C2_HOOK_FASTCALL LoadHeadupImages(void) {
    NOT_IMPLEMENTED();
}

// OpenRaceFile

// STUB: CARMA2_HW 0x0048c1c0
void C2_HOOK_FASTCALL LoadRaces(tRace_list_spec* pRace_list, int* pCount, int pRace_type_index) {
    NOT_IMPLEMENTED();
}

// LoadRaceInfo

// LoadOpponents

// AboutToLoadFirstCar

// LoadOpponentsCars

// STUB: CARMA2_HW 0x0048cfc0
void C2_HOOK_FASTCALL LoadMiscStrings(void) {
    NOT_IMPLEMENTED();
}

// FillInRaceInfo

// ReadNetworkSettings

// PrintNetOptions

// STUB: CARMA2_HW 0x0048d190
int C2_HOOK_FASTCALL SaveOptions(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0048d8f0
int C2_HOOK_FASTCALL RestoreOptions(void) {
    NOT_IMPLEMENTED();
}

// InitFunkGrooveFlags

// FUNCTION: CARMA2_HW 0x0048ea00
void C2_HOOK_FASTCALL SetDefaultTextFileName(void) {
    gPedTextTxtPath = "TEXT.TXT";
}

