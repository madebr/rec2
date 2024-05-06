#include "pratcam.h"

void C2_HOOK_FASTCALL InitPratcam(void) {

}
C2_HOOK_FUNCTION(0x004e0e50, InitPratcam)

void C2_HOOK_FASTCALL DisposePratcam(void) {

}
C2_HOOK_FUNCTION(0x004e0e60, DisposePratcam)


void C2_HOOK_FASTCALL ChangeAmbientPratcam(int pIndex) {

}
C2_HOOK_FUNCTION(0x004e0e30, ChangeAmbientPratcam)

void C2_HOOK_FASTCALL PratcamEvent(int pIndex) {

}
C2_HOOK_FUNCTION(0x004e0e40, PratcamEvent)
