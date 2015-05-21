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
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);

	virtual void WINAPI AdjustChild();
	virtual HWND WINAPI GetMenuHandle() { return m_hWndMenu; }
	virtual void WINAPI SetMenuHandle(HWND hWndMenu) { m_hWndMenu = hWndMenu; }

	virtual duMenu *WINAPI GetParentMenu() { return m_pParentMenu; }
	virtual void WINAPI SetParentMenu(duMenu *pMenu) { m_pParentMenu = pMenu; }

	virtual HWND WINAPI GetOwnerHwnd() { return m_hWndOwner; }
	virtual void WINAPI SetOwnerHwnd(HWND hWndOwner) { m_hWndOwner = hWndOwner; }

	virtual duMenuItem *WINAPI MenuItemFromClientPoint(LPPOINT ppt);
	virtual duMenu *WINAPI GetNextPopupMenu();
	virtual duMenuItem *WINAPI GetHotMenu();

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duMenu"); }

protected:
	
	duPlugin *WINAPI GetPreVisible(duPlugin *pPlugin);
	
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
