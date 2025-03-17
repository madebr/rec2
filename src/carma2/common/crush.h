#ifndef REC2_CRUSH_H
#define REC2_CRUSH_H

#include "c2_hooks.h"

#include "c2_stdio.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tU8, gCrush_data_entry_counter);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gSoftness_names, 6);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gCrush_type_names, 3);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gEase_of_detachment_names, 5);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gCar_crush_shape_names, 2);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gDetach_type_names, 4);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gPosition_type_names, 2);
C2_HOOK_VARIABLE_DECLARE_ARRAY(undefined4, gCrush_array_0067a190, 10);
C2_HOOK_VARIABLE_DECLARE(undefined4, gDAT_00679440);
C2_HOOK_VARIABLE_DECLARE(tCrush_info_buffer, gDetached_bit_crush_info_buffer);
C2_HOOK_VARIABLE_DECLARE(tCollision_info*, gDetached_bit_collision_infos);
C2_HOOK_VARIABLE_DECLARE(tDriver, gDetached_bit_driver);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tPhysics_joint*, gTrack_crush_joints, 32);
C2_HOOK_VARIABLE_DECLARE(tCrush_info_buffer, gSplit_car_crush_info_buffer);
C2_HOOK_VARIABLE_DECLARE(tCollision_info*, gSplit_car_collision_infos);
C2_HOOK_VARIABLE_DECLARE(tDriver, gSplit_car_driver);
C2_HOOK_VARIABLE_DECLARE(tCollision_shape_sphere*, gGonad_sphere_collision_shape);

void C2_HOOK_FASTCALL InitCrushSystems(void);

void C2_HOOK_FASTCALL ClearCrushLists(void);

void C2_HOOK_FASTCALL ResetCrushSystems(void);

void C2_HOOK_FASTCALL ReadCrushSettings(FILE* file);

br_scalar C2_HOOK_FASTCALL SquaredDistanceFromLineSegment(br_vector3* pP, br_vector3* pA, br_vector3* pB);

void C2_HOOK_FASTCALL LoadMinMax(FILE* pF, br_bounds3* pBounds);

void C2_HOOK_FASTCALL LoadNoncarActivation(FILE* pF, tNoncar_activation** pNoncar_activations, int* pCount_noncar_activations);

void C2_HOOK_FASTCALL ReadAward(FILE* pF, tAward_info* pAward_info);

void C2_HOOK_FASTCALL LoadSmashableLevels(FILE* pF, tSmashable_level** pSmashable_levels, int* pCount_smashable_levels, int pIs_texture_change, tBrender_storage* pBrender_storage);

void C2_HOOK_FASTCALL LoadCarCrushSmashDataEntries(FILE* pF, tCar_crush_buffer_entry* pCar_crush_buffer_entry, tBrender_storage* pBrender_storage);

void C2_HOOK_FASTCALL LoadCarCrushDataEntry(FILE* pF, tCar_crush_buffer_entry* pCar_crush_buffer_entry,tBrender_storage *pBrender_storage);

int C2_HOOK_FASTCALL LoadCarCrush(tCar_crush_buffer* pCar_crush_buffer, const char* pPath, tBrender_storage* pBrender_storage, tCar_crush_spec** pCar_crush_spec);

void C2_HOOK_FASTCALL LinkCarCrushDatas(br_actor *pActor, tCar_crush_buffer *pBuffer);

int C2_HOOK_CDECL LinkCrushData(br_actor* pActor, void* pData);

int C2_HOOK_CDECL LinkCarCrushData(br_actor* pActor, void* data);

int C2_HOOK_CDECL AllocateUserDetailLevel(br_actor* pActor, void* pData);

int C2_HOOK_CDECL LinkCrushModel(br_actor* pActor, void* pData);

intptr_t C2_HOOK_CDECL AccumulateSquashVertices(br_actor* actor, void* pData);

