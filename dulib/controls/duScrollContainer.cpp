//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duScrollContainer.cpp
//
//  History:    Mar-08-2011   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "duScrollContainer.h"

duScrollContainer::duScrollContainer(void) :
	 m_fHorz(FALSE)
	,m_fVert(FALSE)
	,m_nXOffset(0)
	,m_nYOffset(0)
{
	ZeroMemory(m_szVertScroll, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szHorzScroll, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szView, MAX_NAME * sizeof(TCHAR));
	
	m_fDispMouseWheel = FALSE;
}

duScrollContainer::~duScrollContainer(void)
{
	Plugin_UnHookWindowMessage(this);
}

void duScrollContainer::RegisterControlProperty()
{
	RegisterProperty(_T("vertscrollbar"),  DU_PROPERTY_STRING, m_szVertScroll);
	RegisterProperty(_T("horzscrollbar"),  DU_PROPERTY_STRING, m_szHorzScroll);
	RegisterProperty(_T("view"),  DU_PROPERTY_STRING, m_szView);
}

void duScrollContainer::OnCreate()
{
	duPlugin *pPlugin = GetPluginByName(m_szView);
	if (Plugin_IsValid(pPlugin))
	{
		pPlugin->SetPosHorzAnchor(3);
		pPlugin->SetPosVertAnchor(3);
		pPlugin->SetPosTop(0);
		pPlugin->SetPosLeft(0);
		pPlugin->Resize(NULL);
	}

	UpdateScroll();
	//SetCached(TRUE);
	Plugin_HookWindowMessage(this);
}

void duScrollContainer::DrawObject(HDC hDC)
{
	duRect rcCtrl;
	Plugin_GetRect(this, &rcCtrl);
	rcCtrl.OffsetRect(-rcCtrl.left, -rcCtrl.top);

	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
		pStyleGroup->Draw(hDC, &rcCtrl, GetState(), GetText(), GetAlpha());	
}

void duScrollContainer::Resize(LPRECT lpRect/*=NULL*/)
{
	duPlugin::Resize(lpRect);
	//UpdateScroll();
	duRect rcScrollContainer;
	Plugin_GetRect(this, &rcScrollContainer);
	NotifyUser(DUM_SCROLLCONTAINTERSIZE, rcScrollContainer.Width(), rcScrollContainer.Height());
}

LRESULT duScrollContainer::OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!Plugin_IsVisible(this))
	{
		m_fDispMouseWheel = FALSE;
		return 0;
	}

	if (uMsg == WM_LBUTTONDOWN)
	{
		POINT pointClient; 
		pointClient.x = GET_X_LPARAM(lParam);
		pointClient.y = GET_Y_LPARAM(lParam);

		duRect rcListBox;
		Plugin_GetRect(this, &rcListBox);

		if (rcListBox.PtInRect(pointClient))
		{
			duScroll *pVertScrollBar = (duScroll *)GetPluginByName(m_szVertScroll);
			if (pVertScrollBar)
			{
				duRect rcVertScroll;
				pVertScrollBar->GetRect(&rcVertScroll);

				if (m_fVert)
				{
					if (rcVertScroll.PtInRect(pointClient))
					{
						m_fDispMouseWheel = FALSE;
						return 0;
					}
				}
			}

			duScroll *pHorzScrollBar = (duScroll *)GetPluginByName(m_szHorzScroll);
			if (pHorzScrollBar)
			{
				duRect rcHorzScroll;
				pHorzScrollBar->GetRect(&rcHorzScroll);

				if (m_fHorz)
				{
					if (rcHorzScroll.PtInRect(pointClient))
					{
						m_fDispMouseWheel = FALSE;
						return 0;
					}
				}
			}

			m_fDispMouseWheel = TRUE;
		}
		else
		{
			m_fDispMouseWheel = FALSE;
		}
	}
	else if (uMsg == WM_MOUSEWHEEL)
	{
		POINT pt;
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);

		UINT fwKeys = (UINT)(short)LOWORD(wParam);
		int  zDelta = (int)(short)HIWORD(wParam);

		duScroll *pVertScrollBar = (duScroll *)GetPluginByName(m_szVertScroll);
		if (m_fVert && Plugin_IsValid(pVertScrollBar) && m_fDispMouseWheel)
			pVertScrollBar->OnMouseWheel(fwKeys, zDelta, pt);
	}
	
	return 0;
}

