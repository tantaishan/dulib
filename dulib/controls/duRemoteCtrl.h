//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duRemoteCtrl.h
//
//  History:    Mar-08-2011   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"
#include "duScroll.h"
#include "duDrawHelper.h"

class duRemoteCtrl : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);

	virtual void WINAPI Resize(LPRECT lpRect/*=NULL*/);
	virtual void WINAPI OnVScroll(ScrollBarAction sbAction, int nPos);
	virtual void WINAPI OnHScroll(ScrollBarAction sbAction, int nPos);

	virtual LPCTSTR WINAPI GetHorzScroll() { return m_szHorzScroll; }
	virtual void WINAPI SetHorzScrollBar(LPCTSTR lpszScroll);

	virtual LPCTSTR WINAPI GetVertScroll() { return m_szVertScroll; }
	virtual void WINAPI SetVertScrollBar(LPCTSTR lpszScroll);

	virtual LPCTSTR WINAPI GetHwndObj() { return m_szHwndObj; }
	virtual void WINAPI SetHwndObj(LPCTSTR lpszHwndObj);

	virtual int WINAPI GetViewWidth() { return m_nViewWidth; }
	virtual int WINAPI GetViewHeight() { return m_nViewHeight; }
	virtual void WINAPI SetViewSize(int nWidth, int nHeight);

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duRemoteCtrl"); }

public:
	duRemoteCtrl(void);
	~duRemoteCtrl(void);

protected:
	TCHAR m_szVertScroll[MAX_NAME];
	TCHAR m_szHorzScroll[MAX_NAME];
	TCHAR m_szHwndObj[MAX_NAME];
	
protected:
	int m_nXOffset;
	int m_nYOffset;
	int m_nViewWidth;
	int m_nViewHeight;
};
