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

// CompleteRace

extern int C2_HOOK_FASTCALL AbortRace(tFrontend_spec* pFrontend);

// F4Key

// SetFlag

// FinishRace

extern void C2_HOOK_FASTCALL EnsureSpecialVolumesHidden(void);

extern void C2_HOOK_FASTCALL ShowSpecialVolumesIfRequ(void);

// DoEditModeKey

// F5Key

// F6Key

// F7Key

// F8Key

// F10Key

// F11Key

extern void C2_HOOK_FASTCALL F12Key(void);

// NumberKey0

// NumberKey1

// NumberKey2

// NumberKey3

// NumberKey4

// NumberKey5

// NumberKey6

// NumberKey7

// NumberKey8

// NumberKey9

extern void C2_HOOK_FASTCALL LookLeft(void);

// LookForward

// LookRight

extern void C2_HOOK_FASTCALL DamageTest(void);

extern void C2_HOOK_FASTCALL TDamageDriver(void);

// ToggleMiniMap

// CheckToggles

// CarWorldOffFallenCheckThingy

// HasCarFallenOffWorld

// CheckForBeingOutOfThisWorld

// CheckHornLocal

// CheckHorn3D

// CheckHorns

// SetQuickRecovery

// SetRecovery

// RecoverCar

// CheckMapRenderMove

// ExplodeCar

// CheckRecoveryOfCars

// TradeInAPO

// CheckOtherRacingKeys

// CheckRecoverCost

// SortOutRecover

// SetFlipUpCar

extern void C2_HOOK_FASTCALL AlignChildren(tPhysics_object* pCollision_info);

// FlipUpCar

// GetPowerup

// CheckSystemKeys

// CheckForCheatingGits

// CheckKevKeys

// BrakeInstantly

// PollCarControls

// PollCameraControls

// SetFlag2

// ToggleFlying

// CycleInvulnerability

extern void C2_HOOK_FASTCALL ToggleTimerFreeze(void);

extern void C2_HOOK_FASTCALL EarnDosh(void);

extern void C2_HOOK_FASTCALL LoseDosh(void);

extern void C2_HOOK_FASTCALL ToggleHeadupMap(int pArg);

// HackyMapFixForMac3dfx

// ToggleMap

extern int C2_HOOK_FASTCALL GetRecoverVoucherCount(void);

// AddVouchers

extern void C2_HOOK_FASTCALL ResetRecoveryVouchers(void);

extern void C2_HOOK_FASTCALL CycleYonFactor(void);

extern void C2_HOOK_FASTCALL CycleSoundDetailLevel(void);

extern void C2_HOOK_FASTCALL CycleCarSimplificationLevel(void);

extern void C2_HOOK_FASTCALL ToggleAccessoryRendering(void);

// UserSendMessage

// EnterUserMessage

// DisplayUserMessage

extern void C2_HOOK_FASTCALL InitAbuseomatic(void);

extern void C2_HOOK_FASTCALL DisposeAbuseomatic(void);

// ChangeCameraTypeInGame

// SteelBollock

// RichardsSphere

// ToggleHeadupLevel

#endif // GUARD_53_CONTROLS_H
