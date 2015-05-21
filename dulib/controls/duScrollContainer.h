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
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);

	virtual void WINAPI Resize(LPRECT lpRect/*=NULL*/);
	virtual void WINAPI OnMouseWheel(UINT fwKeys, int zDelta, POINT pt);
	virtual void WINAPI OnVScroll(ScrollBarAction sbAction, int nPos);
	virtual void WINAPI OnHScroll(ScrollBarAction sbAction, int nPos);
	//virtual void WINAPI OnChildShow(duPlugin *pChild, BOOL fVisible);
	virtual LRESULT WINAPI OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	virtual LPCTSTR WINAPI GetHorzScroll() { return m_szHorzScroll; }
	virtual LPCTSTR WINAPI GetVertScroll() { return m_szVertScroll; }
	virtual LPCTSTR GetView() { return m_szView; }
	virtual BOOL WINAPI UpdateScroll();
	virtual void WINAPI GetViewSize(LPSIZE lpSize);

	virtual void WINAPI SetHorzScrollBar(LPCTSTR lpszScroll);
	virtual void WINAPI SetVertScrollBar(LPCTSTR lpszScroll);
	virtual void WINAPI SetView(LPCTSTR lpszView);
	
	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duScrollContainer"); }	

public:
	duScrollContainer(void);
	~duScrollContainer(void);
	
public:
	POINT WINAPI GetViewPoint();
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
