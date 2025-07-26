#ifndef REC2_PEDESTRN_H
#define REC2_PEDESTRN_H

#include "c2_stdio.h"

#include "c2_hooks.h"

#include <brender/brender.h>
#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(int, gPed_count);
C2_HOOK_VARIABLE_DECLARE(int, gPed_nearness);
C2_HOOK_VARIABLE_DECLARE(int, gCount_killed_peds);
C2_HOOK_VARIABLE_DECLARE(int, gTotal_count_smash_peds);

void C2_HOOK_FASTCALL ClearOutMorphs(void);

void C2_HOOK_FASTCALL InitBoner(tPedForms_vtable* pTable);

void C2_HOOK_FASTCALL DRVector3SafeCross(br_vector3* pDest, const br_vector3* pA, const br_vector3* pB);

void C2_HOOK_FAKE_THISCALL ScaleModelXYZ(br_model* pModel, int pArg2, float pX, float pY, float pZ);

void C2_HOOK_FAKE_THISCALL ScaleModel(br_model* pModel, int pArg2, float pScale);

void C2_HOOK_FASTCALL ReadPedGiblets(FILE* pFile);

void C2_HOOK_FASTCALL ReadSettingsFile(void);

void C2_HOOK_FASTCALL InitFaceCaches(void);

void C2_HOOK_FASTCALL InitOtherPedStuff(void);

void C2_HOOK_FASTCALL InitPolyPedSystem(void);

void C2_HOOK_FASTCALL InitNapalmNolts(void);

int C2_HOOK_FASTCALL DoToPeds(tCar_spec* pCar_spec, int pOnly_alive, float pMax_distance, int pParam_4, void* pData, tForEachPedestrian_cbfn* pCallback);

void C2_HOOK_FASTCALL ReadPedSpecs(FILE* pF);

tPed_character_instance* C2_HOOK_FASTCALL BuildCharacterInstance(const char* pGroup_name, br_matrix34* pMat34);

br_matrix34* C2_HOOK_FASTCALL GetCharacterMatrixPtr(tPed_character_instance *pCharacter);

void C2_HOOK_CDECL TurnLimbsOnAndOff(br_actor* actor, br_model* model, br_material* material, void* render_data, br_uint_8 style, int on_screen);

int C2_HOOK_FASTCALL MorphCharacterBonePositions(tPed_character_instance* pPed, tU32 pArg2);

void C2_HOOK_FASTCALL SetBoner(tPed_character_instance* pPed, br_matrix34* pMat, br_matrix34* pParent_mat, br_matrix34* pArg4, tU8 pAngle_x, tU8 pAngle_y, tU8 pAngle_z, br_vector3* pArg8, br_vector3* pParg9);

void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, undefined4 pArg2, undefined4 pArg3);

void C2_HOOK_FASTCALL ResetAnimation(tPed_character_instance* pPed);

void C2_HOOK_FASTCALL SetOnGroundVector(tPed_character_instance* pPed, br_vector3* pPos, br_vector3* pNew_pos);

void C2_HOOK_FASTCALL RecalculateOrientationOfRoot(tPed_character_instance* pPed);

int C2_HOOK_FASTCALL SetCharacterMove(tPed_character_instance* pPed,int pMove_action, float pSpeed, int pArg4, int pArg5, undefined4 pArg6);

void C2_HOOK_FASTCALL SetPedMove(tPedestrian* pPed, int pAction, int pWalk_speed_factor, int pArg4, int pArg5, int pArg6, int pArg7);

tCollision_info* C2_HOOK_FASTCALL GetRootObject(tPed_character_instance *pPed);

void C2_HOOK_FASTCALL DropPointOntoPlane(const br_vector3* pPoint, const br_vector3* pPlane, br_vector3* pDest);

void C2_HOOK_FASTCALL OrthogonalVector(br_vector3* pDest, const br_vector3* pA, const br_vector3* pB);

