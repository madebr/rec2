#include "intrface.h"

#include "errors.h"
#include "globvars.h"
#include "loading.h"
#include "utility.h"

#include <brender/brender.h>
#include "rec2_macros.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_item_spec, gDefaultLastInterfaceItem, 0x00604730, FIXME);

int (C2_HOOK_FASTCALL * DoInterfaceScreen_original)(const tInterface_spec* pSpec, int pOptions, int pCurrent_choice);
int C2_HOOK_FASTCALL DoInterfaceScreen(const tInterface_spec* pSpec, int pOptions, int pCurrent_choice) {

    // Relict from Carmageddon 1, unused here
    C2_HOOK_ASSUME_UNUSED();
#if defined(C2_HOOKS_ENABLED)
    return DoInterfaceScreen_original(pSpec, pOptions, pCurrent_choice);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004846e0, DoInterfaceScreen, DoInterfaceScreen_original)

void C2_HOOK_FASTCALL ReadInterfaceTxt(tFrontend_spec* pFrontend) {
    tPath_name path;
    char buffer[256];
    char* str;
    FILE* f;
    int count;
    int version;
    int itemCount;
    int i;
    int j;

    C2_HOOK_BUG_ON(sizeof(tFrontend_spec) != 0xb8c8);
    C2_HOOK_BUG_ON(sizeof(tFrontend_item_spec) != 0x158);
    C2_HOOK_BUG_ON(sizeof(tFrontend_scroller_spec) != 0x34);
    C2_HOOK_BUG_ON(sizeof(tFrontend_radio_spec) != 0x2c);
    C2_HOOK_BUG_ON(offsetof(tFrontend_spec, items) != 0x130);
    C2_HOOK_BUG_ON(offsetof(tFrontend_spec, scrollers) != 0x8794);
    C2_HOOK_BUG_ON(offsetof(tFrontend_spec, radios) != 0x9be8);
    C2_HOOK_BUG_ON(offsetof(tFrontend_spec, levels) != 0xaf24);

    PathCat(path, C2V(gApplication_path), "INTRFACE");
    PathCat(path, path, pFrontend->name);
    c2_strcat(path, ".TXT");

    f = DRfopen(path, "rt");
    if (f == NULL) {
        // Changed 0xb2 (`Mysterious "%" in %`) -> 0x6b (`Can't open %`)
        FatalError(kFatalError_CantOpen_S, path);
        return;
    }
    pFrontend->loaded = 1;
    GetALineAndDontArgue(f, buffer);
    str = c2_strtok(buffer, "\t ,/");
    if (DRStricmp(str, "VERSION") != 0) {
        FatalError(kFatalError_FileMustStartWith_SS, pFrontend->name, "VERSION");
    }
    str = c2_strtok(NULL, "\t ,/");
    count = c2_sscanf(str, "%d", &version);
    if (count == 0 || version != 3) {
        FatalError(kFatalError_CantCopeWithVersionFor_SS, str, pFrontend->name);
    }
    GetAString(f, pFrontend->name);
    itemCount = GetAnInt(f);
    if (itemCount != pFrontend->count_items) {
        BrFailure("Error - Menu item number mismatch ");
    }
    pFrontend->count_groups = GetAnInt(f);
    GetAString(f, pFrontend->backdrop_name);
    for (i = 0; i < pFrontend->count_items; i++) {
        GetAString(f, pFrontend->items[i].text);
        if (DRStricmp(pFrontend->items[i].text, "XXX") == 0) {
            pFrontend->items[i].text[0] = '\0';
        }
        str = pFrontend->items[i].text;
        while (*str != '\0') {
            if (*str == '_') {
                *str = ' ';
            }
            str++;
        }
        GetPairOfInts(f, &pFrontend->items[i].x, &pFrontend->items[i].y);
        pFrontend->items[i].x2 = pFrontend->items[i].x;
        pFrontend->items[i].y2 = pFrontend->items[i].y;
        GetPairOfInts(f, &pFrontend->items[i].width, &pFrontend->items[i].height);
        GetPairOfInts(f, &pFrontend->items[i].unlitFont, &pFrontend->items[i].highFont);
        pFrontend->items[i].map_index = GetAnInt(f);
        pFrontend->items[i].greyedBackdrop = GetAnInt(f);
        pFrontend->items[i].selectable = GetAnInt(f);
        pFrontend->items[i].group = GetAnInt(f);
        pFrontend->items[i].idLevelBar = GetAnInt(f);
        pFrontend->items[i].wrapText = GetAnInt(f);
        pFrontend->items[i].glowDisabled = GetAnInt(f);
    }
    // Copy item 99 (=last index)
    // FIXME: stringid = 0x401 (==> find out function of this item)
    c2_memcpy(&pFrontend->items[REC2_ASIZE(pFrontend->items) - 1], &C2V(gDefaultLastInterfaceItem), sizeof(tFrontend_item_spec));

    pFrontend->count_scrollers = GetAnInt(f);
    for (i = 0; i < pFrontend->count_scrollers; i++) {
        GetPairOfInts(f, &pFrontend->scrollers[i].id, &pFrontend->scrollers[i].count);
        GetPairOfInts(f, &pFrontend->scrollers[i].nbDisplayedAtOnce, &pFrontend->scrollers[i].indexTopItem);
        GetPairOfInts(f, &pFrontend->scrollers[i].indexFirstScrollableItem, &pFrontend->scrollers[i].indexLastScrollableItem);
        pFrontend->scrollers[i].greyedOutBackdrop = GetAnInt(f);
        GetPairOfInts(f, &pFrontend->scrollers[i].x_greyArea, &pFrontend->scrollers[i].y_greyArea);
        GetPairOfInts(f, &pFrontend->scrollers[i].width_greyArea, &pFrontend->scrollers[i].height_greyArea);
        count = pFrontend->scrollers[i].nbDisplayedAtOnce;
        for (j = pFrontend->scrollers[i].indexTopItem; j <= pFrontend->scrollers[i].indexLastScrollableItem; j++) {
            pFrontend->items[j].visible = count > 0;
            count --;
        }
    }

    pFrontend->count_radios = GetAnInt(f);
    for (i = 0; i < pFrontend->count_radios; i++) {
        GetPairOfInts(f, &pFrontend->radios[i].id, &pFrontend->radios[i].count);
        GetPairOfInts(f, &pFrontend->radios[i].indexFirstItem, &pFrontend->radios[i].indexLastItem);
        GetPairOfInts(f, &pFrontend->radios[i].indexSelected, &pFrontend->radios[i].greyboxRequested);
        GetPairOfInts(f, &pFrontend->radios[i].x_greybox, &pFrontend->radios[i].y_greybox);
        GetPairOfInts(f, &pFrontend->radios[i].width_greybox, &pFrontend->radios[i].height_greybox);
    }

    pFrontend->count_levels = GetAnInt(f);
    for (i = 0; i < pFrontend->count_levels; i++) {
        GetPairOfInts(f, &pFrontend->levels[i].unknown0, &pFrontend->levels[i].unknown1);
        GetPairOfInts(f, &pFrontend->levels[i].unknown2, &pFrontend->levels[i].unknown3);
    }

    DRfclose(f);
}
C2_HOOK_FUNCTION(0x00466760, ReadInterfaceTxt)
