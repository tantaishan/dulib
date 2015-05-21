//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duTabButton.cpp
//
//  History:    Apr-03-2010   Denny Chen  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duTabButton.h"
#include "duDrawHelper.h"
#include "duCtrlManager.h"

duTabButton::duTabButton() :
	 m_fAutoSize(FALSE)
	,m_nFixLeft(0)
	,m_nFixRight(0)
	,m_nIconX(0)
	,m_nIconY(0)
	,m_nIconWidth(0)
	,m_nIconHeight(0)
	,m_fSelected(FALSE)
	,m_fFade(FALSE)
	,m_fFadeDrawing(FALSE)
	,m_nFadeTimer(0)
	,m_uFadeInState(DU_STATE_NORMAL)
	,m_uFadeOutState(DU_STATE_NORMAL)
	,m_nFadeInAlpha(0)
	,m_nFadeOutAlpha(0)
	,m_nFadeInSpeed(40)
	,m_nFadeOutSpeed(30)
{
	ZeroMemory(m_szIcon, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szTabPage, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szToolTip, MAX_NAME * sizeof(TCHAR));
}

duTabButton::~duTabButton()
{
}

void WINAPI duTabButton::RegisterControlProperty()
{
	RegisterProperty(_T("autosize"), DU_PROPERTY_BOOL, &m_fAutoSize);
	RegisterProperty(_T("fixleft"),  DU_PROPERTY_LONG, &m_nFixLeft);
	RegisterProperty(_T("fixright"), DU_PROPERTY_LONG, &m_nFixRight);

	RegisterProperty(_T("icon"),  DU_PROPERTY_STRING, &m_szIcon);
	RegisterProperty(_T("iconx"), DU_PROPERTY_LONG, &m_nIconX);
	RegisterProperty(_T("icony"), DU_PROPERTY_LONG, &m_nIconY);
	RegisterProperty(_T("iconwidth"),  DU_PROPERTY_LONG, &m_nIconWidth);
	RegisterProperty(_T("iconheight"), DU_PROPERTY_LONG, &m_nIconHeight);

	RegisterProperty(_T("tabpage"),  DU_PROPERTY_STRING, &m_szTabPage);
	RegisterProperty(_T("selected"), DU_PROPERTY_BOOL, &m_fSelected);

	RegisterProperty(_T("fade"), DU_PROPERTY_BOOL, &m_fFade);
	RegisterProperty(_T("fadeinspeed"), DU_PROPERTY_LONG, &m_nFadeInSpeed);
	RegisterProperty(_T("fadeoutspeed"), DU_PROPERTY_LONG, &m_nFadeOutSpeed);

	RegisterProperty(_T("tooltip"), DU_PROPERTY_STRING, m_szToolTip);
}

void WINAPI duTabButton::OnCreate()
{
	//
	//防止克隆的时候,m_szTabPage为NULL，控件的长度等都不会被计算，所以提到前面
	//
	if (m_nFadeInSpeed == 0)
		m_nFadeInSpeed = 40;

	if (m_nFadeOutSpeed == 0)
		m_nFadeOutSpeed = 30;

	m_nFadeTimer = GetUniTimer();

	if (m_fAutoSize)
		ResizeByText(GetText());

	duPlugin *pTabPage = (duPlugin *)GetPluginByName(m_szTabPage);
	if (!Plugin_IsValid(pTabPage))
		return;

	// 设置初始状态是否为选中状态
	if (m_fSelected)
	{
		Plugin_SetState(this, DU_STATE_CHECKED);
		Plugin_SetVisible(pTabPage, TRUE);
	}
	else
		Plugin_SetVisible(pTabPage, FALSE);
}

