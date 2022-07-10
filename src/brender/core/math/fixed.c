#include "fixed.h"

#include <assert.h>

#define fixed_cos_table ((unsigned short*)&C2V(fixed_sin_table)[64])

static C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(unsigned short, fixed_sin_table, 322, 0x0066cc2c, {
    // start sin table
    0x0000, 0x0324, 0x0647, 0x096a, 0x0c8b, 0x0fab, 0x12c7, 0x15e1,
    0x18f8, 0x1c0b, 0x1f19, 0x2223, 0x2527, 0x2826, 0x2b1e, 0x2e10,
    0x30fb, 0x33de, 0x36b9, 0x398c, 0x3c56, 0x3f16, 0x41cd, 0x447a,
    0x471c, 0x49b3, 0x4c3f, 0x4ebf, 0x5133, 0x539a, 0x55f4, 0x5842,
    0x5a81, 0x5cb3, 0x5ed6, 0x60eb, 0x62f1, 0x64e7, 0x66ce, 0x68a5,
    0x6a6c, 0x6c23, 0x6dc9, 0x6f5e, 0x70e1, 0x7254, 0x73b5, 0x7503,
    0x7640, 0x776b, 0x7883, 0x7989, 0x7a7c, 0x7b5c, 0x7c29, 0x7ce2,
    0x7d89, 0x7e1c, 0x7e9c, 0x7f08, 0x7f61, 0x7fa6, 0x7fd7, 0x7ff5,
    // start cos table
    0x7fff, 0x7ff5, 0x7fd7, 0x7fa6, 0x7f61, 0x7f08, 0x7e9c, 0x7e1c,
    0x7d89, 0x7ce2, 0x7c29, 0x7b5c, 0x7a7c, 0x7989, 0x7883, 0x776b,
    0x7640, 0x7503, 0x73b5, 0x7254, 0x70e1, 0x6f5e, 0x6dc9, 0x6c23,
    0x6a6c, 0x68a5, 0x66ce, 0x64e7, 0x62f1, 0x60eb, 0x5ed6, 0x5cb3,
    0x5a81, 0x5842, 0x55f4, 0x539a, 0x5133, 0x4ebf, 0x4c3f, 0x49b3,
    0x471c, 0x447a, 0x41cd, 0x3f16, 0x3c56, 0x398c, 0x36b9, 0x33de,
    0x30fb, 0x2e10, 0x2b1e, 0x2826, 0x2527, 0x2223, 0x1f19, 0x1c0b,
    0x18f8, 0x15e1, 0x12c7, 0x0fab, 0x0c8b, 0x096a, 0x0647, 0x0324,
    0x0000, 0xfcdc, 0xf9b9, 0xf696, 0xf375, 0xf055, 0xed39, 0xea1f,
    0xe708, 0xe3f5, 0xe0e7, 0xdddd, 0xdad9, 0xd7da, 0xd4e2, 0xd1f0,
    0xcf05, 0xcc22, 0xc947, 0xc674, 0xc3aa, 0xc0ea, 0xbe33, 0xbb86,
    0xb8e4, 0xb64d, 0xb3c1, 0xb141, 0xaecd, 0xac66, 0xaa0c, 0xa7be,
    0xa57f, 0xa34d, 0xa12a, 0x9f15, 0x9d0f, 0x9b19, 0x9932, 0x975b,
    0x9594, 0x93dd, 0x9237, 0x90a2, 0x8f1f, 0x8dac, 0x8c4b, 0x8afd,
    0x89c0, 0x8895, 0x877d, 0x8677, 0x8584, 0x84a4, 0x83d7, 0x831e,
    0x8277, 0x81e4, 0x8164, 0x80f8, 0x809f, 0x805a, 0x8029, 0x800b,
    0x8001, 0x800b, 0x8029, 0x805a, 0x809f, 0x80f8, 0x8164, 0x81e4,
    0x8277, 0x831e, 0x83d7, 0x84a4, 0x8584, 0x8677, 0x877d, 0x8895,
    0x89c0, 0x8afd, 0x8c4b, 0x8dac, 0x8f1f, 0x90a2, 0x9237, 0x93dd,
    0x9594, 0x975b, 0x9932, 0x9b19, 0x9d0f, 0x9f15, 0xa12a, 0xa34d,
    0xa57f, 0xa7be, 0xaa0c, 0xac66, 0xaecd, 0xb141, 0xb3c1, 0xb64d,
    0xb8e4, 0xbb86, 0xbe33, 0xc0ea, 0xc3aa, 0xc674, 0xc947, 0xcc22,
    0xcf05, 0xd1f0, 0xd4e2, 0xd7da, 0xdad9, 0xdddd, 0xe0e7, 0xe3f5,
    0xe708, 0xea1f, 0xed39, 0xf055, 0xf375, 0xf696, 0xf9b9, 0xfcdc,
    0x0000, 0x0324, 0x0647, 0x096a, 0x0c8b, 0x0fab, 0x12c7, 0x15e1,
    0x18f8, 0x1c0b, 0x1f19, 0x2223, 0x2527, 0x2826, 0x2b1e, 0x2e10,
    0x30fb, 0x33de, 0x36b9, 0x398c, 0x3c56, 0x3f16, 0x41cd, 0x447a,
    0x471c, 0x49b3, 0x4c3f, 0x4ebf, 0x5133, 0x539a, 0x55f4, 0x5842,
    0x5a81, 0x5cb3, 0x5ed6, 0x60eb, 0x62f1, 0x64e7, 0x66ce, 0x68a5,
    0x6a6c, 0x6c23, 0x6dc9, 0x6f5e, 0x70e1, 0x7254, 0x73b5, 0x7503,
    0x7640, 0x776b, 0x7883, 0x7989, 0x7a7c, 0x7b5c, 0x7c29, 0x7ce2,
    0x7d89, 0x7e1c, 0x7e9c, 0x7f08, 0x7f61, 0x7fa6, 0x7fd7, 0x7ff5,
    0x7fff, 0x7fff,
});

