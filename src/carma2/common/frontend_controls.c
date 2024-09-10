#include "frontend_controls.h"

#include "frontend.h"
#include "frontend_quit.h"
#include "joystick.h"
#include "loading.h"
#include "options.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_CONTROLS, 0x00604940, {
    FIXME TODO
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_controls_count_keys, 0x006864f8);
C2_HOOK_VARIABLE_IMPLEMENT(tFrontend_slider, gControls_slider_1, 0x00686f68);
C2_HOOK_VARIABLE_IMPLEMENT(tFrontend_slider, gControls_slider_2, 0x006883c8);
C2_HOOK_VARIABLE_IMPLEMENT(tFrontend_slider, gControls_slider_3, 0x00688ac0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(int, gFrontend_controls_indices, 15, 0x00604900, {
    12, 13, 17, 18, 22, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
});


void C2_HOOK_FASTCALL DisplayJoystickSettings(tFrontend_spec *pFrontend) {

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gFrontend_controls_indices)) != 15);

    if (C2V(gJoystick_index) != -1) {
        int i;

        pFrontend->items[10].unlitFont = 1;
        pFrontend->items[10].highFont = 1;
        pFrontend->items[11].unlitFont = 0;
        pFrontend->items[11].highFont = 0;
        C2V(gControls_slider_1).flags = 0;
        C2V(gControls_slider_2).flags = 0;
        C2V(gControls_slider_3).flags = 0;

        for (i = 0; i < 3; i++) {
            pFrontend->items[ 1 + i].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[ 4 + i].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[ 7 + i].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[14 + i].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[19 + i].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[23 + i].enabled = kFrontendItemEnabled_enabled;
        }
        for (i = 0; i < REC2_ASIZE(C2V(gFrontend_controls_indices)); i++) {

            pFrontend->items[C2V(gFrontend_controls_indices)[i]].enabled = kFrontendItemEnabled_enabled;
        }
        pFrontend->items[36].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[37].enabled = kFrontendItemEnabled_enabled;
    } else {
        int i;

        pFrontend->items[10].unlitFont = 0;
        pFrontend->items[10].highFont = 0;
        pFrontend->items[11].unlitFont = 1;
        pFrontend->items[11].highFont = 1;
        C2V(gControls_slider_1).flags = 1;
        C2V(gControls_slider_2).flags = 1;
        C2V(gControls_slider_3).flags = 1;

        for (i = 0; i < 3; i++) {
            pFrontend->items[ 1 + i].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[ 4 + i].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[ 7 + i].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[14 + i].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[19 + i].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[23 + i].enabled = kFrontendItemEnabled_disabled;
        }
        for (i = 0; i < REC2_ASIZE(C2V(gFrontend_controls_indices)); i++) {

            pFrontend->items[C2V(gFrontend_controls_indices)[i]].enabled = kFrontendItemEnabled_disabled;
        }
        pFrontend->items[36].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[37].enabled = kFrontendItemEnabled_disabled;
    }

    C2V(gControls_slider_1).value = C2V(gControls_slider_1).field_0x14 + GetJoystickX() / (C2V(gControls_slider_1).field_0x18 - C2V(gControls_slider_1).field_0x14);
    C2V(gControls_slider_2).value = C2V(gControls_slider_2).field_0x14 + GetJoystickY() / (C2V(gControls_slider_2).field_0x18 - C2V(gControls_slider_2).field_0x14);
    C2V(gControls_slider_3).value = 0.01f * (float)GetJoystickFBBGain();
    if (IsJoystickDPadEnabled()) {
        pFrontend->items[36].unlitFont = 1;
        pFrontend->items[36].highFont = 1;
        pFrontend->items[37].unlitFont = 0;
        pFrontend->items[37].highFont = 0;
    } else {
        pFrontend->items[36].unlitFont = 0;
        pFrontend->items[36].highFont = 0;
        pFrontend->items[37].unlitFont = 1;
        pFrontend->items[37].highFont = 1;
    }
}
C2_HOOK_FUNCTION(0x00472210, DisplayJoystickSettings)

void C2_HOOK_FASTCALL Slider_XProc(tFrontend_slider* pSlider) {

    SetJoystickX((pSlider->field_0x18 - pSlider->field_0x14) * pSlider->value);
}
C2_HOOK_FUNCTION(0x004729d0, Slider_XProc)

void C2_HOOK_FASTCALL Slider_YProc(tFrontend_slider* pSlider) {

    SetJoystickY((pSlider->field_0x18 - pSlider->field_0x14) * pSlider->value);
}
C2_HOOK_FUNCTION(0x004729f0, Slider_YProc)
