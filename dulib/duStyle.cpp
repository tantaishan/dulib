//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duStyle.cpp
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#include "duStyle.h"
#include "duResBase.h"
#include "duImage.h"
#include "duFont.h"
#include "duWindow.h"
#include "duWindowManager.h"
#include "duDrawHelper.h"

//////////////////////////////////////////////////////////////////////////
duStyleBase::duStyleBase() :
	 m_nLeft(0)
	,m_nTop(0)
	,m_nRight(0)
	,m_nBottom(0)
	,m_nWidth(0)
	,m_nHeight(0)
	,m_uState(DU_STATE_NORMAL)
	,m_pResManager(NULL)
{
}

BOOL duStyleBase::OnCreate(TiXmlElement *pElement)
{
	m_nLeft = ReadXmlAttributeInt(pElement, _T("left"));
	m_nTop = ReadXmlAttributeInt(pElement, _T("top"));
	m_nRight = ReadXmlAttributeInt(pElement, _T("right"));
	m_nBottom = ReadXmlAttributeInt(pElement, _T("bottom"));
	m_nWidth = ReadXmlAttributeInt(pElement, _T("width"));
	m_nHeight = ReadXmlAttributeInt(pElement, _T("height"));
	m_uState = ReadXmlAttributeState(pElement, _T("state"));

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
duImageStyle::duImageStyle() : 
	 m_uLeftPart(0)
	,m_uRightPart(0)
	,m_uTopPart(0)
	,m_uBottomPart(0)
	,m_uSelectLeft(0)
	,m_uSelectTop(0)
	,m_uSelectWidth(0)
	,m_uSelectHeight(0)
	,m_strPicFile(_T(""))
	,m_strPaintMode(_T(""))
{
}

void duImageStyle::Draw(HDC hDC, LPRECT lpDstRect, LPCTSTR lpszText, int nAlpha)
{
	if (hDC == NULL || lpDstRect == NULL || nAlpha == 0)
		return;

	duImage *pImage = (duImage *)m_pResManager->GetResObj(GetPicFile(), DU_RES_IMAGE);
	if (pImage == NULL)
		return;
	
	duRect rcDest;
	duRect rcSelection;
	rcDest.SetRectEmpty();
	rcSelection.SetRectEmpty();
	
	MakeRect(m_nLeft, m_nTop, m_nRight, m_nBottom, m_nWidth, m_nHeight, lpDstRect, &rcDest);
	
	rcSelection.SetRect(m_uSelectLeft, m_uSelectTop, m_uSelectLeft + m_uSelectWidth,  m_uSelectTop + m_uSelectHeight);
	if (rcSelection.IsRectEmpty())
		rcSelection.SetRect(0, 0, pImage->GetWidth(), pImage->GetHeight());

	if (lstrcmpi(m_strPaintMode.c_str(), _T("normal")) == 0)
	{
		if(rcDest.Width() <= rcSelection.Width()  &&  rcDest.Height() <= rcSelection.Height())
		{
			DrawNormal(hDC, rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(), pImage, 
				rcSelection.left, rcSelection.top, nAlpha);

		}
		else
		{
			DrawNormal(hDC, rcDest.left, rcDest.top, rcSelection.Width(), rcSelection.Height(), pImage, 
				rcSelection.left, rcSelection.top, nAlpha);
		}
		
	}
	else if (lstrcmpi(m_strPaintMode.c_str(), _T("tile")) == 0)
	{
		DrawTile(hDC, rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(), pImage, 
			rcSelection.left, rcSelection.top, rcSelection.Width(), rcSelection.Height(), nAlpha);
	}
	else if (lstrcmpi(m_strPaintMode.c_str(), _T("stretch")) == 0)
	{
		DrawStretch(hDC, rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(), pImage, 
			rcSelection.left, rcSelection.top, rcSelection.Width(), rcSelection.Height(), nAlpha);
	}
	else if (lstrcmpi(m_strPaintMode.c_str(), _T("stretchvert")) == 0)
	{
		DrawVertStretch(hDC, rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(), pImage, 
			rcSelection.left, rcSelection.top, rcSelection.Width(), rcSelection.Height(), m_uTopPart, m_uBottomPart, nAlpha);
	}
	else if (lstrcmpi(m_strPaintMode.c_str(), _T("stretchhorz")) == 0)
	{
		DrawHorzStretch(hDC, rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(), pImage, 
			rcSelection.left, rcSelection.top, rcSelection.Width(), rcSelection.Height(), m_uLeftPart, m_uRightPart, nAlpha);
	}
	else if (lstrcmpi(m_strPaintMode.c_str(), _T("stretchpartall")) == 0)
	{
		DrawAllPartStretch(hDC, rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(), pImage, 
			rcSelection.left, rcSelection.top, rcSelection.Width(), rcSelection.Height(), 
			m_uLeftPart, m_uRightPart, m_uTopPart, m_uBottomPart, nAlpha);
	}
}

BOOL duImageStyle::OnCreate(TiXmlElement *pElement)
{
	duStyleBase::OnCreate(pElement);

	m_uLeftPart = ReadXmlAttributeInt(pElement, _T("left_part"));
	m_uTopPart = ReadXmlAttributeInt(pElement, _T("top_part"));
	m_uRightPart = ReadXmlAttributeInt(pElement, _T("right_part"));
	m_uBottomPart = ReadXmlAttributeInt(pElement, _T("bottom_part"));

	m_uSelectLeft = ReadXmlAttributeInt(pElement, _T("selectleft"));
	m_uSelectTop = ReadXmlAttributeInt(pElement, _T("selecttop"));
	m_uSelectWidth = ReadXmlAttributeInt(pElement, _T("selectwidth"));
	m_uSelectHeight = ReadXmlAttributeInt(pElement, _T("selectheight"));

	m_strPicFile = ReadXmlAttributeText(pElement, _T("picfile"));
	m_strPaintMode = ReadXmlAttributeText(pElement, _T("paintmode"));

	return TRUE;
}

void duImageStyle::SetPicFile(LPCTSTR lpszName) 
{ 
	if (lpszName) 
		m_strPicFile = lpszName; 
	else 
		m_strPicFile = _T(""); 
}

void duImageStyle::SetPaintMode(LPCTSTR lpszMode) 
{ 
	if (lpszMode) 
		m_strPaintMode = lpszMode; 
	else 
		m_strPaintMode = _T(""); 
}

//////////////////////////////////////////////////////////////////////////
duTextStyle::duTextStyle() :
	 m_clrText(RGB(0,0,0))
	,m_fSingleline(TRUE)
	,m_strHorz(_T(""))
	,m_strVert(_T(""))
	,m_strFont(_T(""))
{
}

BOOL duTextStyle::OnCreate(TiXmlElement *pElement)
{
	duStyleBase::OnCreate(pElement);

	m_clrText = ReadXmlAttributeColor(pElement, _T("textcolor"));
	ReadXmlAttributeBOOL(pElement, _T("bsingleline"), &m_fSingleline);
	m_strFont = ReadXmlAttributeText(pElement, _T("font"));
	m_strHorz = ReadXmlAttributeText(pElement, _T("halign"));
	m_strVert = ReadXmlAttributeText(pElement, _T("valign"));
	
	return TRUE;
}

void duTextStyle::Draw(HDC hDC, LPRECT lpDstRect, LPCTSTR lpszText, int nAlpha)
{
	if (hDC == NULL || lpDstRect == NULL || lpszText == NULL || *lpszText == 0 || nAlpha == 0)
		return;
	
	duRect rectDest;
	rectDest.SetRectEmpty();
	MakeRect(m_nLeft, m_nTop, m_nRight, m_nBottom, m_nWidth, m_nHeight, lpDstRect, &rectDest);
	
	duFont *font = (duFont *)m_pResManager->GetResObj(m_strFont.c_str(), DU_RES_FONT);
	
	UINT uFormat = 0;
	if (lstrcmpi(m_strHorz.c_str(), _T("left")) == 0) 
		uFormat |= DT_LEFT;
	else if (lstrcmpi(m_strHorz.c_str(), _T("center")) == 0)
		uFormat |= DT_CENTER;
	else if (lstrcmpi(m_strHorz.c_str(), _T("right")) == 0)
		uFormat |= DT_RIGHT;

	if (lstrcmpi(m_strVert.c_str(), _T("top")) == 0)
		uFormat |= DT_TOP;
	else if (lstrcmpi(m_strVert.c_str(), _T("center")) == 0)
		uFormat |= DT_VCENTER;
	else if (lstrcmpi(m_strVert.c_str(), _T("bottom")) == 0)
		uFormat |= DT_BOTTOM;
		
	if (m_fSingleline)
		uFormat |= DT_SINGLELINE;

	DrawText32Bpp(hDC, font, m_clrText, lpszText, _tcslen(lpszText), &rectDest, uFormat, nAlpha);
}

HFONT duTextStyle::GetHFont()
{
	duFont *pFont = (duFont *)m_pResManager->GetResObj(m_strFont.c_str(), DU_RES_FONT);
	if (pFont)
		return pFont->GetHFont();

	return NULL;
}

void duTextStyle::CalcTextRect(LPCTSTR lpszText, LPSIZE lpOutSize)
{
	if (lpOutSize == NULL|| lpszText == NULL)
		return;

	lpOutSize->cx = lpOutSize->cy = 0;
	int nTextLength = _tcslen(lpszText);
	if (nTextLength <= 0)
		return;

	HFONT hFont = GetHFont();
	if (hFont == NULL)
		hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);

	HDC hDC = ::GetDC(NULL);
	HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);

	duRect rcText;
	rcText.SetRectEmpty();
	if (m_fSingleline)
		::DrawText(hDC, lpszText, nTextLength, &rcText, DT_TOP|DT_LEFT|DT_SINGLELINE|DT_CALCRECT);
	else
		::DrawText(hDC, lpszText, nTextLength, &rcText, DT_WORDBREAK|DT_TOP|DT_LEFT|DT_CALCRECT);

	::SelectObject(hDC, hOldFont);
	::ReleaseDC(NULL, hDC);

	lpOutSize->cx = rcText.Width();
	lpOutSize->cy = rcText.Height();
}

