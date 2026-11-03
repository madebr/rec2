/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "22-replay.h"

#include <math.h>
#include <stdlib.h>

#include "18-graphics2.h"
#include "30-opponent.h"
#include "globvars.h"

/* defined by this file's original anyway; declared here so every use below sees it */
// GLOBAL: CARMA2_HW 0x006a2378
int gUnk_006a2378;

// GLOBAL: CARMA2_HW 0x006a23c8
tU32 gUnk_006a23c8;

/* defined in carma2/common/replay.c, which upstream does not compile */
extern int gKey_down;
extern int gSingle_frame_mode;

extern tU32 C2_HOOK_FASTCALL GetTotalTime(void);

// GLOBAL: CARMA2_HW 0x006768fc
// GLOBAL: CARMA2_SW 0x0067106c
int gPaused;

// GLOBAL: CARMA2_HW 0x0058f2d8
// GLOBAL: CARMA2_SW 0x0058a2d8
const int gPow2_array[32] = {
    0x1, 0x2, 0x4, 0x8,
    0x10, 0x20, 0x40, 0x80,
    0x100, 0x200, 0x400, 0x800,
    0x1000, 0x2000, 0x4000, 0x8000,
    0x10000, 0x20000, 0x40000, 0x80000,
    0x100000, 0x200000, 0x400000, 0x800000,
    0x1000000, 0x2000000, 0x4000000, 0x8000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
};

// GLOBAL: CARMA2_HW 0x0058f600
// GLOBAL: CARMA2_SW 0x0058a600
tU8 gCamera_type_allowed_replay[9] = {
    1, 1, 1, 1, 1, 0, 0, 1, 1,
};

// GLOBAL: CARMA2_HW 0x0058f610
// GLOBAL: CARMA2_SW 0x0058a610
tU8 gCamera_type_allowed_gameplay[9] = {
    1, 0, 0, 0, 1, 0, 0, 1, 1,
};

// GLOBAL: CARMA2_HW 0x0058f620
// GLOBAL: CARMA2_SW 0x0058a620
int gAR_camera_type = 2;

// GLOBAL: CARMA2_HW 0x0058f6a4
// GLOBAL: CARMA2_SW 0x0058a6a4
int gCar_car_collisions = 1;

// GLOBAL: CARMA2_HW 0x0058f6b0
// GLOBAL: CARMA2_SW 0x0058a6b0
const float gCar_simplification_factor[2][5] = {
    { 50.0f, 3.0f, 1.5f, 0.75f, 0.0f },
    { 50.0f, 5.0f, 2.5f, 1.5f, 0.0f }
};

