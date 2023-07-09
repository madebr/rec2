#ifndef PASSWORD_H
#define PASSWORD_H

#include <windows.h>

extern int g_parental_lock;

int AskPassword(HWND hWnd);

#endif /* PASSWORD_H */
