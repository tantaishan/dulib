//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duStyle.h
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duObject.h"

class duResManager;

// duStyleBase
//
//
class duStyleBase : public duObject
{
public:
	virtual int WINAPI GetType() = 0;
	virtual void WINAPI Draw(HDC hDC, LPRECT lpDstRect, LPCTSTR lpszText, int nAlpha) = 0;
	virtual BOOL WINAPI OnCreate(TiXmlElement *pElement);
	
	virtual int WINAPI GetLeft() { return m_nLeft; }
	virtual void WINAPI SetLeft(int nLeft) { m_nLeft = nLeft; }
	virtual int WINAPI GetTop() { return m_nTop; }
	virtual void WINAPI SetTop(int nTop) { m_nTop = nTop; }
	virtual int WINAPI GetRight() { return m_nRight; }
	virtual void WINAPI SetRight(int nRight) { m_nRight = nRight; }
	virtual int WINAPI GetBottom() { return m_nBottom; }
	virtual void WINAPI SetBottom(int nBottom) { m_nBottom = nBottom; }
	virtual int WINAPI GetWidth() { return m_nWidth; }
	virtual void WINAPI SetWidth(int nWidth) { m_nWidth = nWidth; }
	virtual int WINAPI GetHeight() { return m_nHeight; }
	virtual void WINAPI SetHeight(int nHeight) { m_nHeight = nHeight; }
	virtual UINT WINAPI GetState() { return m_uState; }
	virtual void WINAPI SetState(UINT uState) { m_uState = uState; }

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duStyleBase"); }

public:
	duStyleBase();
	~duStyleBase() {}
	
	duResManager *GetResManager() { return m_pResManager; }
	void SetResManager(duResManager *pResManager) { m_pResManager = pResManager; }

protected:
	int m_nLeft;
	int m_nTop;
	int m_nRight;
	int m_nBottom;
	int m_nWidth;
	int m_nHeight;
	UINT m_uState;
	duResManager *m_pResManager;
};

// duImageStyle
//
// 
class duImageStyle : public duStyleBase
{
public:
	virtual int WINAPI GetType() { return STYLE_IMAGE; }
	virtual void WINAPI Draw(HDC hDC, LPRECT lpDstRect, LPCTSTR lpszText, int nAlpha);
	
	virtual BOOL WINAPI OnCreate(TiXmlElement *pElement);
	
	virtual UINT WINAPI GetLeftPart() { return m_uLeftPart; }
	virtual void WINAPI SetLeftPart(UINT uLeftPart) { m_uLeftPart = uLeftPart; }
	virtual UINT WINAPI GetTopPart() { return m_uTopPart; }
	virtual void WINAPI SetTopPart(UINT uTopPart) { m_uTopPart = uTopPart; }
	virtual UINT WINAPI GetRightPart() { return m_uRightPart; }
	virtual void WINAPI SetRightPart(UINT uRightPart) { m_uRightPart = uRightPart; }
	virtual UINT WINAPI GetBottomPart() { return m_uBottomPart; }
	virtual void WINAPI SetBottomPart(UINT uBottomPart) { m_uBottomPart = uBottomPart; }

	virtual UINT WINAPI GetSelectLeft() { return m_uSelectLeft; }
	virtual void WINAPI SetSelectLeft(UINT uSelectLeft) { m_uSelectLeft = uSelectLeft; }
	virtual UINT WINAPI GetSelectTop() { return m_uSelectTop; }
	virtual void WINAPI SetSelectTop(UINT uSelectTop) { m_uSelectTop = uSelectTop; }
	virtual UINT WINAPI GetSelectWidth() { return m_uSelectWidth; }
	virtual void WINAPI SetSelectWidth(UINT uSelectWidth) { m_uSelectWidth = uSelectWidth; }
	virtual UINT WINAPI GetSelectHeight() { return m_uSelectHeight; }
	virtual void WINAPI SetSelectHeight(UINT uSelectHeight) { m_uSelectHeight = uSelectHeight; }
	
	virtual LPCTSTR WINAPI GetPicFile() { return m_strPicFile.c_str(); }
	virtual void WINAPI SetPicFile(LPCTSTR lpszName);

	virtual LPCTSTR WINAPI GetPaintMode() { return m_strPaintMode.c_str(); }
	virtual void WINAPI SetPaintMode(LPCTSTR lpszPaintMode);

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duImageStyle"); }

public:
	duImageStyle();
	~duImageStyle() {}

protected:
	UINT m_uLeftPart;
	UINT m_uRightPart;
    UINT m_uTopPart;
	UINT m_uBottomPart;
	UINT m_uSelectLeft;
	UINT m_uSelectTop;
	UINT m_uSelectWidth;
	UINT m_uSelectHeight;
	dustring m_strPicFile;
	dustring m_strPaintMode;
};


// duTextStyle
//
//
class duTextStyle : public duStyleBase
{ 
public:
	virtual int WINAPI GetType() { return STYLE_TEXT; }
	virtual void WINAPI Draw(HDC hDC, LPRECT lpDstRect, LPCTSTR lpszext, int nAlpha);
	
