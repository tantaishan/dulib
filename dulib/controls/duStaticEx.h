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
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

typedef struct tagStaticTextInfo
{
	int iCy;
	int iCHeight;
	dustring strText;
}duStaticTextInfo;

// duStaticEx
//
//
class duStaticEx : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);

	virtual void WINAPI SetText(LPCTSTR lpszText);

	virtual void WINAPI Resize(LPRECT lpRect);

	virtual LPCTSTR WINAPI GetFont();
	virtual void WINAPI SetFont(LPCTSTR lpszFont);

	virtual COLORREF WINAPI GetTextColor();
	virtual void WINAPI SetTextColor(COLORREF clrText);


	virtual int WINAPI GetLineSpace();
	virtual void WINAPI SetLineSpace(int nLineSpace);

	virtual int WINAPI CalcTextCache();

	virtual int WINAPI GetLineCount() { return  m_vtTextCache.size();}
	virtual LPCTSTR WINAPI GetLineText(int index);
	virtual void WINAPI GetLineTextRect(int index, LPSIZE lpOutSize);
	
	
	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duStaticEx"); }	
	
public:
	duStaticEx();
	~duStaticEx();

protected:
	void StDrawText(HDC hDC, LPTSTR lpszText, int len);
	void ClearTextCache();
	

protected:
	TCHAR m_szFont[MAX_NAME];
	int m_nLineSpace;
	COLORREF m_clrText;
	BOOL m_fAutoSize;
	
protected:
	vector<duStaticTextInfo *> m_vtTextCache;
};
