#include "devlist.h"

#include "fwsetup.h"
#include "image.h"
#include "pattern.h"
#include "resource.h"

#include "core/std/brstdlib.h"

#include "core/v1db/sys_conf.h"

br_error C2_HOOK_CDECL AddRequestedDrivers(void) {
    char devstr[256];

    if (!C2V(fw).bAlreadyLoadedDrivers) {
        BrSystemConfigQueryString(BRT_BRENDER_DRIVERS_STR, devstr, sizeof(devstr));
        if (devstr[0] != '\0') {
            BrDevAddConfig(devstr);
        }
        C2V(fw).bAlreadyLoadedDrivers = 1;
    }
    return 0;
}

br_error (C2_HOOK_STDCALL * devAdd_original)(br_device** pdev, br_device_begin_fn* dev_begin, char* args, br_image* image);
br_error C2_HOOK_STDCALL devAdd(br_device** pdev, br_device_begin_fn* dev_begin, char* args, br_image* image) {
#if 0//defined(C2_HOOKS_ENABLED)
return devAdd_original(pdev, dev_begin, args, image);
#else
    int i;
    br_device* dev;
    br_open_device* new_slots;

    dev = dev_begin(args);
    if (dev == NULL) {
        return 0x1002;
    }
    for (i = 0; i < C2V(fw).ndev_slots; i++) {
        if (C2V(fw).dev_slots[i].dev == NULL) {
            C2V(fw).dev_slots[i].dev = dev;
            C2V(fw).dev_slots[i].image = image;
            if (pdev != NULL) {
                *pdev = dev;
            }
            return 0;
        }
    }
    new_slots = BrResAllocate(dev, (C2V(fw).ndev_slots + 16) * sizeof(br_open_device), BR_MEMORY_DRIVER);
    if (new_slots == NULL) {
        dev->dispatch->_free((br_object*)dev);
        /* FIXME: also do BrImageDereference? */
        return 0x1003;
    }
    for (i = 0; i < C2V(fw).ndev_slots; i++) {
        new_slots[i].dev = C2V(fw).dev_slots[i].dev = dev;
        new_slots[i].image = C2V(fw).dev_slots[i].image = image;
    }
    BrResFree(C2V(fw).dev_slots);
    C2V(fw).dev_slots = new_slots;
    C2V(fw).dev_slots[C2V(fw).ndev_slots].dev = dev;
    C2V(fw).dev_slots[C2V(fw).ndev_slots].image = image;
    C2V(fw).ndev_slots++;
    if (pdev != NULL) {
        *pdev = dev;
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528490, devAdd, devAdd_original)

br_error (C2_HOOK_CDECL * BrDevAdd_original)(br_device** pdev, char* image, char* args);
br_error C2_HOOK_CDECL BrDevAdd(br_device** pdev, char* image, char* args) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrDevAdd_original(pdev, image, args);
#else
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
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528430, BrDevAdd, BrDevAdd_original)

br_error C2_HOOK_CDECL BrDevAddStatic(br_device** pdev, br_device_begin_fn* dev_begin, char* args) {
    return devAdd(pdev, dev_begin, args, NULL);
}
C2_HOOK_FUNCTION(0x005285a0, BrDevAddStatic)

br_error (C2_HOOK_CDECL * BrDevCheckAdd_original)(br_device** pdev, char* name, char* args);
br_error C2_HOOK_CDECL BrDevCheckAdd(br_device** pdev, char* name, char* args) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrDevCheckAdd_original(pdev, name, args);
#else

    if (BrDevFind(pdev, name) == 0) {
        return 0;
    }
    return BrDevAdd(pdev, name, args);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005285c0, BrDevCheckAdd, BrDevCheckAdd_original)

br_error (C2_HOOK_CDECL * BrDevAddConfig_original)(char* config);
br_error C2_HOOK_CDECL BrDevAddConfig(char* config) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrDevAddConfig_original(config);
#else
    char* end;
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
            for (arg = tmp; *arg != '\0' && *arg != ',' && *arg != ':'; arg++) {
            }
            if (*arg != '\0') {
                *arg = '\0';
            }
            dev = tmp;
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
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005286d0, BrDevAddConfig, BrDevAddConfig_original)

br_error (C2_HOOK_CDECL * BrDevRemove_original)(br_device* dev);
br_error C2_HOOK_CDECL BrDevRemove(br_device* dev) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrDevRemove_original(dev);
#else
    int i;

    for (i = 0; i < C2V(fw).ndev_slots; i++) {
        if (C2V(fw).dev_slots[i].dev == dev) {
            C2V(fw).dev_slots[i].dev->dispatch->_free((br_object*)C2V(fw).dev_slots[i].dev);
            if (C2V(fw).dev_slots[i].image != NULL) {
                BrImageDereference(C2V(fw).dev_slots[i].image);
            }
            C2V(fw).dev_slots[i].dev = NULL;
            C2V(fw).dev_slots[i].image = NULL;
        }
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528800, BrDevRemove, BrDevRemove_original)

