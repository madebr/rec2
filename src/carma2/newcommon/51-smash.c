#include "51-smash.h"

#include "09-funks.h"
#include "23-piping.h"
#include "41-utility.h"
#include "59-camera.h"
#include "69-sound.h"
#include "c2_math.h"
#include "globvars.h"
#include "rec2_types.h"
#include "rec2_macros.h"

#include "brender/brender.h"

// GLOBAL: CARMA2_HW 0x006abee8
br_vector3 gZero_vector__smash;

// GLOBAL: CARMA2_HW 0x006a55c8
tExplosion gExplosions[50];

// GLOBAL: CARMA2_HW 0x006a6d40
tSmash_explosion gSmash_explosions[20];

// GLOBAL: CARMA2_HW 0x006a7fc8
tRepair_animation gRepair_animations[25];

// GLOBAL: CARMA2_HW 0x006a80f8
tDecal gDecals[50];

// FUNCTION: CARMA2_HW 0x004ea880
void C2_HOOK_FASTCALL InitExplosions(void) {
    int i;
    int capacity;
    tExplosion* explosion;

    capacity = (int)REC2_ASIZE(gExplosions);
    for (i = 0, explosion = gExplosions; i < capacity; explosion++, i++) {

        explosion->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

        explosion->actor->model = BrModelAllocate(NULL, 4, 2);
        explosion->actor->model->faces[0].vertices[0] = 0;
        explosion->actor->model->faces[0].vertices[1] = 2;
        explosion->actor->model->faces[0].vertices[2] = 1;
        explosion->actor->model->faces[0].material = NULL;
        explosion->actor->model->faces[0].smoothing = 1;
        explosion->actor->model->faces[1].vertices[0] = 0;
        explosion->actor->model->faces[1].vertices[1] = 3;
        explosion->actor->model->faces[1].vertices[2] = 2;
        explosion->actor->model->faces[1].material = NULL;
        explosion->actor->model->faces[1].smoothing = 1;
        BrVector3Set(&explosion->actor->model->vertices[0].p, -.5f, -.5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[0].map, 0.f, 1.f);
        BrVector3Set(&explosion->actor->model->vertices[1].p, -.5f, .5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[1].map, 0.f, 0.f);
        BrVector3Set(&explosion->actor->model->vertices[2].p, .5f, .5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[2].map, 1.f, 0.f);
        BrVector3Set(&explosion->actor->model->vertices[3].p, .5f, -.5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[3].map, 1.f, 1.f);
        BrModelAdd(explosion->actor->model);

        explosion->actor->material = BrMaterialAllocate("BANG!");
        explosion->actor->material->flags &= ~BR_MATF_LIGHT;
        explosion->actor->material->flags |= BR_MATF_ALWAYS_VISIBLE;
        explosion->actor->material->colour_map = gBack_screen;
        GlorifyMaterial(&explosion->actor->material, 1, kRendererShadingType_AmbientOnly);
        BrMaterialAdd(explosion->actor->material);
        explosion->actor->render_style = BR_RSTYLE_FACES;

        explosion->start = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004eaac0
void C2_HOOK_FASTCALL ResetExplosions(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gExplosions); i++) {
        gExplosions[i].start = 0;
        gExplosions[i].finished = 0;
    }
    for (i = 0; i < (int)REC2_ASIZE(gSmash_explosions); i++) {
        gSmash_explosions[i].active = 0;
    }
}

void C2_HOOK_FASTCALL RemoveAllExplosionettes(void) {
    int i;
    br_actor* actor;

    for (i = 0; i < (int)REC2_ASIZE(gExplosions); i++) {
        actor = gExplosions[i].actor;
        if (actor->parent != NULL) {
            BrActorRemove(actor);
        }
    }
}

// MungeExplosions

// CopyExplosionData

// DoExplosionette

// Explode

