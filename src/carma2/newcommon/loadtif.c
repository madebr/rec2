#include "loadtif.h"

#include "08-loading1.h"
#include "41-utility.h"
#include "70-packfile.h"
#include "globvars.h"
#include "loadtif_zlib.h"
#include "platform.h"
#include "rec2_macros.h"
#include "rec2_types.h"

#include "c2_string.h"

#include <tiffio.h>

#include <sys/types.h>
#include <sys/stat.h>

// FUNCTION: CARMA2_HW 0x00485590
br_pixelmap* C2_HOOK_FASTCALL CreatePalettePixelmapFromRGBChannels(br_uint_16* pRed_channel, br_uint_16* pGrn_channel, br_uint_16* pBlu_channel, int pRGB555) {
    br_pixelmap* pm;
    int i;

    if (pRGB555) {
        br_uint_16* pixels;

        pm = BrPixelmapAllocate(BR_PMT_RGB_565, 1, 256, NULL, 0);
        if (pm == NULL) {
            return NULL;
        }
        pixels = pm->pixels;
        for (i = 0; i < 256; i++) {
                pixels[i] = ((pRed_channel[i] & 0xf800) >> 0) | ((pGrn_channel[i] & 0xfc00) >> 5) | ((pBlu_channel[i] >> 11));
        }
    } else {
        br_uint_8* pixels;

        pm = BrPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, NULL, 0);
        if (pm == NULL) {
            return NULL;
        }
        pixels = pm->pixels;

        pixels[0] = 0;
        pixels[1] = 0;
        pixels[2] = 0;
        for (i = 1; i < 256; i++) {
            pixels[4 * i + 0] = pBlu_channel[i] >> 8;
            pixels[4 * i + 1] = pGrn_channel[i] >> 8;
            pixels[4 * i + 2] = pRed_channel[i] >> 8;
            if (pixels[4 * i + 0] == 0 && pixels[4 * i + 1] == 0 && pixels[4 * i + 2] == 0) {
                pixels[4 * i + 0] = 1;
                pixels[4 * i + 1] = 1;
                pixels[4 * i + 2] = 1;
            }
        }
    }
    pm->identifier = BrResStrDup(pm, "palette");
    if (pm->identifier == NULL) {
        BrPixelmapFree(pm);
        return NULL;
    }
    return pm;
}

br_pixelmap* C2_HOOK_FASTCALL LoadDefaultPalette(const char* pData_dir_path, int pFlags, int *pError_code) {
    int i;
    tPath_name path;
    br_uint_8 buffer[256 * 3];
    FILE* f;
    br_pixelmap* pm;

    sprintf(path, "%s%s%s%s%s", pData_dir_path, gDir_separator, "PALETTE", gDir_separator, "DEFAULT.ACT");
    f = DRfopen(path, "rb");
    if (f == NULL) {
        *pError_code = 5;
        return NULL;
    }
    if (fread(buffer, REC2_ASIZE(buffer), 1, f) == 0) {
#ifdef REC2_FIX_BUGS
        fclose(f);
#endif
        *pError_code = 5;
        return NULL;
    }
    fclose(f);
    if (pFlags & kLoadTextureFlags_PalatteRGB555) {
        br_uint_32* pixel;

        pm = BrPixelmapAllocate(BR_PMT_RGB_565, 1, 256, NULL, 0);
        if (pm == NULL) {
            *pError_code = 2;
            return NULL;
        }
        pixel = pm->pixels;
        for (i = 0; i < 256; i++) {
            pixel[i] = ((buffer[3 * i + 0] >> 3) << 11) | ((buffer[3 * i + 1] >> 2) << 5) | ((buffer[3 * i + 2] >> 3) << 0);
        }
    } else {
        br_uint_8* pixel;

        pm = BrPixelmapAllocate(BR_PMT_RGBX_888, 1, 0x100, NULL, 0);
        if (pm == NULL) {
            *pError_code = 2;
            return NULL;
        }
        pixel = pm->pixels;
        pixel[2] = 0;
        pixel[1] = 0;
        pixel[0] = 0;
        for (i = 1; i < 256; i++) {
            pixel[4 * i + 0] = buffer[3 * i + 2];
            pixel[4 * i + 1] = buffer[3 * i + 1];
            pixel[4 * i + 2] = buffer[3 * i + 0];
            if (pixel[4 * i + 0] == 0 && pixel[4 * i + 1] == 0 && pixel[4 * i + 2] == 0) {
                pixel[4 * i + 0] = 1;
                pixel[4 * i + 1] = 1;
                pixel[4 * i + 2] = 1;
            }
        }
    }
    pm->identifier = BrResStrDup(pm, "DEFAULT.ACT");
    if (pm->identifier == NULL) {
        BrPixelmapFree(pm);
        *pError_code = 2;
        return NULL;
    }
    return pm;
}

