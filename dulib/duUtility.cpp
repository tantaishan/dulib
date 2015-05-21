//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duUtility.cpp
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duUtility.h"
#include "duPlugin.h"
#include "duResBase.h"
#include "duCtrlManager.h"
#include "duWindowManager.h"
#include "duResManager.h"
#include "duFont.h"

int g_nTraceCount = 0;
//extern map<duPlugin *, duCtrlManager *> *g_mapValid;


void _TRACE(LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);

	int nBuf;
	TCHAR szBuffer[512];

	nBuf = _vsntprintf(szBuffer, _tcslen(szBuffer), lpszFormat, args);

	TCHAR szOutString[1024];
	_stprintf(szOutString, _T("[%d]%s"), g_nTraceCount++, szBuffer);

	OutputDebugString(szOutString);

	va_end(args);
}

UINT WINAPI GetUniTimer()
{
	static UINT g_nTimer = 0x0F00;
	return g_nTimer++;
}

void MakeRect(int left, int top, int right, int bottom, int width, int height, RECT *lpInRect, RECT *lpOutRect)
{
	if (!lpInRect || !lpOutRect) 
		return;

	if (left < 0) 
	{
		lpOutRect->left = lpInRect->right + left;
		lpOutRect->right = lpOutRect->left + width;
	} 
	else 
	{
		lpOutRect->left = lpInRect->left + left;
	}

	if (top < 0)
	{
		lpOutRect->top = lpInRect->bottom + top;
		lpInRect->bottom = lpOutRect->top + height;
	} 
	else 
	{
		lpOutRect->top = lpInRect->top + top;
	}

	if (right < 0)
		lpOutRect->right = lpInRect->right + right;
	else
		lpOutRect->right = lpInRect->right + right;

	if (bottom < 0) 
		lpOutRect->bottom = lpInRect->bottom + bottom;
	else
		lpOutRect->bottom = lpInRect->bottom + bottom;
	
	if (width && RECT_WIDTH(*lpOutRect) != width) 
		lpOutRect->right = lpOutRect->left + width;

	if (height && RECT_HEIGHT(*lpOutRect) != height) 
		lpOutRect->bottom = lpOutRect->top + height;
	
	if (RECT_WIDTH(*lpOutRect) == 0 && RECT_HEIGHT(*lpOutRect) == 0) 
		CopyRect(lpOutRect, lpInRect);
}