void duTextStyle::SetFont(LPCTSTR lpszFontName) 
{ 
	if (lpszFontName) 
		m_strFont = lpszFontName; 
	else 
		m_strFont = _T(""); 
}

void duTextStyle::SetHAlign(LPCTSTR lpszHorzAlign) 
{ 
	if (lpszHorzAlign) 
		m_strHorz = lpszHorzAlign; 
	else 
		m_strHorz = _T("");
}

void duTextStyle::SetVAlign(LPCTSTR lpszVertAlign) 
{ 
	if (lpszVertAlign) 
		m_strVert = lpszVertAlign; 
	else 
		m_strVert = _T("");
}

//////////////////////////////////////////////////////////////////////////
duRectStyle::duRectStyle() :
	 m_clrFill(RGB(255,255,255))
	,m_clrBorder(RGB(0,0,0))
	,m_fFill(FALSE)
	,m_fBorder(FALSE)
	,m_uPenWidth(1)
	,m_strPenType(_T(""))
{
}

duRectStyle::~duRectStyle()
{
}

BOOL duRectStyle::OnCreate(TiXmlElement *pElement)
{
	duStyleBase::OnCreate(pElement);

	m_clrFill = ReadXmlAttributeColor(pElement, _T("fillcolor")); 
	m_clrBorder = ReadXmlAttributeColor(pElement, _T("bordercolor"));
	ReadXmlAttributeBOOL(pElement, _T("bfill"), &m_fFill);
	ReadXmlAttributeBOOL(pElement, _T("fill"), &m_fFill);
	ReadXmlAttributeBOOL(pElement, _T("bborder"), &m_fBorder);
	ReadXmlAttributeBOOL(pElement, _T("border"), &m_fBorder);
	m_strPenType = ReadXmlAttributeText(pElement, _T("bordertype"));
	m_uPenWidth = ReadXmlAttributeInt(pElement, _T("borderwidth"));

	return TRUE;
}