static C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(unsigned short, fixed_asin_table, 257, 0x0066ceb0, {
    0xc001, 0xc519, 0xc737, 0xc8d7, 0xca37, 0xcb6d, 0xcc87, 0xcd8a,
    0xce7c, 0xcf5f, 0xd037, 0xd104, 0xd1c9, 0xd286, 0xd33c, 0xd3ed,
    0xd498, 0xd53e, 0xd5df, 0xd67c, 0xd716, 0xd7ac, 0xd83f, 0xd8cf,
    0xd95c, 0xd9e7, 0xda6f, 0xdaf4, 0xdb78, 0xdbf9, 0xdc79, 0xdcf7,
    0xdd73, 0xdded, 0xde66, 0xdedd, 0xdf53, 0xdfc8, 0xe03b, 0xe0ad,
    0xe11e, 0xe18d, 0xe1fc, 0xe26a, 0xe2d6, 0xe342, 0xe3ac, 0xe416,
    0xe47f, 0xe4e7, 0xe54e, 0xe5b4, 0xe61a, 0xe67f, 0xe6e3, 0xe746,
    0xe7a9, 0xe80c, 0xe86d, 0xe8ce, 0xe92f, 0xe98f, 0xe9ee, 0xea4d,
    0xeaab, 0xeb09, 0xeb66, 0xebc3, 0xec20, 0xec7c, 0xecd7, 0xed33,
    0xed8d, 0xede8, 0xee42, 0xee9c, 0xeef5, 0xef4e, 0xefa7, 0xefff,
    0xf057, 0xf0af, 0xf106, 0xf15d, 0xf1b4, 0xf20b, 0xf261, 0xf2b8,
    0xf30d, 0xf363, 0xf3b9, 0xf40e, 0xf463, 0xf4b8, 0xf50c, 0xf561,
    0xf5b5, 0xf609, 0xf65d, 0xf6b1, 0xf704, 0xf758, 0xf7ab, 0xf7fe,
    0xf851, 0xf8a4, 0xf8f7, 0xf949, 0xf99c, 0xf9ee, 0xfa41, 0xfa93,
    0xfae5, 0xfb37, 0xfb89, 0xfbdb, 0xfc2d, 0xfc7f, 0xfcd1, 0xfd23,
    0xfd74, 0xfdc6, 0xfe17, 0xfe69, 0xfeba, 0xff0c, 0xff5e, 0xffaf,
    0x0000, 0x0051, 0x00a2, 0x00f4, 0x0146, 0x0197, 0x01e9, 0x023a,
    0x028c, 0x02dd, 0x032f, 0x0381, 0x03d3, 0x0425, 0x0477, 0x04c9,
    0x051b, 0x056d, 0x05bf, 0x0612, 0x0664, 0x06b7, 0x0709, 0x075c,
    0x07af, 0x0802, 0x0855, 0x08a8, 0x08fc, 0x094f, 0x09a3, 0x09f7,
    0x0a4b, 0x0a9f, 0x0af4, 0x0b48, 0x0b9d, 0x0bf2, 0x0c47, 0x0c9d,
    0x0cf3, 0x0d48, 0x0d9f, 0x0df5, 0x0e4c, 0x0ea3, 0x0efa, 0x0f51,
    0x0fa9, 0x1001, 0x1059, 0x10b2, 0x110b, 0x1164, 0x11be, 0x1218,
    0x1273, 0x12cd, 0x1329, 0x1384, 0x13e0, 0x143d, 0x149a, 0x14f7,
    0x1555, 0x15b3, 0x1612, 0x1671, 0x16d1, 0x1732, 0x1793, 0x17f4,
    0x1857, 0x18ba, 0x191d, 0x1981, 0x19e6, 0x1a4c, 0x1ab2, 0x1b19,
    0x1b81, 0x1bea, 0x1c54, 0x1cbe, 0x1d2a, 0x1d96, 0x1e04, 0x1e73,
    0x1ee2, 0x1f53, 0x1fc5, 0x2038, 0x20ad, 0x2123, 0x219a, 0x2213,
    0x228d, 0x2309, 0x2387, 0x2407, 0x2488, 0x250c, 0x2591, 0x2619,
    0x26a4, 0x2731, 0x27c1, 0x2854, 0x28ea, 0x2984, 0x2a21, 0x2ac2,
    0x2b68, 0x2c13, 0x2cc4, 0x2d7a, 0x2e37, 0x2efc, 0x2fc9, 0x30a1,
    0x3184, 0x3276, 0x3379, 0x3493, 0x35c9, 0x3729, 0x38c9, 0x3ae7,
    0x4000,
});

