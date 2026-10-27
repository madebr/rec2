/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "72-interface-startgame.h"

#include "72-interface.h"


// GLOBAL: CARMA2_HW 0x0059c828
tFrontend_spec gFrontend_START_GAME = {
#if 0 // FIXME: enable this
    "StartGame",
    0,
    0,
    StartGameInfunc,
    NULL,
    FRONTEND_StartGameMenuHandler,
    NULL,
    0,
    0,
    0,
    0,
    0,
    0,
{ { 0 } }
#else
    0
#endif
};
