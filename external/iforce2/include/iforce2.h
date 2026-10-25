/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef IFORCE2_H
#define IFORCE2_H

#include <windows.h>
#include <dinput.h>

#define IFR_DECL __stdcall

typedef struct {
    char *name;
    char *pop_periodic;
    CLSID clsid;
    int count_contained;
    CLSID contained;
    int effect_valid;
    void *effect_data;
} IFR_ProjectEffect;

typedef struct {
    IDirectInputDevice2 *device;
    int count_effects;
    IFR_ProjectEffect *effects;
} IFR_Project;

typedef struct {
    GUID guid;
    DIEFFECT *di_effect;
} IFR_Effect;

IFR_Effect ** IFR_DECL IFCreateEffectStructs(IFR_Project *project, const char *name, int *valid);
IDirectInputEffect ** IFR_DECL IFCreateEffects(IFR_Project *project, const char *name, int *count);
IFR_Project * IFR_DECL IFLoadProjectFile(const char *path, IDirectInputDevice2A *device);
IFR_Project * IFR_DECL IFLoadProjectObjectPointer(const void *data, IDirectInputDevice2A *device);
IFR_Project * IFR_DECL IFLoadProjectPointer(const void *data, IDirectInputDevice2A *device);
IFR_Project * IFR_DECL IFLoadProjectResource(HMODULE module, const char *name, IDirectInputDevice2A *device);
int IFR_DECL IFReleaseEffectStructs(IFR_Project *project, IFR_Effect **effects);
int IFR_DECL IFReleaseEffects(IFR_Project *project, IDirectInputEffect **effects);
int IFR_DECL IFReleaseProject(IFR_Project *project);
int IFR_DECL IForceDLLMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);

#endif /* IFORCE2_H */
