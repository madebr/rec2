#include "23-piping.h"

#include "09-funks.h"
#include "13-crush1.h"
#include "16-graphics1.h"
#include "22-replay.h"
#include "41-utility.h"
#include "51-smash.h"
#include "69-sound.h"
#include "42-input.h"
#include "52-errors.h"
#include "globvars.h"
#include "c2_string.h"
#include "platform.h"

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

// GLOBAL: CARMA2_HW 0x00676910
int gPipe_count_callbacks;

// GLOBAL: CARMA2_HW 0x00676918
int gDisable_piped_sounds;

// GLOBAL: CARMA2_HW 0x006768b0
tU32 gPipe_sound_time;

// GLOBAL: CARMA2_HW 0x00676904
tU32 gPipe_sound_start_time;

// GLOBAL: CARMA2_HW 0x006768a8
tU32 gPipe_sound_time_window;

// GLOBAL: CARMA2_HW 0x00694104
undefined4* gCrush_space;

// GLOBAL: CARMA2_HW 0x0069410c
tU32 gUNK_0069410c;

#define CRUSH_SPACE_SIZE 0x4000

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
    memcpy(&gReplay_rate, &pRate, sizeof(gReplay_rate));
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
    /* Save the starting chunk so an advance past the end can be undone. */
    old_chunk = *pChunk;
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
    /* Restore the starting chunk if the advance reaches the session end. */
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

// FUNCTION: CARMA2_HW 0x00403c50
int C2_HOOK_FASTCALL SoundTimeout(tU32 pTime) {
    return gPipe_sound_start_time < pTime
        || gPipe_sound_start_time - pTime < gPipe_sound_time_window;
}

/* _ftol returns a signed 64-bit integer; these original callers use its low
 * word. The indefinite result for nonfinite/out-of-range input has low word0. */
static tU32 C2_HOOK_FASTCALL ReplayFtolLow(double pValue) {
    tU32 bits[2];

    memcpy(bits, &pValue, sizeof(bits));
    if ((bits[1] & 0x7fffffffu) >= 0x43e00000u) {
        return 0;
    }
    return (tU32)(int64_t)pValue;
}

// FUNCTION: CARMA2_HW 0x00403990
void C2_HOOK_FASTCALL ScanAndPlaySoundsToBe(tU8* pSession, tU32 pTime, tU32 pStart_time) {
    tU32 rate_bits;
    float absolute_rate;
    tU32 length;
    tU32 time;
    tU32 end_time;
    tU8* chunk;
    int count;
    int duration;
    int pitch;

    if (gDisable_piped_sounds) {
        return;
    }
    memcpy(&rate_bits, &gReplay_rate, sizeof(rate_bits));
    rate_bits &= 0x7fffffffu;
    memcpy(&absolute_rate, &rate_bits, sizeof(absolute_rate));
    gPipe_sound_start_time = pStart_time;
    gPipe_sound_time = pTime;
    gPipe_sound_time_window = ReplayFtolLow((double)absolute_rate * 1500.0);
    time = pTime;
    do {
        if (ARReplayForwards()) {
            length = LengthOfSession((tPipe_chunk*)pSession);
            pSession = (tU8*)((uintptr_t)pSession + length);
            if ((uintptr_t)pSession >= (uintptr_t)gPipe_buffer_working_end) {
                if (pSession == gPipe_record_ptr) {
                    return;
                }
                pSession = gPipe_buffer_start;
            }
            if (pSession == gPipe_record_ptr) {
                return;
            }
        } else if (MoveSessionPointerBackOne(&pSession)) {
            return;
        }
        length = LengthOfSession((tPipe_chunk*)pSession);
        gPipe_chunk_end = (tU8*)((uintptr_t)pSession + length - 2);
        if (((tPipe_chunk*)pSession)->type == 0) {
            memcpy(&time, pSession + 12, sizeof(time));
        } else if (((tPipe_chunk*)pSession)->type == 2) {
            count = (int)((tPipe_chunk*)pSession)->count;
            chunk = pSession + 8;
            while (count > 0) {
                duration = S3GetSampleDuration(*(int*)chunk);
                pitch = *(int*)(chunk + 4);
                if (pitch > 1) {
                    duration = (int)ReplayFtolLow((double)duration / ((double)pitch * 0.0000152587890625));
                }
                end_time = (tU32)duration + time;
                if (end_time >= gPipe_sound_start_time && end_time < gPipe_sound_time) {
                    if (gPipe_callbacks[2].apply != NULL) {
                        gPipe_callbacks[2].apply((tPipe_chunk**)&chunk);
                    }
                } else {
                    ARAdvanceChunkPtr(&chunk, 2);
                }
                count--;
            }
        }
    } while (SoundTimeout(time));
}

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

