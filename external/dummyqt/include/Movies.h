#ifndef __MOVIES__
#define __MOVIES__

#include "Files.h"
#include "MacTypes.h"

#include "ImageCompression.h"

typedef struct MediaType**              Media;
typedef struct MovieType**              Movie;
typedef struct TrackType**              Track;

enum {
    flattenAddMovieToDataFork = 1L << 0,
};
enum {
    createMovieFileDeleteCurFile  = 1L << 31,
};

extern OSErr EnterMovies(void);

extern OSErr CloseMovieFile(short resRefNum);

extern void DisposeMovie(Movie theMovie);

extern OSErr EndMediaEdits(Media theMedia);

extern TimeValue GetMediaDuration(Media theMedia);

extern OSErr InsertMediaIntoTrack(
    Track       theTrack,
    TimeValue   trackStart,
    TimeValue   mediaTime,
    TimeValue   mediaDuration,
    Fixed       mediaRate);


extern Movie  FlattenMovieData(
    Movie           theMovie,
    long            movieFlattenFlags,
    const FSSpec *  theFile,
    OSType          creator,
    ScriptCode      scriptTag,
    long            createMovieFileFlags);

#endif /* __MOVIES__ */