static C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(unsigned short, fixed_acos_table, 257, 0x0066d0b2, {
    0x7fff, 0x7ae7, 0x78c9, 0x7729, 0x75c9, 0x7493, 0x7379, 0x7276,
    0x7184, 0x70a1, 0x6fc9, 0x6efc, 0x6e37, 0x6d7a, 0x6cc4, 0x6c13,
    0x6b68, 0x6ac2, 0x6a21, 0x6984, 0x68ea, 0x6854, 0x67c1, 0x6731,
    0x66a4, 0x6619, 0x6591, 0x650c, 0x6488, 0x6407, 0x6387, 0x6309,
    0x628d, 0x6213, 0x619a, 0x6123, 0x60ad, 0x6038, 0x5fc5, 0x5f53,
    0x5ee2, 0x5e73, 0x5e04, 0x5d96, 0x5d2a, 0x5cbe, 0x5c54, 0x5bea,
    0x5b81, 0x5b19, 0x5ab2, 0x5a4c, 0x59e6, 0x5981, 0x591d, 0x58ba,
    0x5857, 0x57f4, 0x5793, 0x5732, 0x56d1, 0x5671, 0x5612, 0x55b3,
    0x5555, 0x54f7, 0x549a, 0x543d, 0x53e0, 0x5384, 0x5329, 0x52cd,
    0x5273, 0x5218, 0x51be, 0x5164, 0x510b, 0x50b2, 0x5059, 0x5001,
    0x4fa9, 0x4f51, 0x4efa, 0x4ea3, 0x4e4c, 0x4df5, 0x4d9f, 0x4d48,
    0x4cf3, 0x4c9d, 0x4c47, 0x4bf2, 0x4b9d, 0x4b48, 0x4af4, 0x4a9f,
    0x4a4b, 0x49f7, 0x49a3, 0x494f, 0x48fc, 0x48a8, 0x4855, 0x4802,
    0x47af, 0x475c, 0x4709, 0x46b7, 0x4664, 0x4612, 0x45bf, 0x456d,
    0x451b, 0x44c9, 0x4477, 0x4425, 0x43d3, 0x4381, 0x432f, 0x42dd,
    0x428c, 0x423a, 0x41e9, 0x4197, 0x4146, 0x40f4, 0x40a2, 0x4051,
    0x3fff, 0x3fae, 0x3f5d, 0x3f0b, 0x3eb9, 0x3e68, 0x3e16, 0x3dc5,
    0x3d73, 0x3d22, 0x3cd0, 0x3c7e, 0x3c2c, 0x3bda, 0x3b88, 0x3b36,
    0x3ae4, 0x3a92, 0x3a40, 0x39ed, 0x399b, 0x3948, 0x38f6, 0x38a3,
    0x3850, 0x37fd, 0x37aa, 0x3757, 0x3703, 0x36b0, 0x365c, 0x3608,
    0x35b4, 0x3560, 0x350b, 0x34b7, 0x3462, 0x340d, 0x33b8, 0x3362,
    0x330c, 0x32b7, 0x3260, 0x320a, 0x31b3, 0x315c, 0x3105, 0x30ae,
    0x3056, 0x2ffe, 0x2fa6, 0x2f4d, 0x2ef4, 0x2e9b, 0x2e41, 0x2de7,
    0x2d8c, 0x2d32, 0x2cd6, 0x2c7b, 0x2c1f, 0x2bc2, 0x2b65, 0x2b08,
    0x2aaa, 0x2a4c, 0x29ed, 0x298e, 0x292e, 0x28cd, 0x286c, 0x280b,
    0x27a8, 0x2745, 0x26e2, 0x267e, 0x2619, 0x25b3, 0x254d, 0x24e6,
    0x247e, 0x2415, 0x23ab, 0x2341, 0x22d5, 0x2269, 0x21fb, 0x218c,
    0x211d, 0x20ac, 0x203a, 0x1fc7, 0x1f52, 0x1edc, 0x1e65, 0x1dec,
    0x1d72, 0x1cf6, 0x1c78, 0x1bf8, 0x1b77, 0x1af3, 0x1a6e, 0x19e6,
    0x195b, 0x18ce, 0x183e, 0x17ab, 0x1715, 0x167b, 0x15de, 0x153d,
    0x1497, 0x13ec, 0x133b, 0x1285, 0x11c8, 0x1103, 0x1036, 0x0f5e,
    0x0e7b, 0x0d89, 0x0c86, 0x0b6c, 0x0a36, 0x08d6, 0x0736, 0x0518,
    0x0000,
});

