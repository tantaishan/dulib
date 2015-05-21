//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duMenu.cpp
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duMenu.h"
#include "duMenuItem.h"

duMenu::duMenu() :
	 m_hWndMenu(NULL)
	,m_pParentMenu(NULL)
	,m_hWndOwner(NULL)
{

}

duMenu::~duMenu()
{
	
}

void WINAPI duMenu::RegisterControlProperty()
{
	
}

void WINAPI duMenu::OnCreate()
{
	AdjustChild();
}

void WINAPI duMenu::DrawObject(HDC hDC)
{
	
}

void WINAPI duMenu::AdjustChild()
{
	duRect rectLayout;
	rectLayout.SetRectEmpty();
	GetRect(rectLayout);
	
	duPlugin *pNow = GetFirstChild();
	while(pNow)
	{
		if (!pNow->IsVisible())
		{
			pNow = pNow->GetNextSibling();
			continue;
		}
		
		duPlugin *pPre = GetPreVisible(pNow);
		if (pPre)
		{
			duRect rectPre;
			rectPre.SetRectEmpty();
			pPre->GetRect(&rectPre);
			rectPre.OffsetRect(-rectLayout.left, -rectLayout.top);
			pNow->SetPosTop(rectPre.bottom);
		}
		else
		{
			pNow->SetPosTop(0);
		}
		pNow->SetPosVertAnchor(0);
		pNow->Resize(&rectLayout);
		pNow = pNow->GetNextSibling();
	}
}

duPlugin *WINAPI duMenu::GetPreVisible(duPlugin *pPlugin)
{
	if (pPlugin == NULL)
		return NULL;
	
	duPlugin *pTemp = NULL;
	if (pPlugin)
	{
		pTemp = pPlugin->GetPreSibling();
		while(pTemp)
		{
			if (pTemp->IsVisible())
				break;
			pTemp = pTemp->GetPreSibling();
		}
	}

	return pTemp;
}

duMenuItem *WINAPI duMenu::MenuItemFromClientPoint(LPPOINT ppt)
{
	duPlugin *pPlugin = GetFirstChild();
	while (pPlugin)
	{
		if (pPlugin->IsVisible() && lstrcmpi(pPlugin->GetType(), _T("MenuItem")) == 0)
		{
			duRect rcMenuItem;
			pPlugin->GetRect(&rcMenuItem);
			if (rcMenuItem.PtInRect(*ppt))
				return (duMenuItem *)pPlugin;
		}

		pPlugin = pPlugin->GetNextSibling();
	}

	return NULL;
}

duMenu *WINAPI duMenu::GetNextPopupMenu()
{
	duPlugin *pPlugin = GetFirstChild();
	while (pPlugin)
	{
		if (pPlugin->IsVisible() && lstrcmpi(pPlugin->GetType(), _T("MenuItem")) == 0)
		{
			duMenuItem *pMenuItem = (duMenuItem *)pPlugin;
			if (pMenuItem->IsPopupItem() && pMenuItem->GetPopupMenu())
				return pMenuItem->GetPopupMenu();
		}

		pPlugin = pPlugin->GetNextSibling();
	}

	return NULL;
}

duMenuItem *WINAPI duMenu::GetHotMenu()
{
	duPlugin *pPlugin = GetFirstChild();
	while (pPlugin)
	{
		if (pPlugin->IsVisible() && lstrcmpi(pPlugin->GetType(), _T("MenuItem")) == 0)
		{
			if (pPlugin->GetState() == DU_STATE_OVER)
				return (duMenuItem *)pPlugin;
		}

		pPlugin = pPlugin->GetNextSibling();
	}

	return NULL;
}

LRESULT duMenu::NotifyMenuUser(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	duNotify notify;
	notify.uMsg = uMsg;
	notify.wParam = wParam;
	notify.lParam = lParam;
	return ::SendMessage(GetOwnerHwnd(), WM_DIRECTUI_NOTIFY, (WPARAM)this, (LPARAM)&notify);
}