void MakeRectExt(int l, int t, int r, int b, int horzanchor, int vertanchor, int width, int height, RECT *lpInRect, RECT *lpOutRect)
{
	if (!lpInRect || !lpOutRect)
		return;

	duRect rect = *lpInRect;
	if (horzanchor == 3)
	{
		lpOutRect->left = rect.left + l;
		lpOutRect->right = lpOutRect->left + width;
		goto Rect_Vert;
	}
	
	if (l == DU_HCENTER) 
	{
		if (horzanchor == 0) 
		{
			lpOutRect->left = rect.left + (rect.Width()-width) / 2;
			lpOutRect->right = lpOutRect->left + width;
		} 
		else if (horzanchor == 1) 
		{
			lpOutRect->left = rect.left + ( rect.Width() * (1-width/100) /2 );
			lpOutRect->right = lpOutRect->left + (rect.Width() * width/100);	
		} 
		else if (horzanchor == 2) 
		{
			lpOutRect->left = rect.left + r;
			if (r >= 0)
                lpOutRect->right = rect.right - r;
			else
				lpOutRect->right = rect.left - r;
		}
	}
	else if (l < 0) 
	{
		if (horzanchor == 0) 
		{
			lpOutRect->left = rect.right + l;
			lpOutRect->right = lpOutRect->left + width;
		} 
		else if (horzanchor == 1) 
		{
			lpOutRect->left = rect.right + l;
			lpOutRect->right = lpOutRect->left + rect.Width() * width/100;
		} 
		else if (horzanchor == 2) 
		{
			lpOutRect->left = rect.right + l;
			if (r >= 0)
				lpOutRect->right = rect.right - r;
			else
				lpOutRect->right = rect.left - r;
		}
	}
	else if (l >= 0 ) 
	{
		if (horzanchor == 0) 
		{
			lpOutRect->left = rect.left + l;
			lpOutRect->right = lpOutRect->left + width;
		}
		else if (horzanchor == 1) 
		{
			lpOutRect->left = rect.left + l;
			lpOutRect->right = lpOutRect->left + rect.Width() * width/100;
		} 
		else if (horzanchor == 2) 
		{
			lpOutRect->left = rect.left + l;
			if (r >= 0)
				lpOutRect->right = rect.right - r;
			else
				lpOutRect->right = rect.left - r;
		}
	}

Rect_Vert:
	if (vertanchor == 3)
	{
		lpOutRect->top = rect.top + t;
		lpOutRect->bottom = lpOutRect->top + height;
		return;
	}

	if (t == DU_VCENTER) 
	{
		if (vertanchor == 0) 
		{
			lpOutRect->top = rect.top + (rect.Height() - height) / 2;
			lpOutRect->bottom = lpOutRect->top + height;
		} 
		else if (vertanchor == 1) 
		{
			lpOutRect->top = rect.top + (rect.Height() * ( 1-height/100) ) /2;
			lpOutRect->bottom = lpOutRect->top + rect.Height() * height/100;
		} 
		else if (vertanchor == 2) 
		{
			lpOutRect->top = rect.top + b;
			if (b >= 0)
				lpOutRect->bottom = rect.bottom - b;
			else
				lpOutRect->bottom = rect.top - b;
		}
	}
	else if (t < 0) 
	{
		if (vertanchor == 0) 
		{
			lpOutRect->top = rect.bottom + t;
			lpOutRect->bottom = lpOutRect->top + height;
		} 
		else if (vertanchor == 1) 
		{
			lpOutRect->top = rect.bottom + t;
			lpOutRect->bottom = lpOutRect->top + rect.Height() * height/100;
		} 
		else if (vertanchor == 2) 
		{
			lpOutRect->top = rect.bottom + t;
			if (b >= 0)
				lpOutRect->bottom = rect.bottom - b;
			else
				lpOutRect->bottom = rect.top - b;
		}
	}
	else if (t >= 0) 
	{
		if (vertanchor == 0) 
		{
			lpOutRect->top = rect.top + t;
			lpOutRect->bottom = lpOutRect->top + height;
		}
		else if (vertanchor == 1)
		{
			lpOutRect->top = rect.top + t;
			lpOutRect->bottom = lpOutRect->top + rect.Height() * height/100;
		} 
		else if (vertanchor == 2) 
		{
			lpOutRect->top = rect.top + t;
			if (b >= 0)
				lpOutRect->bottom = rect.bottom - b;
			else
				lpOutRect->bottom = rect.top - b;
		}
	}
}


BOOL WINAPI InitBitmapInfo(BITMAPINFO *lpBitmapInfo, int nWidth, int nHeight)
{
	if (lpBitmapInfo == NULL || nWidth <= 0 || nHeight <= 0)
		return FALSE;

	ZeroMemory(&lpBitmapInfo->bmiHeader, sizeof(BITMAPINFOHEADER));

	lpBitmapInfo->bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
	lpBitmapInfo->bmiHeader.biWidth    = nWidth;
	lpBitmapInfo->bmiHeader.biHeight   = nHeight * -1;
	lpBitmapInfo->bmiHeader.biPlanes   = 1;
	lpBitmapInfo->bmiHeader.biBitCount = 32;

	return TRUE;
}

dustring ReadXmlAttributeText(TiXmlElement *pEle, LPCTSTR keyName)
{
	dustring temp = _T("");

	if (pEle)
	{
		LPCTSTR p = NULL;
		p = pEle->Attribute(keyName);
		if (p)
		{
#ifdef _UNICODE
			temp = p;
#else
			int len = (int)strlen(p);
			char *pAscii = new char[len + 10];
			wchar_t *pWide = new wchar_t[len + 10];
			memset(pAscii, 0, len + 10);
			memset(pWide, 0, (len + 10) * 2);
			::MultiByteToWideChar(CP_UTF8, NULL, p, -1, pWide, (len + 10) * 2);
			::WideCharToMultiByte(CP_ACP, NULL, pWide, -1, pAscii, len + 10, 0, 0);

			temp = pAscii;

			SAFE_DELETE_ARRAY(pAscii);
			SAFE_DELETE_ARRAY(pWide);
#endif
		}
	}
	return temp;
}

