#include "cutscene.h"

#include "errors.h"
#include "globvars.h"
#include "globvrbm.h"
#include "graphics.h"
#include "input.h"
#include "sound.h"
#include "utility.h"

#include <brender/brender.h>

#include "rec2_types.h"

#include "platform.h"

#include "c2_string.h"

#if defined(C2_HOOKS_ENABLED)
#define USE_SMACKW32 1
#else
#define USE_SMACKW32 0
#endif

#if USE_SMACKW32
#include "smack.h"
#include "win32/win32.h"
#endif


C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gSmackerActor, 0x0079eca4);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gSmackerModel, 0x0079ecac);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gSmackerCurrentPalette, 0x0079eca8);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gSmackerBackPalette, 0x0079ec9c);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gSmackerPrevBackPalette, 0x0079eca0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_material*, gSmackMaterials, 15, 0x0079ec60);

#define SMACKER_INTERPOLATE
#define SMACKER_ANTIALIASING

//tS32 gLast_demo_end_anim = -90000;

//// IDA: void __usercall ShowCutScene(int pIndex@<EAX>, int pWait_end@<EDX>, int pSound_ID@<EBX>, br_scalar pDelay)
//void ShowCutScene(int pIndex, int pWait_end, int pSound_ID, br_scalar pDelay) {
//    LOG_TRACE("(%d, %d, %d, %f)", pIndex, pWait_end, pSound_ID, pDelay);
//
//    gProgram_state.cut_scene = 1;
//    if (pSound_ID >= 0) {
//        DRS3LoadSound(pSound_ID);
//        SetFlicSound(pSound_ID, PDGetTotalTime() + 1000.f * pDelay);
//    }
//    SetNonFatalAllocationErrors();
//    RunFlic(pIndex);
//    ResetNonFatalAllocationErrors();
//    if (pWait_end) {
//        WaitForAKey();
//    } else {
//        WaitForNoKeys();
//    }
//    FadePaletteDown();
//    ClearEntireScreen();
//    if (pSound_ID >= 0) {
//        DRS3ReleaseSound(pSound_ID);
//        SetFlicSound(0, 0);
//    }
//    gProgram_state.cut_scene = 0;
//}
//
//// IDA: void __cdecl DoSCILogo()
//void DoSCILogo() {
//}
//
//// IDA: void __cdecl DoStainlessLogo()
//void DoStainlessLogo() {
//    LOG_TRACE("()");
//}
//

C2_HOOK_VARIABLE_IMPLEMENT(void*, gSmackerBuffer, 0x0067c3b0);