// FUNCTION: CARMA2_HW 0x00403260
void C2_HOOK_FASTCALL MoveReplayBuffer(int pFrames, tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame) {
    tU8* session;
    tU8* first_session;
    tU8* previous_session;
    tU8* frame_start;
    tU8* look;
    tU8* chunk;
    tU8* saved_chunk_end;
    tPipe_chunk* previous_chunk;
    const tReplay_callback* callbacks;
    tU32 time;
    tU32 length;
    tU32 i;
    int count;
    int frame;
    int type;
    int flags;
    int done;

    session = gPipe_play_ptr;
    first_session = session;
    time = GetTotalTime();
    count = pFrames < 0 ? (int)(0u - (tU32)pFrames) : pFrames;
    previous_session = NULL;
    for (frame = 0; frame < count && previous_session != session && !KeyIsDown(0); frame++) {
        look = session;
        if (pFrames > 0) {
            if (gPipe_play_ptr != gPipe_record_ptr) {
                for (;;) {
                    if (((tPipe_chunk*)look)->type == 0) {
                        memcpy(&gLast_replay_frame_time, look + 12, sizeof(gLast_replay_frame_time));
                        break;
                    }
                    length = LengthOfSession((tPipe_chunk*)look);
                    look = (tU8*)((uintptr_t)look + length);
                    if ((uintptr_t)look >= (uintptr_t)gPipe_buffer_working_end) {
                        if (look == gPipe_record_ptr) {
                            break;
                        }
                        look = gPipe_buffer_start;
                    }
                    if (look == gPipe_record_ptr) {
                        break;
                    }
                }
            }
        } else if (!MoveSessionPointerBackOne(&look)) {
            while (!MoveSessionPointerBackOne(&look)) {
                if (((tPipe_chunk*)look)->type == 0) {
                    memcpy(&gLast_replay_frame_time, look + 12, sizeof(gLast_replay_frame_time));
                    break;
                }
            }
        }
        if (ARSomeReplayLeft()) {
            for (i = 0; (int)i < gPipe_count_callbacks; i++) {
                if (gPipe_callbacks[i].reset != NULL) {
                    gPipe_callbacks[i].reset();
                }
            }
        }
        pBefore_frame(gFrame_period);
        frame_start = session;
        if (pFrames > 0) {
            for (;;) {
                if (session == gPipe_record_ptr) {
                    break;
                }
                length = LengthOfSession((tPipe_chunk*)session);
                gPipe_chunk_end = (tU8*)((uintptr_t)session + length - 2);
                chunk = session + 8;
                type = ((tPipe_chunk*)session)->type;
                for (i = 0; i < ((tPipe_chunk*)session)->count; i++) {
                    if (gPipe_callbacks[type].apply != NULL) {
                        gPipe_callbacks[type].apply((tPipe_chunk**)&chunk);
                    }
                    ARAdvanceChunkPtr(&chunk, type);
                }
                length = LengthOfSession((tPipe_chunk*)session);
                session = (tU8*)((uintptr_t)session + length);
                if ((uintptr_t)session >= (uintptr_t)gPipe_buffer_working_end && session != gPipe_record_ptr) {
                    session = gPipe_buffer_start;
                }
                if (type == 0) {
                    break;
                }
                pBetween_sessions(pFrames);
            }
            gPipe_play_ptr = session;
        } else if (pFrames < 0) {
            for (;;) {
                done = MoveSessionPointerBackOne(&session);
                if (!done) {
                    length = LengthOfSession((tPipe_chunk*)session);
                    gPipe_chunk_end = (tU8*)((uintptr_t)session + length - 2);
                    chunk = session + 8;
                    type = ((tPipe_chunk*)session)->type;
                    callbacks = gPipe_callbacks;
                    for (i = 0; i < ((tPipe_chunk*)session)->count; i++) {
                        saved_chunk_end = gPipe_chunk_end;
                        flags = (int)callbacks[type].flags;
                        if (flags < 2 || flags == 5) {
                            previous_chunk = NULL;
                        } else {
                            previous_chunk = (tPipe_chunk*)FindPreviousChunk(session, ((tPipe_chunk*)session)->type, *(uintptr_t*)chunk);
                            callbacks = gPipe_callbacks;
                        }
                        gPipe_chunk_end = saved_chunk_end;
                        switch (callbacks[type].flags) {
                        case 1:
                            if (callbacks[type].apply != NULL) {
                                callbacks[type].apply((tPipe_chunk**)&chunk);
                                ARAdvanceChunkPtr(&chunk, type);
                                callbacks = gPipe_callbacks;
                            }
                            break;
                        case 2:
                            if (callbacks[type].apply != NULL) {
                                if (previous_chunk != NULL) {
                                    gDisable_advance = 1;
                                    callbacks[type].apply(&previous_chunk);
                                    gDisable_advance = 0;
                                }
                                ARAdvanceChunkPtr(&chunk, type);
                                callbacks = gPipe_callbacks;
                            }
                            break;
                        case 3:
                            if (callbacks[type].apply != NULL) {
                                if (previous_chunk != NULL) {
                                    gDisable_advance = 1;
                                    callbacks[type].apply(&previous_chunk);
                                    gDisable_advance = 0;
                                    ARAdvanceChunkPtr(&chunk, type);
                                } else {
                                    callbacks[type].apply((tPipe_chunk**)&chunk);
                                }
                                callbacks = gPipe_callbacks;
                            }
                            break;
                        case 4:
                        case 5:
                            if (callbacks[type].undo != NULL) {
                                callbacks[type].undo((tPipe_chunk**)&chunk, previous_chunk);
                            }
                            ARAdvanceChunkPtr(&chunk, type);
                            callbacks = gPipe_callbacks;
                            break;
                        }
                    }
                    look = session;
                    done = MoveSessionPointerBackOne(&look) || ((tPipe_chunk*)look)->type == 0;
                }
                if (done) {
                    break;
                }
                pBetween_sessions(pFrames);
            }
            gPipe_play_ptr = session;
        }
        pAfter_frame(gFrame_period);
        previous_session = frame_start;
    }
    if (!ARReplayForwards()) {
        ScanAndPlaySoundsToBe(first_session, time, GetTotalTime());
    }
    if (previous_session == session) {
        gReplay_rate = 0.0f;
        gReplay_pause_state = 1;
    }
    if (KeyIsDown(0)) {
        WaitForNoKeys();
    }
}