int ReadXmlAttributeText(TiXmlElement *pEle, LPCTSTR keyName, TCHAR *pVal, int maxlen)
{
	if (pEle == NULL || pVal == NULL || maxlen == 0)
		return 0;

	int len = 0;
	LPCTSTR p = NULL;
	p = pEle->Attribute(keyName);
	if (p)
	{
#ifdef _UNICODE
		memset(pVal, 0, maxlen * sizeof(TCHAR));
		len = (int)wcslen(p);
		wcsncpy(pVal, p, min(maxlen, len));
#else
		memset(pVal, 0, maxlen * sizeof(TCHAR));
		len = (int)strlen(p);
		char *pAscii = new char[len + 10];
		wchar_t *pWide = new wchar_t[len + 10];
		memset(pAscii, 0, len + 10);
		memset(pWide, 0, (len + 10)*2);
		::MultiByteToWideChar(CP_UTF8, NULL, p, -1, pWide, (len + 10)*2);
		::WideCharToMultiByte(CP_ACP, NULL, pWide, -1, pAscii, len + 10, 0, 0);

		strncpy(pVal, pAscii, min(maxlen, len));

		SAFE_DELETE_ARRAY(pAscii);
		SAFE_DELETE_ARRAY(pWide);
#endif
	}
	return len;
}

int ReadXmlAttributeInt(TiXmlElement *pEle, LPCTSTR keyName)
{
	if (pEle == NULL)
		return 0;

	int i = 0;
	LPCTSTR p = NULL;

	p = pEle->Attribute(keyName);
	if (p)
		i = _tstoi(p);

	return i;
}

float ReadXmlAttributeFloat(TiXmlElement *pEle, LPCTSTR keyName)
{
	if (pEle == NULL) 
		return 0;

	float f = 0;
	LPCTSTR p = NULL;

	p = pEle->Attribute(keyName);
	if (p)
		f = (float)_tstof(p);

	return f;
}

double ReadXmlAttributeDouble(TiXmlElement *pEle, LPCTSTR keyName)
{
	if (pEle == NULL) 
		return 0;

	double d = 0;
	LPCTSTR p = NULL;

	p = pEle->Attribute(keyName);
	if (p)
		d = _tstof(p);

	return d;
}

COLORREF ReadXmlAttributeColor(TiXmlElement *pEle, LPCTSTR keyName)
{
	if (pEle == NULL) 
		return 0;

	int r = 255;
	int g = 255;
	int b = 255;
	LPCTSTR p = NULL;

	p = pEle->Attribute(keyName);
	if (p)
		_stscanf(p, _T("RGB(%d,%d,%d)"), &r, &g, &b);

	return RGB(r,g,b);
}

void ReadXmlAttributeBOOL(TiXmlElement *pEle, LPCTSTR keyName, BOOL *pbResult)
{
	if (pEle == NULL || pbResult == NULL) 
		return;

	LPCTSTR p = NULL;

	p = pEle->Attribute(keyName);
	if (p)
	{
		if (_tcsicmp(p, _T("true")) == 0)
			*pbResult = TRUE;
		else if (_tcsicmp(p, _T("false")) == 0)
			*pbResult = FALSE;
	}
}