// GLOBAL: CARMA2_HW 0x0058f7f0
// GLOBAL: CARMA2_SW 0x0058a7f0
tPolyhedron_edge_indexes gDefault_box_collision_wireframe_edges[12] = {
    { 0, 1 }, { 2, 3 }, { 4, 5 }, { 6, 7 },
    { 0, 2 }, { 1, 3 }, { 4, 6 }, { 5, 7 },
    { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
};

// GLOBAL: CARMA2_HW 0x0065fbb0
const char* gReplay_pixie_names[11] = {
    "REPLAY.PIX",
    "RBUTTONS.PIX",
    "REWSTART.PIX",
    "REW.PIX",
    "REVPLAY.PIX",
    "PAUSE.PIX",
    "PLAY.PIX",
    "FFWD.PIX",
    "FWDEND.PIX",
    "CAMERA.PIX",
    "RECORD.PIX",
};

// GLOBAL: CARMA2_HW 0x006a2388
br_pixelmap* gReplay_pixelmaps[11];

// GLOBAL: CARMA2_HW 0x0065fbe0
int gReplay_timeline_x1[2] = { 240, 443 };

// GLOBAL: CARMA2_HW 0x0065fbe8
int gReplay_timeline_x2[2] = { 306, 612 };

// GLOBAL: CARMA2_HW 0x0065fbf0
int gReplay_timeline_y[2] = { 184, 446 };


// GLOBAL: CARMA2_HW 0x006a2360
tU32 gUnk_006a2360;

// GLOBAL: CARMA2_HW 0x006a2364
tU32 gUnk_006a2364;


// GLOBAL: CARMA2_HW 0x006a237c
tU32 gUnk_006a237c;



// GLOBAL: CARMA2_HW 0x0074abf0
tU32 gCamera_period;

// FUNCTION: CARMA2_HW 0x004e6280
void C2_HOOK_FASTCALL ActualActionReplayHeadups(int pSpecial_zappy_bastard) {
    extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
    extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);
    extern int C2_HOOK_FASTCALL MovieRecording(void);
    extern int C2_HOOK_FASTCALL DoMouseCursor(void);
    extern void C2_HOOK_FASTCALL DRPixelmapText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge);
    extern void C2_HOOK_FASTCALL DRPixelmapCentredText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText);
    extern void C2_HOOK_FASTCALL PossibleLock(int pValue);
    extern int C2_HOOK_FASTCALL PossibleUnlock(int pValue);
    extern tU16 C2_HOOK_FASTCALL PaletteEntry16Bit(br_pixelmap* pPal, int pEntry);
    extern float C2_HOOK_FASTCALL ARGetReplayRate(void);
    extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
    extern tActionReplayCameraMode gAction_replay_camera_mode;
    extern tGraf_data* gCurrent_graf_data;
    extern int gReal_graf_data_index;
    extern tU32 gLast_replay_frame_time;
    extern br_pixelmap* gReplay_pixelmaps[11];
    extern tU32 gUnk_006a2360;
    extern tU32 gUnk_006a2364;
    extern tU32 gUnk_006a23c8;
    extern tDR_font gFonts[24];
    extern int gReplay_show_controls;
    extern int gShow_status_message;
    extern int gNext_replay_record_state;
    extern int gStatus_message_1;
    extern int gStatus_message_2;
    extern int gRecording_mouse_disabled;
    tU16 colour1;
    tU16 colour2;
    tU16 colour3;
    tU16 colour4;
    tU32 time;
    int elapsed;
    int total;
    int pos;
    int i;
    int x;
    int text_y;
    int showed_camera;

    colour3 = 4;
    colour2 = 2;
    colour4 = 0x52;
    colour1 = 0x51;
    if (gBack_screen->type != 3) {
        colour3 = PaletteEntry16Bit(gRender_palette, 4);
        colour2 = PaletteEntry16Bit(gRender_palette, 2);
        colour4 = PaletteEntry16Bit(gRender_palette, 0x52);
        colour1 = PaletteEntry16Bit(gRender_palette, 0x51);
    }
    time = PDGetTotalTime();
    if (PDKeyDown(0)) {
        return;
    }
    PossibleLock(1);
    DRMaskedStamp((br_int_16)gCurrent_graf_data->field_0x300, (br_int_16)gCurrent_graf_data->field_0x304, gReplay_pixelmaps[1]);
    if (MovieRecording()) {
        DRMaskedStamp((br_int_16)(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x358), (br_int_16)(gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x35c), gReplay_pixelmaps[10]);
    }
    if (pSpecial_zappy_bastard < 0) {
        DRMaskedStamp((br_int_16)(gCurrent_graf_data->field_0x310 + gCurrent_graf_data->field_0x300), (br_int_16)(gCurrent_graf_data->field_0x314 + gCurrent_graf_data->field_0x304), gReplay_pixelmaps[2]);
    } else if (pSpecial_zappy_bastard > 0) {
        DRMaskedStamp((br_int_16)(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x340), (br_int_16)(gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x344), gReplay_pixelmaps[8]);
    } else if (ARGetReplayRate() < -1.0f) {
        DRMaskedStamp((br_int_16)(gCurrent_graf_data->field_0x318 + gCurrent_graf_data->field_0x300), (br_int_16)(gCurrent_graf_data->field_0x31c + gCurrent_graf_data->field_0x304), gReplay_pixelmaps[3]);
    } else if (ARGetReplayRate() > 1.0f) {
        DRMaskedStamp((br_int_16)(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x338), (br_int_16)(gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x33c), gReplay_pixelmaps[7]);
    } else if (ARGetReplayRate() == 1.0f) {
        DRMaskedStamp((br_int_16)(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x330), (br_int_16)(gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x334), gReplay_pixelmaps[6]);
    } else if (ARGetReplayRate() == -1.0f) {
        DRMaskedStamp((br_int_16)(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x320), (br_int_16)(gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x324), gReplay_pixelmaps[4]);
    } else {
        DRMaskedStamp((br_int_16)(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x328), (br_int_16)(gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x32c), gReplay_pixelmaps[5]);
    }
    elapsed = gLast_replay_frame_time - gUnk_006a2360;
    total = gUnk_006a2364 - gUnk_006a2360;
    pos = (int)((float)elapsed / (float)total * (gReplay_timeline_x2[gReal_graf_data_index] - gReplay_timeline_x1[gReal_graf_data_index])) + gReplay_timeline_x1[gReal_graf_data_index];
    if (pos > gReplay_timeline_x1[gReal_graf_data_index]) {
        BrPixelmapLine(gBack_screen, gReplay_timeline_x1[gReal_graf_data_index], gReplay_timeline_y[gReal_graf_data_index], pos - 1, gReplay_timeline_y[gReal_graf_data_index], colour2);
        for (i = 1; i < 6; i++) {
            BrPixelmapLine(gBack_screen, gReplay_timeline_x1[gReal_graf_data_index], gReplay_timeline_y[gReal_graf_data_index] + i, pos - 1, gReplay_timeline_y[gReal_graf_data_index] + i, colour3);
        }
        BrPixelmapLine(gBack_screen, gReplay_timeline_x1[gReal_graf_data_index], gReplay_timeline_y[gReal_graf_data_index] + 6, pos - 1, gReplay_timeline_y[gReal_graf_data_index] + 6, colour2);
    }
    if (pos < gReplay_timeline_x2[gReal_graf_data_index]) {
        BrPixelmapLine(gBack_screen, pos, gReplay_timeline_y[gReal_graf_data_index], gReplay_timeline_x2[gReal_graf_data_index] - 1, gReplay_timeline_y[gReal_graf_data_index], colour1);
        for (i = 1; i < 6; i++) {
            BrPixelmapLine(gBack_screen, pos, gReplay_timeline_y[gReal_graf_data_index] + i, gReplay_timeline_x2[gReal_graf_data_index] - 1, gReplay_timeline_y[gReal_graf_data_index] + i, colour4);
        }
        BrPixelmapLine(gBack_screen, pos, gReplay_timeline_y[gReal_graf_data_index] + 6, gReplay_timeline_x2[gReal_graf_data_index] - 1, gReplay_timeline_y[gReal_graf_data_index] + 6, colour1);
    }
    BrPixelmapLine(gBack_screen, gReplay_timeline_x1[gReal_graf_data_index] - 1, gReplay_timeline_y[gReal_graf_data_index], gReplay_timeline_x1[gReal_graf_data_index] - 1, gReplay_timeline_y[gReal_graf_data_index] + 6, colour2);
    BrPixelmapLine(gBack_screen, gReplay_timeline_x2[gReal_graf_data_index], gReplay_timeline_y[gReal_graf_data_index], gReplay_timeline_x2[gReal_graf_data_index], gReplay_timeline_y[gReal_graf_data_index] + 6, colour1);
    if (gReplay_show_controls) {
        DRMaskedStamp((br_int_16)(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x348), (br_int_16)(gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x34c), gReplay_pixelmaps[9]);
    }
    PossibleUnlock(1);
    showed_camera = 0;
    if (time - gUnk_006a23c8 < 2000) {
        showed_camera = 1;
        DRPixelmapText(gBack_screen, gCurrent_graf_data->field_0x350, gCurrent_graf_data->field_0x354, &gFonts[1], GetMiscString(eMiscString_standard_cam + gAction_replay_camera_mode), gCurrent_graf_data->field_0x350 * 2);
    }
    if (MovieRecording()) {
        if (gShow_status_message) {
            if (gNext_replay_record_state == 4) {
                x = gCurrent_graf_data->field_0x350;
                text_y = gCurrent_graf_data->field_0x354;
                if (!showed_camera) {
                    DRPixelmapText(gBack_screen, x, text_y, &gFonts[1], GetMiscString(gStatus_message_1), gBack_screen->width);
                }
            } else {
                x = gCurrent_graf_data->width / 2;
                text_y = gCurrent_graf_data->height / 2;
                DRPixelmapCentredText(gBack_screen, x, text_y - 0x40, &gFonts[4], GetMiscString(gStatus_message_1));
                if (gStatus_message_2) {
                    DRPixelmapCentredText(gBack_screen, x, text_y - 0x20, &gFonts[6], GetMiscString(gStatus_message_2));
                }
            }
        }
    } else if ((time / 400 & 1) != 0) {
        PossibleLock(1);
        DRMaskedStamp((br_int_16)gCurrent_graf_data->field_0x2f8, (br_int_16)gCurrent_graf_data->field_0x2fc, gReplay_pixelmaps[0]);
        PossibleUnlock(1);
    }
    if (!gRecording_mouse_disabled && !MovieRecording()) {
        PossibleLock(1);
        DoMouseCursor();
        PossibleUnlock(1);
    }
}

