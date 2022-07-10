#ifndef REC2_LEXER_H
#define REC2_LEXER_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void C2_HOOK_CDECL lexerError(br_lexer* l, char* string);

br_lexer* C2_HOOK_CDECL BrLexerAllocate(br_lexer_keyword* keywords, int nkeywords);

char C2_HOOK_CDECL BrLexerCommentSet(br_lexer* l, char eol_comment);

br_lexer_error_cbfn* C2_HOOK_CDECL BrLexerErrorSet(br_lexer* l, br_lexer_error_cbfn* error);

void C2_HOOK_CDECL BrLexerFree(br_lexer* l);

void C2_HOOK_STDCALL fileGetchar(br_lexer_source* source);

br_error C2_HOOK_CDECL BrLexerPushFile(br_lexer* l, char* file);

void C2_HOOK_STDCALL stringGetchar(br_lexer_source* source);

br_error C2_HOOK_CDECL BrLexerPushString(br_lexer* l, char* string, char* name);

br_lexer_source* C2_HOOK_CDECL BrLexerPop(br_lexer* l);

void C2_HOOK_STDCALL lexerAdvance(br_lexer* l);

void C2_HOOK_STDCALL lexerAdvanceDump(br_lexer* l);

br_error C2_HOOK_CDECL BrLexerDumpSet(br_lexer* l, br_putline_cbfn* putline, void* putline_arg);

void C2_HOOK_CDECL BrLexerTokenError(br_lexer* l, br_lexer_token_id t);

void C2_HOOK_CDECL BrLexerPosition(br_lexer* l, char* buf, br_size_t buf_size);

br_fixed_ls C2_HOOK_CDECL BrParseFixed(br_lexer* l);

br_float C2_HOOK_CDECL BrParseFloat(br_lexer* l);

br_int_32 C2_HOOK_CDECL BrParseInteger(br_lexer* l);

br_int_32 C2_HOOK_CDECL BrParseVectorFixed(br_lexer* l, br_fixed_ls* v, br_int_32 max);

br_int_32 C2_HOOK_CDECL BrParseVectorFloat(br_lexer* l, br_float* v, br_int_32 max);

br_int_32 C2_HOOK_CDECL BrParseMatrixFixed(br_lexer* l, br_fixed_ls* m, br_int_32 width, br_int_32 max_h);

br_int_32 C2_HOOK_CDECL BrParseMatrixFloat(br_lexer* l, br_float* m, br_int_32 width, br_int_32 max_h);

#endif // REC2_LEXER_H
