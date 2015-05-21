#pragma once

#include <windows.h>

HRESULT WINAPI CreateAxControl(HWND hWnd,const wchar_t * ProgId,IUnknown ** ppControlUnknown,IUnknown ** ppContainerUnknown);