// FUNCTION: CARMA2_HW 0x004eb020
void C2_HOOK_FASTCALL RemoveExplosions(tCar_spec* pCar_spec) {
    int i;
    tExplosion* explosion;

    for (i = 0; i < (int)REC2_ASIZE(gExplosions); i++) {
        explosion = &gExplosions[i];
        if (explosion->start != 0 && explosion->collision_actor == pCar_spec->collision_info->actor) {
            if (explosion->actor->parent != NULL) {
                BrActorRemove(explosion->actor);
            }
            explosion->finished = 1;
        }
    }
}

// CalcRandomPointBetween

// ApplyInitialMovement

// ApplyTimeSpec

// FindShrapnelFragment

// SetShrapnelFragment

// ShatterObject

void C2_HOOK_FASTCALL MungeInternalCarGlass(tCar_spec* pCar_spec) {
    if (pCar_spec == &gProgram_state.current_car && gAction_replay_camera_mode == kActionReplayCameraMode_Internal) {
        MungeCarMaterials(pCar_spec, 1);
    }
}

// ActuallySmashPart

// MakeSmashSound

// MungeSmokeFire

// Random34Matrix

// SetInitPosition

// DoGhostPartShrapnel

// DoNonCarShrapnel

// DoShrapnel

// DoAward

// DoSpillage

// DoSpecial

// GiveNonCarsAShove

// QueueDelayedSideEffect

// DoDelayedNonCar

// DoDelayedSmash

// MungeDelayedSideEffects

// ActivateNonCarSideEffects

// ActivateSmashSideEffects

// DoSideEffects

// DoVariableEffects

// DoConnotations

// DoPowerupRespawnSmash

// DoConnotationsMultiLevel

// SmashItIntoVerySmallPiecesIndeed

// InitSmashQueue

// FlushPowerupQueue

// FlushSmashQueue

// QueueSmashIfNecessary3

// QueueSmashIfNecessary2

// AllocateSmashLevels

// ReadSmashSounds

// ReadShrapnelMovementSpec

// ReadShrapnelInitialPosition

// ReadShrapnelTimeInfo

// NoteChildren

// ReadSmokeFireChance

// LoadActorFromRace

// ReadShrapnelSpec

// ReadSpillData

// ReadExplosion

// FreeExplosion

// ReadSpecialEffectsSpec

// ReadBounds

// ReadNoncarSideEffects

// ReadSmashSideEffects

// ReadSideEffects

// ReadLightingInformation

// ReadAward

// ReadVariableChanges

// ReadConnotations

// ReadSmashLevels

// ReadSmashableData

// FUNCTION: CARMA2_HW 0x004ef840
void C2_HOOK_FASTCALL ActuallyRepairSmash(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel) {
    br_pixelmap* texture;
    int texture_index;

    pSmashable->field_0x4c = pLevel;
    texture_index = IRandomBetween(0, pSmashable->levels[pLevel].count_pixelmaps - 1);
    texture = pSmashable->levels[pLevel].pixelmaps[texture_index];
    PipeSingleSmashTextureChange(pCar_spec, pSmashable->funk_material, texture);
    pSmashable->funk_material->user = texture;
    if (pCar_spec->field_0x1960 == NULL) {
        pSmashable->funk_material->colour_map = texture;
    }
    BrMaterialUpdate(pSmashable->funk_material, BR_MATU_COLOURMAP);
    MungeInternalCarGlass(pCar_spec);
    if (pLevel == 0 && pSmashable->funk >= 0) {
        EnableFunkotronic(pSmashable->funk);
    }
    if (pCar_spec != NULL && pCar_spec->driver == eDriver_local_human && gProgram_state.racing) {
        DRS3StartSound(gCar_outlet, eSoundId_SmashRepair);
    }
}

void C2_HOOK_FASTCALL KillFragment(tSmash_vertex* pFragment) {
    BrActorRemove(pFragment->actor);
    pFragment->end_time = 0;
}

