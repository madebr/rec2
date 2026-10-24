#ifndef GUARD_56_AIWORLD_H
#define GUARD_56_AIWORLD_H

#include "c2_hooks.h"

#include "brender/brender.h"

// MakeVertexAndOffsetIt

extern void C2_HOOK_FASTCALL MakeFaceAndTextureIt(br_model* pModel, int pFace_num, br_uint_16 pV0, br_uint_16 pV1, br_uint_16 pV2, br_material* pMaterial);

extern void C2_HOOK_FASTCALL InitPanGameAIWorld(void);

// LoadAIWorldTrackInfo

// LoadAIWorldForRace

// InitialiseAIWorldForRace

extern void C2_HOOK_FASTCALL DisposeAIWorldRaceStuff(void);

// MungeAIWorld

extern void C2_HOOK_FASTCALL PointActorAlongThisBloodyVector(br_actor* pThe_actor, br_vector3* pThe_vector);

// MakeAIEditCubeHere

// CalcNegativeXVector

// MakeAISimpleEditSectionHere

#endif // GUARD_56_AIWORLD_H
