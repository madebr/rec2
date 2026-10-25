/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_TOKEN_H
#define REC2_TOKEN_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void C2_HOOK_CDECL BrTokenBegin(void);

br_token C2_HOOK_CDECL BrTokenCreate(const char* identifier, br_token type);

char* C2_HOOK_CDECL BrTokenIdentifier(br_token t);

br_token C2_HOOK_CDECL BrTokenType(br_token t);

br_int_32 C2_HOOK_CDECL BrTokenCount(const char* pattern);

br_token C2_HOOK_CDECL BrTokenFind(const char* pattern);

br_int_32 C2_HOOK_CDECL BrTokenFindMany(const char* pattern, br_token* tokens, br_int_32 max_tokens);

br_token C2_HOOK_CDECL BrTokenFindType(br_token* ptype, const char* base, br_token* types, br_int_32 ntypes);

#endif // REC2_TOKEN_H