void duScrollContainer::OnMouseWheel(UINT fwKeys, int zDelta, POINT pt)
{
//	duScroll *pVScroll = (duScroll *)GetPluginByName(m_szVertScroll);
//	if (pVScroll == NULL)
//		return;

//	if (m_fVert)
//		pVScroll->OnMouseWheel(fwKeys, zDelta, pt);
}

void duScrollContainer::OnVScroll(ScrollBarAction sbAction, int nPos)
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
				SetViewTop(-m_nYOffset);
				Plugin_Redraw(this, TRUE);
			}
		}
		break;
	
	default:
		_TRACE(_T("OnVScroll Error pos[%d]\n"), nPos);
		break;
	}
}

void duScrollContainer::OnHScroll(ScrollBarAction sbAction, int nPos)
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
				SetViewLeft(-m_nXOffset);
				Plugin_Redraw(this, TRUE);
			}
		}
		break;

	default:
		break;
	}
}
/*
void duScrollContainer::OnChildShow(duPlugin *pChild, BOOL fVisible)
{
	if (!Plugin_IsValid(pChild))
		return;

	if (lstrcmpi(pChild->GetName(), m_szVertScroll) == 0)
	{
		_TRACE(_T("VertScrollBar Show[%d]\n"), fVisible);
	}
	else if (lstrcmpi(pChild->GetName(), m_szHorzScroll) == 0)
	{
		_TRACE(_T("HorzScrollBar Show[%d]\n"), fVisible);
	}
}
*/
void duScrollContainer::GetViewSize(LPSIZE lpSize)
{
	if (lpSize == NULL)
		return;
		
	lpSize->cx = lpSize->cy = 0;
	
	duPlugin *pPlugin = GetPluginByName(m_szView);
	if (!Plugin_IsValid(pPlugin))
		return;
	
	duRect rcView;
	Plugin_GetRect(pPlugin, &rcView);
	lpSize->cx = rcView.Width();
	lpSize->cy = rcView.Height();
}

POINT duScrollContainer::GetViewPoint()
{
	POINT pt = {m_nXOffset, m_nYOffset};
	return pt;
}