// FUNCTION: CARMA2_HW 0x00403210
void C2_HOOK_FASTCALL ARMoveToEndOfReplay(tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame) {
    tU32 old_rate;

    gDisable_piped_sounds = 1;
    memcpy(&old_rate, &gReplay_rate, sizeof(old_rate));
    gReplay_rate = 100.0f;
    MoveReplayBuffer(0x7fffffff, pBetween_sessions, pBefore_frame, pAfter_frame);
    gDisable_piped_sounds = 0;
    memcpy(&gReplay_rate, &old_rate, sizeof(old_rate));
}

// FUNCTION: CARMA2_HW 0x00403c70
void C2_HOOK_FASTCALL ARMoveToStartOfReplay(tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame) {
    tU32 old_rate;

    gDisable_piped_sounds = 1;
    memcpy(&old_rate, &gReplay_rate, sizeof(old_rate));
    gReplay_rate = -100.0f;
    MoveReplayBuffer((int)0x80000001u, pBetween_sessions, pBefore_frame, pAfter_frame);
    gDisable_piped_sounds = 0;
    memcpy(&gReplay_rate, &old_rate, sizeof(old_rate));
}

// FUNCTION: CARMA2_HW 0x00403cc0
void C2_HOOK_FASTCALL ARDoActionReplay(int pUnused, tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame) {
    (void)pUnused;
    if (!ARReplayIsReallyPaused()) {
        MoveReplayBuffer((int)ReplayFtolLow(gReplay_rate), pBetween_sessions, pBefore_frame, pAfter_frame);
    }
}

// FUNCTION: CARMA2_HW 0x00403d10
void C2_HOOK_FASTCALL ReverseSound(tS3_effect_tag pEffect, tS3_sound_tag pTag) {
    S3ReverseSample(pEffect, pTag);
}

// FUNCTION: CARMA2_HW 0x00403d00
int C2_HOOK_FASTCALL ARGoBackwards(void) {
    return S3SetSampleFilterCallbacks(ReverseSound, ReverseSound);
}

// FUNCTION: CARMA2_HW 0x00403d20
int C2_HOOK_FASTCALL ARGoForwards(void) {
    return S3SetSampleFilterCallbacks(NULL, NULL);
}

// FUNCTION: CARMA2_HW 0x00403d30
void C2_HOOK_FASTCALL ARService(void) {
}