// FUNCTION: CARMA2_HW 0x004e6260
// FUNCTION: CARMA2_SW 0x004e2280
void C2_HOOK_FASTCALL DoActionReplayPostSwap(void) {
    RemoveTransientBitmaps(1);
}

// FUNCTION: CARMA2_HW 0x004e6900
void C2_HOOK_FASTCALL DoZappyActionReplayHeadups(int pFrame_number) {
    extern tU32 gTime_of_last_zappy_headups;
    extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
    extern void C2_HOOK_FASTCALL PDScreenBufferSwap(int pRendering_area_only);
    tU32 time;

    time = PDGetTotalTime();
    if (abs(pFrame_number) > 0x2710 && time - gTime_of_last_zappy_headups > 0x32) {
        ActualActionReplayHeadups(pFrame_number);
        gTime_of_last_zappy_headups = time;
        PDScreenBufferSwap(0);
        RemoveTransientBitmaps(1);
    }
}

// FUNCTION: CARMA2_HW 0x004e6270
void C2_HOOK_FASTCALL DoActionReplayHeadups(void) {

    ActualActionReplayHeadups(0);
}

/* the original passes pFrame_period in ecx to MungePedestrians (rec2 defines it with no parameter) */
#ifdef REC2_MATCHING
#define REPLAY_CALL_MUNGE_PEDESTRIANS(P) ((void (C2_HOOK_FASTCALL *)(int))MungePedestrians)(P)
#else
#define REPLAY_CALL_MUNGE_PEDESTRIANS(P) MungePedestrians()
#endif

