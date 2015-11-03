//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duUtility.h
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#pragma once

#ifdef _UNICODE
#include "tinyxmlW/tinyxml.h"
#else
#include "tinyxmlA/tinyxml.h"
#endif

#include <Windows.h>
#include "duTypes.h"
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <tchar.h>
#include <time.h>
#include <list>
#include <map>
#include <deque>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
using namespace std;


#ifdef _UNICODE
typedef wstring dustring;
#else
typedef	string dustring;
#endif

#include <GdiPlus.h>
using namespace Gdiplus;

class duPlugin;
class duFont;
class duCtrlManager;
class duResManager;
class duWindowManager;

//
// Safe delete macro
#define SAFE_DELETE(p) \
	if (NULL != p) \
{ \
	delete p; \
	p = NULL; \
}

#define SAFE_DELETE_ARRAY(p) \
	if (NULL != p) \
{ \
	delete [] p; \
	p = NULL; \
}

#define SAFE_DELETE_OBJECT(p) \
	if (NULL != p) \
{ \
	::DeleteObject(p); \
	p = NULL; \
}

#define SAFE_DELETE_PROPERTY(p) \
	if (NULL != p) \
{ \
	if ((p)->duPropertyName) delete [] (p)->duPropertyName; \
	if ( (p)->duVariant.vt == DU_PROPERTY_STRING ) \
{ \
	if (NULL != (p)->duVariant.ptcString ) delete [] (p)->duVariant.ptcString; \
	delete p; \
	p = NULL; \
} \
}


//
// Mouse message Point Macro
#define GET_X_LPARAM(lp)                ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                ((int)(short)HIWORD(lp))

//
// rectangle Macro
#define RECT_WIDTH(x)					((x).right-(x).left)
#define RECT_HEIGHT(x)					((x).bottom-(x).top)

enum ScrollBarAction
{
	SCROLL_NO_WHERE = 0,
	SCROLL_UP = 1,
	SCROLL_DOWN = 2,
	SCROLL_TRACK = 3,
	SCROLL_PAGEUP = 4,
	SCROLL_PAGEDOWN = 5,
	SCROLL_WHEEL_UP = 6,
	SCROLL_WHEEL_DOWN = 7
};

#define ATOMPLUGIN _T("SYSPLUGIN")

// 跟踪调试字符串
void _TRACE(const TCHAR* lpszFormat, ...);

// 样式矩形计算
void MakeRect(int left, int top, int right, int bottom, int width, int height, RECT *lpInRect, RECT *lpOutRect);

// 控件矩形计算
void MakeRectExt(int left, int top, int right, int bottom, int horzanchor, int vertanchor, int width, int height, RECT *lpInRect, RECT *lpOutRect);

// 初始化32位图片结构
BOOL InitBitmapInfo(BITMAPINFO *lpBitmapInfo, int nWidth, int nHeight);

// XML读写
dustring ReadXmlAttributeText(TiXmlElement *pEle, LPCTSTR keyName);
int ReadXmlAttributeText(TiXmlElement *pEle, LPCTSTR keyName, TCHAR *pVal, int maxlen);
int ReadXmlAttributeInt(TiXmlElement *pEle, LPCTSTR keyName);
float ReadXmlAttributeFloat(TiXmlElement *pEle, LPCTSTR keyName);
double ReadXmlAttributeDouble(TiXmlElement *pEle, LPCTSTR keyName);
COLORREF ReadXmlAttributeColor(TiXmlElement *pEle, LPCTSTR keyName);
void ReadXmlAttributeBOOL(TiXmlElement *pEle, LPCTSTR keyName, BOOL *pbResult);
void ReadXmlAttributeSize(TiXmlElement *pEle, LPCTSTR keyName, int *cx, int *cy);
UINT ReadXmlAttributeState(TiXmlElement *pEle, LPCTSTR keyName);
void ReadXmlAttributePoint(TiXmlElement *pEle, LPCTSTR keyName, POINT *pPoint);
void WriteXmlAttributeText(TiXmlElement *pEle, LPCTSTR keyName, LPCTSTR Val);
void WriteXmlAttributeInt(TiXmlElement *pEle, LPCTSTR keyName, int Val);
void WriteXmlAttributeBOOL(TiXmlElement *pEle, LPCTSTR keyName, BOOL Val);
void WriteXmlAttributeFloat(TiXmlElement *pEle, LPCTSTR keyName, float Val);
void WriteXmlAttributeDouble(TiXmlElement *pEle, LPCTSTR keyName, double Val);
void WriteXmlAttributeColor(TiXmlElement *pEle, LPCTSTR keyName, COLORREF clr);
void WriteXmlArributeState(TiXmlElement *pEle, LPCTSTR keyName, int nState);

// 获得临时文件名
dustring GetTempName();

// 变灰绘制样式
void GrayDrawByStyle(void *pStyleGroup, HDC hDC, RECT *dcRect, BYTE *pBits, RECT *ctrlRect, int nState, LPCTSTR pText);

// 是否为选中状态
BOOL IsStateChecked(int uState);

duCtrlManager *GetCtrlManager(HWND hWnd);

duResManager *GetResManager(HWND hWnd);

UINT GetUniTimer();

int DrawText32Bpp(HDC hDC, duFont *font, COLORREF clrText, LPCTSTR lpString, int nCount, LPRECT lpRect, UINT uFormat, int nAlpha);

void DrawTextPlus(HDC hDC, HBITMAP hCurBitmap, COLORREF clrText, HFONT hFont, LPCTSTR lpString, int nCount, BOOL fSingleLine,UINT uFormat);
