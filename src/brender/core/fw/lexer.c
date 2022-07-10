#include "lexer.h"

#include "brprintf.h"
#include "file.h"
#include "fwsetup.h"
#include "resource.h"

#include "core/math/fixed.h"
#include "core/std/brstdlib.h"

#include "c2_stdlib.h"

void C2_HOOK_CDECL lexerError(br_lexer* l, char* string) {
}
C2_HOOK_FUNCTION(0x0052f3f0, lexerError)

br_lexer* C2_HOOK_CDECL BrLexerAllocate(br_lexer_keyword* keywords, int nkeywords) {
    br_lexer* l;

    l = BrResAllocate(C2V(fw).res, sizeof(br_lexer), BR_MEMORY_LEXER);
    if (l == NULL) {
        return NULL;
    }
    l->keywords = keywords;
    l->nkeywords = nkeywords;
    l->advance = lexerAdvance;
    l->source = NULL;
    l->string_buffer = BrResAllocate(l, 256, BR_MEMORY_LEXER);
    l->string_buffer_size = 256;
    l->error = lexerError;
    return l;
}
C2_HOOK_FUNCTION(0x0052f390, BrLexerAllocate)

char C2_HOOK_CDECL BrLexerCommentSet(br_lexer* l, char eol_comment) {
    char old;

    old = l->eol_comment;
    l->eol_comment = eol_comment;
    return old;
}
C2_HOOK_FUNCTION(0x0052f400, BrLexerCommentSet)

br_lexer_error_cbfn* C2_HOOK_CDECL BrLexerErrorSet(br_lexer* l, br_lexer_error_cbfn* error) {
    br_lexer_error_cbfn* old;

    old = l->error;
    if (error == NULL) {
        l->error = lexerError;
    } else {
        l->error = error;
    }
    return old == lexerError ? NULL : old;
}
C2_HOOK_FUNCTION(0x0052f410, BrLexerErrorSet)

void C2_HOOK_CDECL BrLexerFree(br_lexer* l) {

    BrResFree(l);
}
C2_HOOK_FUNCTION(0x0052f450, BrLexerFree)

void C2_HOOK_STDCALL fileGetchar(br_lexer_source* source) {

    source->next = BrFileGetChar(source->ptr);
}
C2_HOOK_FUNCTION(0x0052f4f0, fileGetchar)

br_error C2_HOOK_CDECL BrLexerPushFile(br_lexer* l, char* file) {
    br_lexer_source* s;
    void* f;
    int mode;

    mode = BR_FS_MODE_TEXT;
    f = BrFileOpenRead(file, 0, NULL, &mode);
    if (f == NULL) {
        return 0x1002;
    }
    s = BrResAllocate(l, sizeof(br_lexer_source), BR_MEMORY_LEXER);
    if (s == NULL) {
        return 0x1003;
    }
    BrResAdd(s, f);
    s->name = file;
    s->line = 1;
    s->getchar = fileGetchar;
    s->ptr = f;
    s->getchar(s);
    s->prev = l->source;
    l->source = s;
    return 0;
}
C2_HOOK_FUNCTION(0x0052f460, BrLexerPushFile)

void C2_HOOK_STDCALL stringGetchar(br_lexer_source* source) {
    char* cp;

    cp = source->ptr;
    if (cp != NULL && *cp != '\0') {
        source->next = *cp;
        source->ptr = cp + 1;
    } else {
        source->next = -1;
    }
}
C2_HOOK_FUNCTION(0x0052f570, stringGetchar)

br_error C2_HOOK_CDECL BrLexerPushString(br_lexer* l, char* string, char* name) {
    br_lexer_source* s;

    s = BrResAllocate(l, sizeof(br_lexer_source), BR_MEMORY_LEXER);
    if (s == NULL) {
        return 0x1003;
    }
    s->name = name;
    s->line = 1;
    s->getchar = stringGetchar;
    s->ptr = string;
    s->getchar(s);
    s->prev = l->source;
    l->source = s;
    return 0;
}
C2_HOOK_FUNCTION(0x0052f510, BrLexerPushString)

br_lexer_source* C2_HOOK_CDECL BrLexerPop(br_lexer* l) {
    br_lexer_source* s;

    s = l->source;
    if (s != NULL) {
        l->source = s->prev;
        BrResFree(s);
    }
    return s;
}
C2_HOOK_FUNCTION(0x0052f5a0, BrLexerPop)

