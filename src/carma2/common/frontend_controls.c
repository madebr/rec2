#include "frontend_controls.h"

#include "frontend.h"
#include "frontend_quit.h"
#include "input.h"
#include "joystick.h"
#include "loading.h"
#include "main.h"
#include "options.h"
#include "platform.h"
#include "sound.h"

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
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_00688748, 0x00688748);


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

void C2_HOOK_FASTCALL Slider_ForceProc(tFrontend_slider* pSlider) {

    SetJoystickFFBGain((int)(100.f * pSlider->value));
}
C2_HOOK_FUNCTION(0x00472a10, Slider_ForceProc)

int C2_HOOK_FASTCALL Controls_Infunc(tFrontend_spec* pFrontend) {

    Generic_Infunc(pFrontend);
    LoadKeyNames();
    C2V(gFrontend_controls_count_keys) = 29;
    BackupKeyMappings();
    Joystick_BackupSettings();

    C2V(gJoystick_index) = 0;

    SelectThisItemIn(pFrontend, 1, 39 + C2V(gKey_map_index));

    C2V(gControls_scroller).field_0x0 = C2V(gFrontend_controls_count_keys);
    C2V(gControls_scroller).range_length = 10;
    C2V(gControls_scroller).field_0x8 = 0;
    C2V(gControls_scroller).count_ranges = 2;
    C2V(gControls_scroller).range_starts[0] = 43;
    C2V(gControls_scroller).range_starts[1] = 53;
    C2V(gControls_scroller).next = NULL;
    C2V(gConnected_items) = &C2V(gControls_scroller);

    C2V(gControls_slider_1).flags = 0;
    C2V(gControls_slider_1).itemid_left_reference = 14;
    C2V(gControls_slider_1).itemid_start = 1;
    C2V(gControls_slider_1).width = 236;
    C2V(gControls_slider_1).callback = Slider_XProc;
    C2V(gControls_slider_1).field_0x14 = 0.f;
    C2V(gControls_slider_1).field_0x18 = 2.f;
    C2V(gControls_slider_1).next = &C2V(gControls_slider_2);

    C2V(gControls_slider_2).flags = 0;
    C2V(gControls_slider_2).itemid_left_reference = 19;
    C2V(gControls_slider_2).itemid_start = 4;
    C2V(gControls_slider_2).width = 236;
    C2V(gControls_slider_2).callback = Slider_YProc;
    C2V(gControls_slider_2).field_0x14 = 0.f;
    C2V(gControls_slider_2).field_0x18 = 2.f;
    C2V(gControls_slider_2).next = &C2V(gControls_slider_3);

    C2V(gControls_slider_3).flags = 0;
    C2V(gControls_slider_3).itemid_left_reference = 23;
    C2V(gControls_slider_3).itemid_start = 7;
    C2V(gControls_slider_3).width = 236;
    C2V(gControls_slider_3).callback = Slider_ForceProc;
    C2V(gControls_slider_3).field_0x14 = 0.f;
    C2V(gControls_slider_3).field_0x18 = 1.f;
    C2V(gControls_slider_3).next = NULL;

    DisplayJoystickSettings(pFrontend);
    RefreshScrollSet(pFrontend);
    return 1;
}
C2_HOOK_FUNCTION(0x004725f0, Controls_Infunc)

int C2_HOOK_FASTCALL Controls_Outfunc(tFrontend_spec* pFrontend) {

    Generic_Outfunc(pFrontend);
    DisposeKeyNames();
    SaveKeyMapping();
    return 1;
}
C2_HOOK_FUNCTION(0x00472a30, Controls_Outfunc)

int C2_HOOK_FASTCALL Controls_JoystickDpadToggle(tFrontend_spec* pFrontend) {

    if (C2V(gFrontend_selected_item_index) == 36) {
        SetJoystickDPadEnabled(1);
    } else {
        SetJoystickDPadEnabled(0);
    }
    DisplayJoystickSettings(pFrontend);
    return 0;
}
C2_HOOK_FUNCTION(0x00472400, Controls_JoystickDpadToggle)

void (C2_HOOK_FASTCALL *SaveAllJoystickData_original)(void);
void C2_HOOK_FASTCALL SaveAllJoystickData(void) {

#if defined(C2_HOOKS_ENABLED)
    SaveAllJoystickData_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045bd90, SaveAllJoystickData, SaveAllJoystickData_original)

int C2_HOOK_FASTCALL Controls_SwitchKeyMapSet(tFrontend_spec* pFrontend) {

    ChangeKeyMapIndex(C2V(gFrontend_selected_item_index) - 39);
    SelectThisItemIn(pFrontend, 2, C2V(gKey_map_index) + 39);
    RefreshScrollSet(pFrontend);
    FuckWithWidths(pFrontend);
    return 0;
}
C2_HOOK_FUNCTION(0x00472440, Controls_SwitchKeyMapSet)