UINT ReadXmlAttributeState(TiXmlElement *pEle, LPCTSTR keyName)
{
	if (pEle == NULL)
		return DU_STATE_NORMAL;

	LPCTSTR p = NULL;
	p = pEle->Attribute(keyName);
	if (p)
	{
		if (_tcsicmp(p, _T("normal")) == 0)
			return DU_STATE_NORMAL;
		else if (_tcsicmp(p, _T("over")) == 0)
			return DU_STATE_OVER;
		else if (_tcsicmp(p, _T("press")) == 0)
			return DU_STATE_PRESS;
		else if (_tcsicmp(p, _T("disable")) == 0)
			return DU_STATE_DISABLE;
		else if (_tcsicmp(p, _T("focus")) == 0)
			return DU_STATE_FOCUS; 
		else if (_tcsicmp(p, _T("normalall")) == 0)
			return DU_STATE_NORMAL_ALL; 
		else if (_tcsicmp(p, _T("checked")) == 0)
			return DU_STATE_CHECKED;
		else if (_tcsicmp(p, _T("checkedover")) == 0)
			return DU_STATE_CHECKEDOVER;
		else if (_tcsicmp(p, _T("checkedpress")) == 0)
			return DU_STATE_CHECKEDPRESS;
		else if (_tcsicmp(p, _T("checkeddisable")) == 0)
			return DU_STATE_CHECKEDDISABLE;
		else if (_tcsicmp(p, _T("checkedfocus")) == 0)
			return DU_STATE_CHECKEDFOCUS;
		else if (_tcsicmp(p, _T("checkedall")) == 0)
			return DU_STATE_CHECKED_ALL;
		else if (_tcsicmp(p, _T("all")) == 0)
			return DU_STATE_ALL;
	}
	return DU_STATE_NORMAL;
}

void ReadXmlAttributePoint(TiXmlElement *pEle, LPCTSTR keyName, POINT *pPoint)
{
	if (pEle == NULL || pPoint == NULL)
		return;
	
	int x, y;
	x = y = 0;
	LPCTSTR p = pEle->Attribute(keyName);
	if (p)
		_stscanf(p, _T("(%d,%d)"), &x, &y);

	pPoint->x = x;
	pPoint->y = y;
}

void WriteXmlAttributeText(TiXmlElement *pEle, LPCTSTR keyName, LPCTSTR Val)
{
	if (!pEle || !keyName || !*keyName) 
		return;

	if (!Val || !*Val)
	{
		pEle->SetAttribute(keyName, _T(""));
		return;
	}

#ifdef _UNICODE
	pEle->SetAttribute(keyName, Val);

#else
	int len = (int)strlen(Val);
	char *pUtf8 = new char[len + 10];
	wchar_t *pWide = new wchar_t[len + 10];

	memset(pUtf8, 0, len + 10);
	memset(pWide, 0, (len + 10) * 2);

	::MultiByteToWideChar(CP_ACP, NULL, Val, -1, pWide, (len + 10) * 2);
	::WideCharToMultiByte(CP_UTF8, NULL, pWide, -1, pUtf8, len + 10, 0, 0);

	pEle->SetAttribute(keyName, pUtf8);

	SAFE_DELETE_ARRAY(pUtf8);
	SAFE_DELETE_ARRAY(pWide);
#endif
}

void WriteXmlAttributeInt(TiXmlElement *pEle, LPCTSTR keyName, int Val)
{
	if (!pEle || !keyName || !*keyName)
		return;

	if (Val != 0)
		pEle->SetAttribute(keyName, Val);
}

void WriteXmlAttributeBOOL(TiXmlElement *pEle, LPCTSTR keyName, BOOL Val)
{
	if (!pEle || !keyName || !*keyName) 
		return;

	if (Val)
		pEle->SetAttribute(keyName, _T("true"));
	else
		pEle->SetAttribute(keyName, _T("false")); 
}

void WriteXmlAttributeFloat(TiXmlElement *pEle, LPCTSTR keyName, float Val)
{
	if (!pEle || !keyName || !*keyName) 
		return;

	TCHAR buf[64] = {0};
	_stprintf(buf, _T("%f"), Val);
	pEle->SetAttribute(keyName, buf);
}

void WriteXmlAttributeDouble(TiXmlElement *pEle, LPCTSTR keyName, double Val)
{
	if (!pEle || !keyName || !*keyName) 
		return;

	TCHAR buf[64] = {0};
	_stprintf(buf, _T("%f"), Val);
	pEle->SetAttribute(keyName, buf);
}

void WriteXmlAttributeColor(TiXmlElement *pEle, LPCTSTR keyName, COLORREF Val)
{
	if (!pEle || !keyName || !*keyName)
		return;

	long r = GetRValue(Val);
	long g = GetGValue(Val);
	long b = GetBValue(Val);
	TCHAR buffer[MAX_NAME];
	_stprintf(buffer, _T("RGB(%d,%d,%d)"), r, g, b);

	pEle->SetAttribute(keyName, buffer);
}