void C2_HOOK_FASTCALL SetCharacterDirection(tPed_character_instance* pPed, const br_vector3* pDir, const br_vector3* pUp);

int C2_HOOK_FASTCALL OrientationChanged(float pCos_angle, const br_matrix34* pOrientation_1, const br_matrix34* pOrientation_2);

void C2_HOOK_FASTCALL AssertRootObjectsMatrix(tPed_character_instance* pPed);

void C2_HOOK_FASTCALL SetCharacterDirectionAR(tPed_character_instance* pPed, br_vector3* pDir, br_vector3* pUp);

br_scalar C2_HOOK_FASTCALL AreaOfTriangle(br_vector3* pV0, br_vector3* pV1, br_vector3* pV2);

void C2_HOOK_FASTCALL MakeRandomPointInTriangle(br_vector3* pDest, br_vector3* pV0, br_vector3* pV1, br_vector3* pV2);

void C2_HOOK_FASTCALL SetModelCallbacks(tPed_character_instance* pPed);

void C2_HOOK_FASTCALL BuildPedestrian(tPedestrian* pPed, const char* pGroup_name, br_vector3* pPos, br_vector3* pDir, tPed_movement_spec* pMovement_spec, tRace_ped_exclusion_spec* pExclusion_spec);

void C2_HOOK_FASTCALL SpawnPedsOnFace(br_face *pFace, br_model *pModel);

void C2_HOOK_FASTCALL FinishUpLoadingPeds(void);

void C2_HOOK_FASTCALL DisposePedestrians(void);

void C2_HOOK_FASTCALL InitPedsForRace(void);

void C2_HOOK_FASTCALL ResetPedNearness(void);

void C2_HOOK_FASTCALL LastChanceForPedEffects(void);

void C2_HOOK_FASTCALL MungeNapalm(void);

void C2_HOOK_FASTCALL StopCharacterMorphing(tPed_character_instance* pCharacter);

void C2_HOOK_CDECL SetCharacterPhysicsLevelAR(tPed_character_instance* pCharacter, tU32 pLevel);

undefined4 C2_HOOK_FASTCALL CharacterNoLongerRenderable(tPed_character_instance* pCharacter);

void C2_HOOK_FASTCALL OneLessPed(tPedestrian* pPed);

void C2_HOOK_FAKE_THISCALL ScoreForKilledPedestrian(tPedestrian* pPed, undefined4 pArg2, float pHeight);

int C2_HOOK_FASTCALL SetCharacterPhysicsLevel(tPed_character_instance* pCharacter, int pLevel);

void C2_HOOK_FASTCALL MungePedestrians(void);

void C2_HOOK_FASTCALL MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo(void);

void C2_HOOK_FASTCALL FlushAllPedCaches(void);

undefined4 C2_HOOK_FASTCALL CBPassiveCollision(undefined4* pArg1, undefined4 pArg2, undefined4* pArg3);

int C2_HOOK_FASTCALL CBActiveHalted(undefined4* pArg1, undefined4* pArg2);

void C2_HOOK_FASTCALL CBMovedByPhysics(undefined4* pArg1, undefined4* pArg2, undefined4 pArg3);

int C2_HOOK_FASTCALL CBMoveCompleted(undefined4* pArg1);

void C2_HOOK_FASTCALL CBFillInObject(undefined4* pArg1, undefined4 pArg2);

void C2_HOOK_FASTCALL CBLoadForm(tPed_form* pPed_form, FILE* pF);

void C2_HOOK_FASTCALL CBDisposeForm(undefined4* pArg1);

void C2_HOOK_FASTCALL CBLoadPersonality(tPed_personality* pPersonality, FILE* pF);

void C2_HOOK_FASTCALL CBDisposePersonality(undefined4* pArg1);

FILE* C2_HOOK_FASTCALL BonerOpenRemaps(const char* pFile_name);

tPed_remap* C2_HOOK_FASTCALL ReadRemap(const char *pFile_name);