int C2_HOOK_FASTCALL Controls_SlotActivated(tFrontend_spec* pFrontend) {
    int key_array_index;
    float dy;
    br_model* model;
    int match;

    key_array_index = DetermineKeyArrayIndex();
    CyclePollKeys();
    PollKeys();
    WaitForNoKeys();
    pFrontend->items[0].visible = 1;
    model = C2V(gFrontend_brender_items)[0].model;
    dy = (float)(17 * (key_array_index - C2V(gControls_scroller).field_0x8));
    model->vertices[0].p.v[1] = model->vertices[3].p.v[1] = -(pFrontend->items[0].y + dy);
    model->vertices[1].p.v[1] = model->vertices[2].p.v[1] = -(pFrontend->items[0].y + dy + 20);
    BrModelUpdate(model, BR_MODU_VERTEX_POSITIONS);
    match = -1;
    for (;;) {
        int key;

        FRONTEND_Redraw();

        key = PDAnyKeyDown();
        if (key != -1 && key != 63) {
            int i;

            for (i = 28; i < REC2_ASIZE(C2V(gKey_mapping)); i++) {

                if (C2V(gKey_mapping)[i] == key && i != C2V(gControls_frontend_to_key_mapping_lut[key_array_index])) {
                    int j;

                    for (j = 0; j < C2V(gFrontend_controls_count_keys); j++) {

                        if (i == C2V(gControls_frontend_to_key_mapping_lut)[j]) {
                            C2V(gKey_mapping)[i] = -2;
                            match = j;
                            break;
                        }
                    }
                    if (match < 0) {
                        DRS3StartSound(C2V(gEffects_outlet), eSoundId_CantAffordPart);
                        key = -1;
                        break;
                    }
                }
            }
        }
        CyclePollKeys();
        PollKeys();
        ServiceGame();
        if (key != -1 || EitherMouseButtonDown()) {
            pFrontend->items[0].visible = 0;
            DRS3StartSound(C2V(gEffects_outlet), eSoundId_Done);
            WaitForNoKeys();
            if (key != 63 && key != -1) {
                C2V(gKey_mapping)[C2V(gControls_frontend_to_key_mapping_lut)[key_array_index]] = key;
            }
            RefreshScrollSet(pFrontend);
            return 0;
        }
    }
}
C2_HOOK_FUNCTION(0x00472b00, Controls_SlotActivated)

int C2_HOOK_FASTCALL Controls_KeyUp(tFrontend_spec* pFrontend) {

    if (C2V(gControls_scroller).field_0x8 != 0) {
        C2V(gControls_scroller).field_0x8 -= 1;
        RefreshScrollSet(pFrontend);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x004725a0, Controls_KeyUp)

int C2_HOOK_FASTCALL Controls_KeyDown(tFrontend_spec* pFrontend) {

    if (C2V(gControls_scroller).field_0x8 < C2V(gControls_scroller).field_0x0 - C2V(gControls_scroller).range_length) {
        C2V(gControls_scroller).field_0x8 += 1;
        RefreshScrollSet(pFrontend);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x004725c0, Controls_KeyDown)

int (C2_HOOK_FASTCALL * Controls_Ok_original)(tFrontend_spec* pFrontend);
int C2_HOOK_FASTCALL Controls_Ok(tFrontend_spec* pFrontend) {

#if 0//defined(C2_HOOKS_ENABLED)
    return Controls_Ok_original(pFrontend);
#else
    int i;
    int prev_item;

    C2V(gINT_00688748) = -1;
    WaitForNoKeys();

    for (i = 28; i < REC2_ASIZE(C2V(gKey_mapping)); i++) {

        if (C2V(gKey_mapping)[i] == -2) {
            C2V(gINT_00688748) = i;
            break;
        }
    }
    if (C2V(gJoystick_index) == -1) {

        for (i = 28; i < REC2_ASIZE(C2V(gKey_mapping)); i++) {

            if (C2V(gKey_mapping)[i] >= 107 && C2V(gKey_mapping)[i] <= 142) {
                C2V(gINT_00688748) = i;
            }
        }
    }
    if (C2V(gINT_00688748) == -1) {
        SaveAllJoystickData();
        return 0;
    }
    for (i = 0; i < C2V(gFrontend_controls_count_keys); i++) {
        if (C2V(gControls_frontend_to_key_mapping_lut)[i] == C2V(gINT_00688748)) {
            break;
        }
    }
    ScrollSet_DisplayEntry(&C2V(gControls_scroller), i);
    RefreshScrollSet(pFrontend);
    prev_item = C2V(gFrontend_selected_item_index);
    if (i < C2V(gControls_scroller).field_0x8 || i > C2V(gControls_scroller).field_0x8 + C2V(gControls_scroller).range_length) {
        C2V(gFrontend_selected_item_index) = -1;
    } else {
        C2V(gFrontend_selected_item_index) = i - C2V(gControls_scroller).field_0x8 + C2V(gControls_scroller).range_starts[0];
    }
    if (C2V(gFrontend_selected_item_index) == -1) {
        PDFatalError("Screwed up in Controls_Ok().");
    }
    Controls_SlotActivated(pFrontend);
    C2V(gFrontend_selected_item_index) = prev_item;
    return 4;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00472d80, Controls_Ok, Controls_Ok_original)

int C2_HOOK_FASTCALL Controls_JoystickToggle(tFrontend_spec* pFrontend) {

    if (C2V(gFrontend_selected_item_index) == 10) {
        EnableJoysticks();
    } else {
        DisableJoysticks();
    }
    DisplayJoystickSettings(pFrontend);
    return 0;
}
C2_HOOK_FUNCTION(0x004721e0, Controls_JoystickToggle)
