//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duButton.cpp
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------

#include "stdafx.h"
#include "duButton.h"
#include "duDrawHelper.h"
#include "duCtrlManager.h"

duButton::duButton() :
	  m_fAutoSize(FALSE)
	 ,m_nFixLeft(0)
	 ,m_nFixRight(0)
	 ,m_nIconX(0)
	 ,m_nIconY(0)
	 ,m_nIconWidth(0)
	 ,m_nIconHeight(0)
	 ,m_fFade(FALSE)
	 ,m_fFadeDrawing(FALSE)
	 ,m_nFadeTimer(0)
	 ,m_uFadeInState(DU_STATE_NORMAL)
	 ,m_uFadeOutState(DU_STATE_NORMAL)
	 ,m_nFadeInAlpha(0)
	 ,m_nFadeOutAlpha(0)
	 ,m_nFadeInSpeed(40)
	 ,m_nFadeOutSpeed(30)
	 ,m_nStatusIconX(0)
	 ,m_nStatusIconY(0)
	 ,m_nStatusIconWidth(0)
	 ,m_nStatusIconHeight(0)
	 ,m_fGrayDisable(FALSE)

{
	ZeroMemory(m_szIcon, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szToolTip, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szStatusIcon, MAX_NAME * sizeof(TCHAR));
}

duButton::~duButton()
{
}

void WINAPI duButton::RegisterControlProperty()
{
	RegisterProperty(_T("autosize"), DU_PROPERTY_BOOL, &m_fAutoSize);
	RegisterProperty(_T("fixleft"), DU_PROPERTY_LONG, &m_nFixLeft);
	RegisterProperty(_T("fixright"), DU_PROPERTY_LONG, &m_nFixRight);

	RegisterProperty(_T("icon"), DU_PROPERTY_IMAGE, &m_szIcon);
	RegisterProperty(_T("iconx"), DU_PROPERTY_LONG, &m_nIconX);
	RegisterProperty(_T("icony"), DU_PROPERTY_LONG, &m_nIconY);
	RegisterProperty(_T("iconwidth"), DU_PROPERTY_LONG, &m_nIconWidth);
	RegisterProperty(_T("iconheight"), DU_PROPERTY_LONG, &m_nIconHeight);
	RegisterProperty(_T("fade"), DU_PROPERTY_BOOL, &m_fFade);
	RegisterProperty(_T("fadeinspeed"), DU_PROPERTY_LONG, &m_nFadeInSpeed);
	RegisterProperty(_T("fadeoutspeed"), DU_PROPERTY_LONG, &m_nFadeOutSpeed);
	
	RegisterProperty(_T("tooltip"), DU_PROPERTY_STRING, m_szToolTip);

	RegisterProperty(_T("status_icon"), DU_PROPERTY_IMAGE, &m_szStatusIcon);
	RegisterProperty(_T("status_iconx"), DU_PROPERTY_LONG, &m_nStatusIconX);
	RegisterProperty(_T("status_icony"), DU_PROPERTY_LONG, &m_nStatusIconY);
	RegisterProperty(_T("status_iconwidth"), DU_PROPERTY_LONG, &m_nStatusIconWidth);
	RegisterProperty(_T("status_iconheight"), DU_PROPERTY_LONG, &m_nStatusIconHeight);

	RegisterProperty(_T("graydisable"), DU_PROPERTY_BOOL, &m_fGrayDisable);
}

void WINAPI duButton::OnCreate()
{
	if (m_nFadeInSpeed == 0)
		m_nFadeInSpeed = 40;

	if (m_nFadeOutSpeed == 0)
		m_nFadeOutSpeed = 30;


	m_nFadeTimer = GetUniTimer();
	
	if (m_fAutoSize)
		ResizeByText(GetText());
}

