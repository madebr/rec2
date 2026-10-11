/*
 * Runtime tests for the actual SDL3 platform input implementation.
 * Compile this file together with src/carma2/platform/sdl3/sdl3_input.c,
 * the repository headers, and SDL3 >= 3.2.0. No physical controller is needed.
 * Tests use SDL virtual gamepads and feed real SDL events to the same dispatcher
 * used by SDL3ServiceMessages. Shared application globals are provided below;
 * gamepad APIs, device handles, event generation, and sampling use real SDL.
 * The test requires an otherwise empty controller inventory for isolation.
 * Haptic hardware gain cannot be exercised with SDL virtual gamepads.
 *
 * Example from a modern MSVC x86 developer prompt, with the official SDL3 VC
 * development archive extracted to SDL_SDK (put its lib/x86 on PATH to run):
 * cl /W3 /WX /O2 /D_CRT_SECURE_NO_WARNINGS /I%SDL_SDK%/include
 *   /Isrc/carma2/platform /Isrc/carma2/include /Isrc/carma2/hooks
 *   /Isrc/brender/include /Isrc/carma2/newcommon /Isrc/carma2/s3/include
 *   scripts/verify_sdl3_joystick.c src/carma2/platform/sdl3/sdl3_input.c
 *   /Fobuild-audit/joystick/ /Febuild-audit/joystick/verify_sdl3_joystick.exe
 *   /link %SDL_SDK%/lib/x86/SDL3.lib
 * Join the command lines above, then run the resulting executable.
 */
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#include "sdl3/sdl3.h"
#include "sdl3/sdl3_input.h"
#include "platform.h"
#include "joystick.h"

#include <stdio.h>
#include <stdlib.h>

tJoystick_info g_Joystick_infos[16];
int g_Count_Joystick_infos;
int gJoystick_index = -1;
float gJoystick_x_steering = 1.f;
float gJoystick_y_throttle = 1.f;
tU32 gKeys_pressed;

#define CHECK(condition) do { if (!(condition)) { fprintf(stderr, "FAIL line %d: %s (%s)\n", __LINE__, #condition, SDL_GetError()); exit(1); } } while (0)

void SDL3ServiceMessages(void) {
    SDL_Event event;

    SDL_PumpEvents();
    while (SDL_PollEvent(&event)) {
        SDL3HandleGamepadEvent(&event);
    }
}

static SDL_JoystickID attach_gamepad(const char* pName) {
    SDL_VirtualJoystickDesc description;
    SDL_JoystickID id;

    SDL_INIT_INTERFACE(&description);
    description.type = SDL_JOYSTICK_TYPE_GAMEPAD;
    description.name = pName;
    description.naxes = 2;
    description.nbuttons = 4;
    description.axis_mask = (1 << SDL_GAMEPAD_AXIS_LEFTX) | (1 << SDL_GAMEPAD_AXIS_LEFTY);
    description.button_mask = (1 << SDL_GAMEPAD_BUTTON_SOUTH) | (1 << SDL_GAMEPAD_BUTTON_EAST)
        | (1 << SDL_GAMEPAD_BUTTON_WEST) | (1 << SDL_GAMEPAD_BUTTON_NORTH);
    id = SDL_AttachVirtualJoystick(&description);
    CHECK(id != 0);
    CHECK(SDL_IsGamepad(id));
    return id;
}

