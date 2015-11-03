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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);
	
	virtual void OnMouseMove(POINT pt);
	virtual void OnMouseLeave(POINT pt);
	virtual void OnMouseLDown(POINT pt);
	virtual void OnMouseLUp(POINT pt);
	virtual void OnMouseDbClick(POINT pt);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnSetCursor();
	
	virtual int GetPageCount();
	virtual int GetSel();
	virtual void SetSel(int nSel);
	
	virtual int GetTotal() { return m_nTotalCount; }
	virtual void SetTotal(int nTotal);
	
	virtual int GetPerPage() { return m_nPerPage; }
	virtual void SetPerPage(int nPerPage);
	
	virtual int GetArrowWidth() { return m_nArrowWidth; }
	virtual void SetArrowWidth(int nArrowWidth) { m_nArrowWidth = nArrowWidth; AutoSize(); }
	
	virtual int GetArrowHeight() { return m_nArrowHeight; }
	virtual void SetArrowHeight(int nArrowHeight) { m_nArrowHeight = nArrowHeight; AutoSize(); }
	
	virtual int GetArrowButtonSpace() { return m_nArrowButtonSpace; }
	virtual void SetArrowButtonSpace(int nSpace) { m_nArrowButtonSpace = nSpace; AutoSize(); }
	
	virtual int GetButtonWidth() { return m_nButtonWidth; }
	virtual void SetButtonWidth(int nButtonWidth) { m_nButtonWidth = nButtonWidth; AutoSize(); }
	
	virtual int GetButtonHeight() { return m_nButtonHeight; }
	virtual void SetButtonHeight(int nButtonHeight) { m_nButtonHeight = nButtonHeight; AutoSize(); }
	
	virtual int GetButtonSpace() { return m_nButtonSpace; }
	virtual void SetButtonSpace(int nSpace) { m_nButtonSpace = nSpace; AutoSize(); }
	
	virtual LPCTSTR GetLeftArrowStyle() { return m_szLeftArrowStyle; }
	virtual void SetLeftArrowStyle(LPCTSTR lpszStyle);
	
	virtual LPCTSTR GetRightArrowStyle() { return m_szRightArrowStyle; }
	virtual void SetRightArrowStyle(LPCTSTR lpszStyle);
	
	virtual LPCTSTR GetButtonStyle() { return m_szButtonStyle; }
	virtual void SetButtonStyle(LPCTSTR lpszStyle);
	
	virtual BOOL MoveLeft(BOOL fRedraw);
	virtual BOOL MoveRight(BOOL fRedraw);

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duPage"); }	

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