	virtual BOOL WINAPI OnCreate(TiXmlElement *pElement);
	
	virtual HFONT WINAPI GetHFont();
	virtual void WINAPI CalcTextRect(LPCTSTR lpszText, LPSIZE lpOutSize);

	virtual LPCTSTR WINAPI GetFont() { return m_strFont.c_str(); }
	virtual void WINAPI SetFont(LPCTSTR lpszFontName);
	virtual COLORREF WINAPI GetTextColor() { return m_clrText; }
	virtual void WINAPI SetTextColor(COLORREF clrText) { m_clrText = clrText; }
	virtual BOOL WINAPI IsSingleLine() { return m_fSingleline; }
	virtual void WINAPI SetSingleLine(BOOL fSingleLine) { m_fSingleline = fSingleLine; }
	virtual LPCTSTR WINAPI GetHAlign() { return m_strHorz.c_str(); }
	virtual void WINAPI SetHAlign(LPCTSTR lpszHorzAlign);
	virtual LPCTSTR WINAPI GetVAlign() { return m_strVert.c_str(); }
	virtual void WINAPI SetVAlign(LPCTSTR lpszVertAlign);

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duTextStyle"); }

public:
	duTextStyle();
	~duTextStyle() {}

protected:
	COLORREF m_clrText;
	BOOL     m_fSingleline;
	dustring m_strFont;
	dustring m_strHorz;
	dustring m_strVert;
};

// duRectStyle
//
// 
class duRectStyle : public duStyleBase
{
public:
	virtual int WINAPI GetType() { return STYLE_RECT; }
	virtual void WINAPI Draw(HDC hDC, LPRECT lpDstRect, LPCTSTR lpszText, int nAlpha);
	
	virtual BOOL WINAPI OnCreate(TiXmlElement *pElement);

	virtual COLORREF WINAPI GetFillColor() { return m_clrFill; }
	virtual void WINAPI SetFillColor(COLORREF clr) { m_clrFill = clr; }
	virtual COLORREF WINAPI GetBorderColor() { return m_clrBorder; }
	virtual void WINAPI SetBorderColor(COLORREF clr) { m_clrBorder = clr; }
	virtual BOOL WINAPI IsFillRect() { return m_fFill; }
	virtual void WINAPI SetFillRect(BOOL fFill) { m_fFill = fFill; }
	virtual BOOL WINAPI IsDrawBorder() { return m_fBorder; }
	virtual void WINAPI SetDrawBorder(BOOL fBorder) { m_fBorder = fBorder; }
	virtual LPCTSTR WINAPI GetBorderType() { return m_strPenType.c_str(); }
	virtual void WINAPI SetBorderType(LPCTSTR lpszBorderType);
	virtual UINT WINAPI GetBorderWidth() { return m_uPenWidth; }
	virtual void WINAPI SetBorderWidth(UINT uBorderWidth) { m_uPenWidth = uBorderWidth; }

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duRectStyle"); }

public:
	duRectStyle();
	~duRectStyle();
	
protected:
	DashStyle GetPenSolidType();

protected:
	COLORREF m_clrFill;
	COLORREF m_clrBorder;
	BOOL m_fFill;
	BOOL m_fBorder;
	UINT m_uPenWidth;
	dustring m_strPenType;
};

// duLineStyle
//
//
class duLineStyle : public duStyleBase
{
public:
	virtual int WINAPI GetType() { return STYLE_LINE; }
	virtual void WINAPI Draw(HDC hDC, LPRECT lpDstRect, LPCTSTR lpszText, int nAlpha);
	
	virtual BOOL WINAPI OnCreate(TiXmlElement *pElement);
	
	virtual COLORREF WINAPI GetLineColor() { return m_clrLine; }
	virtual void WINAPI SetLineColor(COLORREF clr) { m_clrLine = clr; }
	virtual UINT WINAPI GetLineWidth() { return m_uLineWidth; }
	virtual void WINAPI SetLineWidth(UINT uLineWidth) { m_uLineWidth = uLineWidth; }
	virtual LPCTSTR WINAPI GetLineType() { return m_strLineType.c_str(); }
	virtual void WINAPI SetLineType(LPCTSTR lpszLineType);
	virtual BOOL WINAPI GetStartPoint(LPPOINT lpOutPoint);
	virtual void WINAPI SetStartPoint(LPPOINT lpStartPoint);
	virtual BOOL WINAPI GetEndPoint(LPPOINT lpOutPoint);
	virtual void WINAPI SetEndPoint(LPPOINT lpEndPoint);

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duLineStyle"); }

public:
	duLineStyle();
	~duLineStyle();
	
protected:
	DashStyle GetPenSolidType();
	
protected:
	COLORREF m_clrLine;
	UINT m_uLineWidth;
	dustring m_strLineType;
	duPoint m_ptStart;
	duPoint m_ptEnd;
};
