//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duStaticEx.h
//
//  History:    May-20-2011   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "duStaticEx.h"

duStaticEx::duStaticEx()
{
	ZeroMemory(m_szFont, MAX_NAME *sizeof(TCHAR));
	m_nLineSpace = 0;
	m_clrText = RGB(0,0,0);
	m_vtTextCache.clear();
	m_fAutoSize = FALSE;
}

duStaticEx::~duStaticEx()
{
	ClearTextCache();
}

void duStaticEx::RegisterControlProperty()
{
	RegisterProperty(_T("font"), DU_PROPERTY_FONT, m_szFont);
	RegisterProperty(_T("linespace"), DU_PROPERTY_LONG, &m_nLineSpace);
	RegisterProperty(_T("textcolor"), DU_PROPERTY_COLOR, &m_clrText);
	RegisterProperty(_T("autosize"), DU_PROPERTY_BOOL, &m_fAutoSize);
}

void duStaticEx::OnCreate()
{
}

void duStaticEx::DrawObject(HDC hDC)
{
	duRect rectStaticEx;
	Plugin_GetRect(this, &rectStaticEx);
	rectStaticEx.OffsetRect(-rectStaticEx.left, -rectStaticEx.top);

	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
		pStyleGroup->Draw(hDC, &rectStaticEx, DU_STATE_ALL, NULL, GetAlpha());

	StDrawText(hDC, (LPTSTR)GetText(), _tcslen(GetText()));
}

void duStaticEx::SetText(LPCTSTR lpszText)
{
	duPlugin::SetText(lpszText);
	ClearTextCache();
}

void duStaticEx::Resize(LPRECT lpRect)
{
	RECT rcOld;
	SetRectEmpty(&rcOld);
	GetRect(&rcOld);

	int nOldWidth = rcOld.right - rcOld.left;
	int nOldHeight = rcOld.bottom - rcOld.top;

	duPlugin::Resize(lpRect);

	RECT rcNew;
	SetRectEmpty(&rcNew);
	GetRect(&rcNew);

	int nNewWidth = rcNew.right - rcNew.left;
	int nNewHeight = rcNew.bottom - rcNew.top;

	if ((nOldWidth != nNewWidth) || (nOldHeight != nNewHeight))
		ClearTextCache();
}

void duStaticEx::StDrawText(HDC hDC, LPTSTR lpszText, int len)
{
	if (lpszText == NULL || len == 0)
		return;

	duRect rcStaticEx;
	Plugin_GetRect(this, &rcStaticEx);
	rcStaticEx.OffsetRect(-rcStaticEx.left, -rcStaticEx.top);

	duFont *pFont = (duFont *)GetResObj(m_szFont, DU_RES_FONT);
	// draw text use cache info
	if (m_vtTextCache.empty())
		CalcTextCache();

	if (!m_vtTextCache.empty())
	{
		int i;
		for (i = 0;i < (int)m_vtTextCache.size(); i++)
		{
			duStaticTextInfo *p = m_vtTextCache[i];
			if (p)
			{
				duRect rcDrawText;
				rcDrawText.left   = rcStaticEx.left; 
				rcDrawText.right  = rcStaticEx.right;
				rcDrawText.top    = rcStaticEx.top + p->iCy;
				rcDrawText.bottom = rcDrawText.top + p->iCHeight;
				
				::DrawText32Bpp(hDC, pFont, m_clrText, p->strText.c_str(), p->strText.length(), &rcDrawText, DT_SINGLELINE|DT_LEFT|DT_TOP, GetAlpha());
			}
		}
	}
}