// FUNCTION: CARMA2_HW 0x004e6950
void C2_HOOK_FASTCALL PreProcess(int pFrame_period) {
    extern void C2_HOOK_FASTCALL UpdateFramePeriod(tU32* pCamera_period);
    extern void C2_HOOK_FASTCALL MungePedestrians(void);

    UpdateFramePeriod(&gCamera_period);
    if (ARGetReplayRate() < 0.0f) {
        REPLAY_CALL_MUNGE_PEDESTRIANS(pFrame_period);
    }
}

extern void C2_HOOK_FASTCALL ProcessOilSpills(tU32 pFrame_period);
extern void C2_HOOK_FASTCALL MungePedestrians(void);
extern void C2_HOOK_FASTCALL MungePowerupStuff(tU32 pArg1);

// STUB: CARMA2_HW 0x004e6980
void C2_HOOK_FASTCALL PostProcess(int pFrame_period) { NOT_IMPLEMENTED(); }

// CheckReplayTurnOn

// InitializeActionReplayPix


// GLOBAL: CARMA2_HW 0x006a23d4
tU32 gLast_synch_time;

extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern void C2_HOOK_FASTCALL ServiceGameInRace(void);

// SynchronizeActionReplay

// STUB: CARMA2_HW 0x004e71c0
void C2_HOOK_FASTCALL TurnOnActionReplay(void) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004e7270
void C2_HOOK_FASTCALL TurnOffActionReplay(void) { NOT_IMPLEMENTED(); }

