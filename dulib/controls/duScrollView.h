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
	virtual HRESULT QueryInterface(REFIID iid, void ** ppvObject) = 0;
	virtual ULONG AddRef(void) = 0;
	virtual ULONG Release(void) = 0;
	
	// IScrollViewImpl
	virtual void DrawObject(HDC hDC) = 0;
	virtual void GetViewSize(LPSIZE lpSize) = 0;
};
*/

class duScrollView : public duPlugin
{
public:
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);

	virtual void Resize(LPRECT lpRect/*=NULL*/);
	virtual void OnMouseLeave(POINT pt) {}
	virtual void OnMouseLDown(POINT pt);
	virtual void OnMouseLUp(POINT pt);
	virtual void OnMouseRDown(POINT pt);
	virtual void OnMouseRUp(POINT pt);
	virtual void OnMouseHover(POINT pt);
	virtual void OnMouseMove(POINT pt);
	virtual void OnMouseDbClick(POINT pt);
	virtual void OnMouseWheel(UINT fwKeys, int zDelta, POINT pt);
	virtual void OnCaptureChanged(duPlugin *newCaptured) {}
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {}
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {}
	virtual void OnVScroll(ScrollBarAction sbAction, int nPos);
	virtual void OnHScroll(ScrollBarAction sbAction, int nPos);
	virtual void OnChildShow(duPlugin *pChild, BOOL fVisible);
	virtual BOOL OnSetCursor();
	
	virtual LPCTSTR GetHorzScroll() { return m_szHorzScroll; }
	virtual LPCTSTR GetVertScroll() { return m_szVertScroll; }
	virtual BOOL UpdateScroll();
	virtual void DrawView(HDC hDC);
	virtual void GetViewSize(LPSIZE lpSize);
	
	virtual void OnViewMouseLDown(POINT pt) {}
	virtual void OnViewMouseLUp(POINT pt) {}
	virtual void OnViewMouseRDown(POINT pt) {}
	virtual void OnViewMouseRUp(POINT pt) {}
	virtual void OnViewMouseHover(POINT pt) {}
	virtual void OnViewMouseMove(POINT pt) {}
	virtual void OnViewMouseDbClick(POINT pt)  {}
	virtual BOOL OnViewSetCursor(POINT pt) { return FALSE; }

	virtual void SetHorzScrollBar(LPCTSTR lpszScroll);
	virtual void SetVertScrollBar(LPCTSTR lpszScroll);
	
	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duScrollView"); }	

public:
	duScrollView(void);
	~duScrollView(void);
	
public:
	
	POINT GetViewPoint();
	duRect _GetClientRect();
	
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