void C2_HOOK_FASTCALL AddRepairAnimationToQueue(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable) {
    int i;
    int index = -1;
    int count = (int)REC2_ASIZE(gRepair_animations);

    for (i = 0; i < count; i++) {
        if (gRepair_animations[i].field_0x0 == pSmashable) {
            index = i;
            break;
        }
    }
    if (index < 0) {
        for (i = 0; i < count; i++) {
            if (gRepair_animations[i].field_0x0 == NULL) {
                index = i;
                break;
            }
        }
    }
    if (index >= 0) {
        gRepair_animations[index].field_0x0 = pSmashable;
        gRepair_animations[index].field_0x4 = GetTotalTime();
        gRepair_animations[index].field_0x8 = pCar_spec;
    }
}

// FUNCTION: CARMA2_HW 0x004ef910
void C2_HOOK_FASTCALL RepairSmash(tCar_spec* pCar_spec, tCar_crush_buffer_entry* pSmash_data) {
    tCar_crush_smashable_part* smashable = pSmash_data->smashables;
    int i;

    for (i = 0; i < pSmash_data->count_smashables; i++, smashable++) {
        if (smashable->field_0x4c != 0) {
            ActuallyRepairSmash(pCar_spec, smashable, smashable->field_0x4c - 1);
            AddRepairAnimationToQueue(pCar_spec, smashable);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ef9c0
void C2_HOOK_FASTCALL TotallyRepairSmash(tCar_spec* pCar_Spec, tCar_crush_buffer_entry* pSmash_data) {
    tCar_crush_smashable_part* smashable = pSmash_data->smashables;
    int i;

    for (i = 0; i < pSmash_data->count_smashables; i++, smashable++) {
        if (smashable->field_0x4c != 0) {
            ActuallyRepairSmash(pCar_Spec, smashable, 0);
        }
    }
}

// InitGlassFragments

// InitDecals

void C2_HOOK_FASTCALL RemoveAllDecals(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gDecals); i++) {
        if (gDecals[i].time != 0) {
            BrActorRemove(gDecals[i].actor);
        }
        gDecals[i].time = 0;
    }
}

// ReadPowerupSmash

// STUB: CARMA2_HW 0x004efe00
void C2_HOOK_FASTCALL InitSmashing(void) {
    NOT_IMPLEMENTED();
}

// VerySimpleRotate

// FUNCTION: CARMA2_HW 0x004eff30
void C2_HOOK_FASTCALL DoFragMovement(tSmash_vertex* pFragment, tU32 pTime, float pDelta_time) {
    float f_dt;
    double omega;
    br_vector3 pos;
    br_vector3 axis;
    br_actor* actor;
    double angle;
    br_angle rotation;

    f_dt = pDelta_time * 0.001f;
    if ((pDelta_time < 0.f || pDelta_time > 0.f) && !pFragment->field_0x34) {
        pFragment->actor->t.t.translate.t.v[0] = (br_scalar)(pFragment->actor->t.t.translate.t.v[0]
            - ((double)pFragment->v.v[0] * f_dt) * (-1.0 / 6.9));
        pFragment->actor->t.t.translate.t.v[1] = (br_scalar)(pFragment->actor->t.t.translate.t.v[1]
            - ((double)pFragment->v.v[1] * f_dt
                - ((double)gGravity_multiplier * f_dt) * f_dt * 4.9) * (-1.0 / 6.9));
        pFragment->actor->t.t.translate.t.v[2] = (br_scalar)(pFragment->actor->t.t.translate.t.v[2]
            - ((double)pFragment->v.v[2] * f_dt) * (-1.0 / 6.9));
        actor = pFragment->actor;
        pFragment->v.v[1] = (br_scalar)(pFragment->v.v[1]
            - ((double)f_dt * gGravity_multiplier) * 9.8);

        omega = sqrt(((double)pFragment->omega.v[1] * pFragment->omega.v[1]
            + (double)pFragment->omega.v[2] * pFragment->omega.v[2])
            + (double)pFragment->omega.v[0] * pFragment->omega.v[0]);
        axis.v[0] = (br_scalar)(pFragment->omega.v[0] / omega);
        axis.v[1] = (br_scalar)(pFragment->omega.v[1] / omega);
        axis.v[2] = (br_scalar)(pFragment->omega.v[2] / omega);
        angle = omega * f_dt;
        if (fabs(angle) >= 0.0001) {
            angle *= 32768.0 / PI;
            if (angle >= -9223372036854775808.0 && angle < 9223372036854775808.0) {
                rotation = (br_angle)(int64_t)angle;
            } else {
                rotation = 0;
            }

            /* Rotate around the fragment's origin without changing its position. */
            BrVector3Copy(&pos, &actor->t.t.translate.t);
            BrVector3Set(&actor->t.t.translate.t, 0.f, 0.f, 0.f);
            BrMatrix34PostRotate(&actor->t.t.mat, rotation, &axis);
            BrVector3Copy(&actor->t.t.translate.t, &pos);
        }
    }
    if (pTime < pFragment->field_0x10) {
        pFragment->actor->render_style = BR_RSTYLE_FACES;
    } else {
        pFragment->actor->render_style = BR_RSTYLE_NONE;
    }
}

// MungeGlassFragments2

// MungeAnimationRepairs

// MungeGlassFragments

// KillAllGlassFragments

// STUB: CARMA2_HW 0x004f02b0
void C2_HOOK_FASTCALL CleanUpSmashStuff(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004f0310
void C2_HOOK_FASTCALL DisposeSmashData(tCar_crush_buffer_entry* pCrush_data) {
    int i;

    if (pCrush_data->smashables != NULL) {
        for (i = 0; i < pCrush_data->count_smashables; i++) {
            if (pCrush_data->smashables[i].field_0x70 != NULL) {
                BrMemFree(pCrush_data->smashables[i].field_0x70);
            }
            if (pCrush_data->smashables[i].field_0x78 != NULL) {
                BrMemFree(pCrush_data->smashables[i].field_0x78);
            }
            if (pCrush_data->smashables[i].levels != NULL) {
                BrMemFree(pCrush_data->smashables[i].levels);
            }
        }
        BrMemFree(pCrush_data->smashables);
    }
}

// DoBackwardShrapnel

// FUNCTION: CARMA2_HW 0x004f0430
void C2_HOOK_FASTCALL InitRepairAnimations(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gRepair_animations); i++) {
        gRepair_animations[i].field_0x0 = NULL;
    }
}

