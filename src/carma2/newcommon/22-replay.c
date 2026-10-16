#include "22-replay.h"

#include "globvars.h"

// GLOBAL: CARMA2_HW 0x0058f620
int gAR_camera_type = 2;

// ActualActionReplayHeadups

// DoActionReplayPostSwap

// DoZappyActionReplayHeadups

// DoActionReplayHeadups

// PreProcess

// PostProcess

// DoActionReplay

// MoveToEndOfReplay

// MoveToStartOfReplay

// PollActionReplayControls

// CheckReplayTurnOn

// InitializeActionReplayPix

// FreeActionReplayPix

// SynchronizeActionReplay

// TurnOnActionReplay

// TurnOffActionReplay

// AfterActionReplay

// ToggleReplay

// DisplayMovieStatusPanel

// FUNCTION: CARMA2_HW 0x004022e0
void C2_HOOK_FASTCALL ApplyTime(tU32 pTime) {
    gLast_replay_frame_time = pTime;
}

// ARSetPause