static C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(unsigned short, fixed_atan_table, 257, 0x0066d2b4, {
    0x0000, 0x0028, 0x0051, 0x007a, 0x00a2, 0x00cb, 0x00f4, 0x011d,
    0x0145, 0x016e, 0x0197, 0x01bf, 0x01e8, 0x0211, 0x0239, 0x0262,
    0x028b, 0x02b3, 0x02dc, 0x0304, 0x032d, 0x0355, 0x037e, 0x03a6,
    0x03ce, 0x03f7, 0x041f, 0x0448, 0x0470, 0x0498, 0x04c0, 0x04e8,
    0x0511, 0x0539, 0x0561, 0x0589, 0x05b1, 0x05d9, 0x0601, 0x0628,
    0x0650, 0x0678, 0x06a0, 0x06c7, 0x06ef, 0x0716, 0x073e, 0x0765,
    0x078d, 0x07b4, 0x07db, 0x0803, 0x082a, 0x0851, 0x0878, 0x089f,
    0x08c6, 0x08ed, 0x0913, 0x093a, 0x0961, 0x0987, 0x09ae, 0x09d4,
    0x09fb, 0x0a21, 0x0a47, 0x0a6d, 0x0a94, 0x0aba, 0x0ae0, 0x0b05,
    0x0b2b, 0x0b51, 0x0b77, 0x0b9c, 0x0bc2, 0x0be7, 0x0c0c, 0x0c32,
    0x0c57, 0x0c7c, 0x0ca1, 0x0cc6, 0x0ceb, 0x0d0f, 0x0d34, 0x0d58,
    0x0d7d, 0x0da1, 0x0dc6, 0x0dea, 0x0e0e, 0x0e32, 0x0e56, 0x0e7a,
    0x0e9e, 0x0ec1, 0x0ee5, 0x0f08, 0x0f2c, 0x0f4f, 0x0f72, 0x0f95,
    0x0fb8, 0x0fdb, 0x0ffe, 0x1021, 0x1044, 0x1066, 0x1089, 0x10ab,
    0x10cd, 0x10ef, 0x1111, 0x1133, 0x1155, 0x1177, 0x1199, 0x11ba,
    0x11dc, 0x11fd, 0x121e, 0x123f, 0x1260, 0x1281, 0x12a2, 0x12c3,
    0x12e4, 0x1304, 0x1325, 0x1345, 0x1365, 0x1385, 0x13a5, 0x13c5,
    0x13e5, 0x1405, 0x1424, 0x1444, 0x1463, 0x1483, 0x14a2, 0x14c1,
    0x14e0, 0x14ff, 0x151e, 0x153c, 0x155b, 0x1579, 0x1598, 0x15b6,
    0x15d4, 0x15f2, 0x1610, 0x162e, 0x164c, 0x166a, 0x1687, 0x16a5,
    0x16c2, 0x16df, 0x16fc, 0x1719, 0x1736, 0x1753, 0x1770, 0x178c,
    0x17a9, 0x17c5, 0x17e2, 0x17fe, 0x181a, 0x1836, 0x1852, 0x186e,
    0x188a, 0x18a5, 0x18c1, 0x18dc, 0x18f7, 0x1913, 0x192e, 0x1949,
    0x1964, 0x197f, 0x1999, 0x19b4, 0x19ce, 0x19e9, 0x1a03, 0x1a1d,
    0x1a37, 0x1a51, 0x1a6b, 0x1a85, 0x1a9f, 0x1ab9, 0x1ad2, 0x1aec,
    0x1b05, 0x1b1e, 0x1b37, 0x1b50, 0x1b69, 0x1b82, 0x1b9b, 0x1bb4,
    0x1bcc, 0x1be5, 0x1bfd, 0x1c16, 0x1c2e, 0x1c46, 0x1c5e, 0x1c76,
    0x1c8e, 0x1ca5, 0x1cbd, 0x1cd5, 0x1cec, 0x1d04, 0x1d1b, 0x1d32,
    0x1d49, 0x1d60, 0x1d77, 0x1d8e, 0x1da5, 0x1dbb, 0x1dd2, 0x1de9,
    0x1dff, 0x1e15, 0x1e2c, 0x1e42, 0x1e58, 0x1e6e, 0x1e84, 0x1e99,
    0x1eaf, 0x1ec5, 0x1eda, 0x1ef0, 0x1f05, 0x1f1b, 0x1f30, 0x1f45,
    0x1f5a, 0x1f6f, 0x1f84, 0x1f99, 0x1fad, 0x1fc2, 0x1fd7, 0x1feb,
    0x2000,
});

static C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(uint16_t, fast_sqrt_table, 128, 0x0066d4b6, {
    0x8000, 0x80ff, 0x81fc, 0x82f7, 0x83f0, 0x84e7, 0x85dd, 0x86d1,
    0x87c3, 0x88b4, 0x89a3, 0x8a90, 0x8b7c, 0x8c66, 0x8d4e, 0x8e36,
    0x8f1b, 0x9000, 0x90e2, 0x91c4, 0x92a4, 0x9383, 0x9460, 0x953c,
    0x9617, 0x96f1, 0x97ca, 0x98a1, 0x9977, 0x9a4c, 0x9b20, 0x9bf2,
    0x9cc4, 0x9d94, 0x9e64, 0x9f32, 0xa000, 0xa0cc, 0xa197, 0xa261,
    0xa32b, 0xa3f3, 0xa4ba, 0xa581, 0xa646, 0xa70b, 0xa7cf, 0xa892,
    0xa953, 0xaa15, 0xaad5, 0xab94, 0xac53, 0xad11, 0xadcd, 0xae8a,
    0xaf45, 0xb000, 0xb0b9, 0xb172, 0xb22b, 0xb2e2, 0xb399, 0xb44f,
    0xb504, 0xb5b9, 0xb66d, 0xb720, 0xb7d3, 0xb885, 0xb936, 0xb9e7,
    0xba97, 0xbb46, 0xbbf5, 0xbca3, 0xbd50, 0xbdfd, 0xbea9, 0xbf55,
    0xc000, 0xc0aa, 0xc154, 0xc1fd, 0xc2a5, 0xc34e, 0xc3f5, 0xc49c,
    0xc542, 0xc5e8, 0xc68e, 0xc732, 0xc7d7, 0xc87a, 0xc91d, 0xc9c0,
    0xca62, 0xcb04, 0xcba5, 0xcc46, 0xcce6, 0xcd86, 0xce25, 0xcec3,
    0xcf62, 0xd000, 0xd09d, 0xd13a, 0xd1d6, 0xd272, 0xd30d, 0xd3a8,
    0xd443, 0xd4dd, 0xd577, 0xd610, 0xd6a9, 0xd742, 0xd7da, 0xd871,
    0xd908, 0xd99f, 0xda35, 0xdacb, 0xdb61, 0xdbf6, 0xdc8b, 0xdd1f,
    0xddb3, 0xde47, 0xdeda, 0xdf6d, 0xe000, 0xe092, 0xe123, 0xe1b5,
    0xe246, 0xe2d6, 0xe367, 0xe3f7, 0xe486, 0xe515, 0xe5a4, 0xe633,
    0xe6c1, 0xe74f, 0xe7dc, 0xe869, 0xe8f6, 0xe983, 0xea0f, 0xea9b,
    0xeb26, 0xebb1, 0xec3c, 0xecc7, 0xed51, 0xeddb, 0xee65, 0xeeee,
    0xef77, 0xf000, 0xf088, 0xf110, 0xf198, 0xf21f, 0xf2a6, 0xf32d,
    0xf3b4, 0xf43a, 0xf4c0, 0xf546, 0xf5cb, 0xf651, 0xf6d6, 0xf75a,
    0xf7de, 0xf863, 0xf8e6, 0xf96a, 0xf9ed, 0xfa70, 0xfaf3, 0xfb75,
    0xfbf7, 0xfc79, 0xfcfb, 0xfd7c, 0xfdfd, 0xfe7e, 0xfeff, 0xff7f,
});