BOOL duScrollContainer::UpdateScroll()
{	
	duScroll *pVScroll = (duScroll *)GetPluginByName(m_szVertScroll);
	duScroll *pHScroll = (duScroll *)GetPluginByName(m_szHorzScroll);

	if (pVScroll == NULL || pHScroll == NULL)
		return FALSE;

	duSize sizeView;
	GetViewSize(&sizeView);
	
	duRect rcScrollView;
	GetRect(&rcScrollView);
	duRect rcHScroll;
	pHScroll->GetRect(&rcHScroll);
	duRect rcVScroll;
	pVScroll->GetRect(&rcVScroll);

	if (rcScrollView.Width() >= sizeView.cx && rcScrollView.Height() >= sizeView.cy)
	{
		pHScroll->SetVisible(FALSE);
		m_fHorz = FALSE;
		pVScroll->SetVisible(FALSE);
		m_fVert = FALSE;

		pHScroll->SetPos(0);
		pVScroll->SetPos(0);
	}
	else if (rcScrollView.Width() < sizeView.cx && rcScrollView.Height() < sizeView.cy)
	{
		pHScroll->SetVisible(TRUE);
		m_fHorz = TRUE;
		pVScroll->SetVisible(TRUE);
		m_fVert = TRUE;
	}
	else if (rcScrollView.Width() >= sizeView.cx && rcScrollView.Height() < sizeView.cy)
	{
		pVScroll->SetVisible(TRUE);
		m_fVert = TRUE;

		if (sizeView.cx + rcVScroll.Width() > rcScrollView.Width())
		{
			pHScroll->SetVisible(TRUE);
			m_fHorz = TRUE;
		}
		else
		{
			pHScroll->SetVisible(FALSE);
			m_fHorz = FALSE;
			pHScroll->SetPos(0);
		}
	}
	else if (rcScrollView.Width() < sizeView.cx && rcScrollView.Height() >= sizeView.cy)
	{
		pHScroll->SetVisible(TRUE);
		m_fHorz = TRUE;
		
		if (sizeView.cy + rcHScroll.Height() > rcScrollView.Height())
		{
			pVScroll->SetVisible(TRUE);
			m_fVert = TRUE;
		}
		else
		{
			pVScroll->SetVisible(FALSE);
			m_fVert = FALSE;
			pVScroll->SetPos(0);
		}
	}
	
	int nClientHeight = rcScrollView.Height() - rcHScroll.Height();
	int nClientWidth = rcScrollView.Width() - rcVScroll.Width();
	int nVMaxPosValue = sizeView.cy - nClientHeight;
	int nHMaxPosValue = sizeView.cx - nClientWidth;
		
	if (m_fVert && m_fHorz)
	{
		pVScroll->SetMaxPos(sizeView.cy);
		pVScroll->SetPage(nClientHeight);
		
		if (pVScroll->GetPos() > nVMaxPosValue)
		{
			pVScroll->SetPos(nVMaxPosValue);
			//m_nYOffset = pVScroll->GetPos();
		}

		pVScroll->SetPosBottom(0);
		pVScroll->Resize(NULL);

		pHScroll->SetMaxPos(sizeView.cx);
		pHScroll->SetPage(nClientWidth);
		if (pHScroll->GetPos() > nHMaxPosValue)
		{
			pHScroll->SetPos(nHMaxPosValue);
			//m_nXOffset = pHScroll->GetPos();
		}
		
		pHScroll->SetPosRight(rcVScroll.Width());
		pHScroll->Resize(NULL);
	}
	else if (m_fVert)
	{
		pVScroll->SetMaxPos(sizeView.cy);
		pVScroll->SetPage(rcScrollView.Height());
		if (pVScroll->GetPos() > (sizeView.cy-rcScrollView.Height()))
		{
			pVScroll->SetPos(sizeView.cy-rcScrollView.Height());
			//m_nYOffset = pVScroll->GetPos();
		}

		pVScroll->SetPosBottom(0);
		pVScroll->Resize(NULL);
	}
	else if (m_fHorz)
	{
		pHScroll->SetMaxPos(sizeView.cx);
		pHScroll->SetPage(rcScrollView.Width());
		if (pHScroll->GetPos() > (sizeView.cx-rcScrollView.Width()))
		{
			pHScroll->SetPos(sizeView.cx-rcScrollView.Width());
			//m_nXOffset = pHScroll->GetPos();
		}

		pHScroll->SetPosRight(0);
		pHScroll->Resize(NULL);
	}

	m_nXOffset = pHScroll->GetPos();
	m_nYOffset = pVScroll->GetPos();

	SetViewLeft(-m_nXOffset);
	SetViewTop(-m_nYOffset);
	return TRUE;
}

void duScrollContainer::SetViewTop(int nTop)
{
	duPlugin *pPlugin = GetPluginByName(m_szView);
	if (!Plugin_IsValid(pPlugin))
		return;
	
	pPlugin->SetPosTop(nTop);
	pPlugin->Resize(NULL);
}

void duScrollContainer::SetViewLeft(int nLeft)
{
	duPlugin *pPlugin = GetPluginByName(m_szView);
	if (!Plugin_IsValid(pPlugin))
		return;
	
	pPlugin->SetPosLeft(nLeft);
	pPlugin->Resize(NULL);
}

void duScrollContainer::SetHorzScrollBar(LPCTSTR lpszScroll)
{
	if (lpszScroll)
		_tcsncpy(m_szHorzScroll, lpszScroll, MAX_NAME);
	else
		ZeroMemory(m_szHorzScroll, sizeof(TCHAR) * MAX_NAME);
}

void duScrollContainer::SetVertScrollBar(LPCTSTR lpszScroll)
{
	if (lpszScroll)
		_tcsncpy(m_szVertScroll, lpszScroll, MAX_NAME);
	else
		ZeroMemory(m_szVertScroll, sizeof(TCHAR) * MAX_NAME);
}

void duScrollContainer::SetView(LPCTSTR lpszView)
{
	if (lpszView)
		_tcsncpy(m_szView, lpszView, MAX_NAME);
	else
		ZeroMemory(m_szView, sizeof(TCHAR) * MAX_NAME);
}
