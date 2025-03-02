#include "smack.h"

#include <stddef.h>

unsigned int SMACK_STDCALL SmackUseMMX(unsigned int flag) {
    (void)flag;
    return 0;
}

int SMACK_STDCALL SmackSoundUseDirectSound(SMACK_HWND hWnd) {
    (void)hWnd;
    return 0;
}

HSMACK SMACK_STDCALL SmackOpen(const char *path, unsigned int uFlags, unsigned int uExtraBuffers) {
    (void)path;
    (void)uFlags;
    (void)uExtraBuffers;
    return NULL;
}
int SMACK_STDCALL SmackBufferOpen(SMACK_HWND a1, long a2, long a3, long a4, long a5, long a6) {
    (void)a1;
    (void)a2;
    (void)a3;
    (void)a4;
    (void)a5;
    (void)a6;
    return 0;
}

void SMACK_STDCALL SmackToBuffer(HSMACK hSmack, unsigned int uX, unsigned int uY, unsigned int uPitch, unsigned int uHeight, void *pBuffer, unsigned int uFlags) {
    (void)hSmack;
    (void)uX;
    (void)uY;
    (void)uPitch;
    (void)uHeight;
    (void)pBuffer;
    (void)uFlags;
}

unsigned int SMACK_STDCALL SmackDoFrame(HSMACK hSmack) {
    (void)hSmack;
    return 0;
}

void SMACK_STDCALL SmackNextFrame(HSMACK hSmack) {
    (void)hSmack;
}

HSMACKBLIT SMACK_STDCALL SmackBlitOpen(unsigned int uSurfaceFormat) {
    return NULL;
}

void SMACK_STDCALL SmackBlitSetPalette(HSMACKBLIT hBlit, void *pPalette, unsigned int uPalType) {
    (void)hBlit;
    (void)pPalette;
    (void)uPalType;
}

void SMACK_STDCALL SmackBlit(HSMACKBLIT hBlit, void *pDest, unsigned int uDestPitch, unsigned int uDestX, unsigned int uDestY, void *pSrc, unsigned int uSrcPitch, unsigned int uSrcX, unsigned int uSrcY, unsigned int uSrcZ, unsigned int uSrcW) {
    (void)hBlit;
    (void)pDest;
    (void)uDestPitch;
    (void)uDestX;
    (void)uDestY;
    (void)pSrc;
    (void)uSrcPitch;
    (void)uSrcX;
    (void)uSrcY;
    (void)uSrcZ;
    (void)uSrcW;
}

unsigned int SMACK_STDCALL SmackWait(HSMACK hSmack) {
    (void)hSmack;
    return 0;
}

void SMACK_STDCALL SmackBlitClose(HSMACKBLIT hBlit) {
    (void)hBlit;
}

void SMACK_STDCALL SmackClose(HSMACK hSmack) {
    (void)hSmack;
}
