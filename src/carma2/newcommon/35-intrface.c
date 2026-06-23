#include "35-intrface.h"

// GLOBAL: CARMA2_HW 0x0068c1f0
int gAlways_typing;

// FUNCTION: CARMA2_HW 0x00484620
void C2_HOOK_FASTCALL SetAlwaysTyping(void) {

    gAlways_typing = 1;
}

// ClearAlwaysTyping

// ChoiceDisabled

// ResetInterfaceTimeout

// ChangeSelection

// RecopyAreas

// DoInterfaceScreen

// WhichItemIsSelectedIn

// SelectThisItemIn

