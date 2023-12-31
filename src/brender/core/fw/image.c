#include "image.h"

#include "loader.h"
#include "brlists.h"
#include "fwsetup.h"
#include "pattern.h"
#include "resource.h"
#include "scratch.h"

#include "core/host/himage.h"
#include "core/std/brstdlib.h"

br_boolean (C2_HOOK_CDECL * BrImageAdd_original)(br_image* img);
br_boolean C2_HOOK_CDECL BrImageAdd(br_image* img) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrImageAdd_original(img);
#else
    BrAddHead(&C2V(fw).images, &img->node);
    return 1;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052fed0, BrImageAdd, BrImageAdd_original)

br_boolean (C2_HOOK_CDECL * BrImageRemove_original)(br_image* img);
br_boolean C2_HOOK_CDECL BrImageRemove(br_image* img) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrImageRemove_original(img);
#else
    BrRemove(&img->node);
    return 1;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052fef0, BrImageRemove, BrImageRemove_original)

br_image* (C2_HOOK_CDECL * BrImageFind_original)(char* pattern);
br_image* C2_HOOK_CDECL BrImageFind(char* pattern) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrImageFind_original(pattern);
#else
    char* c;
    br_image* img;

    c = BrStrRChr(pattern, '.');
    if (c != NULL && (BrStrCmp(c, ".dll") == 0 || BrStrCmp(c, ".bdd") == 0|| BrStrCmp(c, "bed"))) {
        *c = '\0';
    }
    for (img = (br_image*)C2V(fw).images.head; img->node.next != NULL; img = (br_image*)img->node.next) {
        if (BrNamePatternMatch(pattern, img->identifier) != 0) {
            return img;
        }
    }
    return NULL;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052ff10, BrImageFind, BrImageFind_original)

br_image* C2_HOOK_STDCALL imageLoadHost(char* name) {
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

br_image* (C2_HOOK_CDECL * BrImageReference_original)(char* name);
br_image* C2_HOOK_CDECL BrImageReference(char* name) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrImageReference_original(name);
#else
    char* suffix;
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
        for (suffix = img->identifier; *suffix != '\0' && *suffix != '.'; suffix++) {
        }
        if (suffix != NULL) {
            *suffix = '\0';
        }
        BrResAdd(C2V(fw).res, img);
        BrAddHead(&C2V(fw).images, &img->node);
    }
    return img;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052ffa0, BrImageReference, BrImageReference_original)

void* (C2_HOOK_STDCALL * imageLookupName_original)(br_image* img, char* name, br_uint_32 hint);
void* C2_HOOK_STDCALL imageLookupName(br_image* img, char* name, br_uint_32 hint) {
#if 0//defined(C2_HOOKS_ENABLED)
    return imageLookupName_original(img, name, hint);
#else
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
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00530260, imageLookupName, imageLookupName_original)

void* (C2_HOOK_CDECL * BrImageLookupName_original)(br_image* img, char* name, br_uint_32 hint);
void* C2_HOOK_CDECL BrImageLookupName(br_image* img, char* name, br_uint_32 hint) {
#if defined(C2_HOOKS_ENABLED)
    return BrImageLookupName_original(img, name, hint);
#else
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
    *scratch = '_';
    BrStrCpy(&scratch[1], name);
    return imageLookupName(img, scratch, hint);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005301e0, BrImageLookupName, BrImageLookupName_original)

void* (C2_HOOK_CDECL * BrImageLookupOrdinal_original)(br_image* img, br_uint_32 ordinal);
void* C2_HOOK_CDECL BrImageLookupOrdinal(br_image* img, br_uint_32 ordinal) {
#if 0//defined(C2_HOOKS_ENABLED)
    return BrImageLookupOrdinal_original(img, ordinal);
#else
    if (img->type == 3) {
        return HostImageLookupOrdinal(img->type_pointer, ordinal);
    }
    if (ordinal - img->ordinal_base > img->n_functions) {
        return NULL;
    }
    return img->functions[ordinal - img->ordinal_base];
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00530320, BrImageLookupOrdinal, BrImageLookupOrdinal_original)

void (C2_HOOK_CDECL * BrImageDereference_original)(br_image* image);
void C2_HOOK_CDECL BrImageDereference(br_image* image) {
#if 0//defined(C2_HOOKS_ENABLED)
    BrImageDereference_original(image);
#else
    image->ref_count--;

    if (image->ref_count <= 0) {
        switch (image->type) {
        case 2:
            break;
        case 3:
            HostImageUnload(image->type_pointer);
            // fall through
        default:
            BrRemove(&image->node);
            BrResFree(image);
        }

    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00530360, BrImageDereference, BrImageDereference_original)

void (C2_HOOK_CDECL * BrImageFree_original)(br_image* image);
void C2_HOOK_CDECL BrImageFree(br_image* image) {
#if 0//defined(C2_HOOKS_ENABLED)
    BrImageFree_original(image);
#else
    int i;

    for (i = 0; i < image->n_imports; i++) {
        BrImageDereference(image->imports[i]);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005303a0, BrImageFree, BrImageFree_original)

void (C2_HOOK_CDECL * _BrImageFree_original)(void* res, br_uint_8 res_class, br_size_t size);
void C2_HOOK_CDECL _BrImageFree(void* res, br_uint_8 res_class, br_size_t size) {
#if 0//defined(C2_HOOKS_ENABLED)
    _BrImageFree_original(res, res_class, size);
#else
    BrImageFree(res);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00530400, _BrImageFree, _BrImageFree_original)
