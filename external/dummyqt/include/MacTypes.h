#ifndef __MACTYPES__
#define __MACTYPES__

#include <stdint.h>

typedef long                      Fixed;
typedef int16_t                   OSErr;
typedef char                      Str255[256];
typedef Str255                    StrFileName;
typedef char *                    Ptr;
typedef Ptr *                     Handle;
typedef long                      TimeValue;
typedef unsigned char *           StringPtr;
typedef StringPtr *               StringHandle;
typedef int16_t                   ScriptCode;
typedef unsigned long             FourCharCode;
typedef FourCharCode              OSType;

enum {
    noErr                         = 0
};

struct Rect {
    short               top;
    short               left;
    short               bottom;
    short               right;
};

#endif /* __MACTYPES__ */