int main(void) {
    SDL_JoystickID first;
    SDL_JoystickID second;
    SDL_JoystickID devices[17];
    SDL_Joystick* joystick;
    tButtonJoystickInfo* preferences;
    SDL_Event malformed;
    int keys[151] = { 0 };
    SDL_JoystickID* initial_devices;
    int i;

    CHECK(SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1"));
    CHECK(SDL_Init(SDL_INIT_GAMEPAD));
    initial_devices = SDL_GetGamepads(&i);
    CHECK(initial_devices != NULL);
    CHECK(i == 0);
    SDL_free(initial_devices);
    first = attach_gamepad("rec2 virtual first");
    /* Early registration before haptic initialization is valid and idempotent. */
    SDL3ServiceMessages();
    CHECK(g_Count_Joystick_infos == 1);
    preferences = PDGetCurrentJoystickData();
    CHECK(preferences != NULL);
    preferences->field_0xd8 = 3.f;
    PDSetJoystickFFBGain(25);
    CHECK(g_Joystick_infos[0].ffb_gain == 25);
    PDInitJoysticks();
    PDInitJoysticks();
    CHECK(g_Count_Joystick_infos == 1);
    CHECK(PDGetCurrentJoystickData()->field_0xd8 == 3.f);
    CHECK(g_Joystick_infos[0].ffb_gain == 25);
    CHECK(SDL_strcmp(GetCurrentJoystickName(), "rec2 virtual first") == 0);
    CHECK(PDGetCurrentJoystickData()->count_buttons == 4);
    CHECK(PDGetCurrentJoystickData()->buttons[0] == 0);
    CHECK(PDGetCurrentJoystickData()->buttons[4] == -1);
    CHECK(PDGetCurrentJoystickData()->field_0xd4 == 1);
    CHECK(PDGetCurrentJoystickData()->field_0xe4 == 50);

    second = attach_gamepad("rec2 virtual second");
    SDL3ServiceMessages();
    CHECK(g_Count_Joystick_infos == 2);
    CHECK(g_Joystick_infos[0].which == first);
    CHECK(g_Joystick_infos[1].which == second);
    joystick = SDL_GetGamepadJoystick(g_Joystick_infos[0].gamepad);
    CHECK(SDL_SetJoystickVirtualAxis(joystick, 0, -32768));
    CHECK(SDL_SetJoystickVirtualAxis(joystick, 1, 32767));
    CHECK(SDL_SetJoystickVirtualButton(joystick, 0, true));
    joystick = SDL_GetGamepadJoystick(g_Joystick_infos[1].gamepad);
    CHECK(SDL_SetJoystickVirtualAxis(joystick, 0, 12345));
    CHECK(SDL_SetJoystickVirtualAxis(joystick, 1, -12345));
    PDReadJoysticks();
    SDL3ServiceMessages();
    CHECK(PDGetJoy1X() == 0);
    CHECK(PDGetJoy1Y() == 65535);
    CHECK(PDGetJoy2X() == 45113);
    CHECK(PDGetJoy2Y() == 20423);
    CHECK(PDGetJoy1Button1() == 1);
    CHECK(PDGetJoystickButtonStates() == 1);
    PDSetKeysFromJoystick(keys);
    CHECK(keys[107] == 1 && keys[108] == 0);
    gINT_00595f9c = 0;
    CHECK(PDGetJoy1Y() == -1);
    gINT_00595f9c = 1;
    gJoystick_x_steering = 2.f;
    CHECK(PDGetJoy1X() == 0);
    gJoystick_x_steering = 1.f;
    preferences->field_0xe0 = 1;
    CHECK(PDIsJoystickDPadEnabled() == 1);

    SDL_zero(malformed);
    malformed.type = SDL_EVENT_GAMEPAD_AXIS_MOTION;
    malformed.gaxis.which = first;
    malformed.gaxis.axis = SDL_GAMEPAD_AXIS_COUNT;
    malformed.gaxis.value = 17;
    CHECK(SDL3HandleGamepadEvent(&malformed));
    CHECK(PDGetJoy1X() == 0);
    malformed.type = SDL_EVENT_GAMEPAD_BUTTON_DOWN;
    malformed.gbutton.which = first;
    malformed.gbutton.button = 255;
    CHECK(SDL3HandleGamepadEvent(&malformed));
    CHECK(PDGetJoystickButtonStates() == 1);

    gJoystick_index = 1;
    g_Joystick_infos[1].preferences.buttons[0] = 19;
    CHECK(PDDisableJoysticks() == 2);
    CHECK(PDGetCurrentJoystickData() == NULL);
    CHECK(GetCurrentJoystickName() == NULL);
    CHECK(PDGetJoy1X() == -1 && PDGetJoy2Y() == -1);
    /* SaveJoystickPreferences temporarily selects each connected index even
     * while input stays disabled; metadata must remain accessible. */
    CHECK(PDIsJoystickAvailable(-1) == 0);
    CHECK(PDIsJoystickAvailable(0) == 1);
    CHECK(PDIsJoystickAvailable(1) == 1);
    CHECK(PDIsJoystickAvailable(2) == 0);
    for (i = 0; i < 16; i++) {
        if (PDIsJoystickAvailable(i)) {
            gJoystick_index = i;
            CHECK(PDGetCurrentJoystickData() == &g_Joystick_infos[i].preferences);
            CHECK(PDGetJoy1X() == -1);
            CHECK(PDGetJoystickButtonStates() == 0);
        }
    }
    gJoystick_index = 1;
    SDL_strlcpy(g_Joystick_infos[1].preferences.productName, "loaded preference alias", sizeof(g_Joystick_infos[1].preferences.productName));
    CHECK(SDL_strcmp(GetCurrentJoystickName(), "rec2 virtual second") == 0);
    gJoystick_index = -1;
    PDSetKeysFromJoystick(keys);
    CHECK(keys[107] == 0);
    CHECK(SDL_DetachVirtualJoystick(first));
    SDL3ServiceMessages();
    CHECK(g_Count_Joystick_infos == 1);
    CHECK(g_Joystick_infos[0].which == second);
    CHECK(g_Joystick_infos[0].preferences.buttons[0] == 19);
    CHECK(SDL_GetGamepadFromID(first) == NULL);
    CHECK(PDEnableJoysticks() == 1);
    CHECK(gJoystick_index == 0);
    CHECK(SDL_strcmp(GetCurrentJoystickName(), "rec2 virtual second") == 0);
    PDSetJoystickFFBGain(-20);
    CHECK(g_Joystick_infos[0].ffb_gain == 0);
    PDSetJoystickFFBGain(125);
    CHECK(g_Joystick_infos[0].ffb_gain == 100);
    CHECK(SDL_DetachVirtualJoystick(second));
    SDL3ServiceMessages();
    CHECK(g_Count_Joystick_infos == 0 && gJoystick_index == -1);
    CHECK(PDGetCurrentJoystickData() == NULL);
    CHECK(PDEnableJoysticks() == 0);

    /* The seventeenth virtual device must not overwrite the sixteen slots. */
    for (i = 0; i < 17; i++) {
        devices[i] = attach_gamepad("rec2 capacity test");
        SDL3ServiceMessages();
    }
    CHECK(g_Count_Joystick_infos == 16);
    CHECK(SDL_GetGamepadFromID(devices[16]) == NULL);
    CloseDirectInput();
    CloseDirectInput();
    CHECK(g_Count_Joystick_infos == 0 && gJoystick_index == -1);
    for (i = 0; i < 17; i++) {
        CHECK(SDL_GetGamepadFromID(devices[i]) == NULL);
        CHECK(SDL_DetachVirtualJoystick(devices[i]));
    }
    SDL3ServiceMessages();
    CHECK(g_Count_Joystick_infos == 0);
    SDL_Quit();
    puts("PASS: actual SDL virtual gamepads; initialization, events, sampling, preference identity, enable/disable, compaction, capacity, gain bounds, and handle cleanup");
    return 0;
}
