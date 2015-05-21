//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duMenuBar.h
//
//  History:    MAR-27-2010   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

typedef struct tagMENUBARITEM {
	int iPosition;
	UINT uState;
	TCHAR szItem[MAX_NAME];
}MenuBarItem;

// duMenuBar
//
//
class duMenuBar : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual LRESULT WINAPI OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void WINAPI DrawObject(HDC hDC);

	virtual void WINAPI OnMouseMove(POINT pt);
	virtual void WINAPI OnMouseLeave(POINT pt);
	virtual void WINAPI OnMouseLDown(POINT pt);

	virtual LPCTSTR WINAPI GetItemStyle();
	virtual void WINAPI SetItemStyle(LPCTSTR lpszItemStyle);

	virtual int WINAPI GetItemFixWidth();
	virtual void WINAPI SetItemFixWidth(int nFixWidth);

	virtual HMENU WINAPI GetHMenu();
	virtual void WINAPI SetHMenu(HMENU hMenu);
	
	virtual int WINAPI GetMenuItemCount() { return (int)m_vtMenuItem.size(); }

	virtual void  WINAPI RefreshMenuBar();

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duMenuBar"); }

public:
	duMenuBar(void);
	~duMenuBar(void);

protected:
	void  Destroy();
	UINT  CalcItemWidth(UINT nState, LPCTSTR lpszText);

	
	void OnEnterIdle(UINT uWhy, HWND hWnd);
	MenuBarItem *GetHotItemByPoint(POINT &pointClient);
	duRect GetItemRect(int iPosition);

protected:
	TCHAR m_szItemStyle[MAX_NAME];
	int m_nItemFixWidth;

private:
	HMENU m_hMenu;
	BOOL m_fTracking;
	BOOL m_fMenuPopuped;
	vector<MenuBarItem *> m_vtMenuItem;
	MenuBarItem *m_pHotItem;
	MenuBarItem *m_pSelItem;
};
