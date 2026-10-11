#include "23-piping.h"

#include "52-errors.h"
#include "globvars.h"
#include "c2_string.h"

#include <stdarg.h>

// GLOBAL: CARMA2_HW 0x00676908
tU8* gPipe_buffer_start;

// GLOBAL: CARMA2_HW 0x006768e4
int gReentrancy_count;

// GLOBAL: CARMA2_HW 0x006768d0
tPipe_chunk_type gReentrancy_array[5];

// GLOBAL: CARMA2_HW 0x006768b4
tU8* gLocal_buffer;

// GLOBAL: CARMA2_HW 0x006768f4
tU32 gLocal_buffer_size;

// GLOBAL: CARMA2_HW 0x006768a0
tU8* gMr_chunky;

// GLOBAL: CARMA2_HW 0x006768e8
const tReplay_callback* gPipe_callbacks;

// GLOBAL: CARMA2_HW 0x006768f0
tU8* gPipe_buffer_phys_end;

// GLOBAL: CARMA2_HW 0x006768c4
tU8* gPipe_buffer_working_end;

// GLOBAL: CARMA2_HW 0x006768ac
tU8* gPipe_buffer_oldest;

// GLOBAL: CARMA2_HW 0x006768bc
tU8* gPipe_record_ptr;

// GLOBAL: CARMA2_HW 0x006768b8
tU8* gPipe_play_ptr;

// GLOBAL: CARMA2_HW 0x006768c0
int gPlay_direction;

// GLOBAL: CARMA2_HW 0x006768fc
int gReplay_pause_state;

// GLOBAL: CARMA2_HW 0x00676900
float gReplay_rate;

// GLOBAL: CARMA2_HW 0x0067690c
int gDisable_advance;

// GLOBAL: CARMA2_HW 0x006768c8
tU8* gPipe_chunk_end;

// GLOBAL: CARMA2_HW 0x0058f1e8
int gMax_previous_pipe_sessions = 1000;

// GLOBAL: CARMA2_HW 0x0069410c
tU32 gUNK_0069410c;

/* The original inline memcpy copies forward in four-byte groups, then bytes. */
static void C2_HOOK_CDECL ReplayCopyForward(void* pDest, const void* pSource, tU32 pSize) {
    tU8* dest;
    const tU8* source;
    tU32 word;

    dest = (tU8*)pDest;
    source = (const tU8*)pSource;
    while (pSize >= 4) {
        memcpy(&word, source, 4);
        memcpy(dest, &word, 4);
        source += 4;
        dest += 4;
        pSize -= 4;
    }
    while (pSize != 0) {
        *dest++ = *source++;
        pSize--;
    }
}

// FUNCTION: CARMA2_HW 0x00402300
int C2_HOOK_FASTCALL ARReplayForwards(void) {
    tU32 rate_bits;

    if (gPipe_play_ptr == gPipe_record_ptr) {
        return 0;
    }
    if (gPipe_play_ptr == gPipe_buffer_oldest) {
        return 1;
    }
    memcpy(&rate_bits, &gReplay_rate, sizeof(rate_bits));
    /* The original tests x87 C3, which is set for zero and unordered. */
    if (gReplay_rate == 0.0f || (rate_bits & 0x7fffffffu) > 0x7f800000u) {
        return gPlay_direction > 0;
    }
    return gReplay_rate > 0.0f;
}

// FUNCTION: CARMA2_HW 0x00402360
int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void) {
    tU32 rate_bits;

    memcpy(&rate_bits, &gReplay_rate, sizeof(rate_bits));
    return gReplay_rate == 0.0f || (rate_bits & 0x7fffffffu) > 0x7f800000u;
}

// FUNCTION: CARMA2_HW 0x00402380
int C2_HOOK_FASTCALL ARReplayPauseState(void) {
    return gReplay_pause_state;
}

// FUNCTION: CARMA2_HW 0x00402390
float C2_HOOK_FASTCALL ARGetReplayRate(void) {
    return gReplay_rate;
}

// FUNCTION: CARMA2_HW 0x004023a0
void C2_HOOK_FASTCALL ARSetReplayRate(float pRate) {
    tU32 bits;
    volatile tU32 stored_bits;

    /* MOV copies preserve signaling NaNs in the stored replay rate. */
    memcpy(&bits, &pRate, sizeof(bits));
    stored_bits = bits;
    bits = stored_bits;
    memcpy(&gReplay_rate, &bits, sizeof(bits));
}

