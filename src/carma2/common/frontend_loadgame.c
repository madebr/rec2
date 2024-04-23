#include "frontend_loadgame.h"

#include "frontend.h"
#include "frontend_main.h"
#include "frontend_options.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_LOAD_GAME, 0x005ed5a0, {
    "LoadGame",
    0,
    19,
    LoadGameInFunc,
    LoadGameOutFunc,
    LoadGameUpdateFunc,
    &C2V(gFrontend_MAIN),
    0,
    0,
    0,
    0,
    9,
    0,
    {
        { 0x18,     temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot1,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot2,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot3,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot4,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot5,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot6,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot7,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot8,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x4b,     temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0xff,     temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x4e,     temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x4f,     temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x100,    temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0xfe,     temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x8,      temp,               &C2V(gFrontend_OPTIONS),    0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadGameScrollUp,   NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadGameScrollDown, NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
    },
});
