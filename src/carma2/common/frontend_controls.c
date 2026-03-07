#include "frontend_controls.h"

#include "frontend.h"
#include "frontend_options.h"
#include "frontend_quit.h"
#include "input.h"
#include "joystick.h"
#include "loading.h"
#include "main.h"
#include "options.h"
#include "platform.h"
#include "sound.h"

#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x00604940
tFrontend_spec gFrontend_CONTROLS = {
    "Controls",
    0,
    68,
    Controls_Infunc,
    Controls_Outfunc,
    Generic_MenuHandler,
    &gFrontend_OPTIONS,
    0,
    0,
    0,
    7,
    0,
    0,
    {
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 1, 0, },
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x56,  Controls_JoystickToggle,       NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x57,  Controls_JoystickToggle,       NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x59,  temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 0, 1, },
        { 0x5b,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 1, 1, },
        { 0x402, temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 0, 0, },
        { 0x402, temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 0, 0, },
        { 0x5a,  temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, },
        { 0x5c,  temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, },
        { 0x402, temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 1, 1, },
        { 0x402, temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 0, 0, },
        { 0x402, temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 0, 0, },
        { 0x63,  temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, },
        { 0x402, temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 1, 1, },
        { 0x402, temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 0, 0, },
        { 0x402, temp,                          NULL,       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 0, 0, },
        { 0x61,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x5f,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x62,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x61,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x5f,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x62,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x61,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x60,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x62,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xef,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf1,  Controls_JoystickDpadToggle,   NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf2,  Controls_JoystickDpadToggle,   NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x68,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf3,  Controls_SwitchKeymapSet,      NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf4,  Controls_SwitchKeymapSet,      NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf5,  Controls_SwitchKeymapSet,      NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xf6,  Controls_SwitchKeymapSet,      NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x404, Controls_SlotActivated,        NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403, Controls_KeyUp,                NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x403, Controls_KeyDown,              NULL,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xee,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0xf0,  temp,                          NULL,       0, 17, 18, 0, 0, 0, 0, 0, 1, },
        { 0x7,   Controls_Ok,                   (void*)1,   0, 0,  0,  0, 0, 0, 0, 1, 1, },
    },
};

// GLOBAL: CARMA2_HW 0x006864f8
int gFrontend_controls_count_keys;

// GLOBAL: CARMA2_HW 0x00686f68
tFrontend_slider gControls_slider_1;

// GLOBAL: CARMA2_HW 0x006883c8
tFrontend_slider gControls_slider_2;

// GLOBAL: CARMA2_HW 0x00688ac0
tFrontend_slider gControls_slider_3;

// GLOBAL: CARMA2_HW 0x00604900
int gFrontend_controls_indices[15] = {
    12, 13, 17, 18, 22, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
};

// GLOBAL: CARMA2_HW 0x00688748
int gINT_00688748;