// FUNCTION: CARMA2_HW 0x004023b0
int C2_HOOK_FASTCALL ARGetReplayDirection(void) {
    return gPlay_direction;
}

// FUNCTION: CARMA2_HW 0x004023c0
void C2_HOOK_FASTCALL ARSetReplayDirection(float pDirection) {
    tU32 bits;

    memcpy(&bits, &pDirection, sizeof(bits));
    /* _ftol stores a signed 64-bit integer and this caller keeps EAX. Its
     * indefinite result for NaN, infinity or overflow has a zero low word. */
    if ((bits & 0x7fffffffu) >= 0x5f000000u) {
        gPlay_direction = 0;
    } else {
        gPlay_direction = (int)(tU32)(int64_t)pDirection;
    }
}

// FUNCTION: CARMA2_HW 0x004023e0
void C2_HOOK_FASTCALL ARResetPiping(void) {
    gPipe_record_ptr = gPipe_buffer_start;
    gPipe_buffer_working_end = gPipe_buffer_phys_end;
    gPipe_buffer_oldest = NULL;
    gReentrancy_count = 0;
}

// ARInitialise

// ARDispose

// FUNCTION: CARMA2_HW 0x00403720
int C2_HOOK_FASTCALL LengthOfChunk(void* pChunk, int pType) {
    tU32 length;
    const tReplay_callback* callbacks;

    callbacks = gPipe_callbacks;
    if (callbacks[pType].calc_length != NULL) {
        length = callbacks[pType].calc_length((tPipe_chunk*)pChunk);
        callbacks = gPipe_callbacks;
        return (int)(length + callbacks[pType].length + 4);
    }
    return (int)(callbacks[pType].length + 4);
}

/* The search loops keep the table cached until a callback can replace it.
 * Header type/count remain live, including changes made by length callbacks. */
static tU32 C2_HOOK_FASTCALL ReplaySearchSessionLength(tPipe_chunk* pSession, const tReplay_callback** pCallbacks) {
    tU32 size;
    tU32 i;
    tU32 length;
    tPipe_chunk_type type;
    const tReplay_callback* callbacks;

    callbacks = *pCallbacks;
    size = 0;
    for (i = 0; i < pSession->count; i++) {
        type = pSession->type;
        if (callbacks[type].calc_length != NULL) {
            length = callbacks[type].calc_length((tPipe_chunk*)((tU8*)pSession + 8 + size));
            callbacks = gPipe_callbacks;
            length += callbacks[type].length + 4;
        } else {
            length = callbacks[type].length + 4;
        }
        size += length;
    }
    size += 10;
    if ((size & 1) != 0) {
        FatalError(0x62);
    }
    *pCallbacks = callbacks;
    return size;
}

// FUNCTION: CARMA2_HW 0x00403750
tU32 C2_HOOK_FASTCALL LengthOfSession(tPipe_chunk* pSession) {
    tU32 size;
    tU32 i;
    tU32 length;
    tPipe_chunk_type type;
    const tReplay_callback* callbacks;

    size = 0;
    callbacks = gPipe_callbacks;
    for (i = 0; i < pSession->count; i++) {
        type = pSession->type;
        if (callbacks[type].calc_length != NULL) {
            length = callbacks[type].calc_length((tPipe_chunk*)((tU8*)pSession + 8 + size));
            callbacks = gPipe_callbacks;
            length += callbacks[type].length + 4;
        } else {
            length = callbacks[type].length + 4;
        }
        size += length;
    }
    size += 10;
    if ((size & 1) != 0) {
        FatalError(0x62);
    }
    return size;
}

// FUNCTION: CARMA2_HW 0x004024e0
void C2_HOOK_FASTCALL ARAdvanceChunkPtr(tU8** pChunk, int pType) {
    int disabled;
    tU8* old_chunk;
    tU32 length;
    const tReplay_callback* callbacks;

    disabled = gDisable_advance;
    /* The original reads the pointer even when advancement is disabled. */
    old_chunk = *(tU8* volatile*)pChunk;
    if (disabled) {
        return;
    }
    callbacks = gPipe_callbacks;
    if (callbacks[pType].calc_length != NULL) {
        length = callbacks[pType].calc_length((tPipe_chunk*)old_chunk);
        callbacks = gPipe_callbacks;
        length += callbacks[pType].length + 4;
    } else {
        length = callbacks[pType].length + 4;
    }
    *pChunk = (tU8*)((uintptr_t)*pChunk + length);
    /* This particular original boundary comparison is signed. */
    if ((intptr_t)*pChunk >= (intptr_t)gPipe_chunk_end) {
        *pChunk = old_chunk;
    }
}