// FUNCTION: CARMA2_HW 0x00403d40
void C2_HOOK_FASTCALL ARMainLoopStart(void) {
    gAction_replay_mode = 0;
    gLast_replay_frame_time = PDGetTotalTime();
}

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

// FUNCTION: CARMA2_HW 0x004c6f20
void C2_HOOK_FASTCALL AddSparkToPipingSession(int pSpark_index, const br_vector3* pPos, const br_vector3* pV) {
    ARAddVariedDataToSession(ePipe_chunk_spark, pSpark_index, 2,
        12, 0, pPos,
        12, 12, pV);
}

// FUNCTION: CARMA2_HW 0x004c6f40
void C2_HOOK_FASTCALL AddShrapnelToPipingSession(int pShrapnel_index, const br_vector3* pPos, tU16 pAge, br_material* pMaterial) {
    if (pShrapnel_index & 0x8000) {
        ARAddVariedDataToSession(ePipe_chunk_shrapnel, pShrapnel_index, 3,
            12, 0, pPos,
            2, 12, (int)pAge,
            4, 16, pMaterial);
    } else {
        ARAddVariedDataToSession(ePipe_chunk_shrapnel, pShrapnel_index, 1,
            12, 0, pPos);
    }
}

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

// FUNCTION: CARMA2_HW 0x004c7550
void C2_HOOK_FASTCALL AddModelMashToPipingSession(br_model* pModel, br_vector3* pMash) {
    size_t size;
    tPipe_model_mash* pipe_model_mash;

    pipe_model_mash = (tPipe_model_mash*)gCrush_space;
    if (pipe_model_mash == NULL) {
        return;
    }
    size = pModel->nvertices * sizeof(tCompressed_vector3) + sizeof(br_model*);
    if (size > CRUSH_SPACE_SIZE) {
        FatalError(kFatalError_ChunkIsTooLargeToPipe);
    }
    pipe_model_mash->model = pModel;
    ARCompressModelMash(pipe_model_mash->vectors, pModel, pMash);
    ARAddDataToSession(ePipe_model_mash, (uintptr_t)-1, pipe_model_mash, (tU32)size);
}

// AddRepulseRayToPipingSession

// AddNapalmBoltToPipingSession

// PipeSingleCrush

// FUNCTION: CARMA2_HW 0x004c8090
void C2_HOOK_FASTCALL PipeSingleScreenWobble(int pX, int pY) {
    ARDoSingleVariedSession(ePipe_chunk_screen_wobble, 0, 2,
        1, 0, (uintptr_t)pX,
        1, 1, (uintptr_t)pY);
}

// FUNCTION: CARMA2_HW 0x004c80b0
void C2_HOOK_FASTCALL PipeSingleGrooveStop(int pGroove_index, br_matrix34* pMatrix, int pPath_interrupt, int pObject_interrupt, float pPath_resumption, float pObject_resumption) {
    tU32 bits;

    (void)pPath_interrupt;
    (void)pObject_resumption;
    memcpy(&bits, &pPath_resumption, sizeof(bits));
    gUNK_0069410c = bits;
    /* Both replay channels share this stop status and resumption. */
    ARDoSingleVariedSession(13, (uintptr_t)(tU32)pGroove_index, 5,
        4, 0, (uintptr_t)(tU32)pObject_interrupt,
        4, 4, (uintptr_t)(tU32)pObject_interrupt,
        4, 8, (uintptr_t)bits,
        4, 12, (uintptr_t)bits,
        48, 16, (uintptr_t)pMatrix);
}

// FUNCTION: CARMA2_HW 0x004c80f0
void C2_HOOK_FASTCALL PipeSingleNonCar(uintptr_t pId, br_actor* pActor) {
    ARDoSingleVariedSession(ePipe_chunk_non_car, pId, 2,
        4, 0, (uintptr_t)pActor,
        48, 4, (uintptr_t)&pActor->t.t.mat);
}

// PipeSingleOilSpill

// PipeSingleFrameFinish

// PipeSingleGraphicalWheelStuff

// FUNCTION: CARMA2_HW 0x004c84a0
void C2_HOOK_FASTCALL PipeSingleSound(tS3_outlet* pOutlet, int pSound, tS3_volume pL_volume, tS3_volume pR_volume, int pPitch, const br_vector3* pPosition) {
    tU32 packed_volume;
    int outlet_index;

    if (gAction_replay_mode != 0 || !gProgram_state.racing) {
        return;
    }
    if (pPosition == NULL) {
        pPosition = &gZero_vector__smash;
    }
    /* Pack the channel volumes for the replay record's 16-bit volume field. */
    packed_volume = ((tU32)pR_volume << 8) + (tU32)pL_volume;
    outlet_index = GetIndexFromOutlet(pOutlet);
    ARDoSingleVariedSession(2, (uintptr_t)(tU32)pSound, 4,
        4, 0, (uintptr_t)(tU32)pPitch,
        12, 4, (uintptr_t)pPosition,
        2, 16, (uintptr_t)(tU32)outlet_index,
        2, 18, (uintptr_t)packed_volume);
}

