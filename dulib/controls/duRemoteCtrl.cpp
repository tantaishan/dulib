//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duRemoteCtrl.cpp
//
//  History:    Mar-08-2011   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duRemoteCtrl.h"
#include "duScroll.h"
#include "duHwndObj.h"

duRemoteCtrl::duRemoteCtrl() :
	 m_nXOffset(0)
	,m_nYOffset(0)
	,m_nViewWidth(0)
	,m_nViewHeight(0)
{
	ZeroMemory(m_szVertScroll, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szHorzScroll, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szHwndObj, MAX_NAME * sizeof(TCHAR));
}

duRemoteCtrl::~duRemoteCtrl()
{
}

void WINAPI duRemoteCtrl::RegisterControlProperty()
{
	RegisterProperty(_T("vertscrollbar"),  DU_PROPERTY_STRING, m_szVertScroll);
	RegisterProperty(_T("horzscrollbar"),  DU_PROPERTY_STRING, m_szHorzScroll);
	RegisterProperty(_T("hwndobj"),  DU_PROPERTY_STRING, m_szHwndObj);
	
}

void WINAPI duRemoteCtrl::OnCreate()
{
}

void WINAPI duRemoteCtrl::DrawObject(HDC hDC)
{
	duRect rcRemoteCtrl;
	Plugin_GetRect(this, &rcRemoteCtrl);
	rcRemoteCtrl.OffsetRect(-rcRemoteCtrl.left, -rcRemoteCtrl.top);

	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
		pStyleGroup->Draw(hDC, &rcRemoteCtrl, GetState(), GetText(), GetAlpha());
}

void WINAPI duRemoteCtrl::Resize(LPRECT lpRect/*=NULL*/)
{
	duPlugin::Resize(lpRect);
	SetViewSize(m_nViewWidth, m_nViewHeight);
}

void WINAPI duRemoteCtrl::OnVScroll(ScrollBarAction sbAction, int nPos)
{
	switch (sbAction)
	{
	case SCROLL_NO_WHERE:
		m_nYOffset = 0;
		break;

	case SCROLL_UP:	
	case SCROLL_DOWN:
	case SCROLL_PAGEDOWN:
	case SCROLL_PAGEUP:
	case SCROLL_TRACK:
	case SCROLL_WHEEL_UP:
	case SCROLL_WHEEL_DOWN:
		{
			if (m_nYOffset != nPos)
			{
				m_nYOffset = nPos;
				NotifyUser(DUM_REMOTECTRLSCROLL, m_nXOffset, m_nYOffset);
			}
		}
		break;
	
	default:
		//_TRACE(_T("OnVScroll Error pos[%d]\n"), nPos);
		break;
	}
}

void WINAPI duRemoteCtrl::OnHScroll(ScrollBarAction sbAction, int nPos)
{
	switch (sbAction)
	{
	case SCROLL_NO_WHERE:
		m_nXOffset = 0;
		break;
		
	case SCROLL_UP:
	case SCROLL_DOWN:
	case SCROLL_TRACK:
	case SCROLL_PAGEUP:
	case SCROLL_PAGEDOWN:
	case SCROLL_WHEEL_UP:
	case SCROLL_WHEEL_DOWN:
		{
			if (m_nXOffset != nPos)
			{
				m_nXOffset = nPos;
				NotifyUser(DUM_REMOTECTRLSCROLL, m_nXOffset, m_nYOffset);
			}
		}
		break;

	default:
		break;
	}
}

void WINAPI duRemoteCtrl::SetHorzScrollBar(LPCTSTR lpszScroll)
{
	if (lpszScroll)
		_tcsncpy(m_szHorzScroll, lpszScroll, MAX_NAME);
	else
		ZeroMemory(m_szHorzScroll, sizeof(TCHAR) * MAX_NAME);
}

void WINAPI duRemoteCtrl::SetVertScrollBar(LPCTSTR lpszScroll)
{
	if (lpszScroll)
		_tcsncpy(m_szVertScroll, lpszScroll, MAX_NAME);
	else
		ZeroMemory(m_szVertScroll, sizeof(TCHAR) * MAX_NAME);
}

void WINAPI duRemoteCtrl::SetViewSize(int nWidth, int nHeight)
{
	if (nWidth < 0)
		nWidth = 0;

	if (nHeight < 0)
		nHeight = 0;

	m_nViewWidth = nWidth;
	m_nViewHeight = nHeight;

	duRect rcHwndObj;
	rcHwndObj.SetRectEmpty();

	duHwndObj *pHwndObj = (duHwndObj *)GetPluginByName(m_szHwndObj);
	if (pHwndObj)
	{
		HWND hWndObj = pHwndObj->GetAttach();
		if (hWndObj)
			GetWindowRect(hWndObj, &rcHwndObj);
	}

	duScroll *pHorzScroll = (duScroll *)GetPluginByName(m_szHorzScroll);
	if (pHorzScroll)
	{
		pHorzScroll->SetMaxPos(nWidth);
		pHorzScroll->SetPage(rcHwndObj.Width());
		if (pHorzScroll->GetPos() > (nWidth - rcHwndObj.Width()))
		{
			pHorzScroll->SetPos(nWidth - rcHwndObj.Width());
			//m_nYOffset = pVScroll->GetPos();
		}

		pHorzScroll->Resize(NULL);
		Plugin_Redraw(pHorzScroll, TRUE);

		m_nXOffset = pHorzScroll->GetPos();
	}

	duScroll *pVertScroll = (duScroll *)GetPluginByName(m_szVertScroll);
	if (pVertScroll)
	{
		pVertScroll->SetMaxPos(nHeight);
		pVertScroll->SetPage(rcHwndObj.Height());
		if (pVertScroll->GetPos() > (nHeight - rcHwndObj.Height()))
		{
			pVertScroll->SetPos(nHeight - rcHwndObj.Height());
			//m_nYOffset = pVScroll->GetPos();
		}
		pVertScroll->Resize(NULL);
		Plugin_Redraw(pVertScroll, TRUE);

		m_nYOffset = pVertScroll->GetPos();
	}
	
	NotifyUser(DUM_REMOTECTRLSCROLL, m_nXOffset, m_nYOffset);
}

void WINAPI duRemoteCtrl::SetHwndObj(LPCTSTR lpszHwndObj)
{
	if (lpszHwndObj)
		_tcsncpy(m_szHwndObj, lpszHwndObj, MAX_NAME);
	else
		ZeroMemory(m_szHwndObj, sizeof(TCHAR) * MAX_NAME);
}
