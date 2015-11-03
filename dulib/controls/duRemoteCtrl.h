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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);

	virtual void Resize(LPRECT lpRect/*=NULL*/);
	virtual void OnVScroll(ScrollBarAction sbAction, int nPos);
	virtual void OnHScroll(ScrollBarAction sbAction, int nPos);

	virtual LPCTSTR GetHorzScroll() { return m_szHorzScroll; }
	virtual void SetHorzScrollBar(LPCTSTR lpszScroll);

	virtual LPCTSTR GetVertScroll() { return m_szVertScroll; }
	virtual void SetVertScrollBar(LPCTSTR lpszScroll);

	virtual LPCTSTR GetHwndObj() { return m_szHwndObj; }
	virtual void SetHwndObj(LPCTSTR lpszHwndObj);

	virtual int GetViewWidth() { return m_nViewWidth; }
	virtual int GetViewHeight() { return m_nViewHeight; }
	virtual void SetViewSize(int nWidth, int nHeight);

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duRemoteCtrl"); }

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