void WriteXmlArributeState(TiXmlElement *pEle, LPCTSTR keyName, int nState)
{
	if (!pEle || !keyName)
		return;

	TCHAR buffer[MAX_NAME];
	ZeroMemory(buffer, MAX_NAME * sizeof(TCHAR));

	if (nState == DU_STATE_NORMAL)
		_tcscpy(buffer, _T("normal"));
	else if (nState == DU_STATE_OVER)
		_tcscpy(buffer, _T("over"));
	else if (nState == DU_STATE_PRESS)
		_tcscpy(buffer, _T("press"));
	else if (nState == DU_STATE_FOCUS)
		_tcscpy(buffer, _T("focus"));
	else if (nState == DU_STATE_DISABLE)
		_tcscpy(buffer, _T("disable"));
	else if (nState == DU_STATE_NORMAL_ALL)
		_tcscpy(buffer, _T("normalall"));
	else if (nState == DU_STATE_CHECKED)
		_tcscpy(buffer, _T("checked"));
	else if (nState == DU_STATE_CHECKEDOVER)
		_tcscpy(buffer, _T("checkedover"));
	else if (nState == DU_STATE_CHECKEDPRESS)
		_tcscpy(buffer, _T("checkedpress"));
	else if (nState == DU_STATE_CHECKEDFOCUS)
		_tcscpy(buffer, _T("checkedfocus"));
	else if (nState == DU_STATE_CHECKEDDISABLE)
		_tcscpy(buffer, _T("checkeddisable"));
	else if (nState == DU_STATE_CHECKED_ALL)
		_tcscpy(buffer, _T("checkedall"));
	else if (nState == DU_STATE_ALL)
		_tcscpy(buffer, _T("all"));

	pEle->SetAttribute(keyName, buffer);
}

BOOL IsStateChecked(int uState)
{
	if (IS_STATE_CHECKED(uState))
		return TRUE;

	return FALSE;
}

duCtrlManager *WINAPI GetCtrlManager(HWND hWnd)
{
	return (duCtrlManager *)::GetProp(hWnd, ATOMPLUGIN);
}

duResManager *WINAPI GetResManager(HWND hWnd)
{	
	duCtrlManager *pCtrlManager = GetCtrlManager(hWnd);
	if (pCtrlManager == NULL)
		return NULL;
	
	duWindowManager *pWindowManager = pCtrlManager->GetWindowManager();
	if (pWindowManager == NULL)
		return NULL;
	
	return pWindowManager->GetResManager();
}

void CopyPixel(RGBQUAD *pBits, int nBitmapWidth, int nBitmapHeight, HDC hDC, int x, int y);

