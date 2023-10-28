#include "errors.h"

#include "loading.h"
#include "utility.h"

#include "platform.h"

#include "c2_stdio.h"
#include "c2_string.h"

#include <stdarg.h>
#include <stdint.h>
//#include <string.h>

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gError_messages, 186, 0x00591708,
    "Unable to support this screen depth setting",
    "Couldn't allocate off-screen buffer",
    "Couldn't allocate Z-Buffer",
    "Couldn't allocate root actor",
    "Couldn't calculate the application's pathname",
    "Couldn't allocate camera",
    "Couldn't allocate self",
    "There was a problem with the Preferences file - try deleting it.",
    "Couldn't open General Settings file",
    "Couldn't open Key Map file",
    "Unable to find required palette",
    "Couldn't allocate and install standard lamp",
    "Couldn't allocate and install stupid test bastard",
    "Couldn't load FLIC file '%'",
    "Couldn't allocate memory to play FLIC file '%'",
    "The FLIC file '%' was not 8-bits deep",
    "Couldn't ascertain a frame-rate for FLIC file '%'",
    "A problem occurred whilst trying to free up some FLICs '%'",
    "Couldn't find a spare transient bitmap",
    "Save file was bigger than save structure",
    "Can't load font image '%'",
    "Can't load font width table '%'",
    "Can't load cursor image",
    "Can't load cursor giblet",
    "Screen width/height doesn't match graf data",
    "Can't load car resolution-independant file",
    "Can't load cockpit image",
    "Can't load damage image",
    "Can't open prat cam text file",
    "Premature flic specs end in prat cam text file",
    "Can't open prat cam flic file '%'",
    "Can't allocate memory for prat cam flic file",
    "Too many flic specs in prat cam text file",
    "Premature sequences end in prat cam text file",
    "Too many sequences in prat cam text file",
    "Can't load speedo image",
    "Can't load tacho image",
    "Pratcam sequence % has too many chunks",
    "Pratcam sequence % has too many alternatives",
    "Pratcam sequence % has too many sounds",
    "Can't load hands image",
    "Flying wombat alert: flic palette disposed before it's been allocated",
    "Can't load dials pix",
    "Trying to free a NULL pointer",
    "Trying to free an invalid pointer (%)",
    "Trying to free an already free block (%)",
    "Trying to lock an invalid pointer (%)",
    "Trying to unlock an invalid pointer (%)",
    "Out of memory (insufficient heap space)",
    "Out of memory (insufficient block slots)",
    "Can't open races file",
    "Can't load race scene image",
    "Can't load race map image",
    "Can't load race info image",
    "Can't open opponents file",
    "Opponents count mismatches actual number of opponents",
    "Can't load opponent mug-shot file",
    "Can't load grid image file",
    "Can't load part image file",
    "Can't load chrome font file",
    "Can't load funkotronic file",
    "Can't load groovadelic file",
    "Error within funkotronic file",
    "Error within groovidelic file",
    "Can't find main material '%' referenced in funkotronic file",
    "Can't find actor '%' referenced in groovidelic file",
    "Can't find animation frame pixelmap referenced in funkotronic file",
    "Insufficient pixelmap slots in storage area",
    "Insufficient shade table slots in storage area",
    "Insufficient material slots in storage area",
    "Insufficient model slots in storage area",
    "Can't load car actor",
    "Defined ref-num of controlled groovidelic/funkotronic is out of range",
    "Used ref-num of controlled groovidelic/funkotronic is out of range",
    "MAMS heap is corrupt",
    "Can't open pedestrian file",
    "Can't find pedestrian with ref num %",
    "Can't find pedestrian material '%'",
    "Can't find pedestrian pixelmap '%'",
    "Can't load pixelmap file '%' (or it's empty)",
    "Can't load shade table file '%' (or it's empty)",
    "Can't load material file '%' (or it's empty)",
    "Can't load model file '%' (or it's empty)",
    "Too many frames in pedestrian sequence",
    "Too many choices in pedestrian instructions",
    "Too many bearings in pedestrian sequences",
    "Pedestrian instruction marker not found",
    "Can't load depth-cue shade table",
    "Unrecognised opponent path type '%'",
    "Can't find sky material '%'",
    "Can't find sky pixelmap '%'",
    "Unknown damage type '%'",
    "Confused by format of conditional damage in car file",
    "Too many extra points for car index '%'",
    "Sorry, but Carmageddon has run out of memory (%)",
    "Piping buffer prematurely reached end",
    "Piping buffer prematurely reached beginning",
    "Piped pedestrian made no sense",
    "Something in the piping system went odd",
    "Can't open TEXT.TXT file",
    "Can't open DARE.TXT file",
    "Can't find the FLIC '%' referenced in the interface translation file",
    "Can't open headups file",
    "Can't open partshop file",
    "Can't open car resolution-dependant file",
    "Received new player list - and we're not in it!!!",
    "Can't open key names file",
    "Can't open '%'",
    "Too many planes in special volume (max is %)",
    "Can't load  a generated shade table",
    "Network code self-check failed",
    "Network message corrupted",
    "Memory has become corrupted",
    "Ran out of funk/groove slot bunches",
    "Net contents too big %",
    "File % is corrupted",
    "Random number out of range (%)",
    "Can't find file '%'",
    "Can't load crush data file '%'",
    "Can't load smash pix '%'",
    "Can't find smash material '%'",
    "Can't find smash model '%'",
    "More than 3 crush limits in crush data file",
    "Trying to remove non_car which isn't in either gActive_non_car_list or gActive_car_list",
    "Number of crush_datas for car is greater than kMax_crush_datas",
    "Chunk is too large to pipe",
    "Trying to relink an actor from NULL to NULL in action replay",
    "Didn't reach SNOT in '%' crush data",
    "NAN discovered, probably getting shagged by Grandad",
    "Can't find smash actor/model '%'",
    "Problem with special volume number @",
    "Too many envuironmental sound alternatives",
    "Can't find model '%' referenced in sound generator list",
    "Insufficient sound slots in storage area",
    "BONER ERROR: Unable to open file '%'",
    "BONER ERROR: Unable to allocate memory",
    "BONER ERROR: Unable to link form '%' to personality",
    "BONER ERROR: Unable to find model '%'",
    "BONER ERROR: Unable to find bone '%'",
    "BONER ERROR: Too many forms loaded",
    "BONER ERROR: Too many personalities loaded",
    "BONER ERROR: Too many moves loaded",
    "BONER ERROR: Too many remaps loaded",
    "BONER ERROR: Syntax error in form file (expected '%')",
    "BONER ERROR: Illegal move ID (%)",
    "BONER ERROR: ",
    "BONER ERROR: ",
    "BONER ERROR: ",
    "BONER ERROR: ",
    "BONER ERROR: ",
    "BONER ERROR: ",
    "BONER ERROR: ",
    "BONER ERROR: ",
    "BONER ERROR: ",
    "BONER ERROR: ",
    "BONER ERROR: ",
    "BONER ERROR: ",
    "BONER ERROR: ",
    "BONER ERROR: ",
    "BONER ERROR: ",
    "Can't open ped file '%'",
    "Core move (%) missing for %",
    "Bad move (%) attempted for %",
    "Cyclic move definition (%) for %",
    "Can't find ped spawn material '%'",
    "Can't ped move index out of range for material '%'",
    "Can't ped group index out of range for material '%'",
    "PHIL Integrity Failure: %",
    "Can't find ped texture '%'",
    "Can't find giblet model '%'",
    "Can't find ped sound file '%'",
    "Crush data file '%' is version @. Normal people are using version @.",
    "Problem with blooded material '%'",
    "Shape data is wrong",
    "Unknown non-car command",
    "Unable to open drone file % or file corrupted",
    "File % must start with \"%\"",
    "Can't cope with version % for %",
    "Mysterious \"%\" in %",
    "Duplicate model \"%\"",
    "Duplicate material \"%\"",
    "Duplicate pixelmap \"%\"",
    "Oh my God! Attempt made to write to packed file \"%\". This is stupid.",
    "Could not create textures pages \"%\".",
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gError_code, 0x006b2e00);