static C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(uint16_t, fast_rsqrt_table, 128, 0x0066d636, {
    0xffff, 0xfe05, 0xfc17, 0xfa33, 0xf85b, 0xf68c, 0xf4c8, 0xf30d,
    0xf15b, 0xefb3, 0xee13, 0xec7b, 0xeaeb, 0xe964, 0xe7e3, 0xe66b,
    0xe4f9, 0xe38e, 0xe229, 0xe0cc, 0xdf74, 0xde23, 0xdcd7, 0xdb91,
    0xda51, 0xd916, 0xd7e0, 0xd6b0, 0xd584, 0xd45e, 0xd33c, 0xd21e,
    0xd105, 0xcff1, 0xcee1, 0xcdd4, 0xcccc, 0xcbc8, 0xcac8, 0xc9cb,
    0xc8d2, 0xc7dd, 0xc6eb, 0xc5fc, 0xc511, 0xc429, 0xc344, 0xc263,
    0xc184, 0xc0a8, 0xbfd0, 0xbefa, 0xbe26, 0xbd56, 0xbc88, 0xbbbd,
    0xbaf4, 0xba2e, 0xb96a, 0xb8a9, 0xb7ea, 0xb72d, 0xb673, 0xb5bb,
    0xb504, 0xb450, 0xb39f, 0xb2ef, 0xb241, 0xb195, 0xb0eb, 0xb043,
    0xaf9d, 0xaef8, 0xae56, 0xadb5, 0xad16, 0xac79, 0xabdd, 0xab43,
    0xaaaa, 0xaa13, 0xa97e, 0xa8ea, 0xa858, 0xa7c7, 0xa737, 0xa6a9,
    0xa61d, 0xa592, 0xa508, 0xa47f, 0xa3f8, 0xa372, 0xa2ee, 0xa26a,
    0xa1e8, 0xa167, 0xa0e7, 0xa069, 0x9fec, 0x9f6f, 0x9ef4, 0x9e7a,
    0x9e01, 0x9d89, 0x9d13, 0x9c9d, 0x9c28, 0x9bb4, 0x9b42, 0x9ad0,
    0x9a5f, 0x99ef, 0x9981, 0x9913, 0x98a6, 0x983a, 0x97ce, 0x9764,
    0x96fb, 0x9692, 0x962a, 0x95c3, 0x955d, 0x94f8, 0x9493, 0x9430,
    0x93cd, 0x936b, 0x9309, 0x92a9, 0x9249, 0x91e9, 0x918b, 0x912d,
    0x90d0, 0x9074, 0x9018, 0x8fbd, 0x8f63, 0x8f09, 0x8eb0, 0x8e58,
    0x8e00, 0x8da9, 0x8d53, 0x8cfd, 0x8ca8, 0x8c53, 0x8bff, 0x8bac,
    0x8b59, 0x8b06, 0x8ab5, 0x8a64, 0x8a13, 0x89c3, 0x8973, 0x8924,
    0x88d6, 0x8888, 0x883b, 0x87ee, 0x87a1, 0x8755, 0x870a, 0x86bf,
    0x8675, 0x862b, 0x85e1, 0x8598, 0x8550, 0x8508, 0x84c0, 0x8479,
    0x8432, 0x83ec, 0x83a6, 0x8361, 0x831c, 0x82d7, 0x8293, 0x824f,
    0x820c, 0x81c9, 0x8186, 0x8144, 0x8103, 0x80c1, 0x8080, 0x8040,
});

br_fixed_ls C2_HOOK_CDECL BrFixedAbs(br_fixed_ls a) {

    if (a < 0) {
        a = -a;
    }
    return a;
}
C2_HOOK_FUNCTION(0x00537110, BrFixedAbs)

br_fixed_ls C2_HOOK_FASTCALL BrFixedMul(br_fixed_ls a, br_fixed_ls b) {

    return ((long long)a * (long long)b) >> 16;
}
C2_HOOK_FUNCTION(0x00537120, BrFixedMul)

br_fixed_ls C2_HOOK_CDECL BrFixedMac2(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c, br_fixed_ls d) {

    return ((long long)a * (long long)b + (long long)c * (long long)d) >> 16;
}
C2_HOOK_FUNCTION(0x00537131, BrFixedMac2)

br_fixed_ls C2_HOOK_CDECL BrFixedMac3(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c, br_fixed_ls d, br_fixed_ls e, br_fixed_ls f) {

    return ((long long)a * (long long)b + (long long)c * (long long)d + (long long)e * (long long)f) >> 16;
}
C2_HOOK_FUNCTION(0x00537154, BrFixedMac3)

br_fixed_ls C2_HOOK_CDECL BrFixedMac4(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c, br_fixed_ls d, br_fixed_ls e, br_fixed_ls f, br_fixed_ls g, br_fixed_ls h) {

    return ((long long)a * (long long)b + (long long)c * (long long)d + (long long)e * (long long)f + (long long)g * (long long)h) >> 16;
}
C2_HOOK_FUNCTION(0x00537181, BrFixedMac4)

br_fixed_ls C2_HOOK_CDECL BrFixedLength2(br_fixed_ls a, br_fixed_ls b) {

    return _BrISqrt64((long long)a * (long long)a + (long long)b * (long long)b);
}
C2_HOOK_FUNCTION(0x005371b8, BrFixedLength2)

br_fixed_ls C2_HOOK_CDECL BrFixedLength3(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c) {

    return _BrISqrt64((long long)a * (long long)a + (long long)b * (long long)b + (long long)c * (long long)c);
}
C2_HOOK_FUNCTION(0x005371dc, BrFixedLength3)