int WINAPI DrawText32Bpp(HDC hDC, duFont *font, COLORREF clrText, LPCTSTR lpString, int nCount, LPRECT lpRect, UINT uFormat, int nAlpha)
{
	if (hDC == NULL || lpString == NULL || nCount <= 0 || lpRect == NULL)
		return 0;
	
	duRect rcClipBox;
	::GetClipBox(hDC, &rcClipBox);
	
	duRect rcInter;
	if (!::IntersectRect(&rcInter, &rcClipBox, lpRect))
		return 0;
	
	duRect rectDest(lpRect);
	HFONT hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	if (font && font->GetHFont())
		hFont = font->GetHFont();

	HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);
	duRect rcText;
	rcText.SetRectEmpty();
	if (uFormat & DT_SINGLELINE)
		::DrawText(hDC, lpString, nCount, &rcText, DT_TOP|DT_LEFT|DT_CALCRECT|DT_SINGLELINE);
	else
	{
		rcText.CopyRect(lpRect);
		::DrawText(hDC, lpString, nCount, &rcText, DT_WORDBREAK|DT_TOP|DT_LEFT|DT_CALCRECT);
	}
	::SelectObject(hDC, hOldFont);
	
	int nBitmapWidth = min(rectDest.Width(), rcText.Width());
	int nBitmapHeight = min(rectDest.Height(), rcText.Height());

	int x = rectDest.left;
	int y = rectDest.top;
	if (uFormat & DT_CENTER)
		x = rectDest.left + (rectDest.Width() - nBitmapWidth) / 2;
	else if (uFormat & DT_RIGHT)
		x = rectDest.right - nBitmapWidth;
		
	if (uFormat & DT_VCENTER)
		y = rectDest.top + (rectDest.Height() - nBitmapHeight) / 2;
	else if (uFormat & DT_BOTTOM)
		y = rectDest.bottom - nBitmapHeight;	
	
	HDC hMemDC = ::CreateCompatibleDC(NULL);
	BITMAPINFO bmInfo;
	InitBitmapInfo(&bmInfo, nBitmapWidth, nBitmapHeight);
	
	RGBQUAD *pBits = NULL;
	HBITMAP hBitmap = ::CreateDIBSection(hMemDC, &bmInfo, DIB_RGB_COLORS,(void**)&pBits, 0, 0);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);
	hOldFont = (HFONT)::SelectObject(hMemDC, hFont);
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	//::AlphaBlend(hMemDC, 0, 0, nBitmapWidth, nBitmapHeight, hDC, x, y, nBitmapWidth, nBitmapHeight, bf);

	CopyPixel(pBits, nBitmapWidth, nBitmapHeight, hDC, x, y);

	//RGBQUAD *pTempBits = new RGBQUAD[nBitmapWidth * nBitmapHeight];
	//memcpy(pTempBits, pBits, nBitmapWidth * nBitmapHeight * 4);
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//::SetBkMode(hMemDC, TRANSPARENT);
	//if (clrText == 0)
	//	clrText = RGB(1,1,1);
	//::SetTextColor(hMemDC, clrText);

	if (uFormat & DT_SINGLELINE)
		DrawTextPlus(hMemDC, hBitmap, clrText, hFont, lpString, nCount, TRUE ,uFormat);
	else
		DrawTextPlus(hMemDC, hBitmap, clrText, hFont, lpString, nCount, FALSE ,uFormat);
	
	//RGBQUAD *pTempPtr = pTempBits;
	//int nLoop = nBitmapWidth * nBitmapHeight;
	//int i;
	//for (i = 0;i < nLoop; i++)
	//{
	//	if (*(int *)pBits != *(int *)pTempPtr)
	//	{
	//		//if (pTempPtr->rgbReserved == 0)
	//			pBits->rgbReserved = 255;
	//		//else
	//		//	pBits->rgbReserved = pTempPtr->rgbReserved;
	//	}
	//	
	//	pBits++;
	//	pTempPtr++;
	//}
	//
	//delete [] pTempBits;

	TRIVERTEX vert[2] ;
	GRADIENT_RECT gRect;
	vert [0].x = x;
	vert [0].y = y;
	vert [0].Red = 0;
	vert [0].Green = 0;
	vert [0].Blue = 0;
	vert [0].Alpha = 0;
	
	vert [1].x = x + nBitmapWidth;
	vert [1].y = y + nBitmapHeight; 
	vert [1].Red = 0;
	vert [1].Green = 0;
	vert [1].Blue = 0;
	vert [1].Alpha = 0;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	::GradientFill(hDC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_H);
	::AlphaBlend(hDC, x, y, nBitmapWidth, nBitmapHeight, hMemDC, 0, 0, nBitmapWidth, nBitmapHeight, bf);
	
	::SelectObject(hMemDC, hOldFont);
	::SelectObject(hMemDC, hOldBitmap);
	::DeleteObject(hBitmap);
	::DeleteDC(hMemDC);

	return 1;
}

HBITMAP Split32Bitmap(HDC hDC, RGBQUAD *pBits32, int nWidth, int nHeight, BYTE *&pBits)
{
	if (pBits32 == NULL || nWidth <= 0 || nHeight <= 0)
	{
		return NULL;
	}

	BITMAPINFO bmInfo;
	ZeroMemory(&bmInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

	bmInfo.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth    = nWidth;
	bmInfo.bmiHeader.biHeight   = nHeight * -1;
	bmInfo.bmiHeader.biPlanes   = 1;
	bmInfo.bmiHeader.biBitCount = 24;

	HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmInfo, DIB_RGB_COLORS,(void**)&pBits, 0, 0);

	if (hBitmap == NULL || pBits == NULL)
		return NULL;

	RGBQUAD *pTmp32 = pBits32;
	BYTE *pTmp = NULL;
	
	int nEffWidth = ((((24 * nWidth) + 31) / 32) * 4);

	int x,y;
	for (y = 0;y < nHeight; y++)
	{
		pTmp = pBits + y * nEffWidth;
		for (x = 0;x < nWidth; x++)
		{
			*pTmp = pTmp32->rgbBlue;
			pTmp++;

			*pTmp = pTmp32->rgbGreen;
			pTmp++;

			*pTmp = pTmp32->rgbRed;
			pTmp++;

			pTmp32++;
		}
	}

	return hBitmap;
}

