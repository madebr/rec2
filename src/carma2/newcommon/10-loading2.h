#ifndef GUARD_10_LOADING2_H
#define GUARD_10_LOADING2_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern const char* gPedTextTxtPath;
extern int gGroove_funk_offset;
extern const char* gNet_avail_names[4];

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

extern void C2_HOOK_FASTCALL LoadSpeedo(FILE* pF, int pIndex, tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL LoadTacho(FILE* pF, int pIndex, tCar_spec* pCar_spec);

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

extern intptr_t C2_HOOK_CDECL AllocateCarActorData(br_actor* pActor, void* pData);

extern intptr_t C2_HOOK_CDECL AllocateCarModelData(br_actor* pActor, void* pData);

// AddCarModelToList

// LinkCarModel

// LinkCarModelsToActor

// GetSimpleMaterial

// ReadShrapnelMaterials

// SetModelFlags

// FindCarShellModel

extern intptr_t C2_HOOK_CDECL LinkCarActorsToGroovidelicsCB(br_actor* pActor, void* pData);

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

extern void C2_HOOK_FASTCALL LoadRaceInfo(int pRace_index, tRace_info* pRace_info);

extern void C2_HOOK_FASTCALL LoadOpponents(void);

extern void C2_HOOK_FASTCALL AboutToLoadFirstCar(void);

// LoadOpponentsCars

extern void C2_HOOK_FASTCALL LoadMiscStrings(void);

extern void C2_HOOK_FASTCALL FillInRaceInfo(tRace_info* pThe_race);

extern void C2_HOOK_FASTCALL ReadNetworkSettings(FILE* pF, tNet_game_options* pOptions);

// PrintNetOptions

extern int C2_HOOK_FASTCALL SaveOptions(void);

extern int C2_HOOK_FASTCALL RestoreOptions(void);

extern void C2_HOOK_FASTCALL InitFunkGrooveFlags(void);

extern void C2_HOOK_FASTCALL SetDefaultTextFileName(void);

extern void C2_HOOK_FASTCALL ConfigureBloodPedTextTxtPath(void);

extern void C2_HOOK_FASTCALL ConfigureZombiePedTextTxtPath(void);

extern void C2_HOOK_FASTCALL ConfigureAlienPedTextTxtPath(void);

#endif // GUARD_10_LOADING2_H