int duStaticEx::CalcTextCache()
{
	LPTSTR lpszText = (LPTSTR)GetText();
	int len = lstrlen(lpszText);
	if (lpszText == NULL || len == 0)
		return 0;

	duRect rcStaticEx;
	Plugin_GetRect(this, &rcStaticEx);
	rcStaticEx.OffsetRect(-rcStaticEx.left, -rcStaticEx.top);
	if (rcStaticEx.Width() <= 0 || rcStaticEx.Height() <= 0)
		return 0;

	duFont *pFont = (duFont *)GetResObj(m_szFont, DU_RES_FONT);
	if (!m_vtTextCache.empty())
	{
		duStaticTextInfo *p = m_vtTextCache[m_vtTextCache.size() - 1];
		if (p)
			return  p->iCy + p->iCHeight;
		return 0;
	}
	ClearTextCache();

	HDC hDC = ::GetDC(m_hWnd);

	HFONT hFont = (HFONT)::GetStockObject(SYSTEM_FONT);
	if (pFont)
		hFont = pFont->GetHFont();

	HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);

	LPTSTR lpszTemp = new TCHAR[len + 1];
	LPTSTR lp = lpszText;
	int nWidth = rcStaticEx.Width();
	int nHeight = rcStaticEx.Height();

	int i, iCy, iCWidth, iCHeight;
	iCy = 0;
	iCWidth = 0;
	iCHeight = 0;
	for (i = 0;i < len; i++)
	{
		iCWidth++;

		ZeroMemory(lpszTemp, sizeof(TCHAR) * (len + 1));
		_tcsncpy(lpszTemp, lp, iCWidth);
		

		duRect rcText;
		rcText.SetRectEmpty();
		::DrawText(hDC, lpszTemp, _tcslen(lpszTemp), &rcText, DT_SINGLELINE|DT_LEFT|DT_TOP|DT_CALCRECT);

		iCHeight = rcText.bottom - rcText.top;

		if (lpszTemp[iCWidth-1] == '\n')
		{
			duStaticTextInfo *pStaticTextInfo = new duStaticTextInfo;
			pStaticTextInfo->iCy = iCy;
			pStaticTextInfo->iCHeight = iCHeight;
			pStaticTextInfo->strText = lpszTemp;
			m_vtTextCache.push_back(pStaticTextInfo);

			iCy += m_nLineSpace + (rcText.bottom - rcText.top);
			lp = lp + iCWidth;
			iCWidth = 0;
		}
		else if ((rcText.right - rcText.left) > nWidth)
		{
			lpszTemp[iCWidth - 1] = 0;

			duStaticTextInfo *pStaticTextInfo = new duStaticTextInfo;
			pStaticTextInfo->iCy = iCy;
			pStaticTextInfo->iCHeight = iCHeight;
			pStaticTextInfo->strText = lpszTemp;
			m_vtTextCache.push_back(pStaticTextInfo);

			iCy += m_nLineSpace + (rcText.bottom - rcText.top);
			lp = lp + iCWidth - 1;
			iCWidth = 0;
			i--;
		}
	}

	if (iCWidth > 0)
	{
		ZeroMemory(lpszTemp, sizeof(TCHAR) * (len + 1));
		_tcsncpy(lpszTemp, lp, iCWidth);

		duRect rcDrawText;
		rcDrawText.left   = rcStaticEx.left;
		rcDrawText.right  = rcStaticEx.right;
		rcDrawText.top    = rcStaticEx.top + iCy;
		rcDrawText.bottom = rcStaticEx.top + iCy + iCHeight;
		
		duStaticTextInfo *pStaticTextInfo = new duStaticTextInfo();
		pStaticTextInfo->iCy = iCy;
		pStaticTextInfo->iCHeight = iCHeight;
		pStaticTextInfo->strText = lpszTemp;
		m_vtTextCache.push_back(pStaticTextInfo);
	}
	
	if (lpszTemp)
	{
		delete [] lpszTemp;
		lpszTemp = NULL;
	}
	
	::SelectObject(hDC, hOldFont);
	::ReleaseDC(m_hWnd, hDC);

	if (m_vtTextCache.empty())
		return 0;
	
	return iCy + iCHeight;
}

void duStaticEx::ClearTextCache()
{
	int i;
	for (i = 0;i < (int)m_vtTextCache.size(); i++)
	{
		duStaticTextInfo *p = m_vtTextCache[i];
		if (p)
		{
			delete p;
			p = NULL;
		}
	}
	m_vtTextCache.clear();
}

LPCTSTR duStaticEx::GetFont()
{
	return m_szFont;
}

void duStaticEx::SetFont(LPCTSTR lpszFont)
{
	if (lpszFont)
		_tcsncpy(m_szFont, lpszFont, MAX_NAME);
	else
		ZeroMemory(m_szFont, sizeof(TCHAR) *MAX_NAME);
}

COLORREF duStaticEx::GetTextColor()
{
	return m_clrText;
}

void duStaticEx::SetTextColor(COLORREF clrText)
{
	m_clrText = clrText;
}

int duStaticEx::GetLineSpace()
{
	return m_nLineSpace;
}

void duStaticEx::SetLineSpace(int nLineSpace)
{
	m_nLineSpace = nLineSpace;
}

LPCTSTR duStaticEx::GetLineText(int index)
{
	if (index <= 0 || index > GetLineCount())
		return NULL;

	duStaticTextInfo *pStaticTextInfoLine = m_vtTextCache[index];
	return (LPCTSTR)pStaticTextInfoLine->strText.c_str();
}

void duStaticEx::GetLineTextRect(int index, LPSIZE lpOutSize)
{
	LPCTSTR pLineText = GetLineText(index);
	if (index == NULL|| pLineText == NULL || lpOutSize == NULL)
		return;

	lpOutSize->cx = 0;
	lpOutSize->cy = 0;
	int nTextLength = _tcslen(pLineText);
	if (nTextLength <= 0)
		return;

	duFont *pFont = (duFont *)GetResObj(GetFont(),DU_RES_FONT);
	if (pFont == NULL)
		return;

	HFONT hFont =pFont->GetHFont();
	HDC hDC = ::GetDC(NULL);
	HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);

	duRect rcText;
	rcText.SetRectEmpty();
	
	::DrawText(hDC, pLineText, nTextLength, &rcText, DT_TOP|DT_LEFT|DT_SINGLELINE|DT_CALCRECT);

	::SelectObject(hDC, hOldFont);
	::ReleaseDC(NULL, hDC);

	lpOutSize->cx = rcText.Width();
	lpOutSize->cy = ( rcText.Height()  + m_nLineSpace) * index;
}