void duRectStyle::Draw(HDC hDC, LPRECT lpDstRect, LPCTSTR lpszText, int nAlpha)
{
	if (hDC == NULL || lpDstRect == NULL)
		return;

	duRect rectDest;
	rectDest.SetRectEmpty();
	MakeRect(m_nLeft, m_nTop, m_nRight, m_nBottom, m_nWidth, m_nHeight, lpDstRect, &rectDest);
	
	Graphics gs(hDC);
	if (m_fFill)
	{
		RectF rectF(rectDest.left, rectDest.top, rectDest.Width(), rectDest.Height());
		Color clr(nAlpha, GetRValue(m_clrFill), GetGValue(m_clrFill), GetBValue(m_clrFill));
		SolidBrush SolidBrush(clr);
		gs.FillRectangle(&SolidBrush, rectF);
	}

	if (m_fBorder)
	{
		RectF rectF(rectDest.left, rectDest.top, rectDest.Width() - 1, rectDest.Height() - 1);
		Color clr(nAlpha, GetRValue(m_clrBorder), GetGValue(m_clrBorder), GetBValue(m_clrBorder));
		Pen pen(clr, m_uPenWidth);
		pen.SetDashStyle(GetPenSolidType());
		gs.DrawRectangle(&pen, rectF);
	}
}