// PipeSingleSpecial

// PipeSingleCarIncident

// PipeSinglePedIncident

// PipeSingleWallIncident

// PipeSingleSkidAdjustment

// FUNCTION: CARMA2_HW 0x004c8620
void C2_HOOK_FASTCALL PipeSingleFlap(br_actor* pActor, br_angle pAngle) {
    ARDoSingleVariedSession(ePipe_chunk_flap_bits, (uintptr_t)pActor, 2,
        4, 0, (uintptr_t)pActor,
        2, 4, (uintptr_t)pAngle);
}

// FUNCTION: CARMA2_HW 0x004c86b0
void C2_HOOK_FASTCALL PipeSingleRelink(br_actor* pParent, br_actor* pActor) {
    br_actor* old_parent = pActor->parent;

    ARDoSingleVariedSession(25, (uintptr_t)pActor, 3,
        4, 0, (uintptr_t)pActor,
        4, 4, (uintptr_t)old_parent,
        4, 8, (uintptr_t)pParent);
}

// FUNCTION: CARMA2_HW 0x004c86d0
void C2_HOOK_FASTCALL PipeSingleIdentity(br_actor* pActor) {
    ARDoSingleVariedSession(26, (uintptr_t)pActor, 1, 4, 0, (uintptr_t)pActor);
}

// FUNCTION: CARMA2_HW 0x004c86f0
void C2_HOOK_FASTCALL PipeSingleSplitWeld(uintptr_t pId, const br_vector4* pPlane, int pSplit) {
    ARDoSingleVariedSession(27, pId, 2,
        16, 0, (uintptr_t)pPlane,
        4, 16, (uintptr_t)pSplit);
}

// FUNCTION: CARMA2_HW 0x004c8870
void C2_HOOK_FASTCALL PipeSingleShrapnelShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, br_vector3* pArg5, br_vector3* pArg6, float pArg7, br_vector3* pArg8, br_vector3* pArg9, undefined4 pArg10, br_bounds3* pArg11, undefined4 pArg12, br_matrix34* pArg13, br_vector3* pArg14, void* pArg15) {
    ARDoSingleVariedSession(30, pId, 14,
        4, 64, (uintptr_t)pArg3,
        4, 0, (uintptr_t)pArg4,
        12, 4, (uintptr_t)pArg5,
        12, 16, (uintptr_t)pArg6,
        4, 156, (uintptr_t)ReplayFtolLow(pArg7),
        12, 28, (uintptr_t)pArg8,
        12, 40, (uintptr_t)pArg9,
        4, 68, (uintptr_t)pArg2,
        48, 80, (uintptr_t)pArg13,
        4, 76, (uintptr_t)pArg12,
        4, 128, (uintptr_t)pArg10,
        24, 132, (uintptr_t)pArg11,
        12, 52, (uintptr_t)pArg14,
        48, 160, (uintptr_t)pArg15);
}

// FUNCTION: CARMA2_HW 0x004c8920
void C2_HOOK_FASTCALL PipeSingleExplosion(int pIndex, const tExplosion* pData) {
    ARDoSingleVariedSession(31, (uintptr_t)pIndex, 1, 120, 0, (uintptr_t)pData);
}

// PipeSingleNonCarCreation

// PipeSingleSmashModelChange

// PipeSingleSmashRemoveFaces

// PipeSingleSmashDecal

// FUNCTION: CARMA2_HW 0x004c8a10
void C2_HOOK_FASTCALL PipeSingleSmashTextureChange(tCar_spec* pCar_spec, br_material* pMaterial, br_pixelmap* pTexture) {
    br_pixelmap* old_texture = pMaterial->colour_map;

    ARDoSingleVariedSession(ePipe_smash_texture_change, (uintptr_t)pMaterial, 3,
        4, 0, (uintptr_t)pCar_spec,
        4, 8, (uintptr_t)old_texture,
        4, 12, (uintptr_t)pTexture);
}

// PipeSinglePedStatus

// PipeSinglePedDir

// PipeSinglePedMove

// PipeSinglePedPhysics

