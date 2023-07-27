#include "drone.h"

#include "errors.h"

#include "c2_stdio.h"
#include "c2_string.h"

#include <stdarg.h>

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tDrone, gDrones, 64, 0x00682178);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_drones, 0x0068450c);

void C2_HOOK_CDECL DroneDebug(const char* format, ...) {
// Disabled because too noisy
#if 0
    va_list ap;

    va_start(ap, format);
    c2_vfprintf(c2_stderr, format, ap);
    c2_fputc('\n', c2_stderr);
    va_end(ap);
#endif
}
C2_HOOK_FUNCTION(0x0044cfc0, DroneDebug)