// FUNCTION: CARMA2_HW 0x004e72d0
// FUNCTION: CARMA2_SW 0x004e3110
void C2_HOOK_FASTCALL AfterActionReplay(void) {
    ForceRebuildActiveCarList();
}

// STUB: CARMA2_HW 0x004e72e0
void C2_HOOK_FASTCALL ToggleReplay(int* pArg1, int* pArg2) { NOT_IMPLEMENTED(); }

// FUNCTION: CARMA2_HW 0x004022e0
// FUNCTION: CARMA2_SW 0x004022e0
void C2_HOOK_FASTCALL ApplyTime(tU32 pTime) {
    gLast_replay_frame_time = pTime;
}

// FUNCTION: CARMA2_HW 0x004022f0
// FUNCTION: CARMA2_SW 0x004022f0
void C2_HOOK_FASTCALL ARSetPause(int pPause) {
    gPaused = pPause;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// ARGetStartTime
// FUN_0040b500
// ActorToActorMatrix34Local

// RelinkActorKeepWorldTransform

// FUNCTION: CARMA2_HW 0x0043bd70
void C2_HOOK_FASTCALL WorkerCrush(int field_0x4, void* pField_0x8, void* pField_0xe) {
    extern void C2_HOOK_FASTCALL ExpandVector3(br_vector3* pDest, const tCompressed_vector3* pSrc, float pMin, float pMax);
    extern br_scalar C2_HOOK_FASTCALL DRU16ToScalar(tU16 pValue, float pMin, float pMax);
    extern void C2_HOOK_CDECL JitModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen);
    br_model* pModel;
    br_vector3 offset;
    br_vector3 scaled_offset;
    tU16* pWeights;
    br_scalar scale;
    int i;
    int vertex_offset;

    pModel = (br_model*)field_0x4;
    ExpandVector3(&offset, (const tCompressed_vector3*)pField_0x8, -1.f, 1.f);
    i = 0;
    if (pModel->nvertices > 0) {
        pWeights = (tU16*)pField_0xe;
        vertex_offset = 0;
        do {
            if (*pWeights != 0) {
                scale = DRU16ToScalar(*pWeights, 0.f, 10.f);
                scaled_offset.v[0] = scale * offset.v[0];
                scaled_offset.v[1] = scale * offset.v[1];
                scaled_offset.v[2] = scale * offset.v[2];
                *(br_scalar*)((tU8*)pModel->vertices + vertex_offset) = scaled_offset.v[0] + *(br_scalar*)((tU8*)pModel->vertices + vertex_offset);
                *(br_scalar*)((tU8*)pModel->vertices + vertex_offset + 4) = scaled_offset.v[1] + *(br_scalar*)((tU8*)pModel->vertices + vertex_offset + 4);
                *(br_scalar*)((tU8*)pModel->vertices + vertex_offset + 8) = scaled_offset.v[2] + *(br_scalar*)((tU8*)pModel->vertices + vertex_offset + 8);
            }
            i++;
            pWeights++;
            vertex_offset += sizeof(br_vertex);
        } while (i < pModel->nvertices);
    }
    if ((pModel->flags & BR_MODF_CUSTOM) != 0 && pModel->user != NULL) {
        ((tUser_detail_level_model*)pModel->user)->field_0x0 = 1;
        return;
    }
    pModel->custom = JitModelUpdate;
    pModel->flags |= BR_MODF_CUSTOM;
}