// PipeSinglePedDismember

// PipeSinglePedFPChange

// PipeSinglePedPos

// PipeSinglePedModelChange

// FUNCTION: CARMA2_HW 0x004c8cc0
void C2_HOOK_FASTCALL PipeSinglePHILObject(tPhysics_object* object) {
    br_actor* actor = object->actor;
    br_matrix34* matrix = &actor->t.t.mat;

    ARDoSingleVariedSession(0x30, (uintptr_t)object, 1, 0x30, 0, (uintptr_t)matrix);
}

// FUNCTION: CARMA2_HW 0x004c8ce0
void C2_HOOK_FASTCALL PipeSingleGibShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, undefined4 pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8) {
    gUNK_0069410c = pArg4;
    ARDoSingleVariedSession(49, pId, 7,
        4, 8, (uintptr_t)pArg2,
        4, 4, (uintptr_t)pArg3,
        4, 12, (uintptr_t)gUNK_0069410c,
        4, 0, (uintptr_t)pArg5,
        12, 16, (uintptr_t)pArg6,
        12, 28, (uintptr_t)pArg7,
        12, 40, (uintptr_t)pArg8);
}

// FUNCTION: CARMA2_HW 0x004c8d40
void C2_HOOK_FASTCALL PipeSingleBloodSpurt(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6) {
    ARDoSingleVariedSession(50, pId, 5,
        4, 4, (uintptr_t)pArg2,
        4, 0, (uintptr_t)pArg3,
        12, 8, (uintptr_t)pArg4,
        12, 20, (uintptr_t)pArg5,
        12, 32, (uintptr_t)pArg6);
}

// FUNCTION: CARMA2_HW 0x004c8d80
void C2_HOOK_FASTCALL PipeSingleGrooveOnOff(int pOn, br_actor* pActor, int pBlock_flags) {
    ARDoSingleVariedSession(ePipe_single_groove_on_off, (uintptr_t)pActor, 2,
        4, 0, (uintptr_t)(tU32)pOn,
        4, 4, (uintptr_t)(tU32)pBlock_flags);
}

// PipeSinglePowerupGot

// PipeSinglePowerupLose

// FUNCTION: CARMA2_HW 0x004c8de0
void C2_HOOK_FASTCALL PipeSingleFunkEnable(int pIndex, int pFlags) {
    ARDoSingleVariedSession(ePipe_funk_enable, (uintptr_t)pIndex, 1, 2, 0, (uintptr_t)(tU16)pFlags);
}

// PipeSingleVanishedDismembered

// FUNCTION: CARMA2_HW 0x004c8e30
void C2_HOOK_FASTCALL PipeSingleDSModel(tU16 pArg1, void* pThing) {
    /* A zero value identifies a car; a nonzero value identifies a model. */
    ARDoSingleVariedSession(ePipe_single_ds_model, (uintptr_t)pThing, 1, 2, 0, (uintptr_t)pArg1);
}

// FUNCTION: CARMA2_HW 0x004c8e80
void C2_HOOK_FASTCALL PipeSingleVector3(br_vector3* pDest, br_vector3* pValue) {
    ARDoSingleVariedSession(ePipe_single_vector3, (uintptr_t)pDest, 1, 12, 0, (uintptr_t)pValue);
}

// PipeSingleDroneRender

// PipeSingleDroneCornerPos

// PipeSingleDroneStraightPos

// PipeSinglePowerupRespawn

// PipeSingleShitMine

// PipeSingleEndShitMine

// FUNCTION: CARMA2_HW 0x004c8ff0
void C2_HOOK_FASTCALL PipeSingleTransformType(br_actor* pActor, tS16 pTransform_type) {
    tU16 type_from = pActor->t.type;

    ARDoSingleVariedSession(ePipe_single_transform_type, (uintptr_t)pActor, 2,
        1, 0, (uintptr_t)type_from,
        1, 1, (uintptr_t)(tU16)pTransform_type);
}

// PipeSingleOppoRenderage

// ApplyCrush

// ApplySmudge

// ApplySpark

// ApplyShrapnel

// FUNCTION: CARMA2_HW 0x004c9180
void C2_HOOK_FASTCALL ApplyScreenWobble(tPipe_chunk** pChunk) {
    const tU8* chunk = (const tU8*)*pChunk;
    tPipe_chunk_screen_wobble wobble;

    memcpy(&wobble, chunk + 4, sizeof(wobble));
    SetScreenWobble(wobble.wobble_x, wobble.wobble_y);
}

// ApplyGrooveStop