void WINAPI duButton::DrawObject(HDC hDC)
{
	duRect rcButton;
	Plugin_GetRect(this, &rcButton);
	rcButton.OffsetRect(-rcButton.left, -rcButton.top);

	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (m_fFadeDrawing)
	{
		if (pStyleGroup)
		{
			if(m_fGrayDisable)
			{
				if(GetState() == DU_STATE_DISABLE)
					pStyleGroup->GrayDrawByStyle(hDC, &rcButton, GetState(), GetText(), GetAlpha());
				else
				{
					pStyleGroup->DrawNotText(hDC, &rcButton, m_uFadeOutState, NULL, m_nFadeOutAlpha);
					pStyleGroup->DrawNotText(hDC, &rcButton, m_uFadeInState, NULL, m_nFadeInAlpha);
					pStyleGroup->DrawOnlyText(hDC, &rcButton, m_uFadeInState, GetText(), GetAlpha());
				}
			}
			else
			{
				pStyleGroup->DrawNotText(hDC, &rcButton, m_uFadeOutState, NULL, m_nFadeOutAlpha);
				pStyleGroup->DrawNotText(hDC, &rcButton, m_uFadeInState, NULL, m_nFadeInAlpha);
				pStyleGroup->DrawOnlyText(hDC, &rcButton, m_uFadeInState, GetText(), GetAlpha());
			}			
		}
	}
	else
	{
		if (pStyleGroup)
		{
			if(m_fGrayDisable)
			{
				if(GetState() == DU_STATE_DISABLE)
					pStyleGroup->GrayDrawByStyle(hDC, &rcButton, GetState(), GetText(), GetAlpha());
				else
					pStyleGroup->Draw(hDC, &rcButton, GetState(), GetText(), GetAlpha());
			}
			else
				pStyleGroup->Draw(hDC, &rcButton, GetState(), GetText(), GetAlpha());
		}
			
	}

	duImage *pIcon = (duImage *)GetResObj(m_szIcon, DU_RES_IMAGE);
	if (pIcon == NULL)
		return;
	
	duRect rcIcon;
	GetIconRect(rcIcon);
	if(m_fGrayDisable)
	{
		if(GetState() == DU_STATE_DISABLE)
		{
			duResManager *pResManager = GetResManager(m_hWnd);
			if (pResManager)
			{
				duImage *pGrayImage = pIcon->CreateGrayImage();
				if (pGrayImage)
				{
					DrawStretch(hDC, rcIcon.left, rcIcon.top, rcIcon.Width(), rcIcon.Height(), 
						pGrayImage, 0, 0, pGrayImage->GetWidth(), pGrayImage->GetHeight(), GetAlpha());
					pGrayImage->FinalRelease();
				}
			}
		}
		else
			DrawStretch(hDC, rcIcon.left, rcIcon.top, rcIcon.Width(), rcIcon.Height(), 
			pIcon, 0, 0, pIcon->GetWidth(), pIcon->GetHeight(), GetAlpha());
	}
	else
		DrawStretch(hDC, rcIcon.left, rcIcon.top, rcIcon.Width(), rcIcon.Height(), 
			pIcon, 0, 0, pIcon->GetWidth(), pIcon->GetHeight(), GetAlpha());


	//绘制按钮上的状态图标，用于IM中的好友头像的状态图标
	duImage *pStatusIcon = (duImage *)GetResObj(m_szStatusIcon, DU_RES_IMAGE);
	if(pStatusIcon == NULL)
		return;

	duRect rcStatusIcon;
	GetStatusIconRect(rcStatusIcon);
	if(m_fGrayDisable)
	{
		if(GetState() == DU_STATE_DISABLE)
		{
			duResManager *pResManager = GetResManager(m_hWnd);
			if (pResManager)
			{
				duImage *pGrayImage = pStatusIcon->CreateGrayImage();
				if (pGrayImage)
				{
					DrawStretch(hDC, rcStatusIcon.left, rcStatusIcon.top, rcStatusIcon.Width(), rcStatusIcon.Height(), 
						pGrayImage, 0, 0, pGrayImage->GetWidth(), pGrayImage->GetHeight(), GetAlpha());
					pGrayImage->FinalRelease();
				}
			}
		}
		else
			DrawStretch(hDC, rcIcon.left, rcIcon.top, rcIcon.Width(), rcIcon.Height(), 
			pIcon, 0, 0, pIcon->GetWidth(), pIcon->GetHeight(), GetAlpha());
	}
	else
		DrawStretch(hDC, rcStatusIcon.left, rcStatusIcon.top, rcStatusIcon.Width(), rcStatusIcon.Height(), 
		pStatusIcon, 0, 0, pStatusIcon->GetWidth(), pStatusIcon->GetHeight(), GetAlpha());

	

}

