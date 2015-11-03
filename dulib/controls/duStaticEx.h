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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);

	virtual void SetText(LPCTSTR lpszText);

	virtual void Resize(LPRECT lpRect);

	virtual LPCTSTR GetFont();
	virtual void SetFont(LPCTSTR lpszFont);

	virtual COLORREF GetTextColor();
	virtual void SetTextColor(COLORREF clrText);


	virtual int GetLineSpace();
	virtual void SetLineSpace(int nLineSpace);

	virtual int CalcTextCache();

	virtual int GetLineCount() { return  m_vtTextCache.size();}
	virtual LPCTSTR GetLineText(int index);
	virtual void GetLineTextRect(int index, LPSIZE lpOutSize);
	
	
	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duStaticEx"); }	
	
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