// ApplyNonCar

// ApplySmoke

// ApplySmokeColumn

// ApplyFlame

// ApplySplash

// ApplyExtendedSplash

// ApplyOilSpill

// FUNCTION: CARMA2_HW 0x004c9360
void C2_HOOK_FASTCALL ApplyFrameBoundary(tPipe_chunk** pChunk) {
    const tU8* chunk = (const tU8*)*pChunk;
    tU32 time;

    memcpy(&time, chunk + 4, sizeof(time));
    ApplyTime(time);
}

// ApplySound

// ApplyCar

// ApplyGWS

// ApplyDamage

// ApplySpecial

// ApplyProxRay

// ApplySkidAdjustment

// ApplyFlap

// ApplyModelMash

// FUNCTION: CARMA2_HW 0x004c9850
void C2_HOOK_FASTCALL ApplyRelink(tPipe_chunk** pChunk) {
    const tU8* chunk = (const tU8*)*pChunk;
    br_actor* actor;
    br_actor* parent;

    memcpy(&actor, chunk + 4, sizeof(actor));
    memcpy(&parent, chunk + 12, sizeof(parent));
    if (actor->parent != NULL) {
        if (parent != NULL) {
            BrActorRelink(parent, actor);
        } else {
            BrActorRemove(actor);
        }
    } else if (parent != NULL) {
        BrActorAdd(parent, actor);
    }
}

// FUNCTION: CARMA2_HW 0x004c9890
void C2_HOOK_FASTCALL ApplyIdentity(tPipe_chunk** pChunk) {
    const tU8* chunk = (const tU8*)*pChunk;
    br_actor* actor;

    memcpy(&actor, chunk + 4, sizeof(actor));
    BrMatrix34Identity(&actor->t.t.mat);
}

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

// FUNCTION: CARMA2_HW 0x004c9be0
void C2_HOOK_FASTCALL ApplyGrooveOnOff(tPipe_chunk** pChunk) {
    const tU8* chunk = (const tU8*)*pChunk;
    int on;
    tU32 block_flags;
    br_actor* actor;

    memcpy(&on, chunk + 4, sizeof(on));
    memcpy(&block_flags, chunk + 8, sizeof(block_flags));
    memcpy(&actor, chunk, sizeof(actor));
    if (on != 0) {
        EnableGroovidelic(actor, block_flags);
    } else {
        DisableGroovidelic(actor, block_flags);
    }
}

// ApplyPowerupGot

// ApplyPowerupLose

// FUNCTION: CARMA2_HW 0x004c9c20
void C2_HOOK_FASTCALL ApplyFunkEnable(tPipe_chunk** pChunk) {
    const tU8* chunk = (const tU8*)*pChunk;
    tS16 flags;
    int index;

    memcpy(&flags, chunk + 4, sizeof(flags));
    memcpy(&index, chunk, sizeof(index));
    AdjustFunkEnable(index, flags);
}

// ApplyVanishDismembered

// FUNCTION: CARMA2_HW 0x004c9c40
void C2_HOOK_FASTCALL ApplyDSModel(tPipe_chunk** pChunk) {
    const tU8* chunk = (const tU8*)*pChunk;
    tU16 type;
    void* thing;

    memcpy(&type, chunk + 4, sizeof(type));
    memcpy(&thing, chunk, sizeof(thing));
    if (type != 0) {
        MakeModelMaterialsDoubleSided(thing);
    } else {
        tCar_spec* car_spec = thing;

        DRActorEnumRecurse(car_spec->car_model_actor, MakeCarModelsMaterialsSingleSided, car_spec);
    }
}

// ApplyPedDiagnostics

// FUNCTION: CARMA2_HW 0x004c9c90
void C2_HOOK_FASTCALL ApplyVector3(tPipe_chunk** pChunk) {
    int i;

    for (i = 0; i < 3; i++) {
        const tU8* chunk = (const tU8*)*pChunk;
        br_vector3* vector;
        br_scalar value;

        memcpy(&vector, chunk, sizeof(vector));
        memcpy(&value, chunk + 4 + i * sizeof(value), sizeof(value));
        vector->v[i] = value + vector->v[i];
    }
}

// ApplyDroneRender

// ApplyDroneCornerPos

// ApplyDroneStraightPos

// ApplyDroneUnused

// ApplyNapalmBolt

// ApplyPowerupRespawn

// ApplyShitMine

// ApplyEndShitMine

