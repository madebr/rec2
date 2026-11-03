/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_53_CONTROLS_H
#define GUARD_53_CONTROLS_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gAllow_car_flying;
extern int gEntering_message;
extern int gCheckpoint_finder_enabled;
extern float gMap_render_x;
extern float gMap_render_y;
extern float gMap_render_width;
extern float gMap_render_height;
extern int gMap_trans;
extern int gHeadup_map_x;
extern int gHeadup_map_y;
extern int gHeadup_map_w;
extern int gHeadup_map_h;
extern int gRecovery_voucher_count;

extern float C2_HOOK_FASTCALL RepairCar(tU16 pCar_ID, tU32 pFrame_period, br_scalar* pTotal_deflection);

extern tRace_over_reason gRace_over_reason;

extern void C2_HOOK_FASTCALL CompleteRace(void);

extern int C2_HOOK_FASTCALL AbortRace(tFrontend_spec* pFrontend);

extern void C2_HOOK_FASTCALL FUN_00511210(void);

extern void C2_HOOK_FASTCALL FUN_00511200(void);

extern void C2_HOOK_FASTCALL ToggleJoystickHeadup(void);

extern void C2_HOOK_FASTCALL F4Key(void);

extern void C2_HOOK_FASTCALL SetFlag(int i);

extern void C2_HOOK_FASTCALL FinishRace(int pArg);

extern void C2_HOOK_FASTCALL EnsureSpecialVolumesHidden(void);

extern void C2_HOOK_FASTCALL ShowSpecialVolumesIfRequ(void);

// DoEditModeKey

extern void C2_HOOK_FASTCALL F5Key(void);

extern void C2_HOOK_FASTCALL F6Key(void);

extern void C2_HOOK_FASTCALL F7Key(void);

extern void C2_HOOK_FASTCALL F8Key(void);

extern void C2_HOOK_FASTCALL F10Key(void);

extern void C2_HOOK_FASTCALL F11Key(void);

extern void C2_HOOK_FASTCALL F12Key(void);

extern void C2_HOOK_FASTCALL NumberKey0(void);

extern void C2_HOOK_FASTCALL NumberKey1(void);

extern void C2_HOOK_FASTCALL NumberKey2(void);

extern void C2_HOOK_FASTCALL NumberKey3(void);

extern void C2_HOOK_FASTCALL NumberKey4(void);

extern void C2_HOOK_FASTCALL NumberKey5(void);

extern void C2_HOOK_FASTCALL NumberKey6(void);

extern void C2_HOOK_FASTCALL NumberKey7(void);

extern void C2_HOOK_FASTCALL NumberKey8(void);

extern void C2_HOOK_FASTCALL NumberKey9(void);

extern void C2_HOOK_FASTCALL LookLeft(void);

extern void C2_HOOK_FASTCALL LookForward(void);

extern void C2_HOOK_FASTCALL LookRight(void);

extern void C2_HOOK_FASTCALL DamageTest(void);

extern void C2_HOOK_FASTCALL TDamageEngine(void);

extern void C2_HOOK_FASTCALL TDamageDriver(void);

extern void C2_HOOK_FASTCALL TDamageTrans(void);

extern void C2_HOOK_FASTCALL TDamageLFWheel(void);

extern void C2_HOOK_FASTCALL ToggleMiniMap(void);

// CheckToggles

extern int C2_HOOK_FASTCALL CarWorldOffFallenCheckThingy(tCar_spec* pCar, int pCheck_around);

extern int C2_HOOK_FASTCALL HasCarFallenOffWorld(tCar_spec* pCar);

// CheckForBeingOutOfThisWorld

// CheckHornLocal

extern void C2_HOOK_FASTCALL CheckHorn3D(tCar_spec* pCar);

// CheckHorns

// SetQuickRecovery

// SetRecovery

// RecoverCar

// CheckMapRenderMove

// ExplodeCar

// CheckRecoveryOfCars

// TradeInAPO

// CheckOtherRacingKeys

extern int C2_HOOK_FASTCALL CheckRecoverCost(void);

// SortOutRecover

extern void C2_HOOK_FASTCALL SetFlipUpCar(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL AlignChildren(tPhysics_object* pCollision_info);

extern void C2_HOOK_FASTCALL FlipUpCar(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL GetPowerup(int pNum);

// CheckSystemKeys

extern void C2_HOOK_FASTCALL CheckForCheatingGits(void);

// CheckKevKeys

// BrakeInstantly

// PollCarControls

extern void C2_HOOK_FASTCALL PollCameraControls(tU32 pCamera_period);

extern void C2_HOOK_FASTCALL SetFlag2(int pArg);

extern void C2_HOOK_FASTCALL ToggleFlying(void);

extern void C2_HOOK_FASTCALL CycleInvulnerability(void);

extern void C2_HOOK_FASTCALL ToggleTimerFreeze(void);

extern void C2_HOOK_FASTCALL EarnDosh(void);

extern void C2_HOOK_FASTCALL LoseDosh(void);

extern void C2_HOOK_FASTCALL ToggleHeadupMap(int pArg);

// HackyMapFixForMac3dfx

extern void C2_HOOK_FASTCALL ToggleMap(void);

extern int C2_HOOK_FASTCALL GetRecoverVoucherCount(void);

extern void C2_HOOK_FASTCALL AddVouchers(int pCount);

extern void C2_HOOK_FASTCALL ResetRecoveryVouchers(void);

extern void C2_HOOK_FASTCALL CycleYonFactor(void);

extern void C2_HOOK_FASTCALL CycleSoundDetailLevel(void);

extern void C2_HOOK_FASTCALL CycleCarSimplificationLevel(void);

extern void C2_HOOK_FASTCALL ToggleAccessoryRendering(void);

extern void C2_HOOK_FASTCALL UserSendMessage(void);

// EnterUserMessage

extern void C2_HOOK_FASTCALL DisplayUserMessage(void);

extern void C2_HOOK_FASTCALL InitAbuseomatic(void);

extern void C2_HOOK_FASTCALL DisposeAbuseomatic(void);

extern void C2_HOOK_FASTCALL ChangeCameraTypeInGame(void);

// SteelBollock

extern void C2_HOOK_FASTCALL RichardsSphere(int pNum);

extern void C2_HOOK_FASTCALL ToggleHeadupLevel(void);

extern void C2_HOOK_FASTCALL ToggleShellModel(void);

extern int C2_HOOK_FASTCALL FUN_0045a7f0(char* pFilename, int* pArg2);

extern void C2_HOOK_STDCALL CreateAccentPolyActor(float pX, float pY, float pWidth, float pHeight);

extern void C2_HOOK_FASTCALL FUN_0045acf0(void);

extern void C2_HOOK_FASTCALL FUN_0045c240(void);

extern void C2_HOOK_FASTCALL RightInventory(void);

#endif // GUARD_53_CONTROLS_H
