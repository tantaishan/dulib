//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duMenuItem.cpp
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duMenuItem.h"
#include "duDrawHelper.h"
#include "duCtrlManager.h"
#include "duMenu.h"
#include "MenuPopup.h"

duMenuItem::duMenuItem() :
	 m_nIconX(0)
	,m_nIconY(0)
	,m_nIconWidth(0)
	,m_nIconHeight(0)
	,m_pPopupMenu(NULL)
	,m_nArrowRight(0)
	,m_fPress(FALSE)
{
	ZeroMemory(m_szPopupWindow, sizeof(TCHAR) * MAX_NAME);
	ZeroMemory(m_szChildMenu, sizeof(TCHAR) * MAX_NAME);
	ZeroMemory(m_szIcon, sizeof(TCHAR) * MAX_NAME);
	ZeroMemory(m_szArrow, sizeof(TCHAR) * MAX_NAME);
}

duMenuItem::~duMenuItem()
{
	
}

void WINAPI duMenuItem::RegisterControlProperty()
{
	RegisterProperty(_T("icon"), DU_PROPERTY_STRING, m_szIcon);
	RegisterProperty(_T("iconx"), DU_PROPERTY_LONG, &m_nIconX);
	RegisterProperty(_T("icony"), DU_PROPERTY_LONG, &m_nIconY);	
	RegisterProperty(_T("iconwidth"), DU_PROPERTY_LONG, &m_nIconWidth);
	RegisterProperty(_T("iconheight"), DU_PROPERTY_LONG, &m_nIconHeight);
	RegisterProperty(_T("childmenu"), DU_PROPERTY_STRING, m_szChildMenu);

	RegisterProperty(_T("arrow"), DU_PROPERTY_STRING, m_szArrow);
	RegisterProperty(_T("arrowright"), DU_PROPERTY_LONG, &m_nArrowRight);

	RegisterProperty(_T("window"), DU_PROPERTY_STRING, m_szPopupWindow);
}

void WINAPI duMenuItem::OnCreate()
{
	
}

void WINAPI duMenuItem::DrawObject(HDC hDC)
{
	duRect rcMenuItem;
	Plugin_GetRect(this, &rcMenuItem);
	rcMenuItem.OffsetRect(-rcMenuItem.left, -rcMenuItem.top);

	UINT uState = DU_STATE_NORMAL;
	if (m_pPopupMenu)
		uState = DU_STATE_OVER;
	else
	{
		if (GetState() != DU_STATE_OVER)
			uState = DU_STATE_NORMAL;
		else
			uState = DU_STATE_OVER;
	}
	
	if (IsDisable())
		uState = DU_STATE_DISABLE;

	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
		pStyleGroup->Draw(hDC, &rcMenuItem, uState, GetText(), GetAlpha());

	duImage *pIcon = (duImage *)GetResObj(m_szIcon, DU_RES_IMAGE);
	if (pIcon)
	{
		duRect rcIcon;
		rcIcon.left = rcMenuItem.left + m_nIconX;
		rcIcon.top = rcMenuItem.top + m_nIconY;
		rcIcon.right = rcIcon.left + m_nIconWidth;
		rcIcon.bottom = rcIcon.top + m_nIconHeight;

		if (IsDisable())
		{
			duImage *pDisableIcon = (duImage *)pIcon->CreateGrayImage();
			DrawStretch(hDC, rcIcon.left, rcIcon.top, rcIcon.Width(), rcIcon.Height(), pDisableIcon, 0, 0, pIcon->GetWidth(), pIcon->GetHeight(), 255);
			pDisableIcon->FinalRelease();
		}
		else
			DrawStretch(hDC, rcIcon.left, rcIcon.top, rcIcon.Width(), rcIcon.Height(), pIcon, 0, 0, pIcon->GetWidth(), pIcon->GetHeight(), 255);
	}

	if (IsPopupItem())
	{
		duImage *pArrowImage = (duImage *)GetResObj(m_szArrow, DU_RES_IMAGE);
		if (pArrowImage)
		{
			int nArrowWidth = pArrowImage->GetWidth() / 2;

			duRect rcArrow;
			rcArrow.left = rcMenuItem.right - m_nArrowRight - nArrowWidth;
			rcArrow.right = rcArrow.left + nArrowWidth;
			rcArrow.top = rcMenuItem.top + (rcMenuItem.Height() - pArrowImage->GetHeight()) / 2;
			rcArrow.bottom = rcArrow.top + pArrowImage->GetHeight();

			if (uState == DU_STATE_OVER)
				DrawNormal(hDC, rcArrow.left, rcArrow.top, rcArrow.Width(), rcArrow.Height(), pArrowImage, nArrowWidth, 0, 255);
			else
				DrawNormal(hDC, rcArrow.left, rcArrow.top, rcArrow.Width(), rcArrow.Height(), pArrowImage, 0, 0, 255);
		}
	}
}