// FUNCTION: CARMA2_HW 0x004c9d70
void C2_HOOK_FASTCALL ApplyTransformType(tPipe_chunk** pChunk) {
    const tU8* chunk = (const tU8*)*pChunk;
    br_actor* actor;
    tU8 type;

    type = chunk[5];
    memcpy(&actor, chunk, sizeof(actor));
    actor->t.type = type;
}

// ApplyOppoRenderage

// UndoCrush

// UndoSmudge

// UndoDamage

// UndoSpecial

// FUNCTION: CARMA2_HW 0x004c9e50
void C2_HOOK_FASTCALL UndoScreenWobble(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    (void)pChunk;
    ARDisableAdvance();
    if (pPrev_chunk != NULL) {
        ApplyScreenWobble(&pPrev_chunk);
    } else {
        SetScreenWobble(0, 0);
    }
    AREnableAdvance();
}

// UndoSplash

// UndoExtendedSplash

// UndoSkidAdjustment

// UndoFlap

// UndoModelMash

// FUNCTION: CARMA2_HW 0x004c9f90
void C2_HOOK_FASTCALL UndoRelink(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    const tU8* chunk = (const tU8*)*pChunk;
    br_actor* actor;
    br_actor* parent;

    (void)pPrev_chunk;
    memcpy(&actor, chunk + 4, sizeof(actor));
    memcpy(&parent, chunk + 8, sizeof(parent));
    if (actor->parent != NULL) {
        if (parent != NULL) {
            BrActorRelink(parent, actor);
        } else {
            BrActorRemove(actor);
        }
    } else if (parent != NULL) {
        BrActorAdd(parent, actor);
    }
}

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

// FUNCTION: CARMA2_HW 0x004ca2d0
void C2_HOOK_FASTCALL UndoGrooveOnOff(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    const tU8* chunk = (const tU8*)*pChunk;
    int on;
    tU32 block_flags;
    br_actor* actor;

    (void)pPrev_chunk;
    memcpy(&on, chunk + 4, sizeof(on));
    memcpy(&block_flags, chunk + 8, sizeof(block_flags));
    memcpy(&actor, chunk, sizeof(actor));
    if (on != 0) {
        DisableGroovidelic(actor, block_flags);
    } else {
        EnableGroovidelic(actor, block_flags);
    }
}

// UndoPowerupGot

// UndoPowerupLose

// FUNCTION: CARMA2_HW 0x004ca310
void C2_HOOK_FASTCALL UndoFunkEnable(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    const tU8* chunk = (const tU8*)*pChunk;
    tS16 flags;
    int index;

    (void)pPrev_chunk;
    memcpy(&flags, chunk + 4, sizeof(flags));
    memcpy(&index, chunk, sizeof(index));
    AdjustFunkEnable(index, (tS16)(flags == 0));
}

// UndoVanishDismembered

// FUNCTION: CARMA2_HW 0x004ca360
void C2_HOOK_FASTCALL UndoVector3(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    int i;

    (void)pPrev_chunk;
    for (i = 0; i < 3; i++) {
        const tU8* chunk = (const tU8*)*pChunk;
        br_vector3* vector;
        br_scalar value;

        memcpy(&vector, chunk, sizeof(vector));
        memcpy(&value, chunk + 4 + i * sizeof(value), sizeof(value));
        vector->v[i] = vector->v[i] - value;
    }
}

// UndoNapalmBolt

// UndoPowerupRespawn

// UndoShitMine

// UndoEndShitMine

// FUNCTION: CARMA2_HW 0x004ca3e0
void C2_HOOK_FASTCALL UndoTransformType(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    const tU8* chunk = (const tU8*)*pChunk;
    br_actor* actor;
    tU8 type;

    (void)pPrev_chunk;
    type = chunk[4];
    memcpy(&actor, chunk, sizeof(actor));
    actor->t.type = type;
}

// CalcCrushLength

// FUNCTION: CARMA2_HW 0x004ca400
int C2_HOOK_FASTCALL CalcShrapnelLength(tPipe_chunk* pChunk) {
    tU32 owner;

    memcpy(&owner, pChunk, sizeof(owner));
    return (owner & 0x8000) ? 20 : 12;
}

// CalcSmudgeLength

// FUNCTION: CARMA2_HW 0x004ca430
int C2_HOOK_FASTCALL CalcModelMashLength(tPipe_chunk* pChunk) {
    br_model* model;

    memcpy(&model, (const tU8*)pChunk + sizeof(tU32), sizeof(model));
    return (int)(model->nvertices * sizeof(tCompressed_vector3) + sizeof(model));
}

// CalcEndMyBendLength

// CalcRemoveFacesLength