DashStyle duRectStyle::GetPenSolidType()
{
	DashStyle nPenStyle = DashStyleSolid;
	if (lstrcmpi(m_strPenType.c_str(), _T("solid")) == 0)
		nPenStyle = DashStyleSolid;
	else if (lstrcmpi(m_strPenType.c_str(), _T("dash")) == 0)
		nPenStyle = DashStyleDash;
	else if (lstrcmpi(m_strPenType.c_str(), _T("dot")) == 0)
		nPenStyle = DashStyleDot;
	else if (lstrcmpi(m_strPenType.c_str(), _T("dashdot")) == 0)
		nPenStyle = DashStyleDashDot;
	else if (lstrcmpi(m_strPenType.c_str(), _T("dashnotdot")) == 0)
		nPenStyle = DashStyleDashDotDot;

	return nPenStyle;
}

void duRectStyle::SetBorderType(LPCTSTR lpszBorderType) 
{
	if (lpszBorderType) 
		m_strPenType = lpszBorderType;
	else 
		m_strPenType = _T(""); 
}


////////////////////////////////////////////////////////////////////////////////////
duLineStyle::duLineStyle() :
	 m_clrLine(RGB(0,0,0))
	,m_uLineWidth(1)
	,m_ptStart(0,0)
	,m_ptEnd(0,0)
	,m_strLineType(_T(""))
{
}

duLineStyle::~duLineStyle()
{
}

