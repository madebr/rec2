#ifndef REC2_SDL3_INPUT_H
#define REC2_SDL3_INPUT_H

#include "rec2_types.h"

extern int gASCII_table[151];
extern int gASCII_shift_table[151];

extern void Win32InitInputDevice(void);

extern void KeyBegin(void);

extern void CloseDirectInput(void);

extern void PDSetKeyArray(int *pKeys, int pMark);

extern void PDReadJoysticks(void);

extern int GetDirectInputJoy1X(void);

extern int GetDirectInputJoy1Y(void);

extern tU32 PDGetJoy1Button1(void);

extern tU32 PDGetJoy1Button2(void);

extern tU32 PDGetJoy1Button3(void);

extern tU32 PDGetJoy1Button4(void);

extern int PDGetJoy1X(void);

extern int PDGetJoy1Y(void);

extern tU32 PDGetJoy2Button1(void);

extern tU32 PDGetJoy2Button2(void);

extern tU32 PDGetJoy2Button3(void);

extern tU32 PDGetJoy2Button4(void);

extern int PDGetJoy2X(void);

extern int PDGetJoy2Y(void);

extern void PDInitJoysticks(void);

#endif /* REC2_SDL3_INPUT_H */
