#include "35-intrface.h"

// GLOBAL: CARMA2_HW 0x0068c1f0
int gAlways_typing;

// GLOBAL: CARMA2_HW 0x0068c1ec
int gDisabled_count;

// GLOBAL: CARMA2_HW 0x0068c1f8
int gDisabled_choices[10];

// FUNCTION: CARMA2_HW 0x00484620
void C2_HOOK_FASTCALL SetAlwaysTyping(void) {

    gAlways_typing = 1;
}

// FUNCTION: CARMA2_HW 0x00484630
void C2_HOOK_FASTCALL ClearAlwaysTyping(void) {

    gAlways_typing = 0;
}

int C2_HOOK_FASTCALL ChoiceDisabled(int pChoice) {
    int i;

    for (i = 0; i < gDisabled_count; ++i) {
        if (gDisabled_choices[i] == pChoice) {
            return 1;
        }
    }
    return 0;
}

// ResetInterfaceTimeout

// FUNCTION: CARMA2_HW 0x00484dd0
void C2_HOOK_FASTCALL ChangeSelection(const tInterface_spec* pSpec, int* pOld_selection, int* pNew_selection, int pMode, int pSkip_disabled) {

    if (ChoiceDisabled(*pNew_selection)) {
        if (pSkip_disabled) {
            if (*pOld_selection < *pNew_selection) {
                do {
                    *pNew_selection += 1;
                    if (*pNew_selection < pSpec->move_up_min[pMode]) {
                        *pNew_selection = pSpec->move_up_max[pMode];
                    }
                    if (*pNew_selection > pSpec->move_up_max[pMode]) {
                        *pNew_selection = pSpec->move_up_min[pMode];
                    }
                } while (*pNew_selection != *pOld_selection && ChoiceDisabled(*pNew_selection));
            } else {
                do {
                    *pNew_selection -= 1;
                    if (*pNew_selection < pSpec->move_up_min[pMode]) {
                        *pNew_selection = pSpec->move_up_max[pMode];
                    }
                    if (*pNew_selection > pSpec->move_up_max[pMode]) {
                        *pNew_selection = pSpec->move_up_min[pMode];
                    }
                } while (*pNew_selection != *pOld_selection && ChoiceDisabled(*pNew_selection));
            }
        } else {
            *pNew_selection = *pOld_selection;
        }
    }

    if (*pOld_selection != *pNew_selection) {
        *pOld_selection = *pNew_selection;
    }
}

// RecopyAreas

// DoInterfaceScreen

// WhichItemIsSelectedIn

// SelectThisItemIn

