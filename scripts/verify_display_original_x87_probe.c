/* Execute original DistanceFromPlane bytes on physical x87 at PC53.
 * The Python verifier hash-checks the PE first. This 80-byte leaf has no calls,
 * branches or absolute data references. Its pointer is ECX; four raw scalar
 * DWORDs are stack arguments, ret16. A thiscall pointer preserves that ABI and
 * avoids caller quieting of signaling NaN coefficient bits.
 */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

typedef double (__thiscall *plane_fn)(void*, unsigned, unsigned, unsigned, unsigned);
typedef void (__thiscall *plane_caller_fn)(void*, unsigned, unsigned, unsigned, unsigned);

int main(int argc, char** argv) {
    FILE* file;
    unsigned char* disk;
    unsigned char* code;
    unsigned size;
    unsigned i;
    unsigned values[12];
    unsigned scalar_bits;
    unsigned double_bits[2];
    IMAGE_DOS_HEADER* dos;
    IMAGE_NT_HEADERS32* nt;
    IMAGE_SECTION_HEADER* sections;
    double result;
    double scale;
    float highest;
    float scalar;
    /* Relocated addressing forms of ProcessShadow's 0x4e9471..0x4e9485:
     * FST scalar copy; FMUL double scale; FMUL scalar A; FSTP first component.
     * Original arithmetic opcodes/order and unspilled ST0 are preserved.
     */
    static const unsigned char first_component[] = {
        0xd9, 0x51, 0x28,
        0xdc, 0x49, 0x20,
        0xd8, 0x4c, 0x24, 0x04,
        0xd9, 0x59, 0x2c,
        0xc2, 0x10, 0x00
    };

    if (argc != 9 && argc != 10) {
        return 2;
    }
    for (i = 0; i < 7; i++) {
        values[i] = (unsigned)strtoul(argv[i + 2], NULL, 0);
    }
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
    sections = IMAGE_FIRST_SECTION(nt);
    code = (unsigned char*)VirtualAlloc(NULL, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (code == NULL) {
        return 5;
    }
    for (i = 0; i < nt->FileHeader.NumberOfSections; i++) {
        if (sections[i].VirtualAddress <= 0x7b910 && 0x7b960 <= sections[i].VirtualAddress + sections[i].SizeOfRawData) {
            memcpy(code, disk + sections[i].PointerToRawData + 0x7b910 - sections[i].VirtualAddress, 80);
            break;
        }
    }
    if (i == nt->FileHeader.NumberOfSections) {
        return 6;
    }
    _controlfp(_PC_53, _MCW_PC);
    result = ((plane_fn)code)(values, values[3], values[4], values[5], values[6]);
    scalar = (float)result;
    memcpy(&scalar_bits, &scalar, 4);
    memcpy(double_bits, &result, 8);
    values[7] = argc == 10 ? (unsigned)strtoul(argv[9], NULL, 0) : 0x3f800000;
    memcpy(&highest, values + 7, 4);
    /* Original constants at 0x58ae58/80/70/88/18, in original operation order. */
    scale = 2.2 - (double)highest;
    scale *= 0.52;
    scale *= -0.45454545454545453;
    scale = 0.4 - scale;
    scale = 1.0 - scale;
    memcpy(values + 8, &scale, 8);
    memcpy(code + 76, first_component, sizeof(first_component));
    ((plane_caller_fn)code)(values, values[3], values[4], values[5], values[6]);
    printf("[%u,\"%08x%08x\",%u,%u]\n", scalar_bits, double_bits[1], double_bits[0], values[10], values[11]);
    VirtualFree(code, 0, MEM_RELEASE);
    free(disk);
    return 0;
}