void WINAPI duTabButton::DrawObject(HDC hDC)
{
	duRect rectTabButton;
	Plugin_GetRect(this, &rectTabButton);
	rectTabButton.OffsetRect(-rectTabButton.left, -rectTabButton.top);

	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
	{
		if (m_fFadeDrawing)
		{
			pStyleGroup->DrawNotText(hDC, &rectTabButton, m_uFadeOutState, NULL, m_nFadeOutAlpha);
			pStyleGroup->DrawNotText(hDC, &rectTabButton, m_uFadeInState, NULL, m_nFadeInAlpha);
			pStyleGroup->DrawOnlyText(hDC, &rectTabButton, m_uFadeInState, GetText(), GetAlpha());
		}
		else
			pStyleGroup->Draw(hDC, &rectTabButton, GetState(), GetText(), GetAlpha());
	}
	
	duImage *pIcon = (duImage *)GetResObj(m_szIcon, DU_RES_IMAGE);
	if (pIcon == NULL)
		return;

	duRect rcIcon;
	GetIconRect(rcIcon);
	DrawStretch(hDC, rcIcon.left, rcIcon.top, rcIcon.Width(), rcIcon.Height(), 
		pIcon, 0, 0, pIcon->GetWidth(), pIcon->GetHeight(), GetAlpha());
}

void duTabButton::GetIconRect(duRect &rcIcon)
{
	rcIcon.SetRectEmpty();

	duRect rcButton;
	Plugin_GetRect(this, &rcButton);
	rcButton.OffsetRect(-rcButton.left, -rcButton.top);

	if (m_nIconX == -1)
		rcIcon.left = (rcButton.Width() - m_nIconWidth) / 2;
	else
		rcIcon.left = m_nIconX;

	if (m_nIconY == -1)
		rcIcon.top = (rcButton.Height() - m_nIconHeight) / 2;
	else
		rcIcon.top = m_nIconY;

	rcIcon.right = rcIcon.left + m_nIconWidth;
	rcIcon.bottom = rcIcon.top + m_nIconHeight;
}

void WINAPI duTabButton::OnMouseIn(POINT pt)
{
	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return;

	if (*m_szToolTip != 0)
		pCtrlManager->AddToolTip(m_szToolTip);

	if (m_fSelected)
		Plugin_SetState(this, DU_STATE_CHECKEDOVER);
	else
		SetState(DU_STATE_OVER);

	if(m_fFade)
	{
		if(m_fSelected)
			FadeRedraw(DU_STATE_CHECKEDOVER, DU_STATE_CHECKED);
		else
			FadeRedraw(DU_STATE_OVER, DU_STATE_NORMAL);
	}
	else
		Plugin_Redraw(this, TRUE);
}

void WINAPI duTabButton::OnMouseLeave(POINT pt)
{
	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return;

	pCtrlManager->DelToolTip();

	if (m_fSelected)
		Plugin_SetState(this, DU_STATE_CHECKED);
	else
		Plugin_SetState(this, DU_STATE_NORMAL);
	
	if(m_fFade)
	{
		if(m_fSelected)
			FadeRedraw(DU_STATE_CHECKED,DU_STATE_CHECKEDOVER);
		else
            FadeRedraw(DU_STATE_NORMAL,DU_STATE_OVER); 
	}
	else
		Plugin_Redraw(this, TRUE);
}

void WINAPI duTabButton::OnMouseLDown(POINT pt)
{
	if (m_fSelected)
		Plugin_SetState(this, DU_STATE_CHECKEDPRESS);
	else
		Plugin_SetState(this, DU_STATE_PRESS);

	Plugin_Redraw(this, TRUE);
}