void Combo32Bitmap(RGBQUAD *pBits32, int nWidth, int nHeight, BYTE *pBits, BYTE *pAlpha)
{
	if (pBits32 == NULL || nWidth <= 0 || nHeight <= 0 || pBits == NULL)
		return;

	RGBQUAD *pTmp32 = pBits32;
	BYTE *pTmp = NULL;
	BYTE *pTmpAlpha = NULL;
	
	int nEffWidth = ((((24 * nWidth) + 31) / 32) * 4);

	int x,y;
	for (y = 0;y < nHeight; y++)
	{
		pTmp = pBits + y * nEffWidth;
		//pTmpAlpha = pAlpha + y * nEffWidth;

		for (x = 0;x < nWidth; x++)
		{
			pTmp32->rgbBlue = *pTmp;
			pTmp++;

			pTmp32->rgbGreen = *pTmp;
			pTmp++;

			pTmp32->rgbRed = *pTmp;
			pTmp++;

			//if (pTmpAlpha[0] != 255 && pTmpAlpha[1] != 255 && pTmpAlpha[2] != 255)
			//	pTmp32->rgbReserved = 255;

			pTmp32++;
			//pTmpAlpha += 3;
		}
	}
}

void WINAPI DrawTextPlus(HDC hDC, HBITMAP hCurBitmap, COLORREF clrText, HFONT hFont, LPCTSTR lpString, int nCount, BOOL fSingleLine ,UINT uFormat)
{
	BITMAP bitmap;
	ZeroMemory(&bitmap, sizeof(BITMAP));
	::GetObject(hCurBitmap, sizeof(BITMAP), &bitmap);

	int nWidth = bitmap.bmWidth;
	int nHeight = bitmap.bmHeight;

	HDC hMemDC = ::CreateCompatibleDC(NULL);
	BYTE *pBits = NULL;
	HBITMAP hBitmap = Split32Bitmap(hDC, (RGBQUAD *)bitmap.bmBits, nWidth, nHeight, pBits);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);
	HFONT hOldFont = (HFONT)::SelectObject(hMemDC, hFont);

	::SetBkMode(hMemDC, TRANSPARENT);
	::SetTextColor(hMemDC, clrText);
	duRect rcText(0, 0, nWidth, nHeight);
	if (fSingleLine)
		::DrawText(hMemDC, lpString, nCount, &rcText, DT_LEFT|DT_TOP|DT_SINGLELINE/*|DT_END_ELLIPSIS*/); // Modify by Alex, 去掉省略号
	else
		{
			
			if (uFormat & DT_CENTER)
			{
				::DrawText(hMemDC, lpString, nCount, &rcText, DT_WORDBREAK|DT_CENTER|DT_TOP);
			}else if (uFormat & DT_RIGHT)
			{
				::DrawText(hMemDC, lpString, nCount, &rcText, DT_WORDBREAK|DT_RIGHT|DT_TOP);
			}else
			{
				::DrawText(hMemDC, lpString, nCount, &rcText, DT_WORDBREAK|DT_LEFT|DT_TOP);	
			}
			
		}
	::SelectObject(hMemDC, hOldFont);

	//
	// 获得Alpha信息
	//
	HDC hAlphaDC = ::CreateCompatibleDC(NULL);
	BYTE *pAlphaBits = NULL;
	BITMAPINFO bmInfo;
	ZeroMemory(&bmInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

	bmInfo.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth    = nWidth;
	bmInfo.bmiHeader.biHeight   = nHeight * -1;
	bmInfo.bmiHeader.biPlanes   = 1;
	bmInfo.bmiHeader.biBitCount = 24;

	HBITMAP hAlphaBmp = ::CreateDIBSection(hAlphaDC, &bmInfo, DIB_RGB_COLORS,(void**)&pAlphaBits, 0, 0);
	HBITMAP hOldAlphaBmp = (HBITMAP)::SelectObject(hAlphaDC, hAlphaBmp);
	::SetTextColor(hAlphaDC, RGB(0, 0, 0));
	::SetBkColor(hAlphaDC, RGB(255,255,255));
	::FillRect(hAlphaDC, &rcText, (HBRUSH)::GetStockObject(WHITE_BRUSH));
	hOldFont = (HFONT)::SelectObject(hAlphaDC, hFont);
	if (fSingleLine)
		::DrawText(hAlphaDC, lpString, nCount, &rcText, DT_LEFT|DT_TOP|DT_SINGLELINE/*|DT_END_ELLIPSIS*/); // Modify by Alex, 去掉省略号
	else
		::DrawText(hAlphaDC, lpString, nCount, &rcText, DT_LEFT|DT_TOP);

	::SelectObject(hAlphaDC, hOldFont);

	Combo32Bitmap((RGBQUAD *)bitmap.bmBits, nWidth, nHeight, pBits, pAlphaBits);
	
	::SelectObject(hMemDC, hOldBitmap);
	::DeleteObject(hBitmap);
	::DeleteDC(hMemDC);

	::SelectObject(hAlphaDC, hOldAlphaBmp);
	::DeleteObject(hAlphaBmp);
	::DeleteDC(hAlphaDC);
}

