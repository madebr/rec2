#include "42-input.h"

// GLOBAL: CARMA2_HW 0x0074b5e0
int gKey_mapping[77];

// GLOBAL: CARMA2_HW 0x0068c1c4
int gEdge_trigger_mode;

// SetJoystickArrays

// STUB: CARMA2_HW 0x00481eb0
void C2_HOOK_FASTCALL PollKeys(void) {
    NOT_IMPLEMENTED();
}

// CyclePollKeys

// ResetPollKeys

// CheckKeysForMouldiness

// STUB: CARMA2_HW 0x004824c0
int C2_HOOK_FASTCALL EitherMouseButtonDown(void) {
    NOT_IMPLEMENTED();
}

// AnyModifiersDown

// EdgeTriggeryKey

// STUB: CARMA2_HW 0x00482550
int C2_HOOK_FASTCALL PDKeyDown(int pKey_index) {
    NOT_IMPLEMENTED();
    return 0;
}

// PDKeyDown3

// STUB: CARMA2_HW 0x00482a00
int C2_HOOK_FASTCALL PDAnyKeyDown(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00482d70
int C2_HOOK_FASTCALL AnyKeyDown(void) {
    NOT_IMPLEMENTED();
}

// KevKeyService

// STUB: CARMA2_HW 0x00483040
int C2_HOOK_FASTCALL KeyIsDown(int pKey_index) {
    NOT_IMPLEMENTED();
}

// KeyIsDownNoMouldiness

// STUB: CARMA2_HW 0x004833b0
void C2_HOOK_FASTCALL WaitForNoKeys(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00483c10
void C2_HOOK_FASTCALL GetMousePosition(int *pX, int *pY) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00483c90
void C2_HOOK_FASTCALL InitRollingLetters(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00483ce0
void C2_HOOK_FASTCALL EndRollingLetters(void) {
    NOT_IMPLEMENTED();
}

// AddRollingLetter

// ChangeCharTo

// STUB: CARMA2_HW 0x00484120
void C2_HOOK_FASTCALL RevertTyping(int pSlot_index, char* pRevert_str) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00484600
void C2_HOOK_FASTCALL EdgeTriggerModeOn(void) {

    gEdge_trigger_mode = 1;
}


// FUNCTION: CARMA2_HW 0x00484610
void C2_HOOK_FASTCALL EdgeTriggerModeOff(void) {

    gEdge_trigger_mode = 0;
}

