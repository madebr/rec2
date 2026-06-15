#ifndef GUARD_53_CONTROLS_H
#define GUARD_53_CONTROLS_H

#include "c2_hooks.h"

extern float gMap_render_x;
extern float gMap_render_y;
extern float gMap_render_width;
extern float gMap_render_height;
extern int gMap_trans;
extern int gHeadup_map_x;
extern int gHeadup_map_y;
extern int gHeadup_map_w;
extern int gHeadup_map_h;

// CompleteRace

// AbortRace

// F4Key

// SetFlag

// FinishRace

// EnsureSpecialVolumesHidden

// ShowSpecialVolumesIfRequ

// DoEditModeKey

// F5Key

// F6Key

// F7Key

// F8Key

// F10Key

// F11Key

// F12Key

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

// LookLeft

// LookForward

// LookRight

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

// AlignChildren

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

// ToggleTimerFreeze

// EarnDosh

// LoseDosh

// ToggleHeadupMap

// HackyMapFixForMac3dfx

// ToggleMap

// GetRecoverVoucherCount

// AddVouchers

// ResetRecoveryVouchers

// CycleYonFactor

// CycleSoundDetailLevel

// CycleCarSimplificationLevel

// ToggleAccessoryRendering

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
