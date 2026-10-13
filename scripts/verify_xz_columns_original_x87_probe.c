/* Execute original XZToColumnXZ + actual original _ftol on physical x87.
 * Python hash-checks the PE; only absolute zero/one references are relocated.
 * Internal relative _ftol calls remain unchanged in the copied full image.
 * Raw unsigned X/Z stack words prevent caller quieting of signaling NaNs.
 */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <io.h>
#include <fcntl.h>

typedef void (__fastcall *column_fn)(unsigned char*, unsigned char*, unsigned, unsigned, void*);

int main(int argc, char** argv) {
    FILE* input;
    unsigned char* disk;
    unsigned char* image;
    unsigned char arena[128];
    unsigned header[10];
    unsigned size;
    unsigned i;
    unsigned value;
    unsigned relocations = 0;
    IMAGE_DOS_HEADER* dos;
    IMAGE_NT_HEADERS32* nt;
    IMAGE_SECTION_HEADER* sections;

    if (argc != 3) { return 2; }
    input = fopen(argv[2], "rb");
    if (input == NULL || fread(header, sizeof(header), 1, input) != 1
            || fread(arena, sizeof(arena), 1, input) != 1) { return 3; }
    fclose(input);
    input = fopen(argv[1], "rb");
    if (input == NULL) { return 4; }
    fseek(input, 0, SEEK_END);
    size = (unsigned)ftell(input);
    rewind(input);
    disk = (unsigned char*)malloc(size);
    if (disk == NULL || fread(disk, 1, size, input) != size) { return 5; }
    fclose(input);
    dos = (IMAGE_DOS_HEADER*)disk;
    nt = (IMAGE_NT_HEADERS32*)(disk + dos->e_lfanew);
    image = (unsigned char*)VirtualAlloc(NULL, nt->OptionalHeader.SizeOfImage,
        MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (image == NULL) { return 6; }
    sections = IMAGE_FIRST_SECTION(nt);
    for (i = 0; i < nt->FileHeader.NumberOfSections; i++) {
        memcpy(image + sections[i].VirtualAddress,
            disk + sections[i].PointerToRawData, sections[i].SizeOfRawData);
    }
    for (i = 0; i + 4 <= 184; i++) {
        memcpy(&value, image + 0xcb90 + i, 4);
        if (value == 0x5893b8 || value == 0x5893bc) {
            value = (unsigned)(ULONG_PTR)image + value - 0x400000;
            memcpy(image + 0xcb90 + i, &value, 4);
            relocations++;
            i += 3;
        }
    }
    if (relocations != 5) { return 7; }
    _controlfp(_PC_53, _MCW_PC);
    ((column_fn)(image + 0xcb90))(arena + header[6], arena + header[7],
        header[0], header[1], arena + 32);
    _setmode(_fileno(stdout), _O_BINARY);
    fwrite(arena, sizeof(arena), 1, stdout);
    VirtualFree(image, 0, MEM_RELEASE);
    free(disk);
    return 0;
}