int C2_HOOK_CDECL IncreasingCompare(const void* pValue1, const void* pValue2);

int C2_HOOK_CDECL DecreasingCompare(const void* pValue1, const void* pValue2);

tU16 C2_HOOK_FASTCALL CrushLimitNumber(const br_vector3* pPoint, const tCar_crush_limits* pLimits, const tCar_crush_count_limits* pCount_limits, int* pInvalid);

void C2_HOOK_FASTCALL InitModelMasterCrushData(tCar_spec* pCar_spec);

tU16 C2_HOOK_FASTCALL FindNearestVertex(br_vector3* pPoint, const br_model* pModel);

tU16 C2_HOOK_FASTCALL FindNearestParentVertex(br_actor* pActor, br_vector3* pPos);

void C2_HOOK_FASTCALL SetUpSemiDetachJointStuff(tCar_crush_detach_data* pDetach_data, const br_model* pModel, const br_bounds3* pBounds);

intptr_t C2_HOOK_CDECL InitPhysCrushDataCB(br_actor* actor, void* data);

void C2_HOOK_FASTCALL InitPhysCrushData(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL CalculateReferencePoints(br_model* pModel, br_model* pParent_model, tCar_crush_detach_data* pDetach_data, br_actor* pActor);

intptr_t C2_HOOK_FASTCALL SoftnessOfNearestPointCB(br_actor* pActor, br_vector3* pPoint, void* pData);

intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseWithTranslation(br_actor* pActor, br_vector3* pDelta, tDRActorEnumRecurseWithTranslation_cbfn* cbfn, void* pContext);

float C2_HOOK_FASTCALL SoftnessOfNearestPoint(tCar_spec* pCar_spec, br_vector3* pPoint);

void C2_HOOK_FASTCALL SetUpShapeLimitingStuff(tCar_crush_spec* pCar_crush, tCar_spec* pCar_spec);

intptr_t C2_HOOK_CDECL InitModelCrushDataCB(br_actor* actor, void* data);

void C2_HOOK_FASTCALL InitModelCrushData(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL CopyShapePolyhedron(tCollision_shape_polyhedron* pDest, const tCollision_shape_polyhedron* pSrc);

void C2_HOOK_FASTCALL InitNetworkShapesStuff(tCar_crush_spec* pCar_crush);

void C2_HOOK_FASTCALL InitShapeStuff(tCar_crush_spec* pCar_crush, tCollision_info* pCollision_info, tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL InitPhysMasterCrushData(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL SetFlapCheckVertices(tCar_crush_flap_data *pFlap_data, br_model* pModel, tModel_detail_vertex_data* pVertex_data);

intptr_t C2_HOOK_CDECL InitVertexDataCB(br_actor* pActor, void* data);

void C2_HOOK_FASTCALL InitVertexData(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL CheckWheelPositions(const tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL InitPhysModCrushData(tCar_spec * pCar_spec);

void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL TotallyRepairCar(void);

void C2_HOOK_FASTCALL WeldCar(tCar_spec* pCar_spec);

void C2_HOOK_FAKE_THISCALL TotallySpamTheModel(tCar_spec* pCar_spec, undefined4 pArg2, float pDamage);

void C2_HOOK_FASTCALL DoDamage(tCar_spec *pCar, tDamage_type pDamage_type, int pMagnitude);

int C2_HOOK_FASTCALL DoCrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2);

void C2_HOOK_FASTCALL SortOutSmoke(tCar_spec* pCar);

void C2_HOOK_FASTCALL CrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2);

void C2_HOOK_FASTCALL DoWheelDamage(tU32 pFrame_period);

void C2_HOOK_FASTCALL CrushBendFlapRend(void);

void C2_HOOK_FASTCALL LinkSmashies(br_actor* pActor, tCar_crush_buffer_entry* pCrush_data, tModel_detail_vertex_data* pVertex_data);

#endif // REC2_CRUSH_H
