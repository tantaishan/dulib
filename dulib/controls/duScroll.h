//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duScroll.h
//
//  History:    Dec-13-2009   Steven Li  Created
//
//--------------------------------------------------------------------------
#pragma once

#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

enum ScrollBarHit
{
	NO_WHERE  = 0,
	UP_RECT = 1,
	PAGE1_RECT = 2,
	THUMB_RECT = 3,
	PAGE2_RECT = 4,
	DOWN_RECT = 5
};


class duScroll : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);
	virtual void WINAPI OnMouseLeave(POINT pt);
	virtual void WINAPI OnMouseLDown(POINT pt);
	virtual void WINAPI OnMouseLUp(POINT pt);
	virtual void WINAPI OnMouseMove(POINT pt);
	virtual void WINAPI OnMouseWheel(UINT fwKeys, int zDelta, POINT pt);
	virtual void WINAPI OnTimer(UINT nEventId);
	virtual void WINAPI Resize(LPRECT lpNewRect);
	virtual void WINAPI RefreshScrollBar();

	virtual int  WINAPI GetMaxPos() { return m_nMaxPos; }
	virtual void WINAPI SetMaxPos(int nMaxPos);
	virtual int  WINAPI GetPos() { return m_nPos; }
	virtual void WINAPI SetPos(int nPos);
	virtual int  WINAPI GetPage() { return m_nPage; }
	virtual void WINAPI SetPage(int nPage);
	virtual int  WINAPI GetArrowHeight() { return m_nArrowHeight; }
	virtual void WINAPI SetArrowHeight(int nArrowHeight);
	virtual BOOL WINAPI IsVert() { return m_fVert; }
	virtual void WINAPI SetVert(BOOL bVert);
	virtual int  WINAPI GetStep() { return m_nStep; }
	virtual void WINAPI SetStep(int nStep);
	virtual LPCTSTR WINAPI GetUpStyle() { return m_szUpStyle; }
	virtual void WINAPI SetUpStyle(LPCTSTR lpszStyle);
	virtual LPCTSTR WINAPI GetDownStyle() { return m_szDownStyle; }
	virtual void WINAPI SetDownStyle(LPCTSTR lpszStyle);
	virtual LPCTSTR WINAPI GetThumbStyle() { return m_szThumbStyle; }
	virtual void WINAPI SetThumbStyle(LPCTSTR lpszStyle);
	
	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duScroll"); }	

public:
	duScroll(void);
	~duScroll(void);

public:
	void _MouseIn(ScrollBarHit sbHit);
	void _MouseLeave(ScrollBarHit sbHit);
	void _MouseDown(POINT pt);
	void _MouseUp(POINT pt);
	void _MouseMove(POINT pt);
	void _MouseWheel(int nDelta);
	void _OnTimer();
	void CalcRect(duRect rcScroll);
	int PtToPage(POINT pt, duRect rcScroll);
	void CalcRectByPt(POINT pt, duRect rcScroll);
	ScrollBarHit HitTest(POINT pt);
	void NotifyParent(UINT uMsg, ScrollBarAction sbAction, int nPos);
	
protected:
	TCHAR m_szUpStyle[MAX_NAME];
	TCHAR m_szDownStyle[MAX_NAME];
	TCHAR m_szThumbStyle[MAX_NAME];
	int m_nMaxPos;
	int m_nPos;
	int m_nPage;
	int m_nArrowHeight;
	BOOL m_fVert;
	int m_nStep;

protected:
	duRect m_rectUp;
	duRect m_rectDown;
	duRect m_rectPage1;
	duRect m_rectPage2;
	duRect m_rectThumb;
	BOOL m_fTracking;
	duPoint m_ptTrack;
	int m_nTrackPos;
	UINT m_uUpState;
	UINT m_uDownState;
	UINT m_uThumbState;
	ScrollBarHit m_nLastActiveRect;
	int m_nLimitPage;
	int m_nOldPos;
	int m_nTimerid;
	int m_nMinThumbSize;
};