// SplondificatalizeIdentifier

// ReadSmashReplace

// ReadSmashTexture

// ReadSmashableEnvironment

// DisposeSmashEnvNonCars

// CloneMemory

// CloneEnvSmash

// FreeConnotations

// FreeEnvSmash

// DisposeSmashableEnvironment

// MungeSmashMaterialNames

// ExtrapolateTextureCoordinates

// ExtrapolateGeometryCoordinates2

// ExtrapolateGeometryCoordinates

// ExtractDeTiledFaces

// AddDecal

// InsertQuad

// SpawnSmashAndMungeFaces

// DistanceFromFaceND

// PointInFace

// FindFaceWithPoint

// ProcessTexture

// ProcessMesh

// ProcessModel

// BashModel

// SendNetSmash

// SendAllSmashes

// HitPowerup

// SmashEnvironment

// BashObject

// NonCarSnapSmashCallback

// SmashFaceMustBeUpdateable

// MungeMaterialCB

// MungeMaterial

// ReinitSmashing

// LinkSmashies

// RepairModels

// RepairAllSmashes

// SmashEverythingCB

// SmashEverything

// DetermineTilitudinality

// SetSmashableModel

// LoadWastedExplosion

// ChangeSmashModel

// TurnOnSmashModel

// TurnOffSmashModel

// SmashRemoveSomeFaces

// SmashRestoreSomeFaces

// TurnOnSmashDecal

// TurnOffSmashDecal

// ChangeSmashTexture

// DoNetSmash

// ReceivedSmashEnvironment

// DoQueuedNetSmashes

// FindTheBastard

// ReceivedSmashShraps

// MungeSmashEdgeTriggers