// FUNCTION: CARMA2_HW 0x0043be50
void C2_HOOK_FASTCALL WorkerCrushUndo(int field_0x4, void* pField_0x8, void* pField_0xe) {
    extern void C2_HOOK_FASTCALL ExpandVector3(br_vector3* pDest, const tCompressed_vector3* pSrc, float pMin, float pMax);
    extern br_scalar C2_HOOK_FASTCALL DRU16ToScalar(tU16 pValue, float pMin, float pMax);
    extern void C2_HOOK_CDECL JitModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen);
    br_model* pModel;
    br_vector3 offset;
    br_vector3 scaled_offset;
    tU16* pWeights;
    br_scalar scale;
    int i;
    int vertex_offset;

    pModel = (br_model*)field_0x4;
    ExpandVector3(&offset, (const tCompressed_vector3*)pField_0x8, -1.f, 1.f);
    i = 0;
    if (pModel->nvertices > 0) {
        pWeights = (tU16*)pField_0xe;
        vertex_offset = 0;
        do {
            if (*pWeights != 0) {
                scale = DRU16ToScalar(*pWeights, 0.f, 10.f);
                BrVector3Scale(&scaled_offset, &offset, -scale);
                BrVector3Accumulate((br_vector3*)((tU8*)pModel->vertices + vertex_offset), &scaled_offset);
            }
            i++;
            pWeights++;
            vertex_offset += sizeof(br_vertex);
        } while (i < pModel->nvertices);
    }
    if ((pModel->flags & BR_MODF_CUSTOM) != 0 && pModel->user != NULL) {
        ((tUser_detail_level_model*)pModel->user)->field_0x0 = 1;
        return;
    }
    pModel->custom = JitModelUpdate;
    pModel->flags |= BR_MODF_CUSTOM;
}

// WorkerFlap

// WorkerSplitWeldUndo

// WorkerPHILObject

// FUNCTION: CARMA2_HW 0x004c9070
void C2_HOOK_FASTCALL WorkerSmudge(tPipe_chunk** pChunk, int pScale) {
    tCar_spec* car;
    tU8* a;
    tU32 code;
    int i;
    tU8* esi;

    code = *(tU32*)*pChunk;
    if ((code & 0xffffff00) == 0) {
        car = &gProgram_state.current_car;
    } else {
        car = GetCarSpec(code >> 8, code & 0xff);
    }
    a = *(tU8**)((tU8*)car + 0xe10);
    i = 0;
    if (*(tU16*)((tU8*)*pChunk + 4) <= 0) {
        return;
    }
    esi = *(tU8**)(a + 0x18);
    do {
        tU32* colors = *(tU32**)(*(tU8**)(*(tU8**)(esi + 0x4c) + 0x18) + 0x14);
        tU16 idx = *(tU16*)((tU8*)*pChunk + i * 4 + 0xc);
        tS8 bright = (tS8)((tS8)*(tU8*)((tU8*)*pChunk + i * 4 + 0xe) * (tS8)pScale);
        tU32 alpha24 = ((tU32)(tU8)bright) << 24;
        tU32 c = colors[idx];

        colors[idx] = ((c & 0xff000000) + alpha24) & 0xff000000;
        if (*(tU8*)(esi + 0x20) & 0x80) {
            tU8* q = *(tU8**)(esi + 0x4c);
            tU8* w = *(tU8**)(q + 0x18);
            tU32* colors2 = (tU32*)(*(tU8**)(w + 0x14));
            tU16* idx2 = (tU16*)(*(tU8**)(w + 0x18));
            tU8 dl = (tU8)(colors2[idx] >> 24) + (tU8)bright;
            tU8* arr = *(tU8**)(esi + 8);
            tU16 ix = idx2[idx];

            arr[(tU32)ix * 40 + 0x14] = dl;
        }
        i++;
    } while (i < *(tU16*)((tU8*)*pChunk + 4));
}

