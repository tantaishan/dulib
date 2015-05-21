//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duMenuItem.h
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

class duMenu;

// duMenuItem
// 
// 
class duMenuItem : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);

	virtual void WINAPI OnMouseIn(POINT pt);
	virtual void WINAPI OnMouseLeave(POINT pt);
	virtual void WINAPI OnMouseLDown(POINT pt);
	virtual void WINAPI OnMouseLUp(POINT pt);
	virtual void WINAPI OnMouseDbClick(POINT pt);
	
	virtual LPCTSTR WINAPI GetChildMenu() { return m_szChildMenu; }
	virtual void WINAPI SetChildMenu(LPCTSTR lpszMenu);
	
	virtual LPCTSTR WINAPI GetIcon() { return m_szIcon; }
	virtual void WINAPI SetIcon(LPCTSTR lpszIcon);
	
	virtual int WINAPI GetIconX() { return m_nIconX; }
	virtual void WINAPI SetIconX(int nIconX) { m_nIconX = nIconX; }

	virtual int WINAPI GetIconY() { return m_nIconY; }
	virtual void WINAPI SetIconY(int nIconY) { m_nIconY = nIconY; }

	virtual int WINAPI GetIconWidth() { return m_nIconWidth; }
	virtual void WINAPI SetIconWidth(int nIconWidth) { m_nIconWidth = nIconWidth; }

	virtual int WINAPI GetIconHeight() { return m_nIconHeight; }
	virtual void WINAPI SetIconHeight(int nIconHeight) { m_nIconHeight = nIconHeight; }

	virtual BOOL WINAPI IsPopupItem();

	virtual duMenu *WINAPI GetPopupMenu() { return m_pPopupMenu; }
	virtual void WINAPI SetPopupMenu(duMenu *pMenu) { m_pPopupMenu = pMenu; }

	virtual LPCTSTR WINAPI GetPopupWindow() { return m_szPopupWindow; }
	virtual void WINAPI SetPopupWindow(LPCTSTR lpszWin);

	virtual void WINAPI SetDisable(BOOL fDisable);
	
	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duMenuItem"); }	

public:
	duMenuItem();
	~duMenuItem();

public:
	BOOL IsPress() { return m_fPress; }
	void SetPress(BOOL fPress) { m_fPress = fPress; }
	
	LRESULT NotifyMenuUser(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
protected:
	duMenu *m_pPopupMenu;
	BOOL m_fPress;
	
protected:
	TCHAR m_szPopupWindow[MAX_NAME];
	TCHAR m_szChildMenu[MAX_NAME];
	TCHAR m_szArrow[MAX_NAME];
	int m_nArrowRight;
	TCHAR m_szIcon[MAX_NAME];
	int m_nIconX;
	int m_nIconY;
	int m_nIconWidth;
	int m_nIconHeight;
};
