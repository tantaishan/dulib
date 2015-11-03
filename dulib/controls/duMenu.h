//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duMenu.h
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

class duMenuItem;

// duMenu
//
//
class duMenu : public duPlugin
{
public:
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);

	virtual void AdjustChild();
	virtual HWND GetMenuHandle() { return m_hWndMenu; }
	virtual void SetMenuHandle(HWND hWndMenu) { m_hWndMenu = hWndMenu; }

	virtual duMenu *GetParentMenu() { return m_pParentMenu; }
	virtual void SetParentMenu(duMenu *pMenu) { m_pParentMenu = pMenu; }

	virtual HWND GetOwnerHwnd() { return m_hWndOwner; }
	virtual void SetOwnerHwnd(HWND hWndOwner) { m_hWndOwner = hWndOwner; }

	virtual duMenuItem *MenuItemFromClientPoint(LPPOINT ppt);
	virtual duMenu *GetNextPopupMenu();
	virtual duMenuItem *GetHotMenu();

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duMenu"); }

protected:
	
	duPlugin *GetPreVisible(duPlugin *pPlugin);
	
public:
	duMenu();
	~duMenu();

public:
	LRESULT NotifyMenuUser(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	HWND m_hWndMenu;
	HWND m_hWndOwner;
	duMenu *m_pParentMenu;
};