void WINAPI duMenuItem::OnMouseIn(POINT pt)
{
	//Plugin_SetState(this, DU_STATE_OVER);
	//Plugin_Redraw(this, TRUE);
}

void WINAPI duMenuItem::OnMouseLeave(POINT pt)
{
	//if (!m_pPopupMenu)
	//{
	//	Plugin_SetState(this, DU_STATE_NORMAL);
	//	Plugin_Redraw(this, TRUE);
	//}
}

void WINAPI duMenuItem::OnMouseLDown(POINT pt)
{
	m_fPress = TRUE;
}

void WINAPI duMenuItem::OnMouseLUp(POINT pt)
{
	if (m_pPopupMenu)
		return;

	if (m_fPress)
	{
		NotifyMenuUser(DUM_MENUCLICK, NULL, NULL);

		if (Plugin_IsValid(this))
		{
			duMenu *pRootMenu = GetRootMenu(m_hWnd);
			MyDestroyMenu(pRootMenu);
		}
	}
	else
	{
		m_fPress = FALSE;
	}
}

void WINAPI duMenuItem::OnMouseDbClick(POINT pt)
{
	
}

void WINAPI duMenuItem::SetChildMenu(LPCTSTR lpszMenu)
{
	if (lpszMenu)
		_tcsncpy(m_szChildMenu, lpszMenu, MAX_NAME);
	else
		ZeroMemory(m_szChildMenu, MAX_NAME * sizeof(TCHAR));
}

void WINAPI duMenuItem::SetIcon(LPCTSTR lpszIcon)
{
	if (lpszIcon)
		_tcsncpy(m_szIcon, lpszIcon, MAX_NAME);
	else
		ZeroMemory(m_szIcon, MAX_NAME * sizeof(TCHAR));
}

BOOL WINAPI duMenuItem::IsPopupItem()
{
	return _tcslen(m_szChildMenu) > 0 ? TRUE : FALSE;
}

void WINAPI duMenuItem::SetPopupWindow(LPCTSTR lpszWin)
{
	if (lpszWin)
		_tcsncpy(m_szPopupWindow, lpszWin, MAX_NAME);
	else
		ZeroMemory(m_szPopupWindow, MAX_NAME * sizeof(TCHAR));
}

void WINAPI duMenuItem::SetDisable(BOOL fDisable)
{
	if (IsPopupItem())
		return;

	duPlugin::SetDisable(fDisable);
}

LRESULT duMenuItem::NotifyMenuUser(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	duMenu *pMenu = (duMenu *)GetParent();
	if (pMenu)
	{
		duNotify notify;
		notify.uMsg = uMsg;
		notify.wParam = wParam;
		notify.lParam = lParam;

		return ::SendMessage(pMenu->GetOwnerHwnd(), WM_DIRECTUI_NOTIFY, (WPARAM)this, (LPARAM)&notify);
	}

	return 0;
}
