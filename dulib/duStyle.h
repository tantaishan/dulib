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

class duResManager;

// duStyleBase
//
//
class duStyleBase
{
public:
	virtual int GetType() = 0;
	virtual void Draw(HDC hDC, LPRECT lpDstRect, LPCTSTR lpszText, int nAlpha) = 0;
	virtual BOOL OnCreate(TiXmlElement *pElement);
	
	virtual int GetLeft() { return m_nLeft; }
	virtual void SetLeft(int nLeft) { m_nLeft = nLeft; }
	virtual int GetTop() { return m_nTop; }
	virtual void SetTop(int nTop) { m_nTop = nTop; }
	virtual int GetRight() { return m_nRight; }
	virtual void SetRight(int nRight) { m_nRight = nRight; }
	virtual int GetBottom() { return m_nBottom; }
	virtual void SetBottom(int nBottom) { m_nBottom = nBottom; }
	virtual int GetWidth() { return m_nWidth; }
	virtual void SetWidth(int nWidth) { m_nWidth = nWidth; }
	virtual int GetHeight() { return m_nHeight; }
	virtual void SetHeight(int nHeight) { m_nHeight = nHeight; }
	virtual UINT GetState() { return m_uState; }
	virtual void SetState(UINT uState) { m_uState = uState; }

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duStyleBase"); }

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
	virtual int GetType() { return STYLE_IMAGE; }
	virtual void Draw(HDC hDC, LPRECT lpDstRect, LPCTSTR lpszText, int nAlpha);
	
	virtual BOOL OnCreate(TiXmlElement *pElement);
	
	virtual UINT GetLeftPart() { return m_uLeftPart; }
	virtual void SetLeftPart(UINT uLeftPart) { m_uLeftPart = uLeftPart; }
	virtual UINT GetTopPart() { return m_uTopPart; }
	virtual void SetTopPart(UINT uTopPart) { m_uTopPart = uTopPart; }
	virtual UINT GetRightPart() { return m_uRightPart; }
	virtual void SetRightPart(UINT uRightPart) { m_uRightPart = uRightPart; }
	virtual UINT GetBottomPart() { return m_uBottomPart; }
	virtual void SetBottomPart(UINT uBottomPart) { m_uBottomPart = uBottomPart; }

	virtual UINT GetSelectLeft() { return m_uSelectLeft; }
	virtual void SetSelectLeft(UINT uSelectLeft) { m_uSelectLeft = uSelectLeft; }
	virtual UINT GetSelectTop() { return m_uSelectTop; }
	virtual void SetSelectTop(UINT uSelectTop) { m_uSelectTop = uSelectTop; }
	virtual UINT GetSelectWidth() { return m_uSelectWidth; }
	virtual void SetSelectWidth(UINT uSelectWidth) { m_uSelectWidth = uSelectWidth; }
	virtual UINT GetSelectHeight() { return m_uSelectHeight; }
	virtual void SetSelectHeight(UINT uSelectHeight) { m_uSelectHeight = uSelectHeight; }
	
	virtual LPCTSTR GetPicFile() { return m_strPicFile.c_str(); }
	virtual void SetPicFile(LPCTSTR lpszName);

	virtual LPCTSTR GetPaintMode() { return m_strPaintMode.c_str(); }
	virtual void SetPaintMode(LPCTSTR lpszPaintMode);

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duImageStyle"); }

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
	virtual int GetType() { return STYLE_TEXT; }
	virtual void Draw(HDC hDC, LPRECT lpDstRect, LPCTSTR lpszext, int nAlpha);
	
	virtual BOOL OnCreate(TiXmlElement *pElement);
	
	virtual HFONT GetHFont();
	virtual void CalcTextRect(LPCTSTR lpszText, LPSIZE lpOutSize);

	virtual LPCTSTR GetFont() { return m_strFont.c_str(); }
	virtual void SetFont(LPCTSTR lpszFontName);
	virtual COLORREF GetTextColor() { return m_clrText; }
	virtual void SetTextColor(COLORREF clrText) { m_clrText = clrText; }
	virtual BOOL IsSingleLine() { return m_fSingleline; }
	virtual void SetSingleLine(BOOL fSingleLine) { m_fSingleline = fSingleLine; }
	virtual LPCTSTR GetHAlign() { return m_strHorz.c_str(); }
	virtual void SetHAlign(LPCTSTR lpszHorzAlign);
	virtual LPCTSTR GetVAlign() { return m_strVert.c_str(); }
	virtual void SetVAlign(LPCTSTR lpszVertAlign);

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duTextStyle"); }

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
	virtual int GetType() { return STYLE_RECT; }
	virtual void Draw(HDC hDC, LPRECT lpDstRect, LPCTSTR lpszText, int nAlpha);
	
	virtual BOOL OnCreate(TiXmlElement *pElement);

	virtual COLORREF GetFillColor() { return m_clrFill; }
	virtual void SetFillColor(COLORREF clr) { m_clrFill = clr; }
	virtual COLORREF GetBorderColor() { return m_clrBorder; }
	virtual void SetBorderColor(COLORREF clr) { m_clrBorder = clr; }
	virtual BOOL IsFillRect() { return m_fFill; }
	virtual void SetFillRect(BOOL fFill) { m_fFill = fFill; }
	virtual BOOL IsDrawBorder() { return m_fBorder; }
	virtual void SetDrawBorder(BOOL fBorder) { m_fBorder = fBorder; }
	virtual LPCTSTR GetBorderType() { return m_strPenType.c_str(); }
	virtual void SetBorderType(LPCTSTR lpszBorderType);
	virtual UINT GetBorderWidth() { return m_uPenWidth; }
	virtual void SetBorderWidth(UINT uBorderWidth) { m_uPenWidth = uBorderWidth; }

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duRectStyle"); }

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
	virtual int GetType() { return STYLE_LINE; }
	virtual void Draw(HDC hDC, LPRECT lpDstRect, LPCTSTR lpszText, int nAlpha);
	
	virtual BOOL OnCreate(TiXmlElement *pElement);
	
	virtual COLORREF GetLineColor() { return m_clrLine; }
	virtual void SetLineColor(COLORREF clr) { m_clrLine = clr; }
	virtual UINT GetLineWidth() { return m_uLineWidth; }
	virtual void SetLineWidth(UINT uLineWidth) { m_uLineWidth = uLineWidth; }
	virtual LPCTSTR GetLineType() { return m_strLineType.c_str(); }
	virtual void SetLineType(LPCTSTR lpszLineType);
	virtual BOOL GetStartPoint(LPPOINT lpOutPoint);
	virtual void SetStartPoint(LPPOINT lpStartPoint);
	virtual BOOL GetEndPoint(LPPOINT lpOutPoint);
	virtual void SetEndPoint(LPPOINT lpEndPoint);

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duLineStyle"); }

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
