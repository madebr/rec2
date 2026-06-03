#include "ImageCompression.h"
#include "Movies.h"
#include "QTML.h"

#include <stdlib.h>

// STUB: CARMA2_HW 0x00575950
OSErr C2_HOOK_CDECL InitializeQTML(long flag) {
    (void)flag;
    return (OSErr)0xf7d3;
}

StringPtr c2pstr(char *aStr) {
    (void)aStr;
    return NULL;
}

void DisposeHandle(Handle h) {
    (void)h;
}

// STUB: CARMA2_HW 0x0056a8e0
OSErr EnterMovies(void) {
    return 0x1;
}

OSErr CloseMovieFile(short resRefNum) {
    (void)resRefNum;
    return 0x1;
}

void DisposeMovie(Movie theMovie) {
    (void)theMovie;
}

OSErr EndMediaEdits(Media theMedia) {
    (void)theMedia;
    return 1;
}

TimeValue GetMediaDuration(Media theMedia) {
    (void)theMedia;
    return 0;
}

OSErr CDSequenceEnd(ImageSequence seqID) {
    (void)seqID;
    return 1;
}

OSErr InsertMediaIntoTrack(Track theTrack, TimeValue trackStart, TimeValue mediaTime, TimeValue mediaDuration, Fixed mediaRate) {
    (void)theTrack;
    (void)trackStart;
    (void)mediaTime;
    (void)mediaDuration;
    (void)mediaRate;
    return 1;
}

Movie FlattenMovieData(Movie theMovie, long movieFlattenFlags, const FSSpec *theFile, OSType creator, ScriptCode scriptTag, long createMovieFileFlags) {
    (void)theMovie;
    (void)movieFlattenFlags;
    (void)theFile;
    (void)creator;
    (void)scriptTag;
    (void)createMovieFileFlags;
    return NULL;
}

OSErr CreateMovieFile(const FSSpec* fileSpec, OSType creator, ScriptCode scriptTag, long createMovieFileFlags, short* resRefNum, Movie* newmovie) {
    return 1;
}
