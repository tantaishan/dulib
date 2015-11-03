//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duListBoxEx.cpp
//
//  History:    MAR-16-2010   Denny Chen  Created
//
//--------------------------------------------------------------------------
#include "duListBoxEx.h"

duListBoxEx::duListBoxEx() :
	 m_nYOffset(0)
	,m_pHotItem(NULL)
	,m_pPressItem(NULL)
	,m_pSelItem(NULL)
	,m_fContentMenu(FALSE)
	,m_fDispMouseWheel(FALSE)
{
	ZeroMemory(m_szItemStyle, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szVertScrollBar, MAX_NAME * sizeof(TCHAR));
}

duListBoxEx::~duListBoxEx()
{
}

void duListBoxEx::RegisterControlProperty()
{
	RegisterProperty(_T("itemstyle"), DU_PROPERTY_STYLEGROUP,  &m_szItemStyle);
	RegisterProperty(_T("vertscrollbar"),  DU_PROPERTY_STRING, m_szVertScrollBar);
}

void duListBoxEx::OnCreate()
{
	duScroll *pVertScrollBar = (duScroll *)GetPluginByName(m_szVertScrollBar);
	if (pVertScrollBar)
		pVertScrollBar->SetPos(0);

	RefreshPane();
	Plugin_HookWindowMessage(this);
}

void duListBoxEx::DrawObject(HDC hDC)
{
	duRect rcListBox;
	Plugin_GetRect(this, &rcListBox);
	rcListBox.OffsetRect(-rcListBox.left, -rcListBox.top);

	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
		pStyleGroup->Draw(hDC, &rcListBox, GetState(), GetText(), GetAlpha());

	pStyleGroup = (duStyleGroup *)GetResObj(m_szItemStyle, DU_RES_STYLEGROUP);
	if (pStyleGroup == NULL)
		return;

	Plugin_GetRect(this, &rcListBox);

	//////////////////////////////////////////////////////////////////////////
	//加入绘制item normal状态的代码
	duRect rectNormalItem;
	duPlugin *pNow = GetFirstChild();
	while(pNow)
	{
		LPCTSTR lpszType = pNow->GetType();
		if (lstrcmpi(lpszType, _T("scrollbar")) == 0)
		{
			pNow = pNow->GetNextSibling();
			continue;
		}

		if (pNow && pNow->IsVisible())
		{
			rectNormalItem.SetRectEmpty();
			pNow->GetRect(&rectNormalItem);
			rectNormalItem.OffsetRect(-rcListBox.left, -rcListBox.top);

			pStyleGroup->Draw(hDC,&rectNormalItem, DU_STATE_NORMAL, GetText(), GetAlpha());
		}
		pNow = pNow->GetNextSibling();
	}


	duRect rcItem;
	if (m_pHotItem)
	{
		Plugin_GetRect(m_pHotItem, &rcItem);
		rcItem.OffsetRect(-rcListBox.left, -rcListBox.top);
		pStyleGroup->Draw(hDC, &rcItem, m_pPressItem ? DU_STATE_PRESS : DU_STATE_OVER, NULL, GetAlpha());
	}

	if (m_pSelItem) 
	{
		Plugin_GetRect(m_pSelItem, &rcItem);
		rcItem.OffsetRect(-rcListBox.left, -rcListBox.top);
		pStyleGroup->Draw(hDC, &rcItem, DU_STATE_FOCUS, NULL, GetAlpha());
	}
}