FILE* C2_HOOK_FASTCALL BonerOpenPersonality(const char* pName);

FILE* C2_HOOK_FASTCALL BonerOpenCharacterForm(const char* pName);

FILE* C2_HOOK_FASTCALL BonerOpenMoves(const char* pName);

int C2_HOOK_FASTCALL DRVector3NonZero(br_vector3* pV);

void C2_HOOK_FASTCALL BonerReadPersonalityModels(const char* pName);

void C2_HOOK_FASTCALL RemapVector(br_vector3* pV, const tPed_remap_bone* pRemap);

tPed_remap* C2_HOOK_FASTCALL FindOrOpenRemap(const char* pName);

void C2_HOOK_FASTCALL Flip3DStoBRaxes(br_matrix34* pMat);

tPed_move* C2_HOOK_FASTCALL ReadMove(const char* pName, tPed_form* pForm, const char* pLooping_reset_flags, const br_vector3* pMovement_direction, tU32 pFlags);

tPed_move* C2_HOOK_FASTCALL FindOrOpenMove(const char* pName, tPed_form* pForm, const char* pLooping_reset_flags, const br_vector3* pMovement_direction, tU32 flags);

tPed_form* C2_HOOK_FASTCALL SetUpCharacterForm(const char* pName);

FILE* C2_HOOK_FASTCALL BonerOpenDefaultMoves(const char* pName);

tPed_form* C2_HOOK_FASTCALL FindOrOpenForm(const char* pName);

void C2_HOOK_FASTCALL RemapModelAxis(br_model* pModel, const tPed_remap_bone* pRemap);

br_model* C2_HOOK_FASTCALL BonerCloneModel(br_model *pModel, int pIndex, int pUpdateable);

float C2_HOOK_FASTCALL CalcBoundsRadius(const br_bounds3* pBounds);

tPed_personality* C2_HOOK_FASTCALL ReadPersonality(const char* pName);

tPed_character_instance* C2_HOOK_FASTCALL GetTestPed(void);

undefined4 C2_HOOK_FASTCALL MakeCharacterRenderable2(tPed_character_instance* pCharacter, int pIndex);

undefined4 C2_HOOK_FASTCALL MakeCharacterRenderable(tPed_character_instance* pCharacter);

int C2_HOOK_FASTCALL MakeCharacterCollideworthy2(tPed_character_instance* pCharacter, int pArg2, undefined4 pArg3, int pArg4);

int C2_HOOK_FASTCALL MakeCharacterCollideworthy(tPed_character_instance *pCharacter, int pArg2, undefined4 pArg3);

void C2_HOOK_FASTCALL DisableOverallMovement(void);

void C2_HOOK_FASTCALL EnableOverallMovement(void);

br_vector3* C2_HOOK_FASTCALL GetPedPos(tPedestrian* pPed);

void C2_HOOK_FASTCALL KillPedestrian(tPedestrian* pPed, tCollision_info* pCollision_info);

void C2_HOOK_FASTCALL DoPostElectricution(tPedestrian* pPed, tU32 pTime, float pF1, float pF2);

void C2_HOOK_FASTCALL MakePedNoise(tPedestrian* pPed, int pArg2, int pArg3, tCollision_info* pCollision_info);

void C2_HOOK_FASTCALL SetCharacterMoveAR(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, undefined4 pArg4, undefined4 pArg5, undefined4 pArg6);

tPed_face_cache_0x34* C2_HOOK_FASTCALL RecacheAndSetFace(tPedestrian* pPed, int* pArg2, int* pArg3, br_vector3* pArg4, int pArg5, tU32 pTime);

float C2_HOOK_FASTCALL GetClearanceFromCharacterInstance(tPed_character_instance* pCharacter, int pMoveNum);

void C2_HOOK_FASTCALL SetCharacterPosition(tPed_character_instance* pCharacter, br_vector3* pPos, int pArg3);

#endif //REC2_PEDESTRN_H