// WorkerPedMove

// WorkerPedMoveUndo
// WorkerPedFPChange

// WorkerPedPhysics

// WorkerPedModelChangeUndo

// WorkerPedModelChange

// WorkerPedDismember

// FUNCTION: CARMA2_HW 0x004d6180
void C2_HOOK_FASTCALL WorkerProxRay(int field_0x0, tU16 field_0x6, tU16 field_0x4, int field_0x8) {
    extern tProximity_ray gProximity_rays[20];
    extern tPedestrian* gPedestrian_array;
    tCar_spec* car;

    if ((field_0x6 >> 8) == 0) {
        car = &gProgram_state.current_car;
    } else {
        car = GetCarSpec(field_0x6 >> 8, field_0x6 & 0xff);
    }
    gProximity_rays[field_0x0].car = car;
    gProximity_rays[field_0x0].ped = &gPedestrian_array[field_0x4];
    gProximity_rays[field_0x0].start_time = field_0x8;
}

// WorkerRepulseRay

// FUNCTION: CARMA2_HW 0x004e0710
void C2_HOOK_FASTCALL WorkerSpecial(void) {
    extern tPhysics_object* gMutant_tail_first_collision_info;
    tPhysics_object* collision_info;

    for (collision_info = gMutant_tail_first_collision_info; collision_info != NULL; collision_info = collision_info->child) {
        collision_info->actor->render_style = BR_RSTYLE_FACES;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// DrawStatusMessage

// WorkerUndoSmashRemoveFaces

// WorkerShrapnelShowerUndo

// WorkerSpark

// WorkerShrapnel

// FUNCTION: CARMA2_HW 0x004fef40
void C2_HOOK_FASTCALL WorkerExtendedSplash(int field_0x0, int field_0x4, int field_0x8, br_vector3* pField_0xc, br_vector3* pField_0x18) {
    extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);
    extern tReplay_splash gReplay_splashes[10];
    extern int gReplay_splash_flags;
    int i;
    int free_slot;
    tReplay_splash* p;
    int* src;

    free_slot = -1;
    i = 0;
    p = gReplay_splashes;
    do {
        if (!(gReplay_splash_flags & (1 << i))) {
            if (free_slot < 0) {
                free_slot = i;
            }
        } else if (p->object.actor == (br_actor*)field_0x0) {
            goto found;
        }
        p++;
        i++;
    } while (p < &gReplay_splashes[10]);
    if (free_slot < 0) {
        free_slot = IRandomBetween(0, 10);
    }
    gReplay_splash_flags |= 1 << free_slot;
    gReplay_splashes[free_slot].object.actor = (br_actor*)field_0x0;
    gReplay_splashes[free_slot].type = field_0x4;
    i = free_slot;
found:
    *(int*)&gReplay_splashes[i].water_density = field_0x8;
    src = (int*)pField_0xc;
    *(int*)&gReplay_splashes[i].normal.v[0] = src[0];
    *(int*)&gReplay_splashes[i].normal.v[1] = src[1];
    *(int*)&gReplay_splashes[i].normal.v[2] = src[2];
    src = (int*)pField_0x18;
    *(int*)&gReplay_splashes[i].bb.min.v[0] = src[0];
    *(int*)&gReplay_splashes[i].bb.min.v[1] = src[1];
    *(int*)&gReplay_splashes[i].bb.min.v[2] = src[2];
    *(int*)&gReplay_splashes[i].bb.max.v[0] = src[3];
    *(int*)&gReplay_splashes[i].bb.max.v[1] = src[4];
    *(int*)&gReplay_splashes[i].bb.max.v[2] = src[5];
}

// FUN_004f8b50

// STUB: CARMA2_HW 0x004ca890
void C2_HOOK_FASTCALL DoVanishDismembered(uintptr_t pOwner, tS32 value) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x00408580
tPhysics_object* C2_HOOK_FASTCALL MakeCharacterPhysicsForBone(tPed_character_instance* pCharacter, int pBone_index, undefined4 pArg3, undefined4 pArg4) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004529d0
void C2_HOOK_FASTCALL WorkerDroneCornerPos(int field_0x0, tU16 field_0x4, tU16 field_0x6, tU16 field_0x8, tU16* pField_0xa) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x00452880
void C2_HOOK_FASTCALL WorkerDroneStraightPos(int field_0x0, int field_0x4, tU16 field_0x8, tU16 field_0xa) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004de020
void C2_HOOK_FASTCALL WorkerEndShitMine(int field_0x0, br_vector3* pPos) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004ccae0
void C2_HOOK_FASTCALL WorkerGibShower(int field_0x0, int field_0xc, int field_0x8, int field_0x10, int field_0x4, br_vector3* pField_0x14, br_vector3* pField_0x20, br_vector3* pField_0x2c) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004ccc40
void C2_HOOK_FASTCALL WorkerGibShowerUndo(int field_0xc, int prev_0x0, int prev_0xc, int prev_0x8, int prev_0x10, int prev_0x4, br_vector3* prev_0x14, br_vector3* prev_0x20, br_vector3* prev_0x2c) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0043c030
void C2_HOOK_FASTCALL WorkerModelMashUndo(int field_0x4, tU8* pField_0x8) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004d6db0
void C2_HOOK_FASTCALL WorkerPedDiagnostics(void* pOpponent, void* pField_0x4, void* pField_0x58, void* pField_0x144) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004ca4d0
void C2_HOOK_FASTCALL WorkerPedDir(int field_0x0, br_vector3* pDir) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004ca830
void C2_HOOK_FASTCALL WorkerPedPos(int field_0x0, br_vector3* pVec) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004ca680
void C2_HOOK_FASTCALL WorkerPedStatus(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x9, tU8 field_0xb, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004ca6e0
void C2_HOOK_FASTCALL WorkerPedStatusUndo(int field_0x0, tU8 field_0x4, tU8 field_0x5, tU8 field_0x6, tU8 field_0x7, tU8 field_0x8, tU8 field_0xa, br_vector3* p0xc, br_vector3* p0x18, br_vector3* p0x24) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004df210
void C2_HOOK_FASTCALL WorkerPowerupLose(int field_0x0, int field_0x4) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004de000
void C2_HOOK_FASTCALL WorkerShitMine(int field_0x0, br_vector3* pPos) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004eb8f0
void C2_HOOK_FASTCALL WorkerShrapnelShower(tU32 pOwner, tU32 pShrapnel, tU32 pSeed, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6, tU32 pActor, tU32 pTime, tU32 pArg9, br_vector3* pTransform, br_vector3* pFragment_owner, br_vector3* pPosition, tU32 pDistance, br_vector3* pArg14, tU32 pArg15) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0043c240
void C2_HOOK_FASTCALL WorkerSplitWeldApply(void* roadMachine, br_vector3* pVector) { NOT_IMPLEMENTED(); }

// GLOBAL: CARMA2_HW 0x006a2374
int gKey_down;

// GLOBAL: CARMA2_HW 0x006a0908
tPhysics_object* gMutant_tail_first_collision_info;

// GLOBAL: CARMA2_HW 0x006a23c4
int gSingle_frame_mode;

// GLOBAL: CARMA2_HW 0x006a2380
tU32 gTime_of_last_zappy_headups;

// STUB: CARMA2_HW 0x004922a0
void C2_HOOK_FASTCALL ServiceGameInRace(void) { NOT_IMPLEMENTED(); }
