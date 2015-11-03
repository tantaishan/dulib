//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duMenuBar.cpp
//
//  History:    MAR-27-2010   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "duMenuBar.h"

duMenuBar::duMenuBar(void) :
	 m_hMenu(NULL)
	,m_fTracking(FALSE)
	,m_nItemFixWidth(0)
	,m_pHotItem(NULL)
	,m_pSelItem(NULL)
	,m_fMenuPopuped(FALSE)
{
	ZeroMemory(m_szItemStyle, MAX_NAME * sizeof(TCHAR));
	m_vtMenuItem.clear();
}

duMenuBar::~duMenuBar(void)
{
	Destroy();
	Plugin_UnHookWindowMessage(this);
}

//////////////////////////////////////////////////////////////////////////
void duMenuBar::RegisterControlProperty()
{
	RegisterProperty(_T("itemstyle"),    DU_PROPERTY_STYLEGROUP, m_szItemStyle);
	RegisterProperty(_T("itemfixwidth"),  DU_PROPERTY_LONG, &m_nItemFixWidth);
}

void duMenuBar::OnCreate()
{
	Plugin_HookWindowMessage(this);
}

LRESULT duMenuBar::OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_ENTERIDLE)
		OnEnterIdle((UINT)wParam, (HWND)lParam);

	return 0;
}

void duMenuBar::DrawObject(HDC hDC)
{
	duRect rectMenuBar;
	Plugin_GetRect(this, &rectMenuBar);
	rectMenuBar.OffsetRect(-rectMenuBar.left, -rectMenuBar.top);

	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
		pStyleGroup->Draw(hDC, &rectMenuBar, GetState(), GetText(), GetAlpha());

	pStyleGroup = (duStyleGroup *)GetResObj(m_szItemStyle, DU_RES_STYLEGROUP);
	if (pStyleGroup == NULL)
		return;
	
	duRect rectMenuItem;
	rectMenuItem.SetRectEmpty();
	rectMenuItem.bottom = rectMenuBar.Height();
	rectMenuItem.right = 0;

	int i;
	int nMenuItemCount = GetMenuItemCount();
	for (i = 0;i < nMenuItemCount; i++)
	{
		MenuBarItem *pItem = m_vtMenuItem[i];
		if (pItem)
		{
			rectMenuItem.left = rectMenuItem.right;
			rectMenuItem.right = rectMenuItem.left + CalcItemWidth(pItem->uState, pItem->szItem) + m_nItemFixWidth;

			pStyleGroup->Draw(hDC, &rectMenuItem, pItem->uState, pItem->szItem, GetAlpha());
		}
	}
}

void duMenuBar::OnMouseMove(POINT pt)
{
	POINT pointClient(pt);
	duRect rectItem;
	rectItem.SetRectEmpty();

	MenuBarItem *pHotItem = GetHotItemByPoint(pointClient);
	if (pHotItem == NULL && m_pHotItem)
	{
		m_pHotItem->uState = DU_STATE_NORMAL;
		m_pHotItem = NULL;
		Plugin_Redraw(this, TRUE);
	}
	else if (pHotItem && pHotItem != m_pHotItem)
	{
		pHotItem->uState = DU_STATE_OVER;
		if (m_pHotItem)
			m_pHotItem->uState = DU_STATE_NORMAL;

		m_pHotItem = pHotItem;
		Plugin_Redraw(this, TRUE);
	}
}

void duMenuBar::OnMouseLeave(POINT pt)
{
	if (m_pHotItem && !m_fMenuPopuped)
	{
		m_pHotItem->uState = DU_STATE_NORMAL;
		m_pHotItem = NULL;
		Plugin_Redraw(this, TRUE);
	}
}

void duMenuBar::OnMouseLDown(POINT pt)
{
	if (m_pHotItem == NULL)
		return;
	
	m_pHotItem->uState = DU_STATE_PRESS;
	Plugin_Redraw(this, TRUE);

	do
	{
		m_fMenuPopuped = TRUE;
		m_fTracking = FALSE;

		duRect rectItem = GetItemRect(m_pHotItem->iPosition);

		POINT *pPoint = (POINT *)(RECT *)&rectItem;
		::ClientToScreen(m_hWnd, pPoint);
		pPoint++;
		::ClientToScreen(m_hWnd, pPoint);

		HMENU hMenuPopup = ::GetSubMenu(m_hMenu, m_pHotItem->iPosition);
		if (hMenuPopup == NULL)
			break;

		//NotifyUser(this, DUM_MENUPOPUP, (WPARAM)hMenuPopup, NULL);
		::TrackPopupMenu(hMenuPopup, TPM_LEFTALIGN, rectItem.left, rectItem.bottom, NULL, m_hWnd, NULL);
	}while(m_fTracking);

	m_fMenuPopuped = FALSE;
	OnMouseLeave(pt);
}