void WINAPI duTabButton::OnMouseLUp(POINT pt)
{
	if (m_fSelected)
		return;

	if (!NotifyUser(DUM_TBBSELECTED, (WPARAM)TRUE, NULL))
	{
		//if (GetState() == DU_STATE_PRESS)
		{
			duPlugin *pParent = GetParent();
			if (!Plugin_IsValid(pParent))
				return;
			
			duPlugin *pSilbing = pParent->GetFirstChild();
			while(pSilbing)
			{
				if (!Plugin_IsValid(pSilbing))
					continue;
				
				LPCTSTR lpszType = pSilbing->GetType();
				if (lstrcmpi(lpszType, _T("tabbutton")) == 0)
				{
					duTabButton *pTemp = (duTabButton *)pSilbing;
					Plugin_SetState(pTemp, DU_STATE_NORMAL);
					if (pTemp->IsSelected())
					{
						pTemp->NotifyUser(DUM_TBBSELECTED, (WPARAM)FALSE, NULL);
						//Plugin_Redraw(pTemp, TRUE);
					}

					if (!Plugin_IsValid(pTemp))
					{
						_TRACE(_T("error: delete tabbutton in DUM_TBBSELECTED notify\n"));
						return;
					}

					pTemp->_SetSelected(FALSE);
					pTemp->OnRedraw();
				}
				pSilbing = pSilbing->GetNextSibling();
			}

			_SetSelected(TRUE);
			Plugin_SetState(this, DU_STATE_CHECKEDOVER);
		}
		::InvalidateRect(m_hWnd, NULL, TRUE);
	}
	else
	{
		if (GetState() == DU_STATE_PRESS)
		{
			Plugin_SetState(this, DU_STATE_OVER);
			Plugin_Redraw(this, TRUE);
		}
	}
}

void WINAPI duTabButton::OnKeyDown(UINT key, UINT nRepCnt, UINT nFlags)
{
// 注释键盘快捷键， modify by Alex Liang, 2014-10-22
/*
	duCtrlManager *pCtrlManager = (duCtrlManager *)GetCtrlManager(m_hWnd);

	switch (key)
	{
	case VK_UP:
	case VK_LEFT:
		{
			duPlugin *pPre = GetPreSibling();
			while (pPre)
			{
				if (lstrcmpi(pPre->GetType(), _T("tabbutton")) == 0)
				{
					if (Plugin_IsVisible(pPre))
					{
						duTabButton *pPreTab = (duTabButton *)pPre;
						pPreTab->SetSelected(TRUE);
						
						if (pCtrlManager)
							pCtrlManager->SetFocusPlugin(pPreTab, TRUE);

						// Notify
						NotifyUser(DUM_TBBSELECTED, (WPARAM)FALSE, NULL);
						pPreTab->NotifyUser(DUM_TBBSELECTED, (WPARAM)TRUE, NULL);

						::InvalidateRect(m_hWnd, NULL, TRUE);
						
						break;
					}
				}
			
				pPre = pPre->GetPreSibling();
			}
		}
		break;

	case VK_DOWN:
	case VK_RIGHT:
		{
			duPlugin *pNext = GetNextSibling();
			while (pNext)
			{
				if (lstrcmpi(pNext->GetType(), _T("tabbutton")) == 0)
				{
					if (Plugin_IsVisible(pNext))
					{
						duTabButton *pNextTab = (duTabButton *)pNext;
						pNextTab->SetSelected(TRUE);

						if (pCtrlManager)
							pCtrlManager->SetFocusPlugin(pNextTab, TRUE);

						// Notify
						NotifyUser(DUM_TBBSELECTED, (WPARAM)FALSE, NULL);
						pNextTab->NotifyUser(DUM_TBBSELECTED, (WPARAM)TRUE, NULL);

						::InvalidateRect(m_hWnd, NULL, TRUE);
						break;
					}
				}

				pNext = pNext->GetNextSibling();
			}
		}
		break;

	default:
		break;
	}
*/
}

BOOL WINAPI duTabButton::IsSelected()
{
	return m_fSelected;
}