inline void SetBitmapPixel(RGBQUAD *pBits, int nDstX, int nDstY, int nBitmapWidth, int nBitmapHeight, BITMAP &bitmap, int x, int y, duRect &rcClipBox)
{
	if (!((y >= rcClipBox.top)  && (y < rcClipBox.bottom) && (x >= rcClipBox.left) && (x < rcClipBox.right)))
		return;

	if (x < 0 || y < 0 || x >= bitmap.bmWidth || y >= bitmap.bmHeight)
		return;

	RGBQUAD *pSrc = (RGBQUAD *)bitmap.bmBits;
	pSrc = pSrc + (y * bitmap.bmWidth) + x;

	RGBQUAD *pDst = pBits + (nDstY * nBitmapWidth) + nDstX;
	*pDst = *pSrc;
}

void CopyPixel(RGBQUAD *pBits, int nBitmapWidth, int nBitmapHeight, HDC hDC, int x, int y)
{
	duPoint ptView;
	::GetViewportOrgEx(hDC, &ptView);

	duRect rcClipBox;
	rcClipBox.SetRectEmpty();
	::GetClipBox(hDC, &rcClipBox);
	rcClipBox.OffsetRect(ptView.x, ptView.y);

	duPoint ptReal(x, y);
	ptReal.x += ptView.x;
	ptReal.y += ptView.y;
	if (ptReal.x >= rcClipBox.right || ptReal.y >= rcClipBox.bottom)
		return;

	if (ptReal.x + nBitmapWidth <= 0 || ptReal.y + nBitmapHeight <= 0)
		return;

	HBITMAP hBitmap = (HBITMAP)GetCurrentObject(hDC, OBJ_BITMAP);
	BITMAP bitmap;
	ZeroMemory(&bitmap, sizeof(BITMAP));
	::GetObject(hBitmap, sizeof(BITMAP), &bitmap);

	if (bitmap.bmBitsPixel != 32)
		return;

	int i, j;
	for (j = 0;j < nBitmapHeight; j++)
	{
		for (i = 0;i < nBitmapWidth; i++)
		{
			SetBitmapPixel(pBits, i, j, nBitmapWidth, nBitmapHeight, bitmap, ptReal.x + i, ptReal.y + j, rcClipBox);
		}
	}
}



dustring GetTempName()
{
	srand((unsigned int)time(0));
	TCHAR *_chars = _T("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	TCHAR buffer[128];
	int index = 0;
	int i = 0;
	for (i = 0;i < 100;i++)
	{
		index =  rand() % 62;
		buffer[i] = _chars[index];
	}
	buffer[i] = '\0';
	return dustring(buffer);
}
