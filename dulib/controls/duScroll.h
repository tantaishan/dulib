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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);
	virtual void OnMouseLeave(POINT pt);
	virtual void OnMouseLDown(POINT pt);
	virtual void OnMouseLUp(POINT pt);
	virtual void OnMouseMove(POINT pt);
	virtual void OnMouseWheel(UINT fwKeys, int zDelta, POINT pt);
	virtual void OnTimer(UINT nEventId);
	virtual void Resize(LPRECT lpNewRect);
	virtual void RefreshScrollBar();

	virtual int  GetMaxPos() { return m_nMaxPos; }
	virtual void SetMaxPos(int nMaxPos);
	virtual int  GetPos() { return m_nPos; }
	virtual void SetPos(int nPos);
	virtual int  GetPage() { return m_nPage; }
	virtual void SetPage(int nPage);
	virtual int  GetArrowHeight() { return m_nArrowHeight; }
	virtual void SetArrowHeight(int nArrowHeight);
	virtual BOOL IsVert() { return m_fVert; }
	virtual void SetVert(BOOL bVert);
	virtual int  GetStep() { return m_nStep; }
	virtual void SetStep(int nStep);
	virtual LPCTSTR GetUpStyle() { return m_szUpStyle; }
	virtual void SetUpStyle(LPCTSTR lpszStyle);
	virtual LPCTSTR GetDownStyle() { return m_szDownStyle; }
	virtual void SetDownStyle(LPCTSTR lpszStyle);
	virtual LPCTSTR GetThumbStyle() { return m_szThumbStyle; }
	virtual void SetThumbStyle(LPCTSTR lpszStyle);
	
	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duScroll"); }	

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
