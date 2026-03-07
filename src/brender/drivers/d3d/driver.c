#include "backend.h"
#include "device.h"
#include "outfcty.h"
#include "plib.h"
#include "timestmp.h"

#include <brender/brender.h>

#include "c2_hooks.h"


// FUNCTION: D3D 0x10001320
br_device* C2_HOOK_CDECL BrDrv1Begin(const char* arguments) {
    int type_count;
    br_device_d3d* device = &DriverDeviceD3D;
    br_error r;

    PrimD3DDriverTimestamp = TIMESTAMP_START;

    DetectD3DDevices();

    if (DeviceD3DInitialise(device) != 0) {
        return NULL;
    }

    r = PrimitiveLibraryD3DInitialise(&PrimitiveLibraryD3D, device);
    if (r != 0) {
        return NULL;
    }

    type_count = OutputFacilityD3DInitialise(device, &PrimitiveLibraryD3D);

    if (type_count == 0) {
        return NULL;
    }
    return (br_device*)device;
}