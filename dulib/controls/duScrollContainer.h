//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duScrollContainer.h
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

// duScrollContainer
//
//
class duScrollContainer : public duPlugin
{
public:
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);

	virtual void Resize(LPRECT lpRect/*=NULL*/);
	virtual void OnMouseWheel(UINT fwKeys, int zDelta, POINT pt);
	virtual void OnVScroll(ScrollBarAction sbAction, int nPos);
	virtual void OnHScroll(ScrollBarAction sbAction, int nPos);
	//virtual void OnChildShow(duPlugin *pChild, BOOL fVisible);
	virtual LRESULT OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	virtual LPCTSTR GetHorzScroll() { return m_szHorzScroll; }
	virtual LPCTSTR GetVertScroll() { return m_szVertScroll; }
	virtual LPCTSTR GetView() { return m_szView; }
	virtual BOOL UpdateScroll();
	virtual void GetViewSize(LPSIZE lpSize);

	virtual void SetHorzScrollBar(LPCTSTR lpszScroll);
	virtual void SetVertScrollBar(LPCTSTR lpszScroll);
	virtual void SetView(LPCTSTR lpszView);
	
	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duScrollContainer"); }	

public:
	duScrollContainer(void);
	~duScrollContainer(void);
	
public:
	POINT GetViewPoint();
	void SetViewTop(int nTop);
	void SetViewLeft(int nLeft);

protected:
	TCHAR m_szVertScroll[MAX_NAME];
	TCHAR m_szHorzScroll[MAX_NAME];
	TCHAR m_szView[MAX_NAME];
	
protected:
	BOOL m_fHorz;
	BOOL m_fVert;
	int m_nXOffset;
	int m_nYOffset;
	BOOL m_fDispMouseWheel;
};
