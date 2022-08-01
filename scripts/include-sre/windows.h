#ifndef SRE_WINDOWS_H
#define SRE_WINDOWS_H

#ifndef _SRE_
#error "This file is only meant for SRE"
#endif

typedef unsigned char byte;
typedef char CHAR;
typedef long LONG;
typedef unsigned long ULONG;
typedef unsigned DWORD;
typedef void* LPVOID;
typedef DWORD* LPDWORD;
typedef const char* LPCSTR;

typedef int WINBOOL;

typedef LONG HRESULT;

typedef void* HANDLE;
typedef void* HINSTANCE;
typedef void* HWND;

typedef struct {
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    byte Data4[8];
} GUID;

typedef GUID IID;
typedef GUID *REFGUID;
typedef GUID *LPGUID;
typedef IID *REFIID;

typedef struct IUnknown IUnknown;
typedef IUnknown *LPUNKNOWN;

#endif // SRE_WINDOWS_H
