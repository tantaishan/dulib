//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duRadio.cpp
//
//  History:    Nov-17-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "duRadio.h"
#include "duDrawHelper.h"
#include "duCtrlManager.h"

duRadio::duRadio() :
	m_nGroupId(0)
   ,m_nIconX(0)
   ,m_nIconY(0)
   ,m_nIconWidth(0)
   ,m_nIconHeight(0)
   ,m_fChecked(FALSE)
{
	ZeroMemory(m_szIcon, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szToolTip, MAX_NAME * sizeof(TCHAR));
}

duRadio::~duRadio()
{
}

void duRadio::RegisterControlProperty()
{
	RegisterProperty(_T("icon"), DU_PROPERTY_IMAGE, &m_szIcon);
	RegisterProperty(_T("iconx"), DU_PROPERTY_LONG, &m_nIconX);
	RegisterProperty(_T("icony"), DU_PROPERTY_LONG, &m_nIconY);
	RegisterProperty(_T("iconwidth"), DU_PROPERTY_LONG, &m_nIconWidth);
	RegisterProperty(_T("iconheight"), DU_PROPERTY_LONG, &m_nIconHeight);
	RegisterProperty(_T("groupid"), DU_PROPERTY_LONG, &m_nGroupId);
	RegisterProperty(_T("checked"), DU_PROPERTY_BOOL, &m_fChecked);
	
	RegisterProperty(_T("tooltip"), DU_PROPERTY_STRING, m_szToolTip);
}

void duRadio::OnCreate()
{
	if (m_fChecked)
		SetChecked(TRUE);
}

void duRadio::DrawObject(HDC hDC)
{
	duRect rcRadio;
	Plugin_GetRect(this, &rcRadio);
	
	rcRadio.OffsetRect(-rcRadio.left, -rcRadio.top);
	DrawByStyle(this, GetStyle(), hDC, &rcRadio, GetState(), GetText(), GetAlpha());

	duImage *pIcon = (duImage *)GetResObj(m_szIcon, DU_RES_IMAGE);
	if (pIcon == NULL)
		return;
	
	duRect rcIconDst;
	GetIconRect(rcIconDst);
	DrawStretch(hDC, rcIconDst.left, rcIconDst.top, rcIconDst.Width(), rcIconDst.Height(),
		pIcon, 0, 0, pIcon->GetWidth(), pIcon->GetHeight(), GetAlpha());
}

void duRadio::GetIconRect(duRect &rcIcon)
{
	rcIcon.SetRectEmpty();
	
	duRect rcButton;
	rcButton.SetRectEmpty();
	GetRect(&rcButton);
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

void duRadio::OnMouseIn(POINT pt)
{
	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return;

	if (*m_szToolTip != 0)
		pCtrlManager->AddToolTip(m_szToolTip);


	UINT uState = GetState();
	if (IS_STATE_CHECKED(uState))
		Plugin_SetState(this, DU_STATE_CHECKEDOVER);
	else
		Plugin_SetState(this, DU_STATE_OVER);

	Plugin_Redraw(this, TRUE);
}

void duRadio::OnMouseLeave(POINT pt)
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

	Plugin_Redraw(this, TRUE);
}

void duRadio::OnMouseLDown(POINT pt)
{
	UINT uState = GetState();
	if (IS_STATE_CHECKED(uState))
		Plugin_SetState(this, DU_STATE_CHECKEDPRESS);
	else
		Plugin_SetState(this, DU_STATE_PRESS);

	Plugin_Redraw(this, TRUE);
}

void duRadio::OnMouseLUp(POINT pt)
{
	UINT uState = GetState();
	if (uState == DU_STATE_PRESS)
	{
		NotifyUser(DUM_RADIOCLICK, (WPARAM)TRUE, NULL);
		Plugin_SetState(this, DU_STATE_CHECKED);
		SetChecked(TRUE);
		Plugin_Redraw(this, TRUE);
	}
//	else
//		Plugin_SetState(this, DU_STATE_NORMAL);
}

BOOL duRadio::IsChecked()
{
	UINT uState = GetState();
	if (IS_STATE_CHECKED(uState))
		return TRUE;
	
	return FALSE;
}

void duRadio::SetChecked(BOOL fChecked)
{
	duPlugin *pParent = GetParent();
	if (pParent)
	{
		duPlugin *pChild = pParent->GetFirstChild();
		while(pChild)
		{
			if (lstrcmpi(pChild->GetType(), _T("radio")) == 0)
			{
				duRadio *pRadio = (duRadio *)pChild;
				if (pRadio->GetGroupId() == m_nGroupId)
				{
					UINT uState = pRadio->GetState();
					Plugin_SetState(pRadio, DU_STATE_NORMAL);
					if (IS_STATE_CHECKED(uState))
						Plugin_Redraw(pRadio, TRUE);
				}
			}

			pChild = pChild->GetNextSibling();
		}

		if (fChecked)
			Plugin_SetState(this, DU_STATE_CHECKED);
		else
			Plugin_SetState(this, DU_STATE_NORMAL);
	}
}


void duRadio::SetIcon(LPCTSTR lpszIconName)
{
	if (lpszIconName)
		_tcsncpy(m_szIcon, lpszIconName, MAX_NAME);
	else
		ZeroMemory(m_szIcon, sizeof(TCHAR) * MAX_NAME);
}

void duRadio::SetTooltipText(LPCTSTR lpszText)
{
	if (lpszText)
		_tcsncpy(m_szToolTip, lpszText, MAX_NAME);
	else
		ZeroMemory(m_szToolTip, MAX_NAME * sizeof(TCHAR));
}