br_error (C2_HOOK_CDECL * BrDevFind_original)(br_device** pdev, char* pattern);
br_error C2_HOOK_CDECL BrDevFind(br_device** pdev, char* pattern) {
#if 0//defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    br_error res = BrDevFind_original(pdev, pattern);
    C2_HOOK_DEBUGF("pattern=%s res=%d", pattern, res);
    C2_HOOK_FINISH();
    return res;
#else
    int i;

    AddRequestedDrivers();

    for (i = 0; i < C2V(fw).ndev_slots; i++) {
        if (C2V(fw).dev_slots[i].dev != NULL) {
            if (BrNamePatternMatch(pattern, C2V(fw).dev_slots[i].dev->dispatch->_identifier((br_object*)C2V(fw).dev_slots[i].dev))) {
                if (pdev != NULL) {
                    *pdev = C2V(fw).dev_slots[i].dev;
                }
                return 0;
            }
        }
    }
    return 0x1002;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528860, BrDevFind, BrDevFind_original)

br_error (C2_HOOK_CDECL * BrDevFindMany_original)(br_device** devices, br_int_32* ndevices, br_int_32 max_devices, char* pattern);
br_error C2_HOOK_CDECL BrDevFindMany(br_device** devices, br_int_32* ndevices, br_int_32 max_devices, char* pattern) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrDevFindMany_original(devices, ndevices, max_devices, pattern);
#else
    int i;
    int c;

    AddRequestedDrivers();

    c = 0;
    for (i = 0; i < C2V(fw).ndev_slots; i++) {
        if (C2V(fw).dev_slots[i].dev != NULL) {
            if (BrNamePatternMatch(pattern, C2V(fw).dev_slots[i].dev->dispatch->_identifier((br_object*)C2V(fw).dev_slots[i].dev))) {
                if (c >= max_devices) {
                    return 0x1004;
                }
                /* Order of operations differs from OG */
                devices[c] = C2V(fw).dev_slots[i].dev;
                c++;
            }
        }
    }
    if (ndevices != NULL) {
        *ndevices = c;
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528930, BrDevFindMany, BrDevFindMany_original)

br_error (C2_HOOK_CDECL * BrDevCount_original)(br_int_32* ndevices, char* pattern);
br_error C2_HOOK_CDECL BrDevCount(br_int_32* ndevices, char* pattern) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrDevCount_original(ndevices, pattern);
#else
    int i;
    int c;

    AddRequestedDrivers();

    c = 0;
    for (i = 0; i < C2V(fw).ndev_slots; i++) {
        if (C2V(fw).dev_slots[i].dev != NULL) {
            if (BrNamePatternMatch(pattern, C2V(fw).dev_slots[i].dev->dispatch->_identifier((br_object*)C2V(fw).dev_slots[i].dev))) {
                c++;
            }
        }
    }
    if (ndevices != NULL) {
        *ndevices = c;
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528a10, BrDevCount, BrDevCount_original)

br_error (C2_HOOK_CDECL * BrDevContainedFind_original)(br_object** ph, br_token type, char* pattern, br_token_value* tv);
br_error C2_HOOK_CDECL BrDevContainedFind(br_object** ph, br_token type, char* pattern, br_token_value* tv) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrDevContainedFind_original(ph, type, pattern, tv);
#else
    int i;

    AddRequestedDrivers();

    for (i = 0; i < C2V(fw).ndev_slots; i++) {
        if (C2V(fw).dev_slots[i].dev != NULL) {
            if (C2V(fw).dev_slots[i].dev->dispatch->_find((br_object_container*)C2V(fw).dev_slots[i].dev, ph, type, pattern, tv) == 0) {
                return 0;
            }
        }
    }
    return 0x1002;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528ac0, BrDevContainedFind, BrDevContainedFind_original)

br_error (C2_HOOK_CDECL * BrDevContainedFindMany_original)(br_object** objects, br_int_32 max_objects, br_int_32* pnum_objects, br_token type, char* pattern, br_token_value* tv);
br_error C2_HOOK_CDECL BrDevContainedFindMany(br_object** objects, br_int_32 max_objects, br_int_32* pnum_objects, br_token type, char* pattern, br_token_value* tv) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrDevContainedFindMany_original(objects, max_objects, pnum_objects, type, pattern, tv);
#else
    int i;
    br_int_32 n;
    br_int_32 total;
    br_error r;

    AddRequestedDrivers();

    total = 0;
    for (i = 0; i < C2V(fw).ndev_slots; i++) {
        if (C2V(fw).dev_slots[i].dev != NULL) {
            r = C2V(fw).dev_slots[i].dev->dispatch->_findMany((br_object_container*)C2V(fw).dev_slots[i].dev, objects, max_objects, &n, type, pattern, tv);
            if (r != 0) {
                return r;
            }
            max_objects -= n;
            total += n;
            objects += n;
        }
    }
    if (pnum_objects != NULL) {
        *pnum_objects = total;
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528b90, BrDevContainedFindMany, BrDevContainedFindMany_original)

br_error (C2_HOOK_CDECL * BrDevContainedCount_original)(br_int_32* pcount, br_token type, char* pattern, br_token_value* tv);
br_error C2_HOOK_CDECL BrDevContainedCount(br_int_32* pcount, br_token type, char* pattern, br_token_value* tv) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrDevContainedCount_original(pcount, type, pattern, tv);
#else
    int i;
    br_int_32 n;
    br_int_32 total;
    br_error r;

    AddRequestedDrivers();

    total = 0;
    for (i = 0; i < C2V(fw).ndev_slots; i++) {
        if (C2V(fw).dev_slots[i].dev != NULL) {
            r = C2V(fw).dev_slots[i].dev->dispatch->_count((br_object_container*)C2V(fw).dev_slots[i].dev, &n, type, pattern, tv);
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
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00528c80, BrDevContainedCount, BrDevContainedCount_original)
