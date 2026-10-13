/* Execute the original AdjustFlame bytes directly on physical x87. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

typedef void (__fastcall *adjust_fn)(unsigned, unsigned, unsigned, unsigned, unsigned, unsigned);

int main(int argc, char** argv) {
    FILE* file;
    unsigned char* disk;
    unsigned char* code;
    unsigned columns[310];
    unsigned bits[4];
    unsigned size;
    unsigned i;
    unsigned address;
    IMAGE_NT_HEADERS32* nt;
    IMAGE_SECTION_HEADER* section;

    if (argc != 6) return 2;
    for (i = 0; i < 4; i++) bits[i] = (unsigned)strtoul(argv[i + 2], NULL, 0);
    file = fopen(argv[1], "rb");
    if (file == NULL) return 3;
    fseek(file, 0, SEEK_END);
    size = (unsigned)ftell(file);
    rewind(file);
    disk = (unsigned char*)malloc(size);
    if (disk == NULL || fread(disk, 1, size, file) != size) return 4;
    fclose(file);
    nt = (IMAGE_NT_HEADERS32*)(disk + ((IMAGE_DOS_HEADER*)disk)->e_lfanew);
    section = IMAGE_FIRST_SECTION(nt);
    code = (unsigned char*)VirtualAlloc(NULL, 63, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (code == NULL) return 5;
    for (i = 0; i < nt->FileHeader.NumberOfSections; i++) {
        if (section[i].VirtualAddress <= 0xfba00
                && 0xfba00 + 63 <= section[i].VirtualAddress + section[i].SizeOfRawData) {
            memcpy(code, disk + section[i].PointerToRawData + 0xfba00 - section[i].VirtualAddress, 63);
            break;
        }
    }
    if (i == nt->FileHeader.NumberOfSections) return 6;
    /* Original LEA's sole absolute displacement; instructions stay intact. */
    memcpy(&address, code + 0x17, 4);
    if (address != 0x6a96a8) return 7;
    address = (unsigned)(ULONG_PTR)columns;
    memcpy(code + 0x17, &address, 4);
    memset(columns, 0xa5, sizeof(columns));
    _controlfp(_PC_53, _MCW_PC);
    ((adjust_fn)code)(0, 42, bits[0], bits[1], bits[2], bits[3]);
    printf("[%u,%u,%u,%u]\n", columns[16], columns[19], columns[22], columns[25]);
    VirtualFree(code, 0, MEM_RELEASE);
    free(disk);
    return 0;
}