void (C2_HOOK_FASTCALL * PlaySmackerFile_original)(const char* pSmack_name);
void C2_HOOK_FASTCALL PlaySmackerFile(const char* pSmack_name) {
    C2_HOOK_START();
#if 0 //defined(C2_HOOKS_ENABLED)
    PlaySmackerFile_original(pSmack_name);
#else
#if USE_SMACKW32
    int mati;
    int i;
    int j;
    int k;
    int currentFrame;
    int timeStart, timeEnd;
    int paletteChangeCount;
    int frameCount;
    int frameStart;
    char* palette_pixels;
    int vertex_start;
    int face_start;
    int smackBufferFlags;
    int smackBlitFlags;
    tPath_name the_path;
    HSMACK smackHandle;
    HSMACKBLIT smackBlitHandle;
    int paletteChanged;
    br_pixelmap* tmpPalette;
    br_scalar l, r;
    br_scalar b, u;
    br_uint_32 paletteColour;
    int keyPressed;
    int buffer_start;
    int current_buffer_start;
    char* material_pixels;
    br_uint_8* current_buffer;

    smackBlitHandle = NULL;
    paletteChangeCount = 0;
    frameCount = 0;
    if (C2V(gSound_override)) {
        return;
    }
    if (C2V(gNoCutscenes)) {
        return;
    }
    if (C2V(gReal_back_screen)->type == BR_PMT_INDEX_8) {
        smackBufferFlags = 0;
        palette_pixels = C2V(gCurrent_palette)->pixels;
    } else if (C2V(gReal_back_screen)->type == BR_PMT_RGB_555) {
        smackBufferFlags = SMACKBUFFER555;
        smackBlitFlags = SMACKBUFFER555 | SMACKBLIT2X;
    } else if (C2V(gReal_back_screen)->type == BR_PMT_RGB_565) {
        smackBufferFlags = SMACKBUFFER565;
        smackBlitFlags = SMACKBUFFER565 | SMACKBLIT2X;
    } else {
        PDEnterDebugger("Unexpected backscreen format for Smacker player");
        return;
    }

#define SMACK_WIDTH     320
#define SMACK_HEIGHT    200

    if (C2V(gSmackerBuffer) == NULL) {
        C2V(gSmackerBuffer) = BrMemAllocate(SMACK_WIDTH * SMACK_HEIGHT, kMem_misc);
    }

    StopMusic();
    FadePaletteDown();
    ClearEntireScreen();

    SmackUseMMX(1);
    SmackSoundUseDirectSound(NULL);

    PathCat(the_path, C2V(gApplication_path), "CUTSCENE");
    PathCat(the_path, the_path, pSmack_name);
    dr_dprintf("Trying to open smack file '%s'", the_path);

    smackHandle = SmackOpen(the_path, SMACKTRACKS, SMACKPRELOADALL);
    if (smackHandle == NULL) {
        smackHandle = SmackOpen(the_path, SMACKTRACKS, -1);
    }
    if (smackHandle == NULL) {
        dr_dprintf("Unable to open smack file - attempt to load smack from CD...");
        if (!PDReadSourceLocation(the_path)) {
            dr_dprintf("Can't get CD directory name");
        } else {
            if (the_path[strlen(the_path) - 1] != C2V(gDir_separator)[0]) {
                strcat(the_path, C2V(gDir_separator));
            }
            strcat(the_path, "DATA");
            PathCat(the_path, the_path, "CUTSCENE");
            PathCat(the_path, the_path, pSmack_name);
            dr_dprintf("Got CD path: '%s'", the_path);
            if (PDCheckDriveExists(the_path)) {
                smackHandle = SmackOpen(the_path, SMACKTRACKS, SMACKPRELOADALL);
                if (smackHandle == NULL) {
                    smackHandle = SmackOpen(the_path, SMACKTRACKS, -1);
                }
            }
        }
    }
    if (smackHandle == NULL) {
        dr_dprintf("Smack file '%s' failed to open", pSmack_name);
        PDServiceInput();
        return;
    }
    dr_dprintf("Smack file opened OK");
    timeStart = PDGetTotalTime();
    if (C2V(gNbPixelBits) == 16) {
        C2V(gSmackerActor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        C2V(gSmackerActor)->render_style = BR_RSTYLE_FACES;
        C2V(gSmackerCurrentPalette) = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, NULL, 0);
        C2V(gSmackerBackPalette) = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, NULL, 0);
        BrMapAdd(C2V(gSmackerCurrentPalette));

#define SMACKER_NMATERIALS 15
#define SMACKER_NVERTICES (4 * SMACKER_NMATERIALS)  // 60
#define SMACKER_NFACES (2 * SMACKER_NMATERIALS)  // 30

        C2V(gSmackerModel) = BrModelAllocate("Smack Model", SMACKER_NVERTICES, SMACKER_NFACES);
        if (C2V(gSmackerModel) == NULL) {
            PDEnterDebugger("No Smack model");
            BrActorFree(C2V(gSmackerActor));
            C2V(gSmackerActor) = NULL;
            BrMapRemove(C2V(gSmackerCurrentPalette));
            BrPixelmapFree(C2V(gSmackerCurrentPalette));
            goto load_failed;
        }
        for (i = 0; i < REC2_ASIZE(C2V(gSmackMaterials)); i++) {
            C2V(gSmackMaterials)[i] = BrMaterialAllocate("Devious Smack Material");
            if (C2V(gSmackMaterials)[i] == NULL) {
                PDEnterDebugger("No Smack material");
                BrActorFree(C2V(gSmackerActor));
                C2V(gSmackerActor) = NULL;
                BrMapRemove(C2V(gSmackerCurrentPalette));
                BrPixelmapFree(C2V(gSmackerCurrentPalette));
                goto load_failed;
            }
            C2V(gSmackMaterials)[i]->colour_map = BrPixelmapAllocate(BR_PMT_INDEX_8, 64, 64, NULL, 0);
            C2V(gSmackMaterials)[i]->colour_map->map = C2V(gSmackerCurrentPalette);
            C2V(gSmackMaterials)[i]->flags = BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT;
#if defined(SMACKER_INTERPOLATE)
            C2V(gSmackMaterials)[i]->flags |= BR_MATF_MAP_INTERPOLATION;
#endif
#if defined(SMACKER_ANTIALIASING)
            C2V(gSmackMaterials)[i]->flags |= BR_MATF_MAP_ANTIALIASING;
#endif
            BrMapAdd(C2V(gSmackMaterials)[i]->colour_map->map);
            BrMapAdd(C2V(gSmackMaterials)[i]->colour_map);
            BrMaterialAdd(C2V(gSmackMaterials)[i]);
        }
        for (mati = 0, i = 0, vertex_start = 0, face_start = 0; mati < REC2_ASIZE(C2V(gSmackMaterials)); i++) {
            b = (float)(-128 * i - 48);
            u = (float)(-128 * (i + 1) - 48);
            for (j = 0; j < 5; mati++, j++, vertex_start += 4, face_start += 2) {  // 5 == SMACK_WIDTH / 64
                l = (float)(128 * j);
                r = (float)(128 * (j + 1));
                C2V(gSmackerModel)->vertices[vertex_start + 0].p.v[0] = l;
                C2V(gSmackerModel)->vertices[vertex_start + 0].p.v[1] = u;
                C2V(gSmackerModel)->vertices[vertex_start + 0].p.v[2] = -2.f;
                C2V(gSmackerModel)->vertices[vertex_start + 0].map.v[0] = 0.0078125f;
                C2V(gSmackerModel)->vertices[vertex_start + 0].map.v[1] = 0.9921875f;

                C2V(gSmackerModel)->vertices[vertex_start + 1].p.v[0] = r;
                C2V(gSmackerModel)->vertices[vertex_start + 1].p.v[1] = u;
                C2V(gSmackerModel)->vertices[vertex_start + 1].p.v[2] = -2.f;
                C2V(gSmackerModel)->vertices[vertex_start + 1].map.v[0] = 0.9921875f;
                C2V(gSmackerModel)->vertices[vertex_start + 1].map.v[1] = 0.9921875f;

                C2V(gSmackerModel)->vertices[vertex_start + 2].p.v[0] = r;
                C2V(gSmackerModel)->vertices[vertex_start + 2].p.v[1] = b;
                C2V(gSmackerModel)->vertices[vertex_start + 2].p.v[2] = -2.f;
                C2V(gSmackerModel)->vertices[vertex_start + 2].map.v[0] = 0.9921875f;
                C2V(gSmackerModel)->vertices[vertex_start + 2].map.v[1] = 0.0078125f;

                C2V(gSmackerModel)->vertices[vertex_start + 3].p.v[0] = l;
                C2V(gSmackerModel)->vertices[vertex_start + 3].p.v[1] = b;
                C2V(gSmackerModel)->vertices[vertex_start + 3].p.v[2] = -2.f;
                C2V(gSmackerModel)->vertices[vertex_start + 3].map.v[0] = 0.0078125f;
                C2V(gSmackerModel)->vertices[vertex_start + 3].map.v[1] = 0.0078125f;

                C2V(gSmackerModel)->faces[face_start + 0].vertices[0] = vertex_start + 0;
                C2V(gSmackerModel)->faces[face_start + 0].vertices[1] = vertex_start + 3;
                C2V(gSmackerModel)->faces[face_start + 0].vertices[2] = vertex_start + 1;
                C2V(gSmackerModel)->faces[face_start + 0].smoothing = 0;
                C2V(gSmackerModel)->faces[face_start + 0].material = C2V(gSmackMaterials)[mati];
                C2V(gSmackerModel)->faces[face_start + 0].flags = 0;

                C2V(gSmackerModel)->faces[face_start + 1].vertices[0] = vertex_start + 1;
                C2V(gSmackerModel)->faces[face_start + 1].vertices[1] = vertex_start + 3;
                C2V(gSmackerModel)->faces[face_start + 1].vertices[2] = vertex_start + 2;
                C2V(gSmackerModel)->faces[face_start + 1].smoothing = 0;
                C2V(gSmackerModel)->faces[face_start + 1].material = C2V(gSmackMaterials)[mati];
                C2V(gSmackerModel)->faces[face_start + 1].flags = 0;
            }
        }
        BrModelAdd(C2V(gSmackerModel));
        C2V(gSmackerActor)->model = C2V(gSmackerModel);
        BrActorAdd(C2V(g2d_camera), C2V(gSmackerActor));
load_failed:
        if (C2V(gSmackerActor) != NULL) {
            if (SmackBufferOpen(C2V(gHWnd), SMACKFRAMERATE, SMACK_WIDTH, SMACK_HEIGHT, SMACK_WIDTH, SMACK_WIDTH) == 0) {
                PDEnterDebugger("sbuf");
            }
            SmackToBuffer(smackHandle, 0, 0, SMACK_WIDTH, SMACK_WIDTH, C2V(gSmackerBuffer), 0);
            if (smackHandle->Frames != 0) {
                for (currentFrame = 1; currentFrame != smackHandle->Frames; currentFrame++) {
                    frameStart = PDGetTotalTime();
                    if (smackHandle->NewPalette) {
                        BrMapRemove(C2V(gSmackerCurrentPalette));
                        C2V(gSmackerPrevBackPalette) = C2V(gSmackerBackPalette);
                        tmpPalette = C2V(gSmackerCurrentPalette);
                        C2V(gSmackerCurrentPalette) = C2V(gSmackerBackPalette);
                        C2V(gSmackerBackPalette) = tmpPalette;
                        for (i = 0; i < 256; i++) {
                            paletteColour = BR_COLOUR_RGB(smackHandle->Palette[3 * i], smackHandle->Palette[3 * i + 1],
                                                          smackHandle->Palette[3 * i + 2]);
                            if (paletteColour == 0) {
                                paletteColour = BR_COLOUR_RGB(4, 4, 4);
                            }
                            ((br_uint_32 *) C2V(gSmackerCurrentPalette)->pixels)[i] = paletteColour;
                        }
                        BrMapAdd(C2V(gSmackerCurrentPalette));
                        for (i = 0; i < REC2_ASIZE(C2V(gSmackMaterials)); i++) {
                            C2V(gSmackMaterials)[i]->colour_map->map = C2V(gSmackerCurrentPalette);
                            BrMapUpdate(C2V(gSmackMaterials)[i]->colour_map->map, BR_MAPU_ALL);
                            BrMapUpdate(C2V(gSmackMaterials)[i]->colour_map, BR_MAPU_ALL);
                            BrMaterialUpdate(C2V(gSmackMaterials)[i], BR_MATU_ALL);
                        }
                    }
                    SmackDoFrame(smackHandle);
                    BrPixelmapFill(C2V(gDepth_buffer), -1);
                    for (i = 0, buffer_start = 0; i < REC2_ASIZE(C2V(gSmackMaterials)); buffer_start += 64 * SMACK_WIDTH) {
                        current_buffer_start = buffer_start;
                        for (j = 0; j < 5; j++, current_buffer_start += 64, i++) {
                            material_pixels = C2V(gSmackMaterials)[i]->colour_map->pixels;
                            current_buffer = (br_uint_8 *) C2V(gSmackerBuffer) + current_buffer_start;
                            for (k = 0; k < 64; k++) {
                                c2_memcpy(material_pixels + 64 * k, current_buffer + 320 * k, 64);
                            }
                            BrMapUpdate(C2V(gSmackMaterials)[i]->colour_map, BR_MAPU_ALL);
                            BrMaterialUpdate(C2V(gSmackMaterials)[i], BR_MATU_ALL);
                        }
                    }
                    BrZbSceneRender(C2V(g2d_camera), C2V(g2d_camera), C2V(gReal_back_screen), C2V(gDepth_buffer));
                    SwapBackScreen();
                    PDServiceInput();
                    if (currentFrame != smackHandle->Frames) {
                        SmackNextFrame(smackHandle);
                    }
                    keyPressed = 0;
                    while (SmackWait(smackHandle) && PDGetTotalTime() < frameStart + 67) {
                        keyPressed = AnyKeyDown();
                        if (keyPressed) {
                            break;
                        }
                    }
                    if (keyPressed) {
                        break;
                    }
                }
            }
            if (C2V(gSmackerActor) != NULL) {
                for (i = 0; i < REC2_ASIZE(C2V(gSmackMaterials)); i++) {
                    BrMapRemove(C2V(gSmackMaterials)[i]->colour_map);
                    BrPixelmapFree(C2V(gSmackMaterials)[i]->colour_map);
                    C2V(gSmackMaterials)[i]->colour_map = NULL;
                    BrMaterialRemove(C2V(gSmackMaterials)[i]);
                    C2V(gSmackMaterials)[i] = NULL;
                }
                BrModelRemove(C2V(gSmackerModel));
                BrActorRemove(C2V(gSmackerActor));
                BrModelFree(C2V(gSmackerModel));
                BrActorFree(C2V(gSmackerActor));
            }
        }
        FadePaletteDown();
        ClearEntireScreen();
    } else if (C2V(gReal_back_screen)->type == BR_PMT_RGB_565 || C2V(gReal_back_screen)->type == BR_PMT_RGB_555) {
        smackBlitHandle = SmackBlitOpen(smackBlitFlags);
        if (smackBlitHandle != NULL) {
            if (smackHandle->Frames != 0) {
                for (currentFrame = 1; currentFrame != smackHandle->Frames; currentFrame++) {
                    frameStart = PDGetTotalTime();
                    frameCount++;
                    if (C2V(gReal_back_screen)->type == BR_PMT_RGB_565 ||
                        C2V(gReal_back_screen)->type == BR_PMT_RGB_555) {
                        SmackToBuffer(smackHandle, 0, 0, 320, 200, C2V(gSmackerBuffer), 0);
                    } else {
                        SmackToBuffer(smackHandle, 0, 0, C2V(gReal_back_screen)->row_bytes,
                                      C2V(gReal_back_screen)->height, C2V(gReal_back_screen)->pixels, smackBufferFlags);
                    }
                    paletteChanged = 0;
                    if (smackHandle->NewPalette) {
                        paletteChanged = 1;
                        paletteChangeCount += 1;
                        if (C2V(gReal_back_screen)->type == BR_PMT_RGB_565 ||
                            C2V(gReal_back_screen)->type == BR_PMT_RGB_555) {
                            SmackBlitSetPalette(smackBlitHandle, smackHandle->Palette, smackHandle->PalType);
                        } else {
                            for (i = 0; i < 256; i++) {
                                ((br_uint_32 *) palette_pixels)[i] = BR_COLOUR_RGB(smackHandle->Palette[3 * i + 0],
                                                                                   smackHandle->Palette[3 * i + 1],
                                                                                   smackHandle->Palette[3 * i + 2]);
                            }
                        }
                    }
                    SmackDoFrame(smackHandle);
                    if (C2V(gReal_back_screen)->type == BR_PMT_RGB_565 ||
                        C2V(gReal_back_screen)->type == BR_PMT_RGB_555) {
                        SmackBlit(smackBlitHandle, C2V(gReal_back_screen)->pixels, C2V(gReal_back_screen)->row_bytes, 0,
                                  40, C2V(gSmackerBuffer), 320, 0, 0, 320, 200);
                    }
                    if (currentFrame != smackHandle->Frames) {
                        SmackNextFrame(smackHandle);
                    }
                    if (paletteChanged) {
                        DRSetPalette(C2V(gCurrent_palette));
                    }
                    SwapBackScreen();
                    PDServiceInput();
                    keyPressed = 0;
                    while (SmackWait(smackHandle) && PDGetTotalTime() < frameStart + 67) {
                        keyPressed = AnyKeyDown();
                        if (keyPressed) {
                            break;
                        }
                    }
                    if (keyPressed) {
                        break;
                    }
                }
            }
            FadePaletteDown();
            ClearEntireScreen();
            SmackBlitClose(smackBlitHandle);
        }
    }
    if (smackHandle != NULL) {
        SmackClose(smackHandle);
    }
    timeEnd = PDGetTotalTime();
    dr_dprintf("Frames %d, time %d, pal changes %d, pal changes per sec %f, av fps %f",
        frameCount, 1000 * (timeEnd - timeStart), paletteChangeCount,
        (float)paletteChangeCount / (timeEnd - timeStart), (float)frameCount / (timeEnd - timeStart));
    PDServiceInput();