br_fixed_ls C2_HOOK_CDECL BrFixedLength4(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c, br_fixed_ls d) {

    return _BrISqrt64((long long)a * (long long)a + (long long)b * (long long)b + (long long)c * (long long)c + (long long)d * (long long)d);
}
C2_HOOK_FUNCTION(0x00537209, BrFixedLength4)

br_fixed_ls C2_HOOK_CDECL BrFixedRLength2(br_fixed_ls a, br_fixed_ls b) {

    return _BrFastRSqrt64((long long)a * (long long)a + (long long)b * (long long)b);
}
C2_HOOK_FUNCTION(0x0053723f, BrFixedRLength2)

br_fixed_ls C2_HOOK_CDECL BrFixedRLength3(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c) {

    return _BrFastRSqrt64((long long)a * (long long)a + (long long)b * (long long)b + (long long)c * (long long)c);
}
C2_HOOK_FUNCTION(0x00537263, BrFixedRLength3)

br_fixed_ls C2_HOOK_CDECL BrFixedRLength4(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c, br_fixed_ls d) {

    return _BrFastRSqrt64((long long)a * (long long)a + (long long)b * (long long)b + (long long)c * (long long)c + (long long)d * (long long)d);
}
C2_HOOK_FUNCTION(0x00537290, BrFixedRLength4)

br_fixed_ls C2_HOOK_CDECL BrFixedDiv(br_fixed_ls a, br_fixed_ls b) {

    return (((long long)a) << 16) / (long long)b;
}
C2_HOOK_FUNCTION(0x005372c6, BrFixedDiv)

br_fixed_ls C2_HOOK_CDECL BrFixedDivR(br_fixed_ls a, br_fixed_ls b) {

    return (((long long)a) << 16 | ((((uint32_t)(a)) & 0x80000000) ? 0xffff : 0x0)) / b;
}
C2_HOOK_FUNCTION(0x005372db, BrFixedDivR)

br_fixed_ls C2_HOOK_CDECL BrFixedDivF(br_fixed_ls a, br_fixed_ls b) {

    return (((long long)a) << 31) / b;
}
C2_HOOK_FUNCTION(0x005372f1, BrFixedDivF)

br_fixed_ls C2_HOOK_CDECL BrFixedMulDiv(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c) {

    return ((long long)a * (long long)b) / (long long)c;
}
C2_HOOK_FUNCTION(0x00537322, BrFixedMulDiv)

br_fixed_ls C2_HOOK_CDECL BrFixedMac2Div(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c, br_fixed_ls d, br_fixed_ls e) {

    return ((long long)a * (long long)b + (long long)c * (long long)d) / (long long)e;
}
C2_HOOK_FUNCTION(0x00537332, BrFixedMac2Div)

br_fixed_ls C2_HOOK_CDECL BrFixedMac3Div(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c, br_fixed_ls d, br_fixed_ls e, br_fixed_ls f, br_fixed_ls g) {

    return ((long long)a * (long long)b + (long long)c * (long long)d + (long long)e * (long long)f) / (long long)g;
}
C2_HOOK_FUNCTION(0x00537354, BrFixedMac3Div)

br_fixed_ls C2_HOOK_CDECL BrFixedMac4Div(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c, br_fixed_ls d, br_fixed_ls e, br_fixed_ls f, br_fixed_ls g, br_fixed_ls h, br_fixed_ls i) {

    return ((long long)a * (long long)b + (long long)c * (long long)d + (long long)e * (long long)f + (long long)g * (long long)h) / (long long)i;
}
C2_HOOK_FUNCTION(0x00537380, BrFixedMac4Div)

br_fixed_ls C2_HOOK_CDECL BrFixedFMac2(br_fixed_lsf a, br_fixed_ls b, br_fixed_lsf c, br_fixed_ls d) {

    return ((long long)a * (long long)b + (long long)c * (long long)d) >> 15;
}
C2_HOOK_FUNCTION(0x005373b6, BrFixedFMac2)

br_fixed_ls C2_HOOK_CDECL BrFixedFMac3(br_fixed_lsf a, br_fixed_ls b, br_fixed_lsf c, br_fixed_ls d, br_fixed_lsf e, br_fixed_ls f) {

    return ((long long)a * (long long)b + (long long)c * (long long)d + (long long)e * (long long)f) >> 15;
}
C2_HOOK_FUNCTION(0x005373db, BrFixedFMac3)

br_fixed_ls C2_HOOK_CDECL BrFixedFMac4(br_fixed_lsf a, br_fixed_ls b, br_fixed_lsf c, br_fixed_ls d, br_fixed_lsf e, br_fixed_ls f, br_fixed_lsf g, br_fixed_ls h) {

    return ((long long)a * (long long)b + (long long)c * (long long)d + (long long)e * (long long)f + (long long)g * (long long)h) >> 15;
}
C2_HOOK_FUNCTION(0x0053740b, BrFixedFMac4)

br_fixed_ls C2_HOOK_CDECL BrFixedRcp(br_fixed_ls a) {

    return (br_fixed_ls)(((long long)0x100000000) / a);
}
C2_HOOK_FUNCTION(0x00537446, BrFixedRcp)

br_fixed_ls C2_HOOK_CDECL BrFixedSqr(br_fixed_ls a) {

    return ((long long)a * (long long)a) >> 16;
}
C2_HOOK_FUNCTION(0x00537457, BrFixedSqr)

br_fixed_ls C2_HOOK_CDECL BrFixedSqr2(br_fixed_ls a, br_fixed_ls b) {

    return ((long long)a * (long long)a + (long long)b * (long long)b) >> 16;
}
C2_HOOK_FUNCTION(0x00537467, BrFixedSqr2)

