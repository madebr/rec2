/* Execute original CreatePuffOfSmoke on physical x87, without translating it.
 * The Python verifier first checks the original SHA256. This routine has no
 * external calls; only its 26 absolute data references/immediates are relocated.
 * Unsigned stack arguments deliver signaling NaN bits without caller quieting.
 */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>

typedef void (__fastcall *puff_fn)(void*, void*, unsigned, unsigned, int);

int main(int argc, char** argv) {
    FILE* input;
    unsigned char* disk;
    unsigned char* image;
    unsigned size;
    unsigned i;
    unsigned j;
    unsigned value;
    unsigned strength;
    IMAGE_DOS_HEADER* dos;
    IMAGE_NT_HEADERS32* nt;
    IMAGE_SECTION_HEADER* sections;
    float pos[3] = { 3.f, -2.f, 0.f };
    float velocity[3] = { 6.9f, -6.9f, 13.8f };
    unsigned references[] = {
        0x660110, 0x75bc78, 0x75bc7c, 0x75bc80, 0x655f54, 0x75bc2c,
        0x58b0ac, 0x6aa594, 0x58b060, 0x6a8cf0, 0x6a8cf4, 0x6a8cf8,
        0x58b0b0, 0x6a8ce4, 0x6a8ce8, 0x6a8cec, 0x6a8cfc, 0x58b0b8,
        0x58afdc, 0x6aa590, 0x6a8d00, 0x74a5e4, 0x6a8ce0, 0x6a8d08,
        0x6a8d04, 0x6a8d09
    };

    if (argc != 3) {
        return 2;
    }
    strength = (unsigned)strtoul(argv[2], NULL, 0);
    input = fopen(argv[1], "rb");
    if (input == NULL) {
        return 3;
    }
    fseek(input, 0, SEEK_END);
    size = (unsigned)ftell(input);
    rewind(input);
    disk = (unsigned char*)malloc(size);
    if (disk == NULL || fread(disk, 1, size, input) != size) {
        return 4;
    }
    fclose(input);
    dos = (IMAGE_DOS_HEADER*)disk;
    nt = (IMAGE_NT_HEADERS32*)(disk + dos->e_lfanew);
    image = (unsigned char*)VirtualAlloc(NULL, nt->OptionalHeader.SizeOfImage,
        MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (image == NULL) {
        return 5;
    }
    sections = IMAGE_FIRST_SECTION(nt);
    for (i = 0; i < nt->FileHeader.NumberOfSections; i++) {
        memcpy(image + sections[i].VirtualAddress,
            disk + sections[i].PointerToRawData, sections[i].SizeOfRawData);
    }
    for (i = 0; i + 4 <= 0x1d7; i++) {
        memcpy(&value, image + 0xfb330 + i, 4);
        for (j = 0; j < sizeof(references) / sizeof(references[0]); j++) {
            if (value == references[j]) {
                value = (unsigned)(ULONG_PTR)image + value - 0x400000;
                memcpy(image + 0xfb330 + i, &value, 4);
                i += 3;
                break;
            }
        }
    }
#define ORIGINAL_PTR(address) ((unsigned*)(image + (address) - 0x400000))
    *ORIGINAL_PTR(0x660110) = 1;
    *ORIGINAL_PTR(0x6aa590) = 0;
    *ORIGINAL_PTR(0x6aa594) = 0;
    *ORIGINAL_PTR(0x655f54) = (unsigned)(ULONG_PTR)ORIGINAL_PTR(0x75bc2c);
    *ORIGINAL_PTR(0x74a5e4) = 0x12345678;
    memset(ORIGINAL_PTR(0x75bc78), 0, 12);
    _controlfp(_PC_53, _MCW_PC);
    ((puff_fn)(image + 0xfb330))(pos, velocity, strength, 0x3e99999a, 8);
    printf("%u\n", *ORIGINAL_PTR(0x6a8d00));
    VirtualFree(image, 0, MEM_RELEASE);
    free(disk);
    return 0;
}