void C2_HOOK_STDCALL lexerAdvance(br_lexer* l) {
    int n;
    br_boolean got_point;

    while (1) {
        if (l->source == NULL) {
            l->current.id = T_EOF;
            return;
        }
        if (l->source->next == -1) {
            BrLexerPop(l);
            continue;
        }
        if (l->eol_comment != '\0' && l->eol_comment == l->source->next) {
            l->source->getchar(l->source);
            while (l->source->next != '\n' && l->source->next != -1) {
                l->source->getchar(l->source);
            }
            continue;
        }
        if (l->source->next == '\n') {
            l->source->line++;
            l->source->getchar(l->source);
            continue;
        }
        if (l->source->next == '\"') {
            for (n = 0; n < l->string_buffer_size; n++) {
                l->source->getchar(l->source);
                l->string_buffer[n] = l->source->next;
                if ((l->source->next == '\"') || (l->source->next == '\n') || (l->source->next == -1)) {
                    break;
                }
            }
            if (l->source->next != '\"') {
                l->current.id = T_ERROR;
                l->current.v.string = "Unterminated string";
            } else {
                l->string_buffer[n] = '\0';
                l->current.id = T_STRING;
                l->current.v.string = l->string_buffer;
            }
            l->source->getchar(l->source);
            return;
        }
        if (BrIsSpace(l->source->next) || l->source->next == '\x1a') {
            l->source->getchar(l->source);
            continue;
        }
        if (BrIsAlpha(l->source->next) != 0 || l->source->next == '_') {
            l->string_buffer[0] = l->source->next;
            for (n = 1; n < l->string_buffer_size - 1; n++) {
                l->source->getchar(l->source);
                if (BrIsAlpha(l->source->next) == 0 && BrIsDigit(l->source->next) == 0 && l->source->next != '_') {
                    break;
                }
                l->string_buffer[n] = l->source->next;
            }
            l->string_buffer[n] = '\0';
            l->current.v.string = l->string_buffer;
            for (n = 0; n < l->nkeywords; n++) {
                if ((l->keywords[n].name[0] == l->string_buffer[0]) && (BrStrCmp(l->keywords[n].name, l->string_buffer) == 0)) {
                    l->current.id = l->keywords[n].id;
                    return;
                }
            }
            l->current.id = T_IDENT;
            return;
        }
        if (BrIsDigit(l->source->next) != 0) {
            got_point = 0;
            l->string_buffer[0] = l->source->next;
            for (n = 1; n < l->string_buffer_size - 1; n++) {
                l->source->getchar(l->source);
                if (l->source->next == '.') {
                    got_point = 1;
                } else if (!(('0' <= l->source->next && l->source->next <= '9')
                    || ('A' <= l->source->next && l->source->next <= 'F') || (l->source->next == 'X')
                    || ('a' <= l->source->next && l->source->next <= 'f') || (l->source->next == 'x'))) {
                    break;
                }
                l->string_buffer[n] = l->source->next;
            }
            l->string_buffer[n] = '\0';
            if (got_point != 0) {
                l->current.v.real = BrStrToF(l->string_buffer, NULL);
                l->current.id = T_REAL;
            } else {
                l->current.v.integer = BrStrToL(l->string_buffer, NULL, 0);
                l->current.id = T_INTEGER;
            }
            return;
        }
        if (BrIsPrint(l->source->next) != 0) {
            l->current.id = l->source->next;
            l->source->getchar(l->source);
            return;
        }
        l->current.id = T_ERROR;
        l->current.v.string = "Unexpected character";
        l->source->getchar(l->source);
        return;
    }
}
C2_HOOK_FUNCTION(0x0052f5c0, lexerAdvance)

void C2_HOOK_STDCALL lexerAdvanceDump(br_lexer* l) {
    c2_abort();
}

br_error C2_HOOK_CDECL BrLexerDumpSet(br_lexer* l, br_putline_cbfn* putline, void* putline_arg) {

    return 0;
}
C2_HOOK_FUNCTION(0x0052f950, BrLexerDumpSet)

