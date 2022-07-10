#ifndef REC2_TOKEN_H
#define REC2_TOKEN_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void C2_HOOK_CDECL BrTokenBegin(void);

br_token C2_HOOK_CDECL BrTokenCreate(char* identifier, br_token type);

char* C2_HOOK_CDECL BrTokenIdentifier(br_token t);

br_token C2_HOOK_CDECL BrTokenType(br_token t);

br_int_32 C2_HOOK_CDECL BrTokenCount(char* pattern);

br_token C2_HOOK_CDECL BrTokenFind(char* pattern);

br_int_32 C2_HOOK_CDECL BrTokenFindMany(char* pattern, br_token* tokens, br_int_32 max_tokens);

br_token C2_HOOK_CDECL BrTokenFindType(br_token* ptype, char* base, br_token* types, br_int_32 ntypes);

#endif // REC2_TOKEN_H
