#include "63-loading3.h"

// GLOBAL: CARMA2_HW 0x0065852c
char* gPedTexturePath = "PIXELMAP";

// NearestPowerOfTwo

// LoadBunchOfPixies

// DRLdImg

// RealLoadPixelmap

// LoadPixelmap

// LoadShadeTable

// LoadMaterial

// LoadModel

// LoadActor

// DRLoadPalette

// DRLoadShadeTable

// DRLoadMaterials

// DRLoadModels

// DRLoadActors

// DRLoadLights

// LoadInFiles

// TestForOriginalCarmaCDinDrive

// FUNCTION: CARMA2_HW 0x0048f7f0
void C2_HOOK_FASTCALL SetDefaultPixelmapFolderName(void) {
    gPedTexturePath = "PIXELMAP";
}

