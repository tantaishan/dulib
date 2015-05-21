//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duPage.h
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

// duPage
// 
// 
class duPage : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);
	
	virtual void WINAPI OnMouseMove(POINT pt);
	virtual void WINAPI OnMouseLeave(POINT pt);
	virtual void WINAPI OnMouseLDown(POINT pt);
	virtual void WINAPI OnMouseLUp(POINT pt);
	virtual void WINAPI OnMouseDbClick(POINT pt);
	virtual void WINAPI OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL WINAPI OnSetCursor();
	
	virtual int WINAPI GetPageCount();
	virtual int WINAPI GetSel();
	virtual void WINAPI SetSel(int nSel);
	
	virtual int WINAPI GetTotal() { return m_nTotalCount; }
	virtual void WINAPI SetTotal(int nTotal);
	
	virtual int WINAPI GetPerPage() { return m_nPerPage; }
	virtual void WINAPI SetPerPage(int nPerPage);
	
	virtual int WINAPI GetArrowWidth() { return m_nArrowWidth; }
	virtual void WINAPI SetArrowWidth(int nArrowWidth) { m_nArrowWidth = nArrowWidth; AutoSize(); }
	
	virtual int WINAPI GetArrowHeight() { return m_nArrowHeight; }
	virtual void WINAPI SetArrowHeight(int nArrowHeight) { m_nArrowHeight = nArrowHeight; AutoSize(); }
	
	virtual int WINAPI GetArrowButtonSpace() { return m_nArrowButtonSpace; }
	virtual void WINAPI SetArrowButtonSpace(int nSpace) { m_nArrowButtonSpace = nSpace; AutoSize(); }
	
	virtual int WINAPI GetButtonWidth() { return m_nButtonWidth; }
	virtual void WINAPI SetButtonWidth(int nButtonWidth) { m_nButtonWidth = nButtonWidth; AutoSize(); }
	
	virtual int WINAPI GetButtonHeight() { return m_nButtonHeight; }
	virtual void WINAPI SetButtonHeight(int nButtonHeight) { m_nButtonHeight = nButtonHeight; AutoSize(); }
	
	virtual int WINAPI GetButtonSpace() { return m_nButtonSpace; }
	virtual void WINAPI SetButtonSpace(int nSpace) { m_nButtonSpace = nSpace; AutoSize(); }
	
	virtual LPCTSTR WINAPI GetLeftArrowStyle() { return m_szLeftArrowStyle; }
	virtual void WINAPI SetLeftArrowStyle(LPCTSTR lpszStyle);
	
	virtual LPCTSTR WINAPI GetRightArrowStyle() { return m_szRightArrowStyle; }
	virtual void WINAPI SetRightArrowStyle(LPCTSTR lpszStyle);
	
	virtual LPCTSTR WINAPI GetButtonStyle() { return m_szButtonStyle; }
	virtual void WINAPI SetButtonStyle(LPCTSTR lpszStyle);
	
	virtual BOOL WINAPI MoveLeft(BOOL fRedraw);
	virtual BOOL WINAPI MoveRight(BOOL fRedraw);

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duPage"); }	

public:
	duPage();
	~duPage();
	
protected:
	int CalcPageWidth();
	void AutoSize();
	
protected:
	UINT m_uLeftArrowState;
	UINT m_uRightArrowState;
	vector<UINT> m_vtButtonState;
	int m_nHotButton;

protected:
	int m_nTotalCount;
	int m_nPerPage;
	int m_nArrowWidth;
	int m_nArrowHeight;
	int m_nArrowButtonSpace;
	int m_nButtonWidth;
	int m_nButtonHeight;
	int m_nButtonSpace;
	duRect m_rcLeftArrowBtn;
	duRect m_rcRightArrowBtn;
	TCHAR m_szLeftArrowStyle[MAX_NAME];
	TCHAR m_szRightArrowStyle[MAX_NAME];
	TCHAR m_szButtonStyle[MAX_NAME];
};

