#include "56-aiworld.h"

#include "05-drmem.h"
#include "10-loading2.h"
#include "57-drone.h"

// MakeVertexAndOffsetIt

// FUNCTION: CARMA2_HW 0x00401f90
// FUNCTION: CARMA2_SW 0x00401f90
void C2_HOOK_FASTCALL MakeFaceAndTextureIt(br_model* pModel, int pFace_num, br_uint_16 pV0, br_uint_16 pV1, br_uint_16 pV2, br_material* pMaterial) {

    pModel->faces[pFace_num].vertices[0] = pV0;
    pModel->faces[pFace_num].vertices[1] = pV1;
    pModel->faces[pFace_num].vertices[2] = pV2;
    pModel->faces[pFace_num].smoothing = 0xffff;
    pModel->faces[pFace_num].material = pMaterial;
}

// FUNCTION: CARMA2_HW 0x00401000
void C2_HOOK_FASTCALL InitPanGameAIWorld(void) {

    PrintMemoryDump(0, "BEFORE LoadOpponents()");
    LoadOpponents();
    PrintMemoryDump(0, "AFTER LoadOpponents(), BEFORE LoadPanGameDroneInfo()");
    LoadPanGameDroneInfo();
    PrintMemoryDump(0, "AFTER LoadPanGameDroneInfo()");
}

// LoadAIWorldTrackInfo

// LoadAIWorldForRace

// InitialiseAIWorldForRace

// STUB: CARMA2_HW 0x00401160
void C2_HOOK_FASTCALL DisposeAIWorldRaceStuff(void) {

    NOT_IMPLEMENTED();
}

// MungeAIWorld

// FUNCTION: CARMA2_HW 0x004013d0
// FUNCTION: CARMA2_SW 0x004013d0
void C2_HOOK_FASTCALL PointActorAlongThisBloodyVector(br_actor* pThe_actor, br_vector3* pThe_vector) {
    br_transform trans;

    trans.type = BR_TRANSFORM_LOOK_UP;
    BrVector3Copy(&trans.t.look_up.t, &pThe_actor->t.t.translate.t);
    BrVector3Copy(&trans.t.look_up.look, pThe_vector);
    BrVector3Set(&trans.t.look_up.up, 0.f, 1.f, 0.f);
    BrTransformToTransform(&pThe_actor->t, &trans);
}

// MakeAIEditCubeHere

// CalcNegativeXVector

// MakeAISimpleEditSectionHere