FILE* gDiagnostic_file;

void C2_NORETURN C2_HOOK_CDECL FatalError(int pStr_index, ...) {
    char the_str[1024];
    char* sub_str;
    char temp_str[1024];
    char* sub_pt;
    int sub_int;
    va_list ap;

    C2_HOOK_STARTF("pStr_index=%d", pStr_index);

    if (pStr_index < 1000) {
        c2_strcpy(the_str, C2V(gError_messages)[pStr_index]);
    } else {
        c2_strcpy(the_str, "Physics Error: %");
    }

    sub_pt = temp_str;
    va_start(ap, pStr_index);
    while (1) {
        sub_pt = c2_strchr(the_str, '%');
        if (sub_pt == NULL) {
            break;
        }
        sub_str = va_arg(ap, char*);
        StripCRNL(sub_str);
        c2_strcpy(temp_str, sub_pt + 1);
        c2_strcpy(sub_pt, sub_str);
        c2_strcat(the_str, temp_str);
    }

    dr_dprintf("FatalError After strings %s\n", the_str);
    sub_pt = the_str;
    while (1) {
        sub_str = c2_strchr(the_str, '@');
        if (sub_str == NULL) {
            break;
        }
        sub_int = va_arg(ap, int);
        c2_strcpy(temp_str, sub_str + 1);
        c2_sprintf(sub_str, "%d", sub_int);
        StripCRNL(sub_str);
        c2_strcat(the_str, temp_str);
    }
    va_end(ap);
    dr_dprintf("FatalError Finish %s\n", the_str);

    dr_dprintf(the_str);
    PDFatalError(the_str);
}
C2_HOOK_FUNCTION(0x0044c230, FatalError)

