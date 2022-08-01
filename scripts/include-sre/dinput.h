#ifndef SRE_DINPUT_H
#define SRE_DINPUT_H

#ifndef _SRE_
#error "This file is only meant for SRE"
#endif

#include <windows.h>

typedef struct DIPROPHEADER {
    DWORD	dwSize;
    DWORD	dwHeaderSize;
    DWORD	dwObj;
    DWORD	dwHow;
} DIPROPHEADER,*LPDIPROPHEADER;
typedef const DIPROPHEADER *LPCDIPROPHEADER;

typedef struct _DIOBJECTDATAFORMAT {
    const GUID *pguid;
    DWORD	dwOfs;
    DWORD	dwType;
    DWORD	dwFlags;
} DIOBJECTDATAFORMAT, *LPDIOBJECTDATAFORMAT;
typedef const DIOBJECTDATAFORMAT *LPCDIOBJECTDATAFORMAT;

typedef struct _DIDATAFORMAT {
    DWORD			dwSize;
    DWORD			dwObjSize;
    DWORD			dwFlags;
    DWORD			dwDataSize;
    DWORD			dwNumObjs;
    LPDIOBJECTDATAFORMAT	rgodf;
} DIDATAFORMAT, *LPDIDATAFORMAT;
typedef const DIDATAFORMAT *LPCDIDATAFORMAT;

typedef struct DIDEVICEOBJECTINSTANCEA {
    DWORD	dwSize;
    GUID	guidType;
    DWORD	dwOfs;
    DWORD	dwType;
    DWORD	dwFlags;
    CHAR	tszName[MAX_PATH];
} DIDEVICEOBJECTINSTANCEA, *LPDIDEVICEOBJECTINSTANCEA;
typedef const DIDEVICEOBJECTINSTANCEA *LPCDIDEVICEOBJECTINSTANCEA;

typedef struct DIDEVCAPS {
    DWORD	dwSize;
    DWORD	dwFlags;
    DWORD	dwDevType;
    DWORD	dwAxes;
    DWORD	dwButtons;
    DWORD	dwPOVs;
} DIDEVCAPS,*LPDIDEVCAPS;

typedef struct DIDEVICEOBJECTDATA {
    DWORD	dwOfs;
    DWORD	dwData;
    DWORD	dwTimeStamp;
    DWORD	dwSequence;
} DIDEVICEOBJECTDATA, *LPDIDEVICEOBJECTDATA;
typedef const DIDEVICEOBJECTDATA *LPCDIDEVICEOBJECTDATA;

typedef struct DIDEVICEINSTANCEA {
    DWORD	dwSize;
    GUID	guidInstance;
    GUID	guidProduct;
    DWORD	dwDevType;
    CHAR	tszInstanceName[MAX_PATH];
    CHAR	tszProductName[MAX_PATH];
} DIDEVICEINSTANCEA, *LPDIDEVICEINSTANCEA;
typedef const DIDEVICEINSTANCEA *LPCDIDEVICEINSTANCEA;

typedef WINBOOL (__stdcall *LPDIENUMDEVICEOBJECTSCALLBACKA)(LPCDIDEVICEOBJECTINSTANCEA,LPVOID);
typedef WINBOOL (__stdcall *LPDIENUMDEVICESCALLBACKA)(LPCDIDEVICEINSTANCEA,LPVOID);

typedef struct IDirectInputDeviceA_vtable  {
    // IUnknown methods
    HRESULT (__stdcall * QueryInterface)(struct IDirectInputDeviceA*, REFIID riid, void** ppvObject);
    ULONG (__stdcall * AddRef)(struct IDirectInputDeviceA* pThis);
    ULONG (__stdcall * Release)(struct IDirectInputDeviceA* pThis);
    // IDirectInputDeviceA methods
    HRESULT (__stdcall * GetCapabilities)(struct IDirectInputDeviceA* pThis, LPDIDEVCAPS lpDIDevCaps);
    HRESULT (__stdcall * EnumObjects)(struct IDirectInputDeviceA* pThis, LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags);
    HRESULT (__stdcall * GetProperty)(struct IDirectInputDeviceA* pThis, REFGUID rguidProp, LPDIPROPHEADER pdiph);
    HRESULT (__stdcall * SetProperty)(struct IDirectInputDeviceA* pThis, REFGUID rguidProp, LPCDIPROPHEADER pdiph);
    HRESULT (__stdcall * Acquire)(struct IDirectInputDeviceA* pThis);
    HRESULT (__stdcall * Unacquire)(struct IDirectInputDeviceA* pThis);
    HRESULT (__stdcall * GetDeviceState)(struct IDirectInputDeviceA* pThis, DWORD cbData, LPVOID lpvData);
    HRESULT (__stdcall * GetDeviceData)(struct IDirectInputDeviceA* pThis, DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags);
    HRESULT (__stdcall * SetDataFormat)(struct IDirectInputDeviceA* pThis, LPCDIDATAFORMAT lpdf);
    HRESULT (__stdcall * SetEventNotification)(struct IDirectInputDeviceA* pThis, HANDLE hEvent);
    HRESULT (__stdcall * SetCooperativeLevel)(struct IDirectInputDeviceA* pThis, HWND hwnd, DWORD dwFlags);
    HRESULT (__stdcall * GetObjectInfo)(struct IDirectInputDeviceA* pThis, LPDIDEVICEOBJECTINSTANCEA pdidoi, DWORD dwObj, DWORD dwHow);
    HRESULT (__stdcall * GetDeviceInfo)(struct IDirectInputDeviceA* pThis, LPDIDEVICEINSTANCEA pdidi);
    HRESULT (__stdcall * RunControlPanel)(struct IDirectInputDeviceA* pThis, HWND hwndOwner, DWORD dwFlags);
    HRESULT (__stdcall * Initialize)(struct IDirectInputDeviceA* pThis, HINSTANCE hinst, DWORD dwVersion, REFGUID rguid);
};
typedef struct IDirectInputDeviceA  {
    struct IDirectInputDeviceA_vtable* vtbl;
} *LPDIRECTINPUTDEVICEA;

typedef struct IDirectInputA_vtable {
    //  IUnknown methods
    HRESULT (__stdcall * QueryInterface)(struct IDirectInputA, REFIID riid, void** ppvObject);
    ULONG (__stdcall * AddRef)(struct IDirectInputA* pThis);
    ULONG (__stdcall * Release)(struct IDirectInputA* pThis);
    // IDirectInputA methods
    HRESULT (__stdcall * CreateDevice)(struct IDirectInputA* pThis, REFGUID rguid, LPDIRECTINPUTDEVICEA *lplpDirectInputDevice, LPUNKNOWN pUnkOuter);
    HRESULT (__stdcall * EnumDevices)(struct IDirectInputA* pThis, DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags);
    HRESULT (__stdcall * GetDeviceStatus)(struct IDirectInputA* pThis, REFGUID rguidInstance);
    HRESULT (__stdcall * RunControlPanel)(struct IDirectInputA* pThis, HWND hwndOwner, DWORD dwFlags);
    HRESULT (__stdcall * Initialize)(struct IDirectInputA* pThis, HINSTANCE hinst, DWORD dwVersion);
};
typedef struct IDirectInputA {
    struct IDirectInputA_vtable* vtbl;
};
typedef struct IDirectInputA *LPDIRECTINPUTA;

#endif // SRE_DINPUT_H