// PipingFrameReset

// FUNCTION: CARMA2_HW 0x00402540
int C2_HOOK_FASTCALL ARPipeSearchForwards(void) {
    return ARReplayForwards();
}

// FUNCTION: CARMA2_HW 0x004025a0
int C2_HOOK_FASTCALL ARIsActionReplayAvailable(void) {
    return gPipe_buffer_start != NULL;
}

// FUNCTION: CARMA2_HW 0x004025b0
int C2_HOOK_FASTCALL ARSomeReplayLeft(void) {
    return (gPlay_direction > 0 && gPipe_play_ptr != gPipe_record_ptr)
        || (gPlay_direction < 0 && gPipe_play_ptr != gPipe_buffer_oldest);
}

// DisablePipedSounds

// EnablePipedSounds

// StartPipingSession2

// FUNCTION: CARMA2_HW 0x004025e0
void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType) {
    if (gPipe_buffer_start == NULL || gAction_replay_mode || !gProgram_state.racing) {
        return;
    }
    if (gReentrancy_count != 0) {
        gReentrancy_array[gReentrancy_count - 1] = ((tPipe_chunk*)gLocal_buffer)->type;
        EndPipingSession2(0);
    }
    gReentrancy_count++;
    ((tPipe_chunk*)gLocal_buffer)->type = pType;
    ((tPipe_chunk*)gLocal_buffer)->count = 0;
    gMr_chunky = gLocal_buffer + 8;
    gLocal_buffer_size = 8;
}