void duLineStyle::Draw(HDC hDC, LPRECT lpDstRect, LPCTSTR lpszText, int nAlpha)
{
	if (hDC == NULL || lpDstRect == NULL)
		return;

	duRect rectDest;
	rectDest.SetRectEmpty();
	MakeRect(m_nLeft, m_nTop, m_nRight, m_nBottom, m_nWidth, m_nHeight, lpDstRect, &rectDest);

	Graphics gs(hDC);
	Color clr(nAlpha, GetRValue(m_clrLine), GetGValue(m_clrLine), GetBValue(m_clrLine));
	Pen pen(clr, m_uLineWidth);
	pen.SetDashStyle(GetPenSolidType());
	
	Point pt[2];
	if (m_ptStart.x  < 0)
		pt[0].X = rectDest.right + m_ptStart.x;
	else
		pt[0].X = rectDest.left + m_ptStart.x;
		
	if (m_ptStart.y < 0)
		pt[0].Y = rectDest.bottom + m_ptStart.y;
	else
		pt[0].Y = rectDest.top + m_ptStart.y;
		
	if (m_ptEnd.x < 0)
		pt[1].X = rectDest.right + m_ptEnd.x;
	else
		pt[1].X = rectDest.left + m_ptEnd.x;
	
	if (m_ptEnd.y < 0)
		pt[1].Y = rectDest.bottom + m_ptEnd.y;
	else
		pt[1].Y = rectDest.top + m_ptEnd.y;
	
	Point *points = (Point *)&pt;
	gs.DrawLines(&pen, points, 2);
}

BOOL duLineStyle::OnCreate(TiXmlElement *pElement)
{
	duStyleBase::OnCreate(pElement);
	
	m_clrLine = ReadXmlAttributeColor(pElement, _T("color")); 
	m_uLineWidth = ReadXmlAttributeInt(pElement, _T("linewidth"));
	m_strLineType = ReadXmlAttributeText(pElement, _T("linetype"));
	ReadXmlAttributePoint(pElement, _T("startpoint"), &m_ptStart);
	ReadXmlAttributePoint(pElement, _T("endpoint"), &m_ptEnd);
	
	return TRUE;
}

DashStyle duLineStyle::GetPenSolidType()
{
	DashStyle nPenStyle = DashStyleSolid;
	if (lstrcmpi(m_strLineType.c_str(), _T("solid")) == 0)
		nPenStyle = DashStyleSolid;
	else if (lstrcmpi(m_strLineType.c_str(), _T("dash")) == 0)
		nPenStyle = DashStyleDash;
	else if (lstrcmpi(m_strLineType.c_str(), _T("dot")) == 0)
		nPenStyle = DashStyleDot;
	else if (lstrcmpi(m_strLineType.c_str(), _T("dashdot")) == 0)
		nPenStyle = DashStyleDashDot;
	else if (lstrcmpi(m_strLineType.c_str(), _T("dashnotdot")) == 0)
		nPenStyle = DashStyleDashDotDot;

	return nPenStyle;
}

void duLineStyle::SetLineType(LPCTSTR lpszLineType)
{
	if (lpszLineType == NULL)
		m_strLineType = _T("solid");
	else
		m_strLineType = lpszLineType;
}

BOOL duLineStyle::GetStartPoint(LPPOINT lpOutPoint)
{
	if (lpOutPoint)
	{
		lpOutPoint->x = m_ptStart.x;
		lpOutPoint->y = m_ptStart.y;
		return TRUE;
	}
	return FALSE;
}

void duLineStyle::SetStartPoint(LPPOINT lpStartPoint)
{
	if (lpStartPoint == NULL)
		m_ptStart.x = m_ptStart.y = 0;
	else
	{
		m_ptStart.x = lpStartPoint->x;
		m_ptStart.y = lpStartPoint->y;
	}
}

BOOL duLineStyle::GetEndPoint(LPPOINT lpOutPoint)
{
	if (lpOutPoint)
	{
		lpOutPoint->x = m_ptEnd.x;
		lpOutPoint->y = m_ptEnd.y;
		return TRUE;
	}
	return FALSE;
}

void duLineStyle::SetEndPoint(LPPOINT lpEndPoint)
{
	if (lpEndPoint == NULL)
		m_ptEnd.x = m_ptEnd.y = 0;
	else
	{
		m_ptEnd.x = lpEndPoint->x;
		m_ptEnd.y = lpEndPoint->y;
	}
}