int C2_HOOK_FASTCALL CheckPixOutdated(const char* pDirectory, const char* pStem, const char* pPix_path) {
    tPath_name tif_path;

    sprintf(tif_path, "%s%s%s%s%s%s", pDirectory, gDir_separator, "TIFFRGB", gDir_separator, pStem, ".TIF");
    ReadFileLink(tif_path,tif_path);
    if (PDFileExists(tif_path) && PDGetLastModificationTime(pPix_path) < PDGetLastModificationTime(tif_path)) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00485750
br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_Ex2(const char* pDirectory, const char* pFile_stem, br_pixelmap* pPalette, int pFlags, int* pError_code, int pTiff_palette) {
    tPath_name tif_path;
    tPath_name pix_path;
    tPath_name pix_path_target;
    tPath_name save_pix_dir_path;
    tPath_name save_dir_path;
    tPath_name save_file_path;
    int pix_path_is_link;
    int tif_path_is_link;
    int pix_exists;
    int tif_exists;
    br_pixelmap* pm;
    br_pixelmap* pal;
    br_filesystem* original_filesystem;

    sprintf(tif_path, "%s%s%s%s%s%s", pDirectory, gDir_separator, pTiff_palette ? "TIFFX" : "TIFFRGB", gDir_separator, pFile_stem, ".TIF");
    sprintf(pix_path, "%s%s%s%s%s%s", pDirectory, gDir_separator, (pFlags & kLoadTextureFlags_16bbp) ? "PIX16" : "PIX8", gDir_separator, pFile_stem, ".PIX");
    tif_path_is_link = ReadFileLink(tif_path, tif_path);
    pix_path_is_link = ReadFileLink(pix_path_target, pix_path);
    tif_exists = PDFileExists(tif_path);
    pix_exists = PDFileExists(pix_path_is_link ? pix_path_target : pix_path);

    if (pTiff_palette && pix_exists && !tif_exists) {
        if (CheckPixOutdated(pDirectory, pFile_stem, pix_path_is_link ? pix_path_target : pix_path)) {
            return 0;
        }
    }
    if (!tif_exists || (((pFlags & kLoadTextureFlags_16bbp) || pTiff_palette) && !(pFlags & kLoadTextureFlags_ForceTiff) && pix_exists && PDGetLastModificationTime(pix_path_is_link ? pix_path_target : pix_path) >= PDGetLastModificationTime(tif_path))) {

        if (pix_exists && ((pFlags & kLoadTextureFlags_16bbp) || pTiff_palette)) {
            original_filesystem = BrFilesystemSet(&gZlib_filesystem);
            pm = BrPixelmapLoad(pix_path_is_link ? pix_path_target : pix_path);
            BrFilesystemSet(original_filesystem);
            *pError_code = (pm != NULL) ? 0 : 3;
            return pm;
        } else {
            *pError_code = 1;
            return NULL;
        }
    }

    if (pTiff_palette) {
        pm = LoadTiff8bit(tif_path, pFlags, pError_code);
    } else if (pFlags & kLoadTextureFlags_16bbp) {
        pm = LoadTiff16bit(tif_path, pFlags, pError_code);
    } else {
        pal = pPalette;
        if (pPalette == NULL) {
            pal = LoadDefaultPalette(pDirectory, pFlags, pError_code);
            if (pal == NULL) {
                return NULL;
            }
        }
        pm = LoadTiffUsingPalette(tif_path, pal, pFlags, pError_code);
        if (pm != NULL && (pFlags & kLoadTextureFlags_KeepShadeTable)) {
            pm->map = pal;
            if (pPalette == NULL) {
                BrResAdd(pm, pal);
            }
        } else {
            if (pPalette == NULL) {
                BrPixelmapFree(pal);
            }
        }
    }
    if (pm != NULL && (pFlags & kLoadTextureFlags_SaveBrenderTexture)) {
        if (!pix_path_is_link) {
            sprintf(save_pix_dir_path, "%s%s%s", pDirectory, gDir_separator, (pFlags & kLoadTextureFlags_16bbp) ? "PIX16" : "PIX8");
            PDmkdir(save_pix_dir_path);
            if (tif_path_is_link && !pix_exists) {
                BuildPathWithDirAndSuffix(save_dir_path, save_file_path, tif_path, (pFlags & kLoadTextureFlags_16bbp) ? "PIX16" : "PIX8", ".TIF", ".PIX");
                PDmkdir(save_dir_path);
                WriteFileLink(save_file_path, pix_path);
                pix_path_is_link = 1;
            }
        }
        if (!(pFlags & kLoadTextureFlags_SaveTextureCompressed)) {
            original_filesystem = BrFilesystemSet(&gZlib_filesystem);
        }
        BrPixelmapSave(pix_path_is_link ? pix_path_target : pix_path, pm);
        if (!(pFlags & kLoadTextureFlags_SaveTextureCompressed)) {
            BrFilesystemSet(original_filesystem);
        }
    }
    if (pm != NULL) {
        *pError_code = 0;
    }
    return pm;
}

// STUB: CARMA2_HW 0x004856c0
br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_Ex(const char* pDirectory, const char* pFile_stem, br_pixelmap* pPalette, int pFlags, int* pError_code) {
    br_pixelmap* texture;

    texture = NULL;
    if ((pFlags & kLoadTextureFlags_16bbp) != kLoadTextureFlags_16bbp) {
        texture = LoadTiffTexture_Ex2(pDirectory, pFile_stem, pPalette, pFlags, pError_code, 1);
    }
    if (texture == NULL) {
        texture = LoadTiffTexture_Ex2(pDirectory, pFile_stem, pPalette, pFlags, pError_code, 0);
    }
    if (texture != NULL) {
        texture->identifier = BrResStrDup(texture, pFile_stem);
        if (texture->identifier == NULL) {
            BrPixelmapFree(texture);
            *pError_code = 2;
            return NULL;
        }
    }
    return texture;
}

// FUNCTION: CARMA2_HW 0x004869e0
int C2_HOOK_FASTCALL ReadFileLink(char* pDest, const char* pPath) {
    FILE* f;
    char s1[256];
    char s2[256];
    size_t i;
    size_t len;
    char c;

    f = DRfopen(pPath, "rt");
    if (f == NULL) {
        return 0;
    }
    GetALineAndDontArgue(f, s1);
    if (strstr(s1, ".TIF") != NULL) {

        len = strlen(s1);
        for (i = 0; i < len; i++) {
            c = s1[i];
            if (c == '\\') {
                c = *gDir_separator;
            }
            s2[i] = c;
        }
        s2[len] = '\0';
        fclose(f);
        PathCat(s1, gApplication_path, s2);
        strcpy(pDest, s1);
        return 1;
    } else if (strstr(s1, ".PIX") != NULL) {

        len = strlen(s1);
        for (i = 0; i < len; i++) {
            c = s1[i];
            if (c == '\\') {
                c = *gDir_separator;
            }
            s2[i] = c;
        }
        s2[len] = '\0';
        fclose(f);
        PathCat(s1, gApplication_path, s2);
        strcpy(pDest, s1);
        return 1;
    } else {
        fclose(f);
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00486b20
int C2_HOOK_FASTCALL WriteFileLink(const char* pLink_dest, const char* pLink_source) {
    char *ptr;
    FILE *f;
    int i;
    char rel_dest[256];
    char s[256];

    i = 0;
    strcpy(s, pLink_dest);
    for (ptr = strstr(s, "DATA") + 5; *ptr != '\0'; i++, ptr++) {

        if (*ptr == '\\') {
            rel_dest[i] = *ptr;
        } else if (*ptr < '\\' && *ptr == ':') {
            rel_dest[i] = '\\';
        } else {
            rel_dest[i] = *ptr;
        }
    }
    rel_dest[i] = '\0';
    f = DRfopen(pLink_source, "wt");
    if (f != NULL) {
        fputs(rel_dest, f);
        fclose(f);
        return 0;
    } else {
        return 1;
    }
}

// FUNCTION: CARMA2_HW 0x00486be0
int C2_HOOK_FASTCALL PDGetLastModificationTime(const char* pPath) {
    struct stat statbuf;

    if (stat(pPath, &statbuf) == -1) {
        return 0;
    }
    return (int)statbuf.st_mtime;
}

// FUNCTION: CARMA2_HW 0x00486c00
int C2_HOOK_FASTCALL PDFileExists(const char *pPath) {
    struct stat statbuf;

    return stat(pPath, &statbuf) == 0;
}

// PDmkdir

// FUNCTION: CARMA2_HW 0x00485fe0
br_pixelmap* C2_HOOK_FASTCALL LoadTiff8bit(const char *pPath, int pFlags, int *pError_code) {
    TIFF* tif;
    br_pixelmap* pm;
    br_uint_32 height;
    br_uint_32 width;
    br_uint_16 samples_per_pixel;
    br_uint_8* scanline_buffer;
    br_uint_8* curSrcPos;
    br_uint_32 x;
    br_uint_32 y;
    br_uint_16* colorMap_Red;
    br_uint_16* colorMap_Green;
    br_uint_16* colorMap_Blue;

    tif = TIFFOpen(pPath, "r");
    if (tif == NULL) {
        *pError_code = 4;
        return NULL;
    }
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
    pm = BrPixelmapAllocate(BR_PMT_INDEX_8, width, height, NULL, 0);
    if (pm == NULL) {
        TIFFClose(tif);
        *pError_code = 2;
        return NULL;
    }
    if (samples_per_pixel == 1) {
        for (y = 0; y < height; y++) {
            TIFFReadScanline(tif, (br_uint_8*)pm->pixels + y * pm->row_bytes, y, 0);
        }
    } else {
        scanline_buffer = malloc(TIFFScanlineSize(tif));
        if (scanline_buffer == NULL) {
            TIFFClose(tif);
            BrPixelmapFree(pm);
            *pError_code = 2;
            return NULL;
        }
        for (y = 0; y < height; y++) {
            if (TIFFReadScanline(tif, scanline_buffer, y, 0) < 1) {
                break;
            }
            curSrcPos = scanline_buffer;
            for (x = 0; x < width; x++) {
                if (curSrcPos[1] < 0x80) {
                    *(((br_uint_8 *) pm->pixels) + y * pm->row_bytes + x) = 0;
                } else {
                    *(((br_uint_8 *) pm->pixels) + y * pm->row_bytes + x) = curSrcPos[0];
                }
                curSrcPos += samples_per_pixel;
            }
        }
        free(scanline_buffer);
    }
    if (y < height) {
        TIFFClose(tif);
        BrPixelmapFree(pm);
        *pError_code = 4;
        return NULL;
    }
    if ((pFlags & kLoadTextureFlags_KeepShadeTable) != 0) {
        TIFFGetField(tif, TIFFTAG_COLORMAP, &colorMap_Red, &colorMap_Green, &colorMap_Blue);
        pm->map = CreatePalettePixelmapFromRGBChannels(colorMap_Red, colorMap_Green, colorMap_Blue, pFlags & kLoadTextureFlags_PalatteRGB555);
        if (pm->map == NULL) {
            TIFFClose(tif);
            BrPixelmapFree(pm);
            *pError_code = 2;
            return NULL;
        }
        BrResAdd(pm, pm->map);
    }
    TIFFClose(tif);
    return pm;
}

// FUNCTION: CARMA2_HW 0x004864a0
br_pixelmap* C2_HOOK_FASTCALL LoadTiff16bit(const char *pPath, int pFlags, int *pError_code) {
    TIFF* tif;
    br_pixelmap* pm;
    br_uint_8 pixel_type;
    br_uint_32 height;
    br_uint_32 width;
    br_uint_16 samples_per_pixel;
    br_uint_8* scanline_buffer;
    br_uint_32 x;
    br_uint_32 y;
    br_uint_8 red;
    br_uint_8 grn;
    br_uint_8 blu;
    br_uint_8 alp;

    tif = TIFFOpen(pPath, "r");
    if (tif == NULL) {
        *pError_code = 4;
        return NULL;
    }
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
    if (samples_per_pixel == 3) {
        pixel_type = BR_PMT_RGB_565;
    } else {
        if (pFlags & kLoadTextureFlags_UseARGB1555) {
            pixel_type = BR_PMT_ARGB_1555;
        } else {
            pixel_type = BR_PMT_RGBA_4444;
        }
    }
    pm = BrPixelmapAllocate(pixel_type, width, height, NULL, 0);
    if (pm == NULL) {
        TIFFClose(tif);
        *pError_code = 2;
        return NULL;
    }
    scanline_buffer = _TIFFmalloc(TIFFScanlineSize(tif));
    if (scanline_buffer == NULL) {
        TIFFClose(tif);
        BrPixelmapFree(pm);
        *pError_code = 2;
        return NULL;
    }

    for (y = 0; y < height && TIFFReadScanline(tif, scanline_buffer, y, 0) > 0; y++) {

        if (samples_per_pixel == 3) {
            // RGB888 -> RGB565
            for (x = 0; x < width; x++) {
                red = scanline_buffer[3 * x + 0];
                grn = scanline_buffer[3 * x + 1];
                blu = scanline_buffer[3 * x + 2];

                *((br_uint_16*)((br_uint_8*)pm->pixels + y * pm->row_bytes + sizeof(br_uint_16) * x)) = ((red >> 3) << 11) | ((grn >> 2) << 5) | (blu >> 3);
            }
        }
        else if (pFlags & kLoadTextureFlags_UseARGB1555) {
            // RGBA8888 -> ARGB1555
            for (x = 0; x < width; x++) {
                red = scanline_buffer[4 * x + 0];
                grn = scanline_buffer[4 * x + 1];
                blu = scanline_buffer[4 * x + 2];
                alp = scanline_buffer[4 * x + 3];
                *((br_uint_16*)((br_uint_8*)pm->pixels + y * pm->row_bytes + sizeof(br_uint_16) * x)) = ((alp >= 0x80) << 15) | ((red >> 3) << 10) | ((grn >> 3) << 5) | (blu >> 3);
            }
        } else {
            // RGBA8888 -> ARGB4444
            for (x = 0; x < width; x++) {
                red = scanline_buffer[4 * x + 0];
                grn = scanline_buffer[4 * x + 1];
                blu = scanline_buffer[4 * x + 2];
                alp = scanline_buffer[4 * x + 3];
                *((br_uint_16*)((br_uint_8*)pm->pixels + y * pm->row_bytes + sizeof(br_uint_16) * x)) = ((alp >> 4) << 12) | ((red >> 4) << 8) | ((grn >> 4) << 4) | (blu >> 4);
            }
        }
    }
    _TIFFfree(scanline_buffer);
    TIFFClose(tif);
    if (y != height) {
        BrPixelmapFree(pm);
        *pError_code = 4;
        return NULL;
    }
    return pm;
}

br_pixelmap* C2_HOOK_FASTCALL LoadTiffUsingPalette(const char* pPath, br_pixelmap* pPalette, int pFlags, int* pError_code) {
    TIFF* tif;
    br_pixelmap* pm;
    br_uint_32 height;
    br_uint_32 width;
    br_uint_16 samples_per_pixel;
    br_uint_8* scanline_buffer;
    br_uint_32 x;
    br_uint_32 y;
    br_uint_8 red;
    br_uint_8 grn;
    br_uint_8 blu;
    br_uint_8 alp;

    tif = TIFFOpen(pPath, "r");
    if (tif == NULL) {
        *pError_code = 4;
        return NULL;
    }
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
    pm = BrPixelmapAllocate(BR_PMT_INDEX_8, width, height, NULL, 0);
    if (pm == NULL) {
        TIFFClose(tif);
        *pError_code = 2;
        return NULL;
    }
    scanline_buffer = _TIFFmalloc(TIFFScanlineSize(tif));
    if (scanline_buffer == NULL) {
        TIFFClose(tif);
        BrPixelmapFree(pm);
        *pError_code = 2;
        return NULL;
    }
    for (y = 0; y < height && TIFFReadScanline(tif, scanline_buffer, y, 0) > 0; y++) {
        if (samples_per_pixel == 3) {
            for (x = 0; x < width; x++) {
                red = scanline_buffer[3 * x + 0];
                grn = scanline_buffer[3 * x + 1];
                blu = scanline_buffer[3 * x + 2];
                *(((br_uint_8*)pm->pixels) + y * pm->row_bytes + x) = FindPaletteColour(BR_COLOUR_RGB(red, grn, blu), pPalette);
            }
        } else {
            for (x = 0; x < width; x++) {
                red = scanline_buffer[3 * x + 0];
                grn = scanline_buffer[3 * x + 1];
                blu = scanline_buffer[3 * x + 2];
                alp = scanline_buffer[3 * x + 3];
                if (alp >= 0x80) {
                    *(((br_uint_8 *) pm->pixels) + y * pm->row_bytes + x) = FindPaletteColour(BR_COLOUR_RGB(red, grn, blu), pPalette);
                } else {
                    *(((br_uint_8 *) pm->pixels) + y * pm->row_bytes + x) = 0;
                }
            }
        }
    }
    _TIFFfree(scanline_buffer);
    TIFFClose(tif);
    if (y != height) {
        BrPixelmapFree(pm);
        *pError_code = 2;
        return NULL;
    }
    return pm;
}

// FUNCTION: CARMA2_HW 0x00486240
int C2_HOOK_FASTCALL DRstrrcasestr(int* pOffset, const char* pHaystack, int pLength_haystack, const char* pNeedle) {
    int len_needle;
    int pos_haystack;

    len_needle = strlen(pNeedle);
    (void)strlen(pHaystack);
    if (pLength_haystack < len_needle) {
        return 0;
    } else {
        pos_haystack = pLength_haystack - len_needle;
        while (pos_haystack >= 0 && PDmemicmp(&pHaystack[pos_haystack], pNeedle, len_needle) != 0) {
            pos_haystack -= 1;
        }
        if (pos_haystack < 0) {
            return 0;
        }
        *pOffset = pos_haystack;
        return 1;
    }
}

int BuildPathWithDirAndSuffix(char* pDest_dir_path, char* pDest_file_path, const char* pOld_path, const char* pNew_dirname, const char *pOld_suffix, const char* pNew_suffix) {
    int pos_suffix;
    int pos_filename_sep;
    int pos_dir_sep;
    int pos_endsep;

    if (DRstrrcasestr(&pos_suffix, pOld_path, strlen(pOld_path), pOld_suffix)
            && DRstrrcasestr(&pos_filename_sep, pOld_path, pos_suffix, gDir_separator)
            && DRstrrcasestr(&pos_dir_sep, pOld_path, pos_filename_sep, gDir_separator)) {
        pos_endsep = pos_filename_sep + strlen(gDir_separator);
        sprintf(pDest_dir_path, "%.*s%s%s", pos_dir_sep, pOld_path, gDir_separator, pNew_dirname);
        sprintf(pDest_file_path, "%s%s%.*s%s", pDest_dir_path, gDir_separator, pos_suffix - pos_endsep, pOld_path + pos_endsep, pNew_suffix);
        return 1;
    } else {
        return 0;
    }
}

br_uint_32 C2_HOOK_FASTCALL RGB565Error(br_colour pColour, br_uint_16 pRGB565) {

    return REC2_SQR(((pColour >> 16) & 0xff) - ((pRGB565 >> 8) & 0xf8))
            + REC2_SQR(((pColour >> 8) & 0xff) - ((pRGB565 >> 3) & 0xfc))
            + REC2_SQR(((pColour >> 0) & 0xff) - ((pRGB565 & 0x1f) << 3));
}

int C2_HOOK_FASTCALL FindPaletteColourRGB565(br_colour pColour, br_pixelmap *pPalette) {
    br_uint_16 *pixel;
    int best_index;
    br_uint_32 best_error;
    br_uint_32 error;
    int i;

    pixel = pPalette->pixels;
    best_index = 1;
    best_error = RGB565Error(pColour, pixel[1]);
    for (i = 2; i < 256; i++) {

        error = RGB565Error(pColour, pixel[i]);
        if (error < best_error) {
            best_index = i;
            best_error = error;
        }
    }
    return best_index;
}

br_uint_32 C2_HOOK_FASTCALL XRGB8888Error(br_colour pColour, br_uint_32 pXRGB8888) {

    return REC2_SQR(((pColour >> 16) & 0xff) - ((pXRGB8888 >> 16)  & 0xff))
            + REC2_SQR(((pColour >> 8) & 0xff) - ((pXRGB8888 >> 8) & 0xff))
            + REC2_SQR(((pColour >> 0) & 0xff) - ((pXRGB8888 >> 0) & 0xff));
}

int C2_HOOK_FASTCALL FindPaletteColourXRGB8888(br_colour pColour, br_pixelmap *pPalette) {
    br_uint_32 *pixel;
    int best_index;
    br_uint_32 best_error;
    br_uint_32 error;
    int i;

    pixel = pPalette->pixels;
    best_index = 1;
    best_error = XRGB8888Error(pColour, pixel[1]);
    for (i = 2; i < 256; i++) {

        error = XRGB8888Error(pColour, pixel[i]);
        if (error < best_error) {
            best_index = i;
            best_error = error;
        }
    }
    return best_index;
}

// FUNCTION: CARMA2_HW 0x004862b0
int C2_HOOK_FASTCALL FindPaletteColour(br_colour pColour, br_pixelmap *pPalette) {

    if (pPalette->type == BR_PMT_RGB_565) {
        return FindPaletteColourRGB565(pColour, pPalette);
    } else {
        return FindPaletteColourXRGB8888(pColour, pPalette);
    }
}
