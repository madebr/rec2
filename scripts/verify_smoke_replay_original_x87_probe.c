/* Run original RenderSmoke replay metadata on physical x87.
 * Python verifies the PE hash and supplies exact Capstone data-reference offsets.
 * Actor/transform/random APIs and the final renderer are explicit return stubs;
 * original replay aspect arithmetic and raw radius/strength MOVs execute intact.
 */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

typedef void (__fastcall *render_fn)(void*, void*, void*, void*, unsigned);

int main(int argc, char** argv) {
    FILE* file;
    unsigned char* disk;
    unsigned char* image;
    unsigned char actor[100];
    unsigned size;
    unsigned i;
    unsigned count;
    unsigned offset;
    unsigned expected;
    unsigned value;
    unsigned radius;
    unsigned strength;
    unsigned type;
    IMAGE_DOS_HEADER* dos;
    IMAGE_NT_HEADERS32* nt;
    IMAGE_SECTION_HEADER* sections;
    unsigned returns[] = { 0x51dfe0, 0x51e070, 0x576960, 0x4fb1b0 };

    if (argc != 6) {
        return 2;
    }
    radius = (unsigned)strtoul(argv[3], NULL, 0);
    strength = (unsigned)strtoul(argv[4], NULL, 0);
    type = (unsigned)strtoul(argv[5], NULL, 0);
    file = fopen(argv[1], "rb");
    if (file == NULL) {
        return 3;
    }
    fseek(file, 0, SEEK_END);
    size = (unsigned)ftell(file);
    rewind(file);
    disk = (unsigned char*)malloc(size);
    if (disk == NULL || fread(disk, 1, size, file) != size) {
        return 4;
    }
    fclose(file);
    dos = (IMAGE_DOS_HEADER*)disk;
    nt = (IMAGE_NT_HEADERS32*)(disk + dos->e_lfanew);
    image = (unsigned char*)VirtualAlloc(NULL, nt->OptionalHeader.SizeOfImage,
        MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (image == NULL) {
        return 5;
    }
    sections = IMAGE_FIRST_SECTION(nt);
    for (i = 0; i < nt->FileHeader.NumberOfSections; i++) {
        memcpy(image + sections[i].VirtualAddress, disk + sections[i].PointerToRawData,
            sections[i].SizeOfRawData);
    }
    file = fopen(argv[2], "rb");
    if (file == NULL || fread(&count, 4, 1, file) != 1) {
        return 6;
    }
    for (i = 0; i < count; i++) {
        if (fread(&offset, 4, 1, file) != 1 || fread(&expected, 4, 1, file) != 1 || offset > 2201) {
            return 7;
        }
        memcpy(&value, image + 0xfa910 + offset, 4);
        if (value != expected) {
            return 8;
        }
        value = value - 0x400000 + (unsigned)(ULONG_PTR)image;
        memcpy(image + 0xfa910 + offset, &value, 4);
    }
    fclose(file);
    for (i = 0; i < sizeof(returns) / sizeof(returns[0]); i++) {
        image[returns[i] - 0x400000] = 0xc3;
    }
    memcpy(image + 0x176970, "\x31\xc0\xc3", 3); /* rand: return zero */
    memcpy(image + 0x115610, "\xc2\x04\x00", 3); /* transform: return, pop matrix */
    memset(actor, 0, sizeof(actor));
#define PTR(address) ((unsigned*)(image + (address) - 0x400000))
    *PTR(0x74caac) = (unsigned)(ULONG_PTR)actor;
    *PTR(0x74cf68) = 1;
    *PTR(0x676914) = 1;
    *PTR(0x6aa590) = 1;
    *PTR(0x6aa59c) = 0;
    *PTR(0x74b778) = 0;
    *PTR(0x6a8cfc) = radius;
    *PTR(0x6a8d00) = strength;
    *(unsigned char*)PTR(0x6a8d08) = (unsigned char)type;
    _controlfp(_PC_53, _MCW_PC);
    ((render_fn)(image + 0xfa910))(NULL, NULL, NULL, NULL, 0);
    printf("{\"radius\":%u,\"strength\":%u,\"aspect\":%u,\"count\":%u,\"input_radius\":%u,\"input_strength\":%u}\n",
        *PTR(0x6a87fc), *PTR(0x6a8800), *PTR(0x6a8808), *PTR(0x6aa56c),
        *PTR(0x6a8cfc), *PTR(0x6a8d00));
    VirtualFree(image, 0, MEM_RELEASE);
    free(disk);
    return 0;
}
