// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4018)

#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS
#define TIXML_USE_STL

#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <gdiplus.h>
#include <imm.h>

#pragma comment(lib, "Gdiplus.lib")
#pragma comment(lib, "imm32.lib")