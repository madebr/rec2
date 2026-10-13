#ifndef GUARD_10_LOADING2_H
#define GUARD_10_LOADING2_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tU32 gSimple_material_colours[12];
extern br_material* gSimple_materials[32];

extern tHeadup_info gHeadup_image_info[45];
extern br_pixelmap* gHeadup_images[45];
extern const char* gDamage_names[12];
extern char* gRaces_file_names[9];
extern const char* gNet_avail_names[4];
extern int gCurrent_race_file_index;
extern int gCountRaceGroups;

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

extern void C2_HOOK_FASTCALL AddRefOffset(int* pRef_holder);

extern void C2_HOOK_FASTCALL GetDamageProgram(FILE* pF, tCar_spec* pCar_spec, tImpact_location pImpact_location);

// AllocateCarActorData

// AllocateCarModelData

// AddCarModelToList

// LinkCarModel

// LinkCarModelsToActor

extern br_material* C2_HOOK_FASTCALL GetSimpleMaterial(char* pName, tRendererShadingType pShading_type);

extern void C2_HOOK_FASTCALL ReadShrapnelMaterials(FILE* pF, tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL SetModelFlags(br_model* pModel, int pOwner);

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

extern FILE* C2_HOOK_FASTCALL OpenRaceFile(void);

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


extern int gGroove_funk_offset;
extern const char* gPedTextTxtPath;

void C2_HOOK_FASTCALL ConfigureZombiePedTextTxtPath(void);
void C2_HOOK_FASTCALL ConfigureBloodPedTextTxtPath(void);
void C2_HOOK_FASTCALL ConfigureAlienPedTextTxtPath(void);

#endif // GUARD_10_LOADING2_H
