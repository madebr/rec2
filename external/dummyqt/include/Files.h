#ifndef __FILES___
#define __FILES___

#include "MacTypes.h"

typedef struct FSSpec {
    short               vRefNum;
    long                parID;
    StrFileName         name;
} FSSpec;

#endif /* __FILES___ */
