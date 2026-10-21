#include "56-aiworld.h"

#include "05-drmem.h"
#include "10-loading2.h"
#include "57-drone.h"

// MakeVertexAndOffsetIt

// MakeFaceAndTextureIt

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

// PointActorAlongThisBloodyVector

// MakeAIEditCubeHere

// CalcNegativeXVector

// MakeAISimpleEditSectionHere

