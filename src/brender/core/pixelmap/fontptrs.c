#include "fontptrs.h"

#include "font3x5f.h"
#include "font4x6p.h"
#include "font7x9p.h"


// GLOBAL: CARMA2_HW 0x0066d7b8
br_font* BrFontFixed3x5 = &br_font3x5f;

// GLOBAL: CARMA2_HW 0x0066d7bc
br_font* BrFontProp4x6 = &br_font4x6p;

// GLOBAL: CARMA2_HW 0x0066d7c0
br_font* BrFontProp7x9 = &br_font7x9p;