void C2_HOOK_CDECL BrLexerTokenError(br_lexer* l, br_lexer_token_id t) {
    int i;
    char tmp[256];

    if (l->current.id != T_ERROR) {
        switch (t) {
        case T_EOF:
            l->error(l, "expected end of file");
            break;
        case T_ERROR:
            l->error(l, l->current.v.string);
            break;
        case T_IDENT:
            l->error(l, "expected an identifier");
            break;
        case T_STRING:
            l->error(l, "expected a string");
            break;
        case T_INTEGER:
            l->error(l, "expected an integer");
            break;
        case T_REAL:
            l->error(l, "expected a real number");
            break;
        default:
            if (t < 0x80) {
                if (BrIsPrint(t) != 0) {
                    BrSprintfN(tmp, sizeof(tmp), "expected '%c'", t);
                } else {
                    BrSprintfN(tmp, sizeof(tmp), "expected unknown token %d", t);
                }
            } else {
                for (i = 0; i < l->nkeywords; i++) {
                    if (l->keywords[i].id == t) {
                        break;
                    }
                }
                if (i < l->nkeywords) {
                    BrSprintfN(tmp, sizeof(tmp), "expected '%s'", l->keywords[i].name);
                } else {
                    BrSprintfN(tmp, sizeof(tmp), "expected unknown keyword %d", t);
                }
            }
            l->error(l, tmp);
            break;
        }
    }

    while (l->current.id != t && l->current.id != T_EOF) {
        l->advance(l);
    }
}
C2_HOOK_FUNCTION(0x0052f960, BrLexerTokenError)

void C2_HOOK_CDECL BrLexerPosition(br_lexer* l, char* buf, br_size_t buf_size) {

    if ((l->source != NULL) && (l->source->name != NULL)) {
        BrSprintfN(buf, buf_size, "%s:%d ", l->source->name, l->source->line);
    } else {
        *buf = '\0';
    }
}
C2_HOOK_FUNCTION(0x0052fa90, BrLexerPosition)

br_fixed_ls C2_HOOK_CDECL BrParseFixed(br_lexer* l) {
    br_boolean neg;
    br_fixed_ls x;

    if (l->current.id == T_PLUS) {
        l->advance(l);
    }
    neg = l->current.id == T_DASH;
    if (neg) {
        l->advance(l);
    }
    if (l->current.id == T_REAL) {
        x = BrFloatToFixed(l->current.v.real);
        l->advance(l);
    } else if (l->current.id == T_INTEGER) {
        x = BrIntToFixed(l->current.v.integer);
        l->advance(l);
    } else {
        l->error(l, "expecting a fixed");
        l->advance(l);
        x = 0;
    }
    if (neg) {
        x = -x;
    }
    return x;
}
C2_HOOK_FUNCTION(0x0052fad0, BrParseFixed)

br_float C2_HOOK_CDECL BrParseFloat(br_lexer* l) {
    br_boolean neg;
    br_float f;

    if (l->current.id == T_PLUS) {
        l->advance(l);
    }
    neg = l->current.id == T_DASH;
    if (neg) {
        l->advance(l);
    }
    if (l->current.id == T_REAL) {
        f = l->current.v.real;
        l->advance(l);
    } else if (l->current.id == T_INTEGER) {
        f = (float)l->current.v.integer;
        l->advance(l);
    } else {
        l->error(l, "expecting a float");
        l->advance(l);
        f = 0.f;
    }
    if (neg) {
        f = -f;
    }
    return f;
}
C2_HOOK_FUNCTION(0x0052fb40, BrParseFloat)

br_int_32 C2_HOOK_CDECL BrParseInteger(br_lexer* l) {
    br_boolean neg;
    br_int_32 i;

    if (l->current.id == T_PLUS) {
        l->advance(l);
    }
    neg = l->current.id == T_DASH;
    if (neg) {
        l->advance(l);
    }
    if (l->current.id == T_INTEGER) {
        i = l->current.v.integer;
        l->advance(l);
    } else {
        l->error(l, "expecting an integer");
        l->advance(l);
        i = 0;
    }
    if (neg) {
        i = -i;
    }
    return i;
}
C2_HOOK_FUNCTION(0x0052fbb0, BrParseInteger)

