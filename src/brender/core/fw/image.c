#include "image.h"

#include "loader.h"
#include "brlists.h"
#include "fwsetup.h"
#include "pattern.h"
#include "resource.h"
#include "scratch.h"

#include "core/host/himage.h"
#include "core/std/brstdlib.h"

#include "br_platform.h"

// FUNCTION: CARMA2_HW 0x0052fed0
br_boolean C2_HOOK_CDECL BrImageAdd(br_image* img) {
    BrAddHead(&fw.images, &img->node);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0052fef0
br_boolean C2_HOOK_CDECL BrImageRemove(br_image* img) {
    BrRemove(&img->node);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0052ff10
br_image* C2_HOOK_CDECL BrImageFind(const char* pattern) {
    char* c;
    br_image* img;

    c = BrStrRChr(pattern, '.');
    if (c != NULL && (BrStrCmp(c, ".dll") == 0 || BrStrCmp(c, ".bdd") == 0|| BrStrCmp(c, "bed"))) {
        *c = '\0';
    }
    for (img = (br_image*)fw.images.head; img->node.next != NULL; img = (br_image*)img->node.next) {
        if (BrNamePatternMatch(pattern, img->identifier) != 0) {
            return img;
        }
    }
    return NULL;
}

br_image* C2_HOOK_STDCALL imageLoadHost(const char* name) {
    br_image* img;
    void* host_image;

    host_image = HostImageLoad(name);
    if (host_image != NULL) {
        img = BrResAllocate(NULL, sizeof(br_image), BR_MEMORY_IMAGE);
        img->identifier = BrResStrDup(img, name);
        img->type = 3;
        img->type_pointer = host_image;
    } else {
        img = NULL;
    }
    return img;
}

// FUNCTION: CARMA2_HW 0x0052ffa0
br_image* C2_HOOK_CDECL BrImageReference(const char* name) {
    const char* suffix;
    char* suffix2;
    char* scratch;
    br_image* img;

    scratch = BrScratchString();
    img = BrImageFind(name);
    if (img != NULL) {
        img->ref_count++;
        return img;
    }
    for (suffix = name; *suffix != '\0' && *suffix != '.'; suffix++) {
    }
    if (*suffix == '\0') {
        if (img == NULL) {
            BrStrCpy(scratch, name);
            BrStrCat(scratch, ".BDD");
            img = ImageLoad(scratch);
        }
        if (img == NULL) {
            img = imageLoadHost(name);
        }
        if (img == NULL) {
            BrStrCpy(scratch, name);
            BrStrCat(scratch, ".DLL");
            img = ImageLoad(scratch);
        }
    } else if (BrStrCmp(suffix, ".bdd") == 0 || BrStrCmp(suffix, ".bed") == 0) {
        if (img == NULL) {
            img = ImageLoad(name);
        }
        if (img == NULL) {
            img = imageLoadHost(name);
        }
    } else {
        if (img == NULL) {
            img = imageLoadHost(name);
        }
        if (img == NULL) {
            img = ImageLoad(name);
        }
    }

    if (img != NULL) {
        for (suffix2 = img->identifier; *suffix != '\0' && *suffix != '.'; suffix++) {
        }
        if (suffix2 != NULL) {
            *suffix2 = '\0';
        }
        BrResAdd(fw.res, img);
        BrAddHead(&fw.images, &img->node);
    }
    return img;
}

// FUNCTION: CARMA2_HW 0x00530260
void* C2_HOOK_STDCALL imageLookupName(br_image* img, const char* name, br_uint_32 hint) {
    int c;
    int limit;
    int base;

    if (hint < img->n_names && BrStrCmp(name, img->names[hint]) == 0) {
        return img->functions[img->name_ordinals[hint]];
    }
    base = 0;
    limit = img->n_names;
    while (1) {
        if (limit == 0) {
            return NULL;
        }
        c = BrStrCmp(name, img->names[base + limit / 2]);
        if (c == 0) {
            return img->functions[img->name_ordinals[base + limit / 2]];
        } else if (c < 0) {
            limit = limit / 2;
        } else {
            base += limit / 2 + 1;
            limit = limit - (limit / 2 + 1);
        }
    }
}

// FUNCTION: CARMA2_HW 0x005301e0
void* C2_HOOK_CDECL BrImageLookupName(br_image* img, const char* name, br_uint_32 hint) {
    char* scratch;
    void* p;

    scratch = BrScratchString();
    if (img->type == 3) {
        return HostImageLookupName(img->type_pointer, name, hint);
    }
    p = imageLookupName(img, name, hint);
    if (p != NULL) {
        return p;
    }
    if (*name == '_') {
        p = imageLookupName(img, &name[1], hint);
        if (p != NULL) {
            return p;
        }
    }
    scratch[0] = '_';
    BrStrCpy(&scratch[1], name);
    return imageLookupName(img, scratch, hint);
}

// FUNCTION: CARMA2_HW 0x00530320
void* C2_HOOK_CDECL BrImageLookupOrdinal(br_image* img, br_uint_32 ordinal) {

    if (img->type == 3) {
        return HostImageLookupOrdinal(img->type_pointer, ordinal);
    }
    if (ordinal - img->ordinal_base > img->n_functions) {
        return NULL;
    }
    return img->functions[ordinal - img->ordinal_base];
}

// FUNCTION: CARMA2_HW 0x00530360
void C2_HOOK_CDECL BrImageDereference(br_image* image) {
    int i;
    image->ref_count--;

    if (image->ref_count <= 0) {
        switch (image->type) {
        case 2:
            break;
        case 3:
            HostImageUnload(image->type_pointer);
            // fall through
        default:
#ifdef BRENDER_FIX_BUGS
            /* Added by rec2: fixes DEP */
            for (i = 0; i < image->n_sections; i++) {
                PDMapImageSection(image->sections[i].base, image->sections[i].mem_size, kMemory_section_read | kMemory_section_write);
            }
#endif
            BrRemove(&image->node);
            BrResFree(image);
        }

    }
}

// FUNCTION: CARMA2_HW 0x005303a0
void C2_HOOK_CDECL BrImageFree(br_image* image) {
    int i;

    for (i = 0; i < image->n_imports; i++) {
        BrImageDereference(image->imports[i]);
    }
}

// FUNCTION: CARMA2_HW 0x00530400
void C2_HOOK_CDECL _BrImageFree(void* res, br_uint_8 res_class, br_size_t size) {

    BrImageFree(res);
}