void duMenuBar::OnEnterIdle(UINT uWhy, HWND hWnd)
{
	if (uWhy != MSGF_MENU || !m_fMenuPopuped)
		return;

	POINT pt;
	::GetCursorPos(&pt);
	
	duRect rectWin;
	::GetWindowRect(m_hWnd, &rectWin);
	
	pt.x += -rectWin.left;
	pt.y += -rectWin.top;

	MenuBarItem *pItem = GetHotItemByPoint(pt);
	if (pItem && pItem != m_pHotItem)
	{
		m_fTracking = TRUE;

		::EndMenu();

		if (m_pHotItem)
			m_pHotItem->uState = DU_STATE_NORMAL;

		pItem->uState = DU_STATE_PRESS;
		m_pHotItem = pItem;
		Plugin_Redraw(this, TRUE);
	}
}

HMENU duMenuBar::GetHMenu()
{
	return m_hMenu;
}

void  duMenuBar::SetHMenu(HMENU hMenu)
{
	if (m_hMenu)
	{
		::DestroyMenu(m_hMenu);
		m_hMenu = NULL;
	}

	m_hMenu = hMenu;
	RefreshMenuBar();
}

void  duMenuBar::RefreshMenuBar()
{
	if (!::IsMenu(m_hMenu))
		return;
	
	Destroy();
	
	int nItemCount = ::GetMenuItemCount(m_hMenu);
	int i = 0;
	for (i = 0;i < nItemCount; i++)
	{
		MenuBarItem *pMenuItem = new MenuBarItem();
		ZeroMemory(pMenuItem, sizeof(MenuBarItem));

		pMenuItem->iPosition = i;
		pMenuItem->uState = DU_STATE_NORMAL;
		::GetMenuString(m_hMenu, i, pMenuItem->szItem, MAX_NAME, MF_BYPOSITION);

		m_vtMenuItem.push_back(pMenuItem);
	}
}

void  duMenuBar::Destroy()
{
	MenuBarItem *pMenuItem = NULL;
	int i;
	int nMenuItemCount = GetMenuItemCount();
	for (i = 0;i < nMenuItemCount; i++)
	{
		pMenuItem = m_vtMenuItem[i];
		if (pMenuItem)
		{
			delete pMenuItem;
			pMenuItem = NULL;
		}
	}
	m_vtMenuItem.clear();
}

UINT duMenuBar::CalcItemWidth(UINT nState, LPCTSTR lpszText)
{
	if (!lpszText || !*lpszText)
		return 0;

	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(m_szItemStyle, DU_RES_STYLEGROUP);
	if (pStyleGroup)
	{
		duSize size(0, 0);
		pStyleGroup->CalcTextRect(nState, lpszText, &size);

		return size.cx;
	}

	return 0;
}

MenuBarItem *duMenuBar::GetHotItemByPoint(POINT &pointClient)
{
	duRect rectMenuItem;
	Plugin_GetRect(this, &rectMenuItem);
	rectMenuItem.right = rectMenuItem.left;
	
	int i = 0;
	int nMenuItemCount = GetMenuItemCount();
	for (i = 0;i < nMenuItemCount; i++)
	{
		MenuBarItem *pItem = m_vtMenuItem[i];
		if (pItem)
		{
			rectMenuItem.left  = rectMenuItem.right;
			rectMenuItem.right = rectMenuItem.left + CalcItemWidth(pItem->uState, pItem->szItem) + m_nItemFixWidth;

			if (rectMenuItem.PtInRect(pointClient))
				return pItem;
		}
	}

	return NULL;
}

duRect duMenuBar::GetItemRect(int iPosition)
{
	duRect rectMenuItem;
	Plugin_GetRect(this, &rectMenuItem);
	rectMenuItem.right = rectMenuItem.left;

	int i = 0;
	int nMenuItemCount = GetMenuItemCount();
	for (i = 0;i < nMenuItemCount; i++)
	{
		MenuBarItem *pItem = m_vtMenuItem[i];
		if (pItem)
		{
			rectMenuItem.left  = rectMenuItem.right;
			rectMenuItem.right = rectMenuItem.left + CalcItemWidth(pItem->uState, pItem->szItem) + m_nItemFixWidth;

			if (iPosition == pItem->iPosition)
				return rectMenuItem;
		}
	}

	rectMenuItem.SetRectEmpty();
	return rectMenuItem;
}

LPCTSTR duMenuBar::GetItemStyle()
{
	return m_szItemStyle;
}

void  duMenuBar::SetItemStyle(LPCTSTR lpszItemStyle)
{
	if (lpszItemStyle)
		_tcsncpy(m_szItemStyle, lpszItemStyle, MAX_NAME);
	else
		ZeroMemory(m_szItemStyle, MAX_NAME * sizeof(TCHAR));
}

int  duMenuBar::GetItemFixWidth()
{
	return m_nItemFixWidth;
}

void duMenuBar::SetItemFixWidth(int nFixWidth)
{
	m_nItemFixWidth = nFixWidth;
}
