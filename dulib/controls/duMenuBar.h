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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual LRESULT OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void DrawObject(HDC hDC);

	virtual void OnMouseMove(POINT pt);
	virtual void OnMouseLeave(POINT pt);
	virtual void OnMouseLDown(POINT pt);

	virtual LPCTSTR GetItemStyle();
	virtual void SetItemStyle(LPCTSTR lpszItemStyle);

	virtual int GetItemFixWidth();
	virtual void SetItemFixWidth(int nFixWidth);

	virtual HMENU GetHMenu();
	virtual void SetHMenu(HMENU hMenu);
	
	virtual int GetMenuItemCount() { return (int)m_vtMenuItem.size(); }

	virtual void  RefreshMenuBar();

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duMenuBar"); }

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
