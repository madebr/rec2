#include "devlist.h"

#include "fwsetup.h"
#include "image.h"
#include "pattern.h"
#include "resource.h"

#include "core/std/brstdlib.h"

#include "core/v1db/sys_conf.h"

br_error C2_HOOK_CDECL AddRequestedDrivers(void) {
    char devstr[256];

    if (!fw.bAlreadyLoadedDrivers) {
        BrSystemConfigQueryString(BRT_BRENDER_DRIVERS_STR, devstr, sizeof(devstr));
        if (devstr[0] != '\0') {
            BrDevAddConfig(devstr);
        }
        fw.bAlreadyLoadedDrivers = 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00528490
br_error C2_HOOK_STDCALL devAdd(br_device** pdev, br_device_begin_fn* dev_begin, const char* args, br_image* image) {
    int i;
    br_device* dev;
    br_open_device* new_slots;

    dev = dev_begin(args);
    if (dev == NULL) {
        return 0x1002;
    }
    for (i = 0; i < fw.ndev_slots; i++) {
        if (fw.dev_slots[i].dev == NULL) {
            fw.dev_slots[i].dev = dev;
            fw.dev_slots[i].image = image;
            if (pdev != NULL) {
                *pdev = dev;
            }
            return 0;
        }
    }
    new_slots = BrResAllocate(dev, (fw.ndev_slots + 16) * sizeof(br_open_device), BR_MEMORY_DRIVER);
    if (new_slots == NULL) {
        dev->dispatch->_free((br_object*)dev);
        /* FIXME: also do BrImageDereference? */
        return 0x1003;
    }
    for (i = 0; i < fw.ndev_slots; i++) {
        new_slots[i] = fw.dev_slots[i];
    }
    BrResFree(fw.dev_slots);
    fw.dev_slots = new_slots;
    fw.dev_slots[fw.ndev_slots].dev = dev;
    fw.dev_slots[fw.ndev_slots].image = image;
    fw.ndev_slots += 16;
    if (pdev != NULL) {
        *pdev = dev;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00528430
br_error C2_HOOK_CDECL BrDevAdd(br_device** pdev, const char* image, const char* args) {
    br_image* dev_image;
    br_device_begin_fn* dev_begin;

    dev_image = BrImageReference(image);
    if (dev_image == NULL) {
        return 0x1002;
    }
    dev_begin = BrImageLookupName(dev_image, "BrDrv1Begin", 0);
    if (dev_begin == NULL) {
        BrImageDereference(dev_image);
        return 0x1002;
    }
    return devAdd(pdev, dev_begin, args, dev_image);
}

// FUNCTION: CARMA2_HW 0x005285a0
br_error C2_HOOK_CDECL BrDevAddStatic(br_device** pdev, br_device_begin_fn* dev_begin, const char* args) {
    return devAdd(pdev, dev_begin, args, NULL);
}

// FUNCTION: CARMA2_HW 0x005285c0
br_error C2_HOOK_CDECL BrDevCheckAdd(br_device** pdev, const char* name, const char* args) {

    if (BrDevFind(pdev, name) == 0) {
        return 0;
    }
    return BrDevAdd(pdev, name, args);
}

// FUNCTION: CARMA2_HW 0x005286d0
br_error C2_HOOK_CDECL BrDevAddConfig(const char* config) {
    const char* end;
    char* dev;
    char* arg;
    char tmp[512];
    int n;

    while (1) {
        for (end = config; *end != '\0' && *end != ';'; end++) {
        }
        n = end - config;
        if (n != 0) {
            BrMemCpy(tmp, config, n);
            tmp[n] = '\0';
            dev = tmp;
            for (arg = tmp; *arg != '\0' && *arg != ',' && *arg != ':'; arg++) {
            }
            if (*arg != '\0') {
                *arg++ = '\0';
            }
            while (BrIsSpace(*dev)) {
                dev++;
            }
            n = BrStrLen(dev);
            if (n != 0) {
                while (BrIsSpace(dev[n - 1])) {
                    dev[n - 1] = '\0';
                    n--;
                }
                BrDevAdd(NULL, dev, arg);
            }
        }
        if (*end == ';') {
            config = end + 1;
        } else {
            break;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00528800
br_error C2_HOOK_CDECL BrDevRemove(br_device* dev) {
    int i;

    for (i = 0; i < fw.ndev_slots; i++) {
        if (fw.dev_slots[i].dev == dev) {
            fw.dev_slots[i].dev->dispatch->_free((br_object*)fw.dev_slots[i].dev);
            if (fw.dev_slots[i].image != NULL) {
                BrImageDereference(fw.dev_slots[i].image);
            }
            fw.dev_slots[i].dev = NULL;
            fw.dev_slots[i].image = NULL;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00528860
br_error C2_HOOK_CDECL BrDevFind(br_device** pdev, const char* pattern) {
    int i;

    AddRequestedDrivers();

    for (i = 0; i < fw.ndev_slots; i++) {
        if (fw.dev_slots[i].dev != NULL) {
            if (BrNamePatternMatch(pattern, fw.dev_slots[i].dev->dispatch->_identifier((br_object*)fw.dev_slots[i].dev))) {
                if (pdev != NULL) {
                    *pdev = fw.dev_slots[i].dev;
                }
                return 0;
            }
        }
    }
    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x00528930
br_error C2_HOOK_CDECL BrDevFindMany(br_device** devices, br_int_32* ndevices, br_int_32 max_devices, const char* pattern) {
    int i;
    int c;

    AddRequestedDrivers();

    c = 0;
    for (i = 0; i < fw.ndev_slots; i++) {
        if (fw.dev_slots[i].dev != NULL) {
            if (BrNamePatternMatch(pattern, fw.dev_slots[i].dev->dispatch->_identifier((br_object*)fw.dev_slots[i].dev))) {
                if (c >= max_devices) {
                    return 0x1004;
                }
                /* Order of operations differs from OG */
                devices[c] = fw.dev_slots[i].dev;
                c++;
            }
        }
    }
    if (ndevices != NULL) {
        *ndevices = c;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00528a10
br_error C2_HOOK_CDECL BrDevCount(br_int_32* ndevices, const char* pattern) {
    int i;
    int c;

    AddRequestedDrivers();

    c = 0;
    for (i = 0; i < fw.ndev_slots; i++) {
        if (fw.dev_slots[i].dev != NULL) {
            if (BrNamePatternMatch(pattern, fw.dev_slots[i].dev->dispatch->_identifier((br_object*)fw.dev_slots[i].dev))) {
                c++;
            }
        }
    }
    if (ndevices != NULL) {
        *ndevices = c;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00528ac0
br_error C2_HOOK_CDECL BrDevContainedFind(br_object** ph, br_token type, const char* pattern, br_token_value* tv) {
    int i;

    AddRequestedDrivers();

    for (i = 0; i < fw.ndev_slots; i++) {
        if (fw.dev_slots[i].dev != NULL) {
            if (fw.dev_slots[i].dev->dispatch->_find((br_object_container*)fw.dev_slots[i].dev, ph, type, pattern, tv) == 0) {
                return 0;
            }
        }
    }
    return 0x1002;
}

// FUNCTION: CARMA2_HW 0x00528b90
br_error C2_HOOK_CDECL BrDevContainedFindMany(br_object** objects, br_int_32 max_objects, br_int_32* pnum_objects, br_token type, const char* pattern, br_token_value* tv) {
    int i;
    br_int_32 n;
    br_int_32 total;
    br_error r;

    total = 0;

    AddRequestedDrivers();

    for (i = 0; i < fw.ndev_slots; i++) {
        if (fw.dev_slots[i].dev != NULL) {
            r = fw.dev_slots[i].dev->dispatch->_findMany((br_object_container*)fw.dev_slots[i].dev, objects, max_objects, &n, type, pattern, tv);
            if (r != 0) {
                return r;
            }
            objects += n;
            max_objects -= n;
            total += n;
        }
    }
    if (pnum_objects != NULL) {
        *pnum_objects = total;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00528c80
br_error C2_HOOK_CDECL BrDevContainedCount(br_int_32* pcount, br_token type, const char* pattern, br_token_value* tv) {
    int i;
    br_uint_32 n;
    br_int_32 total;
    br_error r;

    AddRequestedDrivers();

    total = 0;
    for (i = 0; i < fw.ndev_slots; i++) {
        if (fw.dev_slots[i].dev != NULL) {
            r = fw.dev_slots[i].dev->dispatch->_count((br_object_container*)fw.dev_slots[i].dev, &n, type, pattern, tv);
            if (r != 0) {
                return r;
            }
            total += n;
        }
    }
    if (pcount != NULL) {
        *pcount = total;
    }
    return 0;
}