LRESULT duListBoxEx::OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!Plugin_IsVisible(this))
	{
		m_fDispMouseWheel = FALSE;
		return 0;
	}

	if (uMsg == WM_MOUSEMOVE)
	{
		POINT pointClient;
		pointClient.x = GET_X_LPARAM(lParam);
		pointClient.y = GET_Y_LPARAM(lParam);
		
		duRect rcListBox;
		Plugin_GetRect(this, &rcListBox);

		if (!rcListBox.PtInRect(pointClient))
		{
			if (m_pHotItem)
			{
				m_pHotItem = NULL;
				Plugin_Redraw(this, TRUE);
			}
			return 0;
		}

		BOOL fInItem = FALSE;
		duPlugin *pChild = GetFirstChild();
		while (pChild)
		{
			LPCTSTR lpszType = pChild->GetType();
			if (lstrcmpi(lpszType, _T("scrollbar")) == 0)
			{
				pChild = pChild->GetNextSibling();
				continue;
			}

			if (pChild->IsVisible())
			{
				duRect rcChild;
				Plugin_GetRect(pChild, &rcChild);

				if (rcChild.PtInRect(pointClient))
				{
					if (m_pHotItem != pChild)
					{
						m_pPressItem = NULL;
						m_pHotItem = pChild;
						Plugin_Redraw(this, TRUE);
						return 0;
					}
					fInItem = TRUE;
				}
			}

			pChild = pChild->GetNextSibling();
		}

		if (!fInItem)
		{
			if (m_pHotItem)
			{
				m_pPressItem = NULL;
				m_pHotItem = NULL;
				Plugin_Redraw(this, TRUE);
			}
		}
	}
	else if (uMsg == WM_LBUTTONDOWN)
	{
		POINT pointClient; 
		pointClient.x = GET_X_LPARAM(lParam);
		pointClient.y = GET_Y_LPARAM(lParam);

		duRect rcListBox;
		Plugin_GetRect(this, &rcListBox);

		if (Plugin_IsValid(m_pHotItem))
		{
			m_pPressItem = m_pHotItem;
			Plugin_Redraw(this, TRUE);
			m_fDispMouseWheel = TRUE;
		}
		else
		{
			m_pHotItem = NULL;
			m_fDispMouseWheel = FALSE;
		}
	}
	else if (uMsg == WM_LBUTTONUP)
	{
		if (Plugin_IsValid(m_pPressItem))
		{
			NotifyUser(DUM_LBEXSELCHANGED, (WPARAM)m_pSelItem, (LPARAM)m_pPressItem);
			if (Plugin_IsValid(m_pPressItem))
				m_pSelItem = m_pPressItem;

			Plugin_Redraw(this, TRUE);
		}
		else
			m_pPressItem = NULL;
	}
	else if (uMsg == WM_LBUTTONDBLCLK)
	{
		if (Plugin_IsValid(m_pHotItem))
		{
			NotifyUser(DUM_LBEXDBCLICK, (WPARAM)m_pHotItem, NULL);
			if (!Plugin_IsValid(m_pHotItem))
				m_pHotItem = NULL;
		}
		else
			m_pHotItem = NULL;
	}
	else if (uMsg == WM_MOUSEHOVER)
	{
		if (Plugin_IsValid(m_pHotItem))
		{
			NotifyUser(DUM_LBEXHOVER, (WPARAM)m_pHotItem, NULL);
			if (!Plugin_IsValid(m_pHotItem))
				m_pHotItem = NULL;
		}
		else
			m_pHotItem = NULL;
	}
	else if (uMsg == WM_RBUTTONDOWN)
	{
		POINT pointClient;
		pointClient.x = GET_X_LPARAM(lParam);
		pointClient.y = GET_Y_LPARAM(lParam);
		
		duRect rcListBox;
		Plugin_GetRect(this, &rcListBox);

		if (rcListBox.PtInRect(pointClient))
			m_fContentMenu = TRUE;
	}
	else if (uMsg == WM_RBUTTONUP)
	{
		POINT pointClient;
		pointClient.x = GET_X_LPARAM(lParam);
		pointClient.y = GET_Y_LPARAM(lParam);
		
		duRect rcListBox;
		Plugin_GetRect(this, &rcListBox);

		if (rcListBox.PtInRect(pointClient))
		{
			if (m_fContentMenu)
			{
				if (!Plugin_IsValid(m_pHotItem))
					m_pHotItem = NULL;

				NotifyUser(DUM_LBEXCONTENTMENU, (WPARAM)m_pHotItem, NULL);
			}
		}

		m_fContentMenu = FALSE;
	}
	else if (uMsg == WM_MOUSEWHEEL)
	{
		POINT pt;
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);

		UINT fwKeys = (UINT)(short)LOWORD(wParam);
		int  zDelta = (int)(short)HIWORD(wParam);

		duScroll *pVertScrollBar = (duScroll *)GetPluginByName(m_szVertScrollBar);
		if (Plugin_IsValid(pVertScrollBar) && m_fDispMouseWheel)
			pVertScrollBar->OnMouseWheel(fwKeys, zDelta, pt);
	}
	else if(uMsg == WM_MOUSELEAVE)
	{
		if(Plugin_IsValid(m_pHotItem))
		{
			m_pHotItem = NULL;
			if(Plugin_IsValid(m_pPressItem))
				m_pPressItem = NULL;
			Plugin_Redraw(this, TRUE);
		}
	}

	return 0;
}

