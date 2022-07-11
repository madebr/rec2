#include "def_otab.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(void*, _default_table, 256, 0x006ad528);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_order_table, _BrDefaultOrderTable, 0x00665458, {
    C2V(_default_table),
    BR_ASIZE(C2V(_default_table)),
    NULL,
    1.f,
    10.f,
    0.f,
    1.f/9.f,
    0,
    1,
    0,
});