//// IDA: void __cdecl NonFatalError(int pStr_index, ...)
//void NonFatalError(int pStr_index, ...) {
//    char the_str[256];
//    char* sub_str;
//    char temp_str[256];
//    char* sub_pt;
//    va_list ap;
//    int i;
//    LOG_TRACE("(%d)", pStr_index);
//
//    va_start(ap, pStr_index);
//
//    strcpy(the_str, gError_messages[pStr_index - 1]);
//    sub_pt = temp_str;
//
//    for (i = 0; i < strlen(the_str); i++) {
//        if (the_str[i] == '%') {
//            sub_str = va_arg(ap, char*);
//            StripCRNL(sub_str);
//            strcpy(sub_pt, sub_str);
//            sub_pt += strlen(sub_str);
//        } else {
//            *sub_pt = the_str[i];
//            sub_pt++;
//        }
//    }
//    *sub_pt = 0;
//    va_end(ap);
//    PDNonFatalError(temp_str);
//}
//
//// IDA: void __cdecl CloseDiagnostics()
//void CloseDiagnostics() {
//    LOG_TRACE("()");
//
//    fclose(gDiagnostic_file);
//}
//

// This function is stripped from the retail binary, we've guessed at the implementation
void C2_HOOK_FASTCALL OpenDiagnostics(void) {
    // FIXME: use c2_stdio functions
//    gDiagnostic_file = c2_fopen("DIAGNOST.TXT", "w");
    gDiagnostic_file = c2_fopen("DIAGNOST.TXT", "w");
    if (gDiagnostic_file == NULL) {
        return;
    }
    c2_fputs("DIAGNOSTIC OUTPUT\n", gDiagnostic_file);
    // todo: generate a real date
    c2_fprintf(gDiagnostic_file, "Date / time : %s\n\n\n", __DATE__ " : " __TIME__);
}
C2_HOOK_FUNCTION(0x0044c580, OpenDiagnostics);

// Renamed from dprintf to avoid collisions to stdio
// This function is stripped from the retail binary, we've guessed at the implementation
void C2_HOOK_CDECL dr_dprintf(const char* fmt_string, ...) {
    va_list args;
    tU32 the_time;

    // FIXME: use c2_stdio functions
    va_start(args, fmt_string);
    C2_HOOK_VDEBUGF(fmt_string, args);
    va_end(args);

    if (gDiagnostic_file == NULL) {
        OpenDiagnostics();
    }
    the_time = PDGetTotalTime();
    c2_fprintf(gDiagnostic_file, "%7d.%03d: ", the_time / 1000, the_time % 1000);

    va_start(args, fmt_string);
    c2_vfprintf(gDiagnostic_file, fmt_string, args);
    va_end(args);

    c2_fputs("\n", gDiagnostic_file);

    c2_fflush(gDiagnostic_file);
}
C2_HOOK_FUNCTION(0x0044c590, dr_dprintf);