// FUNCTION: CARMA2_HW 0x00402660
void C2_HOOK_FASTCALL EndPipingSession2(int pMunge_reentrancy) {
    tU32 size;
    tPipe_chunk_type type;

    if (gPipe_buffer_start == NULL || gAction_replay_mode || !gProgram_state.racing) {
        return;
    }
    *(tU16*)(gLocal_buffer + gLocal_buffer_size) = (tU16)gLocal_buffer_size;
    gLocal_buffer_size += 2;
    if (((tPipe_chunk*)gLocal_buffer)->count != 0 && gLocal_buffer_size <= 15000) {
        if ((uintptr_t)gPipe_buffer_phys_end < (uintptr_t)gPipe_record_ptr + gLocal_buffer_size) {
            gPipe_buffer_working_end = gPipe_record_ptr;
            gPipe_buffer_oldest = gPipe_buffer_start;
            gPipe_record_ptr = gPipe_buffer_start;
        }
        while ((uintptr_t)gPipe_record_ptr <= (uintptr_t)gPipe_buffer_oldest
            && (uintptr_t)gPipe_buffer_oldest < (uintptr_t)gPipe_record_ptr + gLocal_buffer_size) {
            size = LengthOfSession((tPipe_chunk*)gPipe_buffer_oldest);
            gPipe_buffer_oldest += size;
            if ((uintptr_t)gPipe_buffer_working_end <= (uintptr_t)gPipe_buffer_oldest) {
                gPipe_buffer_working_end = gPipe_buffer_phys_end;
                gPipe_buffer_oldest = gPipe_buffer_start;
            }
        }
        if (gPipe_buffer_oldest == NULL) {
            gPipe_buffer_oldest = gPipe_record_ptr;
        }
        ReplayCopyForward(gPipe_record_ptr, gLocal_buffer, gLocal_buffer_size);
        gPipe_record_ptr += gLocal_buffer_size;
        if ((uintptr_t)gPipe_buffer_working_end < (uintptr_t)gPipe_record_ptr) {
            gPipe_buffer_working_end = gPipe_record_ptr;
        }
    }
    if (pMunge_reentrancy && gReentrancy_count != 0) {
        gReentrancy_count--;
        if (gReentrancy_count != 0) {
            type = gReentrancy_array[gReentrancy_count - 1];
            if (gPipe_buffer_start != NULL && !gAction_replay_mode && gProgram_state.racing) {
                ((tPipe_chunk*)gLocal_buffer)->type = type;
                ((tPipe_chunk*)gLocal_buffer)->count = 0;
                gMr_chunky = gLocal_buffer + 8;
                gLocal_buffer_size = 8;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00402890
void C2_HOOK_FASTCALL AREndPipingSession(void) {
    EndPipingSession2(1);
}

// FUNCTION: CARMA2_HW 0x004028a0
void C2_HOOK_FASTCALL ARAddDataToSession(int pType, uintptr_t pOwner, void* pData, tU32 pSize) {
    tU32 new_size;

    (void)pType;
    if (gPipe_buffer_start == NULL || gAction_replay_mode || !gProgram_state.racing) {
        return;
    }
    new_size = gLocal_buffer_size + pSize + 4;
    if (new_size >= 15000) {
        return;
    }
    ((tPipe_chunk*)gLocal_buffer)->count++;
    *(uintptr_t*)gMr_chunky = pOwner;
    gMr_chunky += 4;
    if (pSize != 0) {
        ReplayCopyForward(gMr_chunky, pData, pSize);
    }
    gMr_chunky += pSize;
    gLocal_buffer_size = new_size;
}

// FUNCTION: CARMA2_HW 0x00402930
void C2_HOOK_CDECL ARAddVariedDataToSession(int pType, uintptr_t pOwner, int pCount, ...) {
    tU8 buffer[5000];
    va_list args;
    int i;
    int size;
    int offset;
    uintptr_t value;
    tU32 length;
    const tReplay_callback* callbacks;

    if (gPipe_buffer_start == NULL || gAction_replay_mode || !gProgram_state.racing) {
        return;
    }
    va_start(args, pCount);
    for (i = 0; i < pCount; i++) {
        size = va_arg(args, int);
        offset = va_arg(args, int);
        value = va_arg(args, uintptr_t);
        if (size < 0) {
            /* The original scalar FLD/FSTP quiets signaling NaNs. */
            if ((value & 0x7f800000u) == 0x7f800000u && (value & 0x7fffffu) != 0) {
                value |= 0x400000u;
            }
            memcpy(buffer + offset, &value, 4);
        } else if (size == 1) {
            buffer[4 + offset] = (tU8)value;
        } else if (size == 2) {
            *(tU16*)(buffer + 4 + offset) = (tU16)value;
        } else if (size == 4) {
            memcpy(buffer + 4 + offset, &value, 4);
        } else if (value != 0) {
            ReplayCopyForward(buffer + 4 + offset, (void*)value, size);
        }
    }
    va_end(args);
    memcpy(buffer, &pOwner, 4);
    callbacks = gPipe_callbacks;
    if (callbacks[pType].calc_length != NULL) {
        length = callbacks[pType].calc_length((tPipe_chunk*)buffer);
        callbacks = gPipe_callbacks;
        length += callbacks[pType].length + 4;
    } else {
        length = callbacks[pType].length + 4;
    }
    ARAddDataToSession(pType, pOwner, buffer + 4, length - 4);
}

// FUNCTION: CARMA2_HW 0x00402ab0
void C2_HOOK_FASTCALL ARDoSingleSession(int pType, uintptr_t pOwner, void* pData, tU32 pSize) {
    if (gPipe_buffer_start == NULL || gAction_replay_mode || !gProgram_state.racing) {
        return;
    }
    ARStartPipingSession((tPipe_chunk_type)pType);
    ARAddDataToSession(pType, pOwner, pData, pSize);
    EndPipingSession2(1);
}

// FUNCTION: CARMA2_HW 0x00402bd0
void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...) {
    tU8 buffer[5000];
    va_list args;
    int i;
    int size;
    int offset;
    uintptr_t value;
    tU32 length;
    const tReplay_callback* callbacks;

    ARStartPipingSession((tPipe_chunk_type)pType);
    if (gPipe_buffer_start != NULL && !gAction_replay_mode && gProgram_state.racing) {
        va_start(args, pCount);
        for (i = 0; i < pCount; i++) {
            size = va_arg(args, int);
            offset = va_arg(args, int);
            value = va_arg(args, uintptr_t);
            if (size < 0) {
                /* The original scalar FLD/FSTP quiets signaling NaNs. */
                if ((value & 0x7f800000u) == 0x7f800000u && (value & 0x7fffffu) != 0) {
                    value |= 0x400000u;
                }
                memcpy(buffer + offset, &value, 4);
            } else if (size == 1) {
                buffer[4 + offset] = (tU8)value;
            } else if (size == 2) {
                *(tU16*)(buffer + 4 + offset) = (tU16)value;
            } else if (size == 4) {
                memcpy(buffer + 4 + offset, &value, 4);
            } else if (value != 0) {
                ReplayCopyForward(buffer + 4 + offset, (void*)value, size);
            }
        }
        va_end(args);
        memcpy(buffer, &pOwner, 4);
        callbacks = gPipe_callbacks;
        if (callbacks[pType].calc_length != NULL) {
            length = callbacks[pType].calc_length((tPipe_chunk*)buffer);
            callbacks = gPipe_callbacks;
            length += callbacks[pType].length + 4;
        } else {
            length = callbacks[pType].length + 4;
        }
        ARAddDataToSession(pType, pOwner, buffer + 4, length - 4);
    }
    EndPipingSession2(1);
}

// ARResetPipePlayToEnd

// FUNCTION: CARMA2_HW 0x00402df0
tU8* C2_HOOK_FASTCALL ARGetPipePlayPtr(void) {
    return gPipe_play_ptr;
}

// FUNCTION: CARMA2_HW 0x00402e00
void C2_HOOK_FASTCALL ARSetPipePlayPtr(tU8* pPtr) {
    gPipe_play_ptr = pPtr;
}

// ApplyPipedSession

// FUNCTION: CARMA2_HW 0x004037c0
int C2_HOOK_FASTCALL MoveSessionPointerBackOne(tU8** pSession) {
    tU8* footer;
    tU16 length;

    if (*pSession == gPipe_buffer_oldest && *pSession != gPipe_record_ptr) {
        return 1;
    }
    if (*pSession == gPipe_buffer_start) {
        *pSession = gPipe_buffer_working_end;
    }
    footer = (tU8*)((uintptr_t)*pSession - 2);
    *pSession = footer;
    memcpy(&length, footer, sizeof(length));
    *pSession = (tU8*)((uintptr_t)footer - length);
    return 0;
}

// MoveSessionPointerForwardOne

// FUNCTION: CARMA2_HW 0x00403800
tPipe_chunk_data* C2_HOOK_FASTCALL FindPreviousChunk(tU8* pSession, int pType, uintptr_t pOwner) {
    const tReplay_callback* callbacks;
    tU32 sessions;
    tU32 i;
    tU32 length;
    tU16 previous_length;
    tU8* chunk;
    tU8* old_chunk;
    tU8* chunk_end;
    int limit;

    pOwner &= 0xfff;
    sessions = 0;
    callbacks = gPipe_callbacks;
    for (;;) {
        if (pSession == gPipe_buffer_oldest && pSession != gPipe_record_ptr) {
            return NULL;
        }
        if (pSession == gPipe_buffer_start) {
            pSession = gPipe_buffer_working_end;
        }
        limit = gMax_previous_pipe_sessions;
        pSession = (tU8*)((uintptr_t)pSession - 2);
        memcpy(&previous_length, pSession, sizeof(previous_length));
        pSession = (tU8*)((uintptr_t)pSession - previous_length);
        if ((int)sessions >= limit) {
            return NULL;
        }
        sessions++;
        length = ReplaySearchSessionLength((tPipe_chunk*)pSession, &callbacks);
        chunk_end = (tU8*)((uintptr_t)pSession + length - 2);
        gPipe_chunk_end = chunk_end;
        chunk = pSession + 8;
        for (i = 0; i < ((tPipe_chunk*)pSession)->count; i++) {
            if ((int)((tPipe_chunk*)pSession)->type != pType) {
                break;
            }
            if ((*(tU32*)chunk & 0xfff) == pOwner) {
                return (tPipe_chunk_data*)chunk;
            }
            old_chunk = chunk;
            if (!gDisable_advance) {
                if (callbacks[pType].calc_length != NULL) {
                    length = callbacks[pType].calc_length((tPipe_chunk*)chunk);
                    callbacks = gPipe_callbacks;
                    length += callbacks[pType].length + 4;
                    chunk_end = gPipe_chunk_end;
                } else {
                    length = callbacks[pType].length + 4;
                }
                chunk = (tU8*)((uintptr_t)chunk + length);
                if ((intptr_t)chunk >= (intptr_t)chunk_end) {
                    chunk = old_chunk;
                }
            }
        }
    }
}

// UndoPipedSession

// FUNCTION: CARMA2_HW 0x00402e10
void C2_HOOK_FASTCALL ARScanBuffer(tU8** pPtr, tPipe_chunk_type pType, tU32 pDefault_time, tARScanBuffer_callback* pCallback, tARScanBuffer_time_check* pTime_check) {
    const tReplay_callback* callbacks;
    tU32 length;
    tPipe_chunk* session;

    callbacks = gPipe_callbacks;
    for (;;) {
        if (ARReplayForwards()) {
            length = ReplaySearchSessionLength((tPipe_chunk*)*pPtr, &callbacks);
            *pPtr = (tU8*)((uintptr_t)*pPtr + length);
            if ((uintptr_t)*pPtr >= (uintptr_t)gPipe_buffer_working_end && *pPtr != gPipe_record_ptr) {
                *pPtr = gPipe_buffer_start;
            }
            if (*pPtr == gPipe_record_ptr) {
                return;
            }
        } else if (MoveSessionPointerBackOne(pPtr)) {
            return;
        }
        callbacks = gPipe_callbacks;
        length = ReplaySearchSessionLength((tPipe_chunk*)*pPtr, &callbacks);
        gPipe_chunk_end = (tU8*)((uintptr_t)*pPtr + length - 2);
        session = (tPipe_chunk*)*pPtr;
        if (session->type == 0) {
            memcpy(&pDefault_time, (tU8*)session + 12, sizeof(pDefault_time));
        } else if (session->type == pType) {
            if (pCallback((tPipe_chunk*)((tU8*)session + 8), session->count, pDefault_time)) {
                return;
            }
            callbacks = gPipe_callbacks;
        }
        if (pTime_check != NULL) {
            if (!pTime_check(pDefault_time)) {
                return;
            }
            callbacks = gPipe_callbacks;
        }
    }
}

// CheckSound

// SoundTimeout

// ScanAndPlaySoundsToBe

// FUNCTION: CARMA2_HW 0x00403020
tU32 C2_HOOK_FASTCALL ARGetStartTime(void) {
    tU8* session;

    session = gPipe_buffer_oldest;
    for (;;) {
        session = (tU8*)((uintptr_t)session + LengthOfSession((tPipe_chunk*)session));
        if ((uintptr_t)session >= (uintptr_t)gPipe_buffer_working_end && session != gPipe_record_ptr) {
            session = gPipe_buffer_start;
        }
        if (session == gPipe_record_ptr) {
            return 0;
        }
        if (((tPipe_chunk*)session)->type == 0) {
            return *(tU32*)(session + 12);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004030d0
void C2_HOOK_FASTCALL ARDisableAdvance(void) {
    gDisable_advance = 1;
}

// FUNCTION: CARMA2_HW 0x004030e0
void C2_HOOK_FASTCALL AREnableAdvance(void) {
    gDisable_advance = 0;
}

// FUNCTION: CARMA2_HW 0x004030f0
tPipe_chunk_data* C2_HOOK_FASTCALL FindNextChunk(tPipe_chunk_type pType, uintptr_t pOwner) {
    tPipe_chunk* session;
    tU8* chunk;
    tU32 i;
    tU32 length;
    const tReplay_callback* callbacks;

    session = (tPipe_chunk*)gPipe_play_ptr;
    if ((tU8*)session == gPipe_record_ptr) {
        return NULL;
    }
    callbacks = gPipe_callbacks;
    for (;;) {
        if (session->type == pType) {
            chunk = (tU8*)session + 8;
            for (i = 0; i < session->count; i++) {
                if (*(uintptr_t*)chunk == pOwner) {
                    return (tPipe_chunk_data*)chunk;
                }
                if (callbacks[pType].calc_length != NULL) {
                    length = callbacks[pType].calc_length((tPipe_chunk*)chunk);
                    callbacks = gPipe_callbacks;
                    length += callbacks[pType].length + 4;
                } else {
                    length = callbacks[pType].length + 4;
                }
                chunk = (tU8*)((uintptr_t)chunk + length);
            }
        }
        length = ReplaySearchSessionLength(session, &callbacks);
        session = (tPipe_chunk*)((uintptr_t)session + length);
        if ((uintptr_t)session >= (uintptr_t)gPipe_buffer_working_end) {
            if ((tU8*)session == gPipe_record_ptr) {
                return NULL;
            }
            session = (tPipe_chunk*)gPipe_buffer_start;
        }
        if ((tU8*)session == gPipe_record_ptr) {
            return NULL;
        }
    }
}

// MoveReplayBuffer

// ARMoveToEndOfReplay

// ARMoveToStartOfReplay

// ARDoActionReplay

// ReverseSound

// ARGoBackwards

// ARGoForwards

// FUNCTION: CARMA2_HW 0x00403d30
void C2_HOOK_FASTCALL ARService(void) {
}

// ARMainLoopStart

// ARToggleReplay

// GetReducedPos

// SaveReducedPos

// InitLastDamageArrayEtc

// PipeObjectPosition

// PipeCarPositions

// DoSmudge

// DoOilSpillAR

// CheckCar

// CarTimeout

// ScanCarsPositions

// InitialiseActionReplay

// STUB: CARMA2_HW 0x004c6c60
void C2_HOOK_FASTCALL DisposeActionReplay(void) {
    NOT_IMPLEMENTED();
}

// CheckIncident

// GetNextIncident

// AddCrushToPipingSession

// AddSmudgeToPipingSession

// AddSparkToPipingSession

// AddShrapnelToPipingSession

// AddNonCarToPipingSession

/* Preserve the explicit original single-precision Z difference spill. */
static br_scalar C2_HOOK_CDECL ReplayScalarRound(double pValue) {
    volatile union { br_scalar scalar; tU32 bits; } stored;
    union { br_scalar scalar; tU32 bits; } result;

    stored.scalar = (br_scalar)pValue;
    result.bits = stored.bits;
    return result.scalar;
}

// FUNCTION: CARMA2_HW 0x004c7010
void C2_HOOK_FASTCALL AddSmokeToPipingSession(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength) {
    tReduced_pos pos;
    br_scalar z;
    br_actor* actor;

    C2_HOOK_BUG_ON(sizeof(tReduced_pos) != 6);
    C2_HOOK_BUG_ON(sizeof(tPipe_chunk_smoke) != 10);
    actor = gProgram_state.current_car.car_master_actor;
    z = ReplayScalarRound((double)pPos->v[2] - actor->t.t.translate.t.v[2]);
    pos.v[0] = (tS16)(br_int_64)(((double)pPos->v[0] - actor->t.t.translate.t.v[0]) * 800.f);
    pos.v[1] = (tS16)(br_int_64)(((double)pPos->v[1] - actor->t.t.translate.t.v[1]) * 800.f);
    pos.v[2] = (tS16)(br_int_64)((double)z * 800.f);
    ARAddVariedDataToSession(ePipe_chunk_smoke, pIndex, 4,
        6, 0, &pos,
        2, 6, (int)(tU16)(br_int_64)((double)pRadius * 1024.f),
        1, 8, (int)(tU8)(br_int_64)((double)pStrength * 255.f),
        1, 9, (int)pType);
}

// FUNCTION: CARMA2_HW 0x004c70c0
void C2_HOOK_FASTCALL AddSmokeColumnToPipingSession(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour) {
    ARAddVariedDataToSession((tPipe_chunk_type)17, ((tU32)pColour << 14) + pIndex, 2,
        16, 0, pCore, 2, 16, pVertex);
}

// FUNCTION: CARMA2_HW 0x004c70f0
void C2_HOOK_FASTCALL AddFlameToPipingSession(int pIndex, int pFrame, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z) {
    tU32 bits;

    (void)pScale_y;
    (void)pOffset_x;
    (void)pOffset_z;
    memcpy(&bits, &pScale_x, sizeof(bits));
    gUNK_0069410c = bits;
    /* The original MOV sequence reuses scale_x for all four scalar fields. */
    ARAddVariedDataToSession((tPipe_chunk_type)18, pIndex, 5,
        2, 0, pFrame, 4, 4, bits, 4, 8, bits, 4, 12, bits, 4, 16, bits);
}

// AddSplashToPipingSession

// AddExtendedSplashToPipingSession

// AddCarToPipingSession

// AddDamageToPipingSession

// AddProxRayToPipingSession

// AddFlapToPipingSession

// AddModelMashToPipingSession

// AddRepulseRayToPipingSession

// AddNapalmBoltToPipingSession

// PipeSingleCrush

// PipeSingleScreenWobble

// PipeSingleGrooveStop

// PipeSingleNonCar

// PipeSingleOilSpill

// PipeSingleFrameFinish

// PipeSingleGraphicalWheelStuff

// STUB: CARMA2_HW 0x004c84a0
void C2_HOOK_FASTCALL PipeSingleSound(tS3_outlet* pOutlet, int pSound, tS3_volume pL_volume, tS3_volume pR_volume, int pPitch, const br_vector3* pPosition) {
    NOT_IMPLEMENTED();
}

// PipeSingleSpecial

// PipeSingleCarIncident

// PipeSinglePedIncident

// PipeSingleWallIncident

// PipeSingleSkidAdjustment

// PipeSingleFlap

// PipeSingleRelink

// PipeSingleIdentity

// PipeSingleSplitWeld

// PipeSingleShrapnelShower

// PipeSingleExplosion

// PipeSingleNonCarCreation

// PipeSingleSmashModelChange

// PipeSingleSmashRemoveFaces

// PipeSingleSmashDecal

// PipeSingleSmashTextureChange

// PipeSinglePedStatus

// PipeSinglePedDir

// PipeSinglePedMove

// PipeSinglePedPhysics

// PipeSinglePedDismember

// PipeSinglePedFPChange

// PipeSinglePedPos

// PipeSinglePedModelChange

// PipeSinglePHILObject

// PipeSingleGibShower

// PipeSingleBloodSpurt

// PipeSingleGrooveOnOff

// PipeSinglePowerupGot

// PipeSinglePowerupLose

// PipeSingleFunkEnable

// PipeSingleVanishedDismembered

// PipeSingleDSModel

// PipeSingleVector3

// PipeSingleDroneRender

// PipeSingleDroneCornerPos

// PipeSingleDroneStraightPos

// PipeSinglePowerupRespawn

// PipeSingleShitMine

// PipeSingleEndShitMine

// PipeSingleTransformType

// PipeSingleOppoRenderage

// ApplyCrush

// ApplySmudge

// ApplySpark

// ApplyShrapnel

// ApplyScreenWobble

// ApplyGrooveStop

// ApplyNonCar

// ApplySmoke

// ApplySmokeColumn

// ApplyFlame

// ApplySplash

// ApplyExtendedSplash

// ApplyOilSpill

// ApplyFrameBoundary

// ApplySound

// ApplyCar

// ApplyGWS

// ApplyDamage

// ApplySpecial

// ApplyProxRay

// ApplySkidAdjustment

// ApplyFlap

// ApplyModelMash

// ApplyRelink

// ApplyIdentity

// ApplySplitWeld

// ApplyBend

// ApplyUnBend

// ApplyEndMyBend

// ApplyShrapnelShower

// ApplyExplosion

// ApplyNonCarCreation

// ApplySmashModelChange

// ApplySmashRemoveFaces

// ApplySmashDecal

// ApplySmashTextureChange

// ApplyRepulseRay

// ApplyActorTrans

// ApplyPedStatus

// ApplyPedDir

// ApplyPedMove

// ApplyPedPhysics

// ApplyPedDismember

// ApplyPedFPChange

// ApplyPedPos

// ApplyPedModelChange

// ApplyPHILObject

// ApplyGibShower

// ApplyBloodSpurt

// ApplyGrooveOnOff

// ApplyPowerupGot

// ApplyPowerupLose

// ApplyFunkEnable

// ApplyVanishDismembered

// ApplyDSModel

// ApplyPedDiagnostics

// ApplyVector3

// ApplyDroneRender

// ApplyDroneCornerPos

// ApplyDroneStraightPos

// ApplyDroneUnused

// ApplyNapalmBolt

// ApplyPowerupRespawn

// ApplyShitMine

// ApplyEndShitMine

// ApplyTransformType

// ApplyOppoRenderage

// UndoCrush

// UndoSmudge

// UndoDamage

// UndoSpecial

// UndoScreenWobble

// UndoSplash

// UndoExtendedSplash

// UndoSkidAdjustment

// UndoFlap

// UndoModelMash

// UndoRelink

// UndoSplitWeld

// UndoBend

// UndoUnBend

// UndoShrapnelShower

// UndoNonCarCreation

// UndoSmashModelChange

// UndoSmashRemoveFaces

// UndoSmashDecal

// UndoSmashTextureChange

// UndoActorTrans

// UndoPedStatus

// UndoPedMove

// UndoPedDismember

// UndoPedFPChange

// UndoPedPos

// UndoPedModelChange

// UndoGibShower

// UndoBloodSpurt

// UndoGrooveOnOff

// UndoPowerupGot

// UndoPowerupLose

// UndoFunkEnable

// UndoVanishDismembered

// UndoVector3

// UndoNapalmBolt

// UndoPowerupRespawn

// UndoShitMine

// UndoEndShitMine

// UndoTransformType

// CalcCrushLength

// CalcShrapnelLength

// CalcSmudgeLength

// CalcModelMashLength

// CalcEndMyBendLength

// CalcRemoveFacesLength
