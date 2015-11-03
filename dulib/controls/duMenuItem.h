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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);

	virtual void OnMouseIn(POINT pt);
	virtual void OnMouseLeave(POINT pt);
	virtual void OnMouseLDown(POINT pt);
	virtual void OnMouseLUp(POINT pt);
	virtual void OnMouseDbClick(POINT pt);
	
	virtual LPCTSTR GetChildMenu() { return m_szChildMenu; }
	virtual void SetChildMenu(LPCTSTR lpszMenu);
	
	virtual LPCTSTR GetIcon() { return m_szIcon; }
	virtual void SetIcon(LPCTSTR lpszIcon);
	
	virtual int GetIconX() { return m_nIconX; }
	virtual void SetIconX(int nIconX) { m_nIconX = nIconX; }

	virtual int GetIconY() { return m_nIconY; }
	virtual void SetIconY(int nIconY) { m_nIconY = nIconY; }

	virtual int GetIconWidth() { return m_nIconWidth; }
	virtual void SetIconWidth(int nIconWidth) { m_nIconWidth = nIconWidth; }

	virtual int GetIconHeight() { return m_nIconHeight; }
	virtual void SetIconHeight(int nIconHeight) { m_nIconHeight = nIconHeight; }

	virtual BOOL IsPopupItem();

	virtual duMenu *GetPopupMenu() { return m_pPopupMenu; }
	virtual void SetPopupMenu(duMenu *pMenu) { m_pPopupMenu = pMenu; }

	virtual LPCTSTR GetPopupWindow() { return m_szPopupWindow; }
	virtual void SetPopupWindow(LPCTSTR lpszWin);

	virtual void SetDisable(BOOL fDisable);
	
	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duMenuItem"); }	

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