br_int_32 C2_HOOK_CDECL BrParseVectorFixed(br_lexer* l, br_fixed_ls* v, br_int_32 max) {
    int n;

    if (l->current.id == T_LSQUARE) {
        l->advance(l);
    } else {
        BrLexerTokenError(l, T_LSQUARE);
    }
    for (n = 0; n < max; n++) {
#ifdef BRENDER_FIX_BUGS
        if (l->current.id == T_RSQUARE) {
            break;
        }
#endif
        *v = BrParseFixed(l);
        v++;
        if (l->current.id == T_RSQUARE) {
            n++;
            break;
        }
        if (n != max - 1) {
            if (l->current.id == T_COMMA) {
                l->advance(l);
            } else {
                BrLexerTokenError(l, T_COMMA);
            }
        }
    }
    if (l->current.id == T_RSQUARE) {
        l->advance(l);
    } else {
        BrLexerTokenError(l, T_RSQUARE);
    }
    return n;
}
C2_HOOK_FUNCTION(0x0052fc00, BrParseVectorFixed)

br_int_32 C2_HOOK_CDECL BrParseVectorFloat(br_lexer* l, br_float* v, br_int_32 max) {
    int n;

    if (l->current.id == T_LSQUARE) {
        l->advance(l);
    } else {
        BrLexerTokenError(l, T_LSQUARE);
    }
    for (n = 0; n < max; n++) {
#ifdef BRENDER_FIX_BUGS
        if (l->current.id == T_RSQUARE) {
            break;
        }
#endif
        *v = BrParseFloat(l);
        v++;
        if (l->current.id == T_RSQUARE) {
            n++;
            break;
        }
        if (n != max - 1) {
            if (l->current.id == T_COMMA) {
                l->advance(l);
            } else {
                BrLexerTokenError(l, T_COMMA);
            }
        }
    }
    if (l->current.id == T_RSQUARE) {
        l->advance(l);
    } else {
        BrLexerTokenError(l, T_RSQUARE);
    }
    return n;
}
C2_HOOK_FUNCTION(0x0052fc90, BrParseVectorFloat)

br_int_32 C2_HOOK_CDECL BrParseMatrixFixed(br_lexer* l, br_fixed_ls* m, br_int_32 width, br_int_32 max_h) {
    int n;

    if (l->current.id == T_LSQUARE) {
        l->advance(l);
    } else {
        BrLexerTokenError(l, T_LSQUARE);
    }
    for (n = 0; n < max_h; n++, m = m + width) {
#ifdef BRENDER_FIX_BUGS
        if (l->current.id == T_RSQUARE) {
            break;
        }
#endif
        BrParseVectorFixed(l, m, width);
        if (l->current.id == T_RSQUARE) {
            n++;
            break;
        }
        if (n != max_h - 1) {
            if (l->current.id == T_COMMA) {
                l->advance(l);
            } else {
                BrLexerTokenError(l, T_COMMA);
            }
        }
    }
    if (l->current.id == T_RSQUARE) {
        l->advance(l);
    } else {
        BrLexerTokenError(l, T_RSQUARE);
    }
    return n;
}
C2_HOOK_FUNCTION(0x0052fd80, BrParseMatrixFixed)

br_int_32 C2_HOOK_CDECL BrParseMatrixFloat(br_lexer* l, br_float* m, br_int_32 width, br_int_32 max_h) {
    int n;

    if (l->current.id == T_LSQUARE) {
        l->advance(l);
    } else {
        BrLexerTokenError(l, T_LSQUARE);
    }
    for (n = 0; n < max_h; n++, m = m + width) {
#ifdef BRENDER_FIX_BUGS
        if (l->current.id == T_RSQUARE) {
            break;
        }
#endif
        BrParseVectorFloat(l, m, width);
        if (l->current.id == T_RSQUARE) {
            n++;
            break;
        }
        if (n != max_h - 1) {
            if (l->current.id == T_COMMA) {
                l->advance(l);
            } else {
                BrLexerTokenError(l, T_COMMA);
            }
        }
    }
    if (l->current.id == T_RSQUARE) {
        l->advance(l);
    } else {
        BrLexerTokenError(l, T_RSQUARE);
    }
    return n;
}
C2_HOOK_FUNCTION(0x0052fe10, BrParseMatrixFloat)