void duListBoxEx::OnVScroll(ScrollBarAction sbAction, int nPos)
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
				RefreshPane();
				Plugin_Redraw(this, TRUE);
			}
		}
		break;
	
	default:
		_TRACE(_T("OnVScroll Error pos[%d]\n"), nPos);
		break;
	}
}

void duListBoxEx::OnChildShow(duPlugin *pChild, BOOL fVisible)
{
	RefreshPane();
	//Plugin_Redraw(this, TRUE);
}

void duListBoxEx::OnChildDelete(duPlugin *pChild)
{
	if (pChild == m_pHotItem)
		m_pHotItem = NULL;
	if (pChild == m_pPressItem)
		m_pPressItem = NULL;
	if (pChild == m_pSelItem)
		m_pSelItem = NULL;
}

LPCTSTR duListBoxEx::GetItemStyle()
{
	return m_szItemStyle;
}

void duListBoxEx::SetItemStyle(LPCTSTR pStyle)
{
	if (pStyle)
		_tcsncpy(m_szItemStyle, pStyle, MAX_NAME);
	else
		ZeroMemory(m_szItemStyle, MAX_NAME * sizeof(TCHAR));
}

void duListBoxEx::RefreshPane()
{
	duRect rcListBox;
	Plugin_GetRect(this, &rcListBox);

	int nTotalHeight = 0;
	duPlugin *pChild = GetFirstChild();
	while (pChild)
	{
		LPCTSTR lpszType = pChild->GetType();
		if (lstrcmpi(lpszType, _T("scrollbar")) == 0)
		{
			pChild = pChild->GetNextSibling();
			continue;
		}

		if (pChild->IsVisible())
		{
			pChild->SetPosTop(nTotalHeight);
			pChild->SetPosHorzAnchor(3);
			pChild->SetPosVertAnchor(3);
			nTotalHeight += pChild->GetPosHeight();
		}

		pChild = pChild->GetNextSibling();
	}

	duScroll *pVertScrollBar = (duScroll *)GetPluginByName(m_szVertScrollBar);
	if (pVertScrollBar == NULL)
		return;

	if (nTotalHeight > rcListBox.Height())
	{
		pVertScrollBar->SetVisible(TRUE);

		if (pVertScrollBar->GetPos() > (nTotalHeight-rcListBox.Height()))
			pVertScrollBar->SetPos(nTotalHeight-rcListBox.Height());

		pVertScrollBar->SetMaxPos(nTotalHeight);
		pVertScrollBar->SetPage(rcListBox.Height());
	}
	else
	{
		pVertScrollBar->SetVisible(FALSE);
		pVertScrollBar->SetPos(0);
	}

	m_nYOffset = pVertScrollBar->GetPos();
	Resize(NULL);
}

