//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duLEDTimer.cpp
//
//  History:    Mar-23-2011   Eric Qian Created
//
//--------------------------------------------------------------------------
#include "duLEDTimer.h"

duLEDTimer::duLEDTimer() :
    m_nTimerID(0)
{
	ZeroMemory(m_szImageFont, MAX_NAME * sizeof(TCHAR));
}

duLEDTimer::~duLEDTimer()
{
	Plugin_KillTimer(this, m_nTimerID);
}

void duLEDTimer::RegisterControlProperty()
{
	RegisterProperty(_T("imagefont"), DU_PROPERTY_STRING, m_szImageFont);
}

void duLEDTimer::OnCreate()
{
	m_nTimerID = GetUniTimer();
	Plugin_SetTimer(this, m_nTimerID, 1000);
}

void duLEDTimer::DrawObject(HDC hDC)
{
	duRect rcLEDTimer;
	Plugin_GetRect(this, &rcLEDTimer);
	rcLEDTimer.OffsetRect(-rcLEDTimer.left, -rcLEDTimer.top);

	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
	    pStyleGroup->Draw(hDC, &rcLEDTimer, GetState(), NULL, GetAlpha());

	duImage *pImage = (duImage *)GetResObj(m_szImageFont, DU_RES_IMAGE);
	if (pImage == NULL)
		return;
		
	LPCTSTR lpszText = GetText();
	int nImageWidth = pImage->GetWidth();
	int nImageHeight = pImage->GetHeight();
	int nTextLength = _tcslen(lpszText);
	int nFontWidth = nImageWidth / 12;
	
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, pImage->GetBitmap());
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, GetAlpha(), AC_SRC_ALPHA};
	
	int i;
	int xDst = 0, yDst = 0;
	int xSrc;
	for (i = 0;i < nTextLength; i++)
	{
		xSrc = 0;
		switch (lpszText[i])
		{
		case '1':
			xSrc = 0;
			break;
		case '2':
			xSrc = nFontWidth;
			break;
		case '3':
			xSrc = nFontWidth * 2;
			break;
		case '4':
			xSrc = nFontWidth * 3;
			break;
		case '5':
			xSrc = nFontWidth * 4;
			break;
		case '6':
			xSrc = nFontWidth * 5;
			break;
		case '7':
			xSrc = nFontWidth * 6;
			break;
		case '8':
			xSrc = nFontWidth * 7;
			break;
		case '9':
			xSrc = nFontWidth * 8;
			break;
		case '0':
			xSrc = nFontWidth * 9;
			break;
		case ':':
			xSrc = nFontWidth * 10;
			break;
		case '-':
			xSrc = nFontWidth * 11;
			break;

		default:
			xSrc = 0;
			break;
		}

		::AlphaBlend(hDC, xDst, 0, nFontWidth, nImageHeight, hMemDC, xSrc, 0, nFontWidth, nImageHeight, bf);
		xDst += nFontWidth;
	}

	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);
}

void duLEDTimer::OnTimer(UINT nEventId)
{
	if (nEventId != m_nTimerID)
		return;
	
	if (!Plugin_IsVisible(this))
		return;
	
	TCHAR szTime[MAX_NAME];
	ZeroMemory(szTime, MAX_NAME *sizeof(TCHAR));
	
	_tstrtime(szTime);
	SetText(szTime);
	Plugin_Redraw(this, TRUE);
}

void duLEDTimer::SetImageFont(LPCTSTR lpszImageFont)
{
	if (lpszImageFont)
		_tcsncpy(m_szImageFont, lpszImageFont, MAX_NAME);
	else
		ZeroMemory(m_szImageFont, MAX_NAME * sizeof(TCHAR));
}