br_fixed_ls C2_HOOK_CDECL BrFixedSqr3(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c) {

    return ((long long)a * (long long)a + (long long)b * (long long)b + (long long)c * (long long)c) >> 16;
}
C2_HOOK_FUNCTION(0x00537488, BrFixedSqr3)

br_fixed_ls C2_HOOK_CDECL BrFixedSqr4(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c, br_fixed_ls d) {

    return ((long long)a * (long long)a + (long long)b * (long long)b + (long long)c * (long long)c + (long long)d * (long long)d) >> 16;
}
C2_HOOK_FUNCTION(0x005374b2, BrFixedSqr4)

static int16_t fixed_lut_interpolate(uint16_t *table, uint32_t value) {
    uint16_t idx = value >> 8;
    int16_t basew = table[idx];
    int16_t deltaw = table[idx+1] - basew;
    int16_t errw = (deltaw * (value & 0xff)) >> 8;
    return basew + errw;
}

br_fixed_ls C2_HOOK_CDECL BrFixedSin(br_angle a) {

    return 2 * fixed_lut_interpolate(C2V(fixed_sin_table), a);
}
C2_HOOK_FUNCTION(0x005374e5, BrFixedSin)

br_fixed_ls C2_HOOK_CDECL BrFixedCos(br_angle a) {

    return 2 * fixed_lut_interpolate(fixed_cos_table, a);
}
C2_HOOK_FUNCTION(0x00537516, BrFixedCos)

br_angle C2_HOOK_CDECL BrFixedASin(br_fixed_ls a) {

    assert(a <= BrFloatToFixed(1.f) && "a must be <= BrFloatToFixed(1.f)");
    assert(a >= BrFloatToFixed(-1.f) && "a must be >= BrFloatToFixed(-1.f)");

    return fixed_lut_interpolate(C2V(fixed_asin_table), (a+0x10000) >> 1);
}
C2_HOOK_FUNCTION(0x00537547, BrFixedASin)

br_angle C2_HOOK_CDECL BrFixedACos(br_fixed_ls a) {

    assert(a <= BrFloatToFixed(1.f) && "a must be <= BrFloatToFixed(1.f)");
    assert(a >= BrFloatToFixed(-1.f) && "a must be >= BrFloatToFixed(-1.f)");

    return fixed_lut_interpolate(C2V(fixed_acos_table), (a+0x10000) >> 1);
}
C2_HOOK_FUNCTION(0x0053757b, BrFixedACos)

br_angle C2_HOOK_CDECL BrFixedATan2(br_fixed_ls x, br_fixed_ls y) {

    if (y > 0) {
        if (x >= 0) {
            if (x > y) {
                return fixed_lut_interpolate(C2V(fixed_atan_table), ((((unsigned long long)y)<<32) / x) >> 16);              // atan(y / x)
            } else if (x == y) {
                return 0x2000;                                                              // PI / 4
            } else {
                return 0x4000 - fixed_lut_interpolate(C2V(fixed_atan_table), ((((unsigned long long)x)<<32) / y) >> 16);     // PI/2 - atan(x / y)
            }
        } else {
            x = -x;
            if (x < y) {
                return 0x4000 + fixed_lut_interpolate(C2V(fixed_atan_table), ((((unsigned long long)x)<<32) / y) >> 16);     // PI/2 + atan(x / y)
            } else if (x == y) {
                return 0x6000;                                                              // 3 * PI / 4
            } else {
                return 0x8000 - fixed_lut_interpolate(C2V(fixed_atan_table), ((((unsigned long long)y)<<32) / x) >> 16);     // PI - atan(y / x)
            }
        }
    } else if (y < 0) {
        y = -y;
        if (x < 0) {
            x = -x;
            if (x > y) {
                return 0x8000 + fixed_lut_interpolate(C2V(fixed_atan_table), ((((unsigned long long)y)<<32) / x) >> 16);     // PI + atan(y / x)
            } else if (x == y) {
                return 0xa000;                                                              // 5 * PI / 4
            } else {
                return 0xc000 - fixed_lut_interpolate(C2V(fixed_atan_table), ((((unsigned long long)x)<<32) / y) >> 16);     // 3 * PI / 2 - atan(x / y)
            }
        } else {
            if (x < y) {
                return 0xc000 + fixed_lut_interpolate(C2V(fixed_atan_table), ((((unsigned long long)x)<<32) / y) >> 16);     // 3 * PI / 2 + atan(x / y)
            } else if (x == y) {
                return 0xe000;                                                              // 7 * PI / 4
            } else {
                return -fixed_lut_interpolate(C2V(fixed_atan_table), ((((unsigned long long)y)<<32) / x) >> 16);             // 2 * pi - atan(y / x)
            }
        }
    } else {
        if (x >= 0) {
            return 0;                                                                       // 0
        } else {
            return 0x8000;                                                                  // PI
        }
    }
}
C2_HOOK_FUNCTION(0x005375af, BrFixedATan2)

