//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duScrollView.h
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

/*
class IScrollViewImpl : public IUnknown
{
public:
	// IUnknown
	virtual HRESULT WINAPI QueryInterface(REFIID iid, void ** ppvObject) = 0;
	virtual ULONG WINAPI AddRef(void) = 0;
	virtual ULONG WINAPI Release(void) = 0;
	
	// IScrollViewImpl
	virtual void WINAPI DrawObject(HDC hDC) = 0;
	virtual void WINAPI GetViewSize(LPSIZE lpSize) = 0;
};
*/

class duScrollView : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);

	virtual void WINAPI Resize(LPRECT lpRect/*=NULL*/);
	virtual void WINAPI OnMouseLeave(POINT pt) {}
	virtual void WINAPI OnMouseLDown(POINT pt);
	virtual void WINAPI OnMouseLUp(POINT pt);
	virtual void WINAPI OnMouseRDown(POINT pt);
	virtual void WINAPI OnMouseRUp(POINT pt);
	virtual void WINAPI OnMouseHover(POINT pt);
	virtual void WINAPI OnMouseMove(POINT pt);
	virtual void WINAPI OnMouseDbClick(POINT pt);
	virtual void WINAPI OnMouseWheel(UINT fwKeys, int zDelta, POINT pt);
	virtual void WINAPI OnCaptureChanged(duPlugin *newCaptured) {}
	virtual void WINAPI OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {}
	virtual void WINAPI OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {}
	virtual void WINAPI OnVScroll(ScrollBarAction sbAction, int nPos);
	virtual void WINAPI OnHScroll(ScrollBarAction sbAction, int nPos);
	virtual void WINAPI OnChildShow(duPlugin *pChild, BOOL fVisible);
	virtual BOOL WINAPI OnSetCursor();
	
	virtual LPCTSTR WINAPI GetHorzScroll() { return m_szHorzScroll; }
	virtual LPCTSTR WINAPI GetVertScroll() { return m_szVertScroll; }
	virtual BOOL WINAPI UpdateScroll();
	virtual void WINAPI DrawView(HDC hDC);
	virtual void WINAPI GetViewSize(LPSIZE lpSize);
	
	virtual void WINAPI OnViewMouseLDown(POINT pt) {}
	virtual void WINAPI OnViewMouseLUp(POINT pt) {}
	virtual void WINAPI OnViewMouseRDown(POINT pt) {}
	virtual void WINAPI OnViewMouseRUp(POINT pt) {}
	virtual void WINAPI OnViewMouseHover(POINT pt) {}
	virtual void WINAPI OnViewMouseMove(POINT pt) {}
	virtual void WINAPI OnViewMouseDbClick(POINT pt)  {}
	virtual BOOL WINAPI OnViewSetCursor(POINT pt) { return FALSE; }

	virtual void WINAPI SetHorzScrollBar(LPCTSTR lpszScroll);
	virtual void WINAPI SetVertScrollBar(LPCTSTR lpszScroll);
	
	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duScrollView"); }	

public:
	duScrollView(void);
	~duScrollView(void);
	
public:
	
	POINT WINAPI GetViewPoint();
	duRect WINAPI _GetClientRect();
	
	void SetImage(LPCTSTR lpszImage) { _tcsncpy(m_szImage, lpszImage, MAX_NAME); }

protected:
	TCHAR m_szVertScroll[MAX_NAME];
	TCHAR m_szHorzScroll[MAX_NAME];
	TCHAR m_szImage[MAX_NAME];
	
protected:
	BOOL m_fHorz;
	BOOL m_fVert;
	int m_nXOffset;
	int m_nYOffset;
};