// FUNCTION: CARMA2_HW 0x00472210
void C2_HOOK_FASTCALL DisplayJoystickSettings(tFrontend_spec *pFrontend) {

    C2_HOOK_BUG_ON(REC2_ASIZE(gFrontend_controls_indices) != 15);

    if (gJoystick_index != -1) {
        int i;

        pFrontend->items[10].unlitFont = 1;
        pFrontend->items[10].highFont = 1;
        pFrontend->items[11].unlitFont = 0;
        pFrontend->items[11].highFont = 0;
        gControls_slider_1.flags = 0;
        gControls_slider_2.flags = 0;
        gControls_slider_3.flags = 0;

        for (i = 0; i < 3; i++) {
            pFrontend->items[ 1 + i].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[ 4 + i].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[ 7 + i].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[14 + i].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[19 + i].enabled = kFrontendItemEnabled_enabled;
            pFrontend->items[23 + i].enabled = kFrontendItemEnabled_enabled;
        }
        for (i = 0; i < REC2_ASIZE(gFrontend_controls_indices); i++) {

            pFrontend->items[gFrontend_controls_indices[i]].enabled = kFrontendItemEnabled_enabled;
        }
        pFrontend->items[36].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[37].enabled = kFrontendItemEnabled_enabled;
    } else {
        int i;

        pFrontend->items[10].unlitFont = 0;
        pFrontend->items[10].highFont = 0;
        pFrontend->items[11].unlitFont = 1;
        pFrontend->items[11].highFont = 1;
        gControls_slider_1.flags = 1;
        gControls_slider_2.flags = 1;
        gControls_slider_3.flags = 1;

        for (i = 0; i < 3; i++) {
            pFrontend->items[ 1 + i].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[ 4 + i].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[ 7 + i].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[14 + i].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[19 + i].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[23 + i].enabled = kFrontendItemEnabled_disabled;
        }
        for (i = 0; i < REC2_ASIZE(gFrontend_controls_indices); i++) {

            pFrontend->items[gFrontend_controls_indices[i]].enabled = kFrontendItemEnabled_disabled;
        }
        pFrontend->items[36].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[37].enabled = kFrontendItemEnabled_disabled;
    }

    gControls_slider_1.value = gControls_slider_1.field_0x14 + GetJoystickX() / (gControls_slider_1.field_0x18 - gControls_slider_1.field_0x14);
    gControls_slider_2.value = gControls_slider_2.field_0x14 + GetJoystickY() / (gControls_slider_2.field_0x18 - gControls_slider_2.field_0x14);
    gControls_slider_3.value = 0.01f * (float)GetJoystickFBBGain();
    if (PDIsJoystickDPadEnabled()) {
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

// FUNCTION: CARMA2_HW 0x004729d0
void C2_HOOK_FASTCALL Slider_XProc(tFrontend_slider* pSlider) {

    SetJoystickX((pSlider->field_0x18 - pSlider->field_0x14) * pSlider->value);
}

// FUNCTION: CARMA2_HW 0x004729f0
void C2_HOOK_FASTCALL Slider_YProc(tFrontend_slider* pSlider) {

    SetJoystickY((pSlider->field_0x18 - pSlider->field_0x14) * pSlider->value);
}

// FUNCTION: CARMA2_HW 0x00472a10
void C2_HOOK_FASTCALL Slider_ForceProc(tFrontend_slider* pSlider) {

    SetJoystickFFBGain((int)(100.f * pSlider->value));
}

// FUNCTION: CARMA2_HW 0x004725f0
int C2_HOOK_FASTCALL Controls_Infunc(tFrontend_spec* pFrontend) {

    Generic_Infunc(pFrontend);
    LoadKeyNames();
    gFrontend_controls_count_keys = 29;
    BackupKeyMappings();
    Joystick_BackupSettings();

    gJoystick_index = 0;

    SelectThisItemIn(pFrontend, 1, 39 + gKey_map_index);

    gControls_scroller.field_0x0 = gFrontend_controls_count_keys;
    gControls_scroller.range_length = 10;
    gControls_scroller.field_0x8 = 0;
    gControls_scroller.count_ranges = 2;
    gControls_scroller.range_starts[0] = 43;
    gControls_scroller.range_starts[1] = 53;
    gControls_scroller.next = NULL;
    gConnected_items = &gControls_scroller;

    gControls_slider_1.flags = 0;
    gControls_slider_1.itemid_left_reference = 14;
    gControls_slider_1.itemid_start = 1;
    gControls_slider_1.width = 236;
    gControls_slider_1.callback = Slider_XProc;
    gControls_slider_1.field_0x14 = 0.f;
    gControls_slider_1.field_0x18 = 2.f;
    gControls_slider_1.next = &gControls_slider_2;

    gControls_slider_2.flags = 0;
    gControls_slider_2.itemid_left_reference = 19;
    gControls_slider_2.itemid_start = 4;
    gControls_slider_2.width = 236;
    gControls_slider_2.callback = Slider_YProc;
    gControls_slider_2.field_0x14 = 0.f;
    gControls_slider_2.field_0x18 = 2.f;
    gControls_slider_2.next = &gControls_slider_3;

    gControls_slider_3.flags = 0;
    gControls_slider_3.itemid_left_reference = 23;
    gControls_slider_3.itemid_start = 7;
    gControls_slider_3.width = 236;
    gControls_slider_3.callback = Slider_ForceProc;
    gControls_slider_3.field_0x14 = 0.f;
    gControls_slider_3.field_0x18 = 1.f;
    gControls_slider_3.next = NULL;

    DisplayJoystickSettings(pFrontend);
    RefreshScrollSet(pFrontend);
    return 1;
}

// FUNCTION: CARMA2_HW 0x00472a30
int C2_HOOK_FASTCALL Controls_Outfunc(tFrontend_spec* pFrontend) {

    Generic_Outfunc(pFrontend);
    DisposeKeyNames();
    SaveKeyMapping();
    return 1;
}

// FUNCTION: CARMA2_HW 0x00472400
int C2_HOOK_FASTCALL Controls_JoystickDpadToggle(tFrontend_spec* pFrontend) {

    if (gFrontend_selected_item_index == 36) {
        SetJoystickDPadEnabled(1);
    } else {
        SetJoystickDPadEnabled(0);
    }
    DisplayJoystickSettings(pFrontend);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045bd90
void C2_HOOK_FASTCALL SaveAllJoystickData(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00472440
int C2_HOOK_FASTCALL Controls_SwitchKeymapSet(tFrontend_spec* pFrontend) {

    ChangeKeyMapIndex(gFrontend_selected_item_index - 39);
    SelectThisItemIn(pFrontend, 2, gKey_map_index + 39);
    RefreshScrollSet(pFrontend);
    FuckWithWidths(pFrontend);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00472b00
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
    model = gFrontend_brender_items[0].model;
    dy = (float)(17 * (key_array_index - gControls_scroller.field_0x8));
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

            for (i = 28; i < REC2_ASIZE(gKey_mapping); i++) {

                if (gKey_mapping[i] == key && i != gControls_frontend_to_key_mapping_lut[key_array_index]) {
                    int j;

                    for (j = 0; j < gFrontend_controls_count_keys; j++) {

                        if (i == gControls_frontend_to_key_mapping_lut[j]) {
                            gKey_mapping[i] = -2;
                            match = j;
                            break;
                        }
                    }
                    if (match < 0) {
                        DRS3StartSound(gEffects_outlet, eSoundId_CantAffordPart);
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
            DRS3StartSound(gEffects_outlet, eSoundId_Done);
            WaitForNoKeys();
            if (key != 63 && key != -1) {
                gKey_mapping[gControls_frontend_to_key_mapping_lut[key_array_index]] = key;
            }
            RefreshScrollSet(pFrontend);
            return 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004725a0
int C2_HOOK_FASTCALL Controls_KeyUp(tFrontend_spec* pFrontend) {

    if (gControls_scroller.field_0x8 != 0) {
        gControls_scroller.field_0x8 -= 1;
        RefreshScrollSet(pFrontend);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004725c0
int C2_HOOK_FASTCALL Controls_KeyDown(tFrontend_spec* pFrontend) {

    if (gControls_scroller.field_0x8 < gControls_scroller.field_0x0 - gControls_scroller.range_length) {
        gControls_scroller.field_0x8 += 1;
        RefreshScrollSet(pFrontend);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00472d80
int C2_HOOK_FASTCALL Controls_Ok(tFrontend_spec* pFrontend) {
    int i;
    int prev_item;

    gINT_00688748 = -1;
    WaitForNoKeys();

    for (i = 28; i < REC2_ASIZE(gKey_mapping); i++) {

        if (gKey_mapping[i] == -2) {
            gINT_00688748 = i;
            break;
        }
    }
    if (gJoystick_index == -1) {

        for (i = 28; i < REC2_ASIZE(gKey_mapping); i++) {

            if (gKey_mapping[i] >= 107 && gKey_mapping[i] <= 142) {
                gINT_00688748 = i;
            }
        }
    }
    if (gINT_00688748 == -1) {
        SaveAllJoystickData();
        return 0;
    }
    for (i = 0; i < gFrontend_controls_count_keys; i++) {
        if (gControls_frontend_to_key_mapping_lut[i] == gINT_00688748) {
            break;
        }
    }
    ScrollSet_DisplayEntry(&gControls_scroller, i);
    RefreshScrollSet(pFrontend);
    prev_item = gFrontend_selected_item_index;
    if (i < gControls_scroller.field_0x8 || i > gControls_scroller.field_0x8 + gControls_scroller.range_length) {
        gFrontend_selected_item_index = -1;
    } else {
        gFrontend_selected_item_index = i - gControls_scroller.field_0x8 + gControls_scroller.range_starts[0];
    }
    if (gFrontend_selected_item_index == -1) {
        PDFatalError("Screwed up in Controls_Ok().");
    }
    Controls_SlotActivated(pFrontend);
    gFrontend_selected_item_index = prev_item;
    return 4;
}

// FUNCTION: CARMA2_HW 0x004721e0
int C2_HOOK_FASTCALL Controls_JoystickToggle(tFrontend_spec* pFrontend) {

    if (gFrontend_selected_item_index == 10) {
        EnableJoysticks();
    } else {
        DisableJoysticks();
    }
    DisplayJoystickSettings(pFrontend);
    return 0;
}
