/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "ImageCompression.h"
#include "Movies.h"
#include "QTML.h"

#include <stdlib.h>

// STUB: CARMA2_HW 0x00575950
// STUB: CARMA2_SW 0x00570ff0
OSErr C2_HOOK_CDECL InitializeQTML(long flag) {
    (void)flag;
    return (OSErr)0xf7d3;
}

// STUB: CARMA2_SW 0x005709e0
// STUB: CARMA2_HW 0x00575340
StringPtr c2pstr(char *aStr) {
    (void)aStr;
    return NULL;
}

// STUB: CARMA2_SW 0x00570ac0
// STUB: CARMA2_SW 0x0056c160
void DisposeHandle(Handle h) {
    (void)h;
}

// STUB: CARMA2_HW 0x0056a8e0
// STUB: CARMA2_SW 0x00565f80
OSErr EnterMovies(void) {
    return 0x1;
}

// STUB: CARMA2_HW 0x0056baa0
// STUB: CARMA2_SW 0x00567140
OSErr CloseMovieFile(short resRefNum) {
    (void)resRefNum;
    return 0x1;
}

// STUB: CARMA2_HW 0x0056abe0
// STUB: CARMA2_SW 0x00566280
void DisposeMovie(Movie theMovie) {
    (void)theMovie;
}

// STUB: CARMA2_HW 0x0056b360
// STUB: CARMA2_SW 0x00566a00
OSErr EndMediaEdits(Media theMedia) {
    (void)theMedia;
    return 1;
}

// STUB: CARMA2_HW 0x0056b240
// STUB: CARMA2_SW 0x005668e0
TimeValue GetMediaDuration(Media theMedia) {
    (void)theMedia;
    return 0;
}

// lIBRARY: CARMA2_HW 0x0056dac0
// lIBRARY: CARMA2_SW 0x00569160
OSErr CDSequenceEnd(ImageSequence seqID) {
    (void)seqID;
    return 1;
}

// STUB: CARMA2_HW 0x0056c600
// STUB: CARMA2_SW 0x00567ca0
OSErr InsertMediaIntoTrack(Track theTrack, TimeValue trackStart, TimeValue mediaTime, TimeValue mediaDuration, Fixed mediaRate) {
    (void)theTrack;
    (void)trackStart;
    (void)mediaTime;
    (void)mediaDuration;
    (void)mediaRate;
    return 1;
}

// STUB: CARMA2_HW 0x0056c800
// STUB: CARMA2_SW 0x00567ea0
Movie FlattenMovieData(Movie theMovie, long movieFlattenFlags, const FSSpec *theFile, OSType creator, ScriptCode scriptTag, long createMovieFileFlags) {
    (void)theMovie;
    (void)movieFlattenFlags;
    (void)theFile;
    (void)creator;
    (void)scriptTag;
    (void)createMovieFileFlags;
    return NULL;
}

// STUB: CARMA2_HW 0x0056c720
// STUB: CARMA2_SW 0x00567dc0
OSErr CreateMovieFile(const FSSpec* fileSpec, OSType creator, ScriptCode scriptTag, long createMovieFileFlags, short* resRefNum, Movie* newmovie) {
    return 1;
}
