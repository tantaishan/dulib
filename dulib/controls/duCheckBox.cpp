//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duCheckBox.cpp
//
//  History:    Nov-13-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duCheckBox.h"
#include "duDrawHelper.h"
#include "duCtrlManager.h"

duCheckBox::duCheckBox(void) : 
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
	,m_fChecked(FALSE)
{
    ZeroMemory(m_szIcon, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szToolTip, MAX_NAME * sizeof(TCHAR));
}

duCheckBox::~duCheckBox(void)
{
}

//////////////////////////////////////////////////////////////////////////
void WINAPI duCheckBox::RegisterControlProperty()
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

    RegisterProperty(_T("checked"), DU_PROPERTY_BOOL, &m_fChecked);
	RegisterProperty(_T("tooltip"), DU_PROPERTY_STRING, m_szToolTip);
}

void WINAPI duCheckBox::OnCreate()
{
	if (m_nFadeInSpeed == 0)
		m_nFadeInSpeed = 40;

	if (m_nFadeOutSpeed == 0)
		m_nFadeOutSpeed = 30;


	m_nFadeTimer = GetUniTimer();

	if (m_fAutoSize)
		ResizeByText(GetText());

	//ÏÈÉèÖÃcheckboxµÄ×´Ì¬
	m_fChecked ? Plugin_SetState(this, DU_STATE_CHECKED) : Plugin_SetState(this, DU_STATE_NORMAL);
}

void WINAPI duCheckBox::DrawObject(HDC hDC)
{

	duRect rectChkBox;
	Plugin_GetRect(this, &rectChkBox);
	rectChkBox.OffsetRect(-rectChkBox.left, -rectChkBox.top);

	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	
	if (m_fFadeDrawing)
	{
		if (pStyleGroup)
		{
			pStyleGroup->DrawNotText(hDC, &rectChkBox, m_uFadeOutState, NULL, m_nFadeOutAlpha);
			pStyleGroup->DrawNotText(hDC, &rectChkBox, m_uFadeInState, NULL, m_nFadeInAlpha);
			pStyleGroup->DrawOnlyText(hDC, &rectChkBox, m_uFadeInState, GetText(), GetAlpha());
		}
	}
	else
	{
		if (pStyleGroup)
			pStyleGroup->Draw(hDC, &rectChkBox, GetState(), GetText(), GetAlpha());
	}

	duImage *pIcon = (duImage *)GetResObj(m_szIcon, DU_RES_IMAGE);
	if (pIcon == NULL)
		return;

	duRect rcIcon;
	GetIconRect(rcIcon);
	DrawStretch(hDC, rcIcon.left, rcIcon.top, rcIcon.Width(), rcIcon.Height(), 
		pIcon, 0, 0, pIcon->GetWidth(), pIcon->GetHeight(), GetAlpha());
}


void WINAPI duCheckBox::OnMouseIn(POINT pt)
{
	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return;

	if (*m_szToolTip != 0)
		pCtrlManager->AddToolTip(m_szToolTip);

	UINT uState = GetState();
	if(IS_STATE_CHECKED(uState))
		Plugin_SetState(this, DU_STATE_CHECKEDOVER);
	else
		Plugin_SetState(this, DU_STATE_OVER);
	if(m_fFade)
	{
		if(IS_STATE_CHECKED(uState))
			FadeRedraw(DU_STATE_CHECKEDOVER, DU_STATE_CHECKED);
		else
			FadeRedraw(DU_STATE_OVER, DU_STATE_NORMAL);
	}
	else
		Plugin_Redraw(this, TRUE);
}

void WINAPI duCheckBox::OnMouseLeave(POINT pt)
{
	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return;

	pCtrlManager->DelToolTip();

	UINT uState = GetState();
	if (IS_STATE_CHECKED(uState))
		Plugin_SetState(this, DU_STATE_CHECKED);
	else
		Plugin_SetState(this, DU_STATE_NORMAL);

	if(m_fFade)
	{
		if(IS_STATE_CHECKED(uState))
			FadeRedraw(DU_STATE_CHECKED, uState);
		else
			FadeRedraw(DU_STATE_NORMAL, uState);
	}
	else
		Plugin_Redraw(this, TRUE);
}

void WINAPI duCheckBox::OnMouseLDown(POINT pt)
{
	UINT uState = GetState();
	if (IS_STATE_CHECKED(uState))
		Plugin_SetState(this, DU_STATE_CHECKEDPRESS);
	else
		Plugin_SetState(this, DU_STATE_PRESS);

	Plugin_Redraw(this, TRUE);
}

