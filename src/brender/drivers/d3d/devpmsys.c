#include "devpmsys.h"

#include <stdlib.h>


br_error AllocateD3DSysMemPixelmap(br_device_pixelmap_d3d* self, br_device_pixelmap_d3d** newpm, int w, int h) {
    abort();
#if 0
    br_uint_16 qual;
    br_device *dev;
    br_device_pixelmap *devpm;
    struct pm_type_info *tip = pmTypeInfo + self->pm_type;

    UASSERT(self != NULL);
    UASSERT(newpm != NULL);
    UASSERT((w > 0) && (h > 0));

    /* Allocate new pixelmap */
    dev = ObjectDevice(self);
    devpm = BrResAllocate(Device3DfxResource(dev), sizeof(*devpm), BR_MEMORY_OBJECT);

    /* Fill members, mostly by copy */
    *devpm = *self;
    devpm->dispatch = &devicePixelmapDispatchSysmem;
    devpm->pm_width = w;
    devpm->pm_height = h;
    devpm->pm_row_bytes = tip->bits * tip->align * ((w+tip->align-1) / tip->align) / 8;
    HostSelectorDS(&qual);
    devpm->pm_pixels_qualifier = qual;
    devpm->buffer_type = BT_SYSMEM;

    /* Allocate pixels */
    devpm->pm_pixels = BrResAllocate(devpm, devpm->pm_row_bytes*devpm->pm_height, BR_MEMORY_PIXELS);

    *newpm = devpm;

    return BRE_OK;
#endif
}
C2_HOOK_FUNCTION(0x10001ec0, Allocate3DfxSysMemPixelmap)