void WINAPI duTabButton::SetSelected(BOOL fSel)
{
	duPlugin *pParent = GetParent();
	if (!Plugin_IsValid(pParent))
		return;
	
	duPlugin *pSilbing = pParent->GetFirstChild();
	while(pSilbing)
	{
		if (!Plugin_IsValid(pSilbing))
			continue;
		
		LPCTSTR lpszType = pSilbing->GetType();
		if (lstrcmpi(lpszType, _T("tabbutton")) == 0)
		{
			duTabButton *pTemp = (duTabButton *)pSilbing;
			Plugin_SetState(pTemp, DU_STATE_NORMAL);

			//if (pTemp->IsSelected())
			//	NotifyUser(pTemp, DUMI_TBBSELECTED, (WPARAM)FALSE, NULL);
			pTemp->_SetSelected(FALSE);
		}
		pSilbing = pSilbing->GetNextSibling();
	}
	
	//NotifyUser(this, DUMI_TBBSELECTED, (WPARAM)TRUE, NULL);
	_SetSelected(TRUE);
	Plugin_SetState(this, DU_STATE_CHECKEDOVER);
	//Plugin_Redraw(this, TRUE);
	//::InvalidateRect(m_hWnd, NULL, TRUE);
}

void WINAPI duTabButton::OnRedraw()
{
	m_fFadeDrawing = FALSE;
	Plugin_KillTimer(this, m_nFadeTimer);
}

void WINAPI duTabButton::OnTimer(UINT nEventId)
{
	if (!m_fFade || !m_fFadeDrawing)
	{
		Plugin_KillTimer(this, nEventId);
		return;
	}

	if (nEventId != m_nFadeTimer)
		return;

	m_nFadeInAlpha += m_nFadeInSpeed;
	if (m_nFadeInAlpha >= 255)
		m_nFadeInAlpha = 255;

	m_nFadeOutAlpha -= m_nFadeOutSpeed;
	if (m_nFadeOutAlpha <= 0)
		m_nFadeOutAlpha = 0;

	Plugin_Redraw(this, FALSE);

	if (m_nFadeInAlpha >= 255 && m_nFadeOutAlpha <= 0)
	{
		Plugin_KillTimer(this, nEventId);
		m_fFadeDrawing = FALSE;
	}
}

void duTabButton::_SetSelected(BOOL fSel)
{
	if (m_fSelected == fSel)
		return;

	m_fSelected = fSel;
	duPlugin *pTabPage = GetPluginByName(m_szTabPage);
	if (Plugin_IsValid(pTabPage))
	{
		Plugin_SetVisible(pTabPage, fSel);
		//Plugin_Redraw(pTabPage, TRUE);
	}
}

void duTabButton::ResizeByText(LPCTSTR lpszText)
{
	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
	{
		duSize size(0, 0);
		pStyleGroup->CalcTextRect(GetState(), lpszText, &size);
		SetPosWidth(size.cx + m_nFixLeft + m_nFixRight);
		Plugin_Resize(this, NULL);
	}
}

void duTabButton::FadeRedraw(UINT uFadeInState, UINT uFadeOutState)
{
	if (!m_fFade)
		return;

	m_fFadeDrawing = TRUE;
	m_uFadeInState = uFadeInState;
	m_uFadeOutState = uFadeOutState;

	m_nFadeInAlpha = 0;
	m_nFadeOutAlpha = GetAlpha();
	Plugin_KillTimer(this, m_nFadeTimer);
	Plugin_SetTimer(this, m_nFadeTimer, 30);
}

void WINAPI duTabButton::SetTooltipText(LPCTSTR lpszText)
{
	if (lpszText)
		_tcsncpy(m_szToolTip, lpszText, MAX_NAME);
	else
		ZeroMemory(m_szToolTip, MAX_NAME * sizeof(TCHAR));
}

void WINAPI duTabButton::SetIcon(LPCTSTR lpszIconName)
{
	if (lpszIconName)
		_tcsncpy(m_szIcon, lpszIconName, MAX_NAME);
	else
		ZeroMemory(m_szIcon, sizeof(TCHAR) * MAX_NAME);
}

void WINAPI duTabButton::SetTabPage(LPCTSTR lpszTabPage)
{
	if (lpszTabPage)
		_tcsncpy(m_szTabPage, lpszTabPage, MAX_NAME);
	else
		ZeroMemory(m_szTabPage, sizeof(TCHAR) * MAX_NAME);
}
