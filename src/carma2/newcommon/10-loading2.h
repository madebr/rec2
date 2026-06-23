#ifndef GUARD_10_LOADING2_H
#define GUARD_10_LOADING2_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern void C2_HOOK_FASTCALL LoadInRegisteeDir(const char *pRoot, const char *pSubDir, int pInitialize_palettes);

extern void C2_HOOK_FASTCALL LoadInRegistees(void);

extern void C2_HOOK_FASTCALL LoadBunchOParameters(tSlot_info* pSlot_info);

extern void C2_HOOK_FASTCALL LoadBunchOFloatParameters(tFloat_bunch_info *pBunch);

extern void C2_HOOK_FASTCALL GetHithers(void);

extern void C2_HOOK_FASTCALL LoadGeneralParameters(void);

extern void C2_HOOK_FASTCALL FinishLoadingGeneral(void);

extern void C2_HOOK_FASTCALL LoadKeyMapping(void);

extern void C2_HOOK_FASTCALL LoadInterfaceStuff(int pWithin_race);

extern void C2_HOOK_FASTCALL InitInterfaceLoadState(void);

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

extern void C2_HOOK_FASTCALL LoadHeadupImages(void);

// OpenRaceFile

extern void C2_HOOK_FASTCALL LoadRaces(tRace_list_spec* pRace_list, int* pCount, int pRace_type_index);

// LoadRaceInfo

// LoadOpponents

// AboutToLoadFirstCar

// LoadOpponentsCars

extern void C2_HOOK_FASTCALL LoadMiscStrings(void);

// FillInRaceInfo

extern void C2_HOOK_FASTCALL ReadNetworkSettings(FILE* pF, tNet_game_options* pOptions);

// PrintNetOptions

extern int C2_HOOK_FASTCALL SaveOptions(void);

extern int C2_HOOK_FASTCALL RestoreOptions(void);

// InitFunkGrooveFlags

extern void C2_HOOK_FASTCALL SetDefaultTextFileName(void);

#endif // GUARD_10_LOADING2_H