void WINAPI duCheckBox::OnMouseLUp(POINT pt)
{
	UINT uState = GetState();
	if (IS_STATE_CHECKED(uState))
	{
        if (uState == DU_STATE_CHECKEDPRESS)
		{
            Plugin_SetState(this, DU_STATE_NORMAL);
			m_fChecked = FALSE;
			NotifyUser(DUM_CKCLICK, (WPARAM)FALSE, NULL);
		}
		else
		{
			Plugin_SetState(this, DU_STATE_CHECKED);
			m_fChecked = TRUE;
		}
	}
	else
	{
		if (uState == DU_STATE_PRESS)
		{
			Plugin_SetState(this, DU_STATE_CHECKED);
			m_fChecked = TRUE;
			NotifyUser(DUM_CKCLICK, (WPARAM)TRUE, NULL);
		}
		else
		{
			Plugin_SetState(this, DU_STATE_NORMAL);
			m_fChecked = FALSE;
		}
	}

	Plugin_Redraw(this, TRUE);
}

void WINAPI duCheckBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
// ×¢ÊÍ¼üÅÌ¿ì½Ý¼ü£¬ modify by Alex Liang, 2014-10-22
/*
	if (nChar == VK_SPACE)
	{
		if(m_fChecked)
		{
			m_fChecked = !m_fChecked;
			SetChecked(m_fChecked);
			if(m_fFade)
			{
				FadeRedraw(DU_STATE_CHECKEDOVER, DU_STATE_OVER);
			}
			else
			{
				Plugin_Redraw(this, TRUE);
			}
		}
		else
		{
			m_fChecked = !m_fChecked;
			SetChecked(m_fChecked);
			if(m_fFade)
			{
				FadeRedraw(DU_STATE_OVER, DU_STATE_CHECKEDOVER);
			}
			else
			{
				Plugin_Redraw(this, TRUE);
			}

		}
		NotifyUser(DUM_CKCLICK, (WPARAM)m_fChecked, NULL);
	}
*/
}

void WINAPI duCheckBox::SetText(LPCTSTR lpszText)
{
	if (m_fAutoSize)
		ResizeByText(lpszText);

	duPlugin::SetText(lpszText);
}

BOOL WINAPI duCheckBox::IsChecked()
{
	return m_fChecked;
}

void WINAPI duCheckBox::SetChecked(BOOL fChecked)
{
	m_fChecked = fChecked;
	fChecked ? Plugin_SetState(this, DU_STATE_CHECKED) : Plugin_SetState(this, DU_STATE_NORMAL);
}

void WINAPI duCheckBox::SetIcon(LPCTSTR lpszIcon)
{
	if (lpszIcon)
		_tcsncpy(m_szIcon, lpszIcon, MAX_NAME);
	else
		ZeroMemory(m_szIcon, sizeof(TCHAR) * MAX_NAME);
}

void WINAPI duCheckBox::ResizeByText(LPCTSTR lpszText)
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

void WINAPI duCheckBox::OnRedraw()
{
	m_fFadeDrawing = FALSE;
	Plugin_KillTimer(this, m_nFadeTimer);
}

void WINAPI duCheckBox::OnTimer(UINT nEventId)
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


void duCheckBox::GetIconRect(duRect &rcIcon)
{
	rcIcon.SetRectEmpty();

	duRect rcChkBox;
	Plugin_GetRect(this, &rcChkBox);
	rcChkBox.OffsetRect(-rcChkBox.left, -rcChkBox.top);

	if (m_nIconX == -1)
		rcIcon.left = (rcChkBox.Width() - m_nIconWidth) / 2;
	else
		rcIcon.left = m_nIconX;

	if (m_nIconY == -1)
		rcIcon.top = (rcChkBox.Height() - m_nIconHeight) / 2;
	else
		rcIcon.top = m_nIconY;

	rcIcon.right = rcIcon.left + m_nIconWidth;
	rcIcon.bottom = rcIcon.top + m_nIconHeight;
}



void duCheckBox::FadeRedraw(UINT uFadeInState, UINT uFadeOutState)
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


void WINAPI duCheckBox::SetTooltipText(LPCTSTR lpszText)
{
	if (lpszText)
		_tcsncpy(m_szToolTip, lpszText, MAX_NAME);
	else
		ZeroMemory(m_szToolTip, MAX_NAME * sizeof(TCHAR));
}
