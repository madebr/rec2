/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef FRONTEND_OPTIONS_H
#define FRONTEND_OPTIONS_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern tFrontend_spec gFrontend_OPTIONS;
extern tFrontend_slider gOptions_sound_slider;


void C2_HOOK_FASTCALL Slider_EffectsProc(tFrontend_slider* pSlider);

void C2_HOOK_FASTCALL DisplayVolumeSettings(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Options_Infunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Options_Outfunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Options_AbortRace(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Options_AbortGame(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Options_MusicOn(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Options_MusicOff(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Options_Ok(tFrontend_spec* pFrontend);

#endif /* FRONTEND_OPTIONS_H */