br_angle C2_HOOK_CDECL BrFixedATan2Fast(br_fixed_ls x, br_fixed_ls y) {
    if (y > 0) {
        if (x >= 0) {
            if (x > y) {
                return ((((unsigned long long)y)<<32) / x) >> 19;                                                       // atan(y / x)
            } else if (x == y) {
                return 0x2000;                                                              // PI / 4
            } else {
                return 0x4000 - (((((unsigned long long)x)<<32) / y) >> 19);                                            // PI/2 - atan(x / y)
            }
        } else {
            x = -x;
            if (x < y) {
                return 0x4000 + (((((unsigned long long)x)<<32) / y) >> 19);                                            // PI/2 + atan(x / y)
            } else if (x == y) {
                return 0x6000;                                                              // 3 * PI / 4
            } else {
                return 0x8000 - (((((unsigned long long)y)<<32) / x) >> 19);                                            // PI - atan(y / x)
            }
        }
    } else if (y < 0) {
        y = -y;
        if (x < 0) {
            x = -x;
            if (x > y) {
                return 0x8000 + (((((unsigned long long)y)<<32) / x) >> 19);                                            // PI + atan(y / x)
            } else if (x == y) {
                return 0xa000;                                                              // 5 * PI / 4
            } else {
                return 0xc000 - (((((unsigned long long)x)<<32) / y) >> 19);                                            // 3 * PI / 2 - atan(x / y)
            }
        } else {
            if (x < y) {
                return 0xc000 + (((((unsigned long long)x)<<32) / y) >> 19);                                            // 3 * PI / 2 + atan(x / y)
            } else if (x == y) {
                return 0xe000;                                                              // 7 * PI / 4
            } else {
                return (br_angle)(0x10000 - (((((unsigned long long)y)<<32) / x) >> 19));                               // 2 * pi - atan(y / x)
            }
        }
    } else {
        if (x >= 0) {
            return 0;                                                                       // 0
        } else {
            return 0x8000;                                                                  // PI
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x005377b3, BrFixedATan2Fast)

uint16_t C2_HOOK_CDECL _BrISqrt32(uint32_t a) {
    // Shifting nth root algorithm
    uint32_t err;
    uint32_t est;
    uint32_t est_2;
    int i;

    err = 0;
    est = 0;
    est_2 = 0;

    // 2 MSB bits
    err = a >> 30;
    a <<= 2;
    if (err != 0) {
        err -= 1;
        est = 1;
        est_2 = 2;
    }
    // 14 iterations: 14*2-bit pairs=28 bits
    for (i = 0; i < 14; i++) {
        err = (err << 2) | (a >> 30);
        a <<= 2;
        est <<= 1;
        est_2 <<= 1;
        if (err > est_2) {
            err -= est_2 + 1;
            est += 1;
            est_2 += 2;
        }
    }
    // 2 LSB bits
    err = (err << 2) | (a >> 30);
    est <<= 1;
    est_2 <<= 1;
    if (err > est_2) {
        est += 1;
    }
    return est;
}
C2_HOOK_FUNCTION(0x005378bb, _BrISqrt32)

br_uint_32 C2_HOOK_FASTCALL _BrISqrt64(uint64_t a) {
    // Shifting nth root algorithm
    uint64_t err;
    uint64_t est;
    uint64_t est_2;
    int i;

    err = 0;
    est = 0;
    est_2 = 0;

    // 32 iterations (32 * 2bits)
    for (i = 0; i < 32; i++) {
        err = (err << 2) | (a >> 62);
        a <<= 2;
        est <<= 1;
        est_2 <<= 1;
        if (err > est_2) {
            err -= est_2 + 1;
            est += 1;
            est_2 += 2;
        }
    }
    return (br_uint_32)est;
}
C2_HOOK_FUNCTION(0x00537a13, _BrISqrt64)

#if 0
// working inprecise
uint16_t _BrFastSqrt32(uint32_t v) {
    uint32_t exp;

    if (v == 0) {
        return v;
    }
    for (exp = 31; (v >> exp) == 0; exp--) {
    }
    // Exponent must be multiple of 2 (because square root divides exponent by 2)
    exp = (exp | 1) + 1;
    // Normalise number: exponent in [0..31], mantissa in [0x40-0xff]
    v = (v << (31 - exp)) >> 24;
    v = fast_sqrt_table[2*v-0x40];
    return (v << 16) >> (32 - (exp >> 1));
}
#endif

uint16_t C2_HOOK_CDECL _BrFastSqrt32(uint32_t v) {
    uint32_t exp;

    if (v == 0) {
        return v;
    }
    for (exp = 31; (v >> exp) == 0; exp--) {
    }
    // Exponent must be multiple of 2 (because square root divides exponent by 2)
    exp = (exp | 1) + 1;
    // Normalise number: exponent in [0..31], mantissa in [0x40-0xff]
    v = (v << (32 - exp)) >> 24;
    v = C2V(fast_sqrt_table)[v-0x40];
    return (v << 16) >> (32 - (exp >> 1));
}
C2_HOOK_FUNCTION(0x00537cba, _BrFastSqrt32)

uint32_t C2_HOOK_CDECL _BrFastRSqrt32(uint32_t v) {
    uint32_t exp;

    if (v == 0) {
        return v;
    }
    for (exp = 31; (v >> exp) == 0; exp--) {
    }
    exp = exp | 1;
    v = (v << (31 - exp)) >> 24;
    v = C2V(fast_rsqrt_table)[v-0x40];
    return v >> (exp >> 1);
}
C2_HOOK_FUNCTION(0x00537ce5, _BrFastRSqrt32)

br_uint_32 C2_HOOK_FASTCALL _BrFastRSqrt64(uint64_t v) {
    uint32_t exp;
    uint32_t tmp;

    if (v == 0) {
        return 0;
    }
    tmp = v >> 32;
    if (tmp != 0) {
        for (exp = 31; (tmp >> exp) == 0; exp--) {
        }
        exp = (31-exp) & 0xfe;
        v = (v << exp) >> 56;
        v = C2V(fast_rsqrt_table)[v-0x40];
        return (br_uint_32)(v >> ((30 - exp) >> 1));
    }
    tmp = (br_uint_32)v;
    for (exp = 31; (tmp >> exp) == 0; exp--) {
    }
    exp = exp | 1;
    v = (v << (31 - exp)) >> 24;
    v = C2V(fast_rsqrt_table)[v-0x40];
    return (br_uint_32)((v << 16) >> (exp >> 1));
}
C2_HOOK_FUNCTION(0x00537d0e, _BrFastRSqrt64)