void duListBoxEx::Resize(LPRECT lpRect/*=NULL*/)
{
	duPlugin *pParent = GetParent();
	if (pParent)
	{
		RECT rectParent;
		if (lpRect)
			CopyRect(&rectParent, lpRect);
		else
			pParent->GetRect(&rectParent);

		MakeRectExt(m_nLeft, m_nTop, m_nRight, m_nBottom, m_nHorzanchor, m_nVertanchor, 
			m_nWidth, m_nHeight, &rectParent, &m_rect);
	}

	duScroll *pVertScrollBar = (duScroll *)GetPluginByName(m_szVertScrollBar);
	if (pVertScrollBar == NULL)
	{
		duPlugin *pChild = GetFirstChild();
		while (pChild)
		{
			pChild->Resize(NULL);
			pChild = pChild->GetNextSibling();
		}
	}
	else
	{
		ResetScrollPos();
		duPlugin *pChild = NULL;
		duRect rcListBox(m_rect);
		if (pVertScrollBar->IsVisible())
		{
			duRect rcVertScrollBar;
			Plugin_GetRect(pVertScrollBar, &rcVertScrollBar);
			int nChildWidth = rcListBox.Width() - rcVertScrollBar.Width();

			rcListBox.OffsetRect(0, -m_nYOffset);
			pChild = GetFirstChild();
			while (pChild)
			{
				LPCTSTR lpszType = pChild->GetType();
				if (lstrcmpi(lpszType, _T("scrollbar")) == 0)
				{
					pChild->Resize(NULL);
					pChild = pChild->GetNextSibling();
					continue;
				}
				
				pChild->SetPosWidth(nChildWidth);
				pChild->Resize(&rcListBox);
				pChild = pChild->GetNextSibling();
			}
		}
		else
		{
			pChild = GetFirstChild();
			while (pChild)
			{
				LPCTSTR lpszType = pChild->GetType();
				if (lstrcmpi(lpszType, _T("scrollbar")) == 0)
				{
					pChild->Resize(NULL);
					pChild = pChild->GetNextSibling();
					continue;
				}
				
				pChild->SetPosWidth(rcListBox.Width());
				pChild->Resize(&rcListBox);
				pChild = pChild->GetNextSibling();
			}
		}
	}
}

duScroll *duListBoxEx::GetVertScrollBar()
{
	return (duScroll *)GetPluginByName(m_szVertScrollBar);
}

void duListBoxEx::SetVertScrollBar(LPCTSTR lpszScroll)
{
	if (lpszScroll)
		_tcsncpy(m_szVertScrollBar, lpszScroll, MAX_NAME);
	else
		ZeroMemory(m_szVertScrollBar, sizeof(TCHAR) * MAX_NAME);
}


void duListBoxEx::ResetScrollPos()
{
	duRect rcListBox;
	Plugin_GetRect(this, &rcListBox);

	int nTotalHeight = 0;
	duPlugin *pChild = GetFirstChild();
	while (pChild)
	{
		LPCTSTR lpszType = pChild->GetType();
		if (lstrcmpi(lpszType, _T("scrollbar")) == 0)
		{
			pChild = pChild->GetNextSibling();
			continue;
		}

		if (pChild->IsVisible())
		{
			pChild->SetPosTop(nTotalHeight);
			pChild->SetPosHorzAnchor(3);
			pChild->SetPosVertAnchor(3);
			nTotalHeight += pChild->GetPosHeight();
		}

		pChild = pChild->GetNextSibling();
	}

	duScroll *pVertScrollBar = (duScroll *)GetPluginByName(m_szVertScrollBar);
	if (pVertScrollBar == NULL)
		return;

	if (nTotalHeight > rcListBox.Height())
	{
		pVertScrollBar->SetVisible(TRUE);

		if (pVertScrollBar->GetPos() > (nTotalHeight-rcListBox.Height()))
			pVertScrollBar->SetPos(nTotalHeight-rcListBox.Height());

		pVertScrollBar->SetMaxPos(nTotalHeight);
		pVertScrollBar->SetPage(rcListBox.Height());
	}
	else
	{
		pVertScrollBar->SetVisible(FALSE);
		pVertScrollBar->SetPos(0);
	}

	m_nYOffset = pVertScrollBar->GetPos();
}