void WINAPI duButton::OnMouseIn(POINT pt)
{
	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return;

	if (*m_szToolTip != 0)
		pCtrlManager->AddToolTip(m_szToolTip);

	Plugin_SetState(this, DU_STATE_OVER);
	if (m_fFade)
		FadeRedraw(DU_STATE_OVER, DU_STATE_NORMAL);
	else
		Plugin_Redraw(this, TRUE);
}

void WINAPI duButton::OnMouseLeave(POINT pt)
{
	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return;

	pCtrlManager->DelToolTip();

	UINT uState = Plugin_GetState(this);
	Plugin_SetState(this, DU_STATE_NORMAL);
	if (m_fFade)
		FadeRedraw(DU_STATE_NORMAL, uState);
	else
		Plugin_Redraw(this, TRUE);
}

void WINAPI duButton::OnMouseLDown(POINT pt)
{
	Plugin_SetState(this, DU_STATE_PRESS);
	Plugin_Redraw(this, TRUE);
}

void WINAPI duButton::OnMouseLUp(POINT pt)
{
	UINT uState = GetState();

	Plugin_SetState(this, DU_STATE_OVER);
	Plugin_Redraw(this, TRUE);

	if (uState == DU_STATE_PRESS)
		NotifyUser(DUM_BTNCLICK, NULL, NULL);
}

void WINAPI duButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
// 注释键盘快捷键， modify by Alex Liang, 2014-10-22
/*
	if (nChar == VK_SPACE)
		NotifyUser(DUM_BTNCLICK, NULL, NULL);
*/
}

void WINAPI duButton::OnMouseDbClick(POINT pt)
{
	NotifyUser(DUM_BTNDBCLICK, NULL, NULL);
}

void WINAPI duButton::OnRedraw()
{
	m_fFadeDrawing = FALSE;
	Plugin_KillTimer(this, m_nFadeTimer);
}

void WINAPI duButton::OnTimer(UINT nEventId)
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

void WINAPI duButton::SetText(LPCTSTR lpszText)
{
	if (m_fAutoSize)
		ResizeByText(lpszText);

	duPlugin::SetText(lpszText);
}

void WINAPI duButton::SetIcon(LPCTSTR lpszIconName)
{
	if (lpszIconName)
		_tcsncpy(m_szIcon, lpszIconName, MAX_NAME);
	else
		ZeroMemory(m_szIcon, sizeof(TCHAR) * MAX_NAME);
}

void duButton::ResizeByText(LPCTSTR lpszText)
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

void duButton::GetIconRect(duRect &rcIcon)
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

void duButton::GetStatusIconRect(duRect &rcStatusIcon)
{
	rcStatusIcon.SetRectEmpty();

	duRect rcButton;
	Plugin_GetRect(this, &rcButton);
	rcButton.OffsetRect(-rcButton.left, -rcButton.top);

	if(m_nStatusIconX == -1)
		rcStatusIcon.left = rcButton.right - m_nStatusIconWidth;
	else
		rcStatusIcon.left = m_nStatusIconX;

	if(m_nStatusIconY == -1)
		rcStatusIcon.top = rcButton.bottom - m_nStatusIconHeight;
	else
		rcStatusIcon.top = m_nStatusIconY;

	rcStatusIcon.right = rcStatusIcon.left + m_nStatusIconWidth;
	rcStatusIcon.bottom = rcStatusIcon.top + m_nStatusIconHeight;
}

void duButton::FadeRedraw(UINT uFadeInState, UINT uFadeOutState)
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

void WINAPI duButton::SetTooltipText(LPCTSTR lpszText)
{
	if (lpszText)
		_tcsncpy(m_szToolTip, lpszText, MAX_NAME);
	else
		ZeroMemory(m_szToolTip, MAX_NAME * sizeof(TCHAR));
}



void WINAPI duButton::SetStatusIcon(LPCTSTR lpszStatusIconName)
{
	if (lpszStatusIconName)
		_tcsncpy(m_szStatusIcon, lpszStatusIconName, MAX_NAME);
	else
		ZeroMemory(m_szStatusIcon, sizeof(TCHAR) * MAX_NAME);
}


BOOL WINAPI duButton::IsGrayDisable()
{
	return m_fGrayDisable;
}

void WINAPI duButton::SetGrayDisable(BOOL fGrayDisable)
{
	m_fGrayDisable = fGrayDisable;
}