#else
#error "Not implemented"
#endif
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0043e770, PlaySmackerFile, PlaySmackerFile_original)

void C2_HOOK_FASTCALL DoOpeningAnimation(void) {
    PlaySmackerFile("INTERPLAYGO.SMK");
    PlaySmackerFile("MIX_INTR.SMK");
    WaitForNoKeys();
}
C2_HOOK_FUNCTION(0x0043f330, DoOpeningAnimation)

//// IDA: void __cdecl DoNewGameAnimation()
//void DoNewGameAnimation() {
//    LOG_TRACE("()");
//}
//
//// IDA: void __cdecl DoGoToRaceAnimation()
//void DoGoToRaceAnimation() {
//    LOG_TRACE("()");
//
//    if (!gNet_mode) {
//        if (PercentageChance(50)) {
//            PlaySmackerFile("GARAGE2.SMK");
//        } else {
//            PlaySmackerFile("GARAGE1.SMK");
//        }
//    }
//}
//
//// IDA: void __cdecl DoEndRaceAnimation()
//void DoEndRaceAnimation() {
//    int made_a_profit;
//    int went_up_a_rank;
//    LOG_TRACE("()");
//
//    made_a_profit = gProgram_state.credits_earned >= gProgram_state.credits_lost;
//    went_up_a_rank = gProgram_state.credits_earned >= gProgram_state.credits_per_rank;
//
//    FadePaletteDown();
//
//    if (gAusterity_mode || gNet_mode != eNet_mode_none) {
//        return;
//    }
//    if (gProgram_state.credits + gProgram_state.credits_earned - gProgram_state.credits_lost >= 0) {
//        if (made_a_profit && went_up_a_rank) {
//            PlaySmackerFile("SUCCESS.SMK");
//        } else if (made_a_profit || went_up_a_rank) {
//            PlaySmackerFile("MUNDANE.SMK");
//        } else {
//            PlaySmackerFile("UNSUCSES.SMK");
//        }
//    }
//}
//
//// IDA: void __cdecl DoGameOverAnimation()
//void DoGameOverAnimation() {
//    LOG_TRACE("()");
//
//    StopMusic();
//    PlaySmackerFile("CRASH.SMK");
//    StartMusic();
//}
//
//// IDA: void __cdecl DoGameCompletedAnimation()
//void DoGameCompletedAnimation() {
//    LOG_TRACE("()");
//
//    StopMusic();
//    PlaySmackerFile("TOPRANK.SMK");
//    StartMusic();
//}
//
//void DoFeatureUnavailableInDemo() {
//    LOG_TRACE("()");
//
//    PrintMemoryDump(0, "BEFORE DEMO-ONLY SCREEN");
//
//    SuspendPendingFlic();
//    FadePaletteDown();
//    ShowCutScene(7, 1, 8502, gCut_delay_3);
//    FadePaletteDown();
//
//    PrintMemoryDump(0, "AFTER DEMO-ONLY SCREEN");
//}
//
//void DoFullVersionPowerpoint() {
//    LOG_TRACE("()");
//
//    FadePaletteDown();
//    DRSetPalette(gRender_palette);
//    ShowCutScene(9, 0, 8503, gCut_delay_4);
//    FadePaletteDown();
//
//    gLast_demo_end_anim = PDGetTotalTime();
//}
//
//void DoDemoGoodbye() {
//    if (PDGetTotalTime() - gLast_demo_end_anim > 90000) {
//        DoFullVersionPowerpoint();
//    }
//}
//
//// IDA: void __cdecl StartLoadingScreen()
//void StartLoadingScreen() {
//    LOG_TRACE("()");
//
//    PossibleService();
//    if (gProgram_state.sausage_eater_mode) {
//        SplashScreenWith(harness_game_info.defines.GERMAN_LOADSCRN);
//    } else {
//        SplashScreenWith("LOADSCRN.PIX");
//    }
//}
