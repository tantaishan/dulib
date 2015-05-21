//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duListBox.cpp
//
//  History:    MAR-03-2010   Denny Chen  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duListBox.h"

duListBox::duListBox() :
	 m_nItemHeight(0)
	,m_nItemIconX(0)
	,m_nItemIconY(0)
	,m_nItemIconWidth(0)
	,m_nItemIconHeight(0)
	,m_nYOffset(0)
	,m_fVert(FALSE)
	,m_pHotItem(NULL)
	,m_pSelItem(NULL)
	,m_fFadeIn(FALSE)
	,m_nAlphaFadeIn(0)
	,m_nTimerFadeIn(0)
	,m_fFade(FALSE)
{
	ZeroMemory(m_szVertScroll, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szItemStyle, MAX_NAME * sizeof(TCHAR));
	m_vtItem.clear();
	m_fCombobox = FALSE;
}

duListBox::~duListBox()
{
	Destroy();
}

void duListBox::Destroy()
{
	int nItemCount = GetItemCount();
	int i;
	ListBoxItem *pItem = NULL;
	for (i = 0; i < nItemCount; i++)
	{
		pItem = m_vtItem[i];
		SAFE_DELETE(pItem);
	}
	m_vtItem.clear();
	
	m_pHotItem = NULL;
	m_pSelItem = NULL;
}

void WINAPI duListBox::OnCreate()
{
	m_nTimerFadeIn = GetUniTimer();
	UpdateScroll();
}

void WINAPI duListBox::RegisterControlProperty()
{
	RegisterProperty(_T("vertscrollbar"),  DU_PROPERTY_STRING, m_szVertScroll);
	RegisterProperty(_T("itemheight"), DU_PROPERTY_LONG,  &m_nItemHeight);
	RegisterProperty(_T("itemstyle"),  DU_PROPERTY_STYLEGROUP, m_szItemStyle);
	RegisterProperty(_T("itemiconx"),  DU_PROPERTY_LONG, &m_nItemIconX);
	RegisterProperty(_T("itemicony"),  DU_PROPERTY_LONG, &m_nItemIconY);
	RegisterProperty(_T("itemiconwidth"),  DU_PROPERTY_LONG, &m_nItemIconWidth);
	RegisterProperty(_T("itemiconheight"), DU_PROPERTY_LONG, &m_nItemIconHeight);
	RegisterProperty(_T("fade"), DU_PROPERTY_BOOL, &m_fFade);
}

void WINAPI duListBox::DrawObject(HDC hDC)
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

	duRect rcItem;
	rcItem.SetRectEmpty();
	rcItem.right = rcListBox.Width();
	rcItem.OffsetRect(0, -m_nYOffset);
	
	ListBoxItem *pItem = NULL;
	int i;
	int nItemCount = GetItemCount();
	for (i = 0;i < nItemCount; i++)
	{
		pItem = m_vtItem[i];
		rcItem.top = rcItem.bottom;
		rcItem.bottom = rcItem.top + m_nItemHeight;
		DrawItem(pItem, hDC, pStyleGroup, rcItem);
	}
}

void duListBox::DrawItem(ListBoxItem *pItem, HDC hDC, duStyleGroup *pStyleGroup, duRect &rcItem)
{
	int nAlpha = GetAlpha();
	UINT uItemState = DU_STATE_NORMAL;
	if (m_pSelItem == pItem)
		uItemState = DU_STATE_PRESS;

	if (pItem->fDisable)
		pStyleGroup->Draw(hDC, &rcItem, DU_STATE_DISABLE, pItem->szText.c_str(), nAlpha);
	else
	{
		if (m_pHotItem == pItem)
		{	
			if (m_pHotItem == m_pSelItem)
				pStyleGroup->Draw(hDC, &rcItem, DU_STATE_PRESS, pItem->szText.c_str(), nAlpha);
			else
			{
				int nStyleCount = pStyleGroup->GetStyleCount();
				int k;
				for (k = 0; k < nStyleCount; k++)
				{
					duStyleBase *pStyle = pStyleGroup->GetStyle(k);
					if (pStyle && (pStyle->GetState() & DU_STATE_OVER)) 
					{
						if (pStyle->GetType() != STYLE_TEXT)
							pStyle->Draw(hDC, &rcItem, pItem->szText.c_str(), m_nAlphaFadeIn);
						else
							pStyle->Draw(hDC, &rcItem, pItem->szText.c_str(), nAlpha);
					}
				}
			}
		}
		else
		{
			pStyleGroup->Draw(hDC, &rcItem, uItemState, pItem->szText.c_str(), nAlpha);
		}
	}

	duImage *pIconImage = (duImage *)GetResObj(pItem->szIcon.c_str(), DU_RES_IMAGE);
	if (pIconImage == NULL)
		return;
				
	DrawStretch(hDC, rcItem.left + pItem->uIconX, rcItem.top + pItem->uIconY, pItem->uIconWidth, pItem->uIconHeight,
	pIconImage, 0, 0, pIconImage->GetWidth(), pIconImage->GetHeight(), nAlpha);
}

void WINAPI duListBox::OnVScroll(ScrollBarAction sbAction, int nPos)
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
				Plugin_Redraw(this, TRUE);
			}
		}
		break;
	
	default:
		_TRACE(_T("OnVScroll Error pos[%d]\n"), nPos);
		break;
	}
}

BOOL WINAPI duListBox::UpdateScroll()
{
	duScroll *pVScroll = (duScroll *)GetPluginByName(m_szVertScroll);
	if (pVScroll == NULL)
		return FALSE;
	
	int nItemCount = GetItemCount();
	int nViewHeight = nItemCount * m_nItemHeight;
	
	duRect rcListBox;
	Plugin_GetRect(this, &rcListBox);
	
	duRect rcVScroll;
	Plugin_GetRect(pVScroll, &rcVScroll);
	
	if (nViewHeight > rcListBox.Height())
	{
		int nPage = rcListBox.Height();

		pVScroll->SetVisible(TRUE);
		pVScroll->SetMaxPos(nViewHeight);
		pVScroll->SetPage(nPage);
		if (pVScroll->GetPos() > (nViewHeight-nPage))
			pVScroll->SetPos(nViewHeight-nPage);

		//pVScroll->SetPosBottom(0);
		pVScroll->Resize(NULL);
		m_nYOffset = pVScroll->GetPos();
		m_fVert = TRUE;
	}
	else
	{
		pVScroll->SetVisible(FALSE);
		m_nYOffset = 0;
		m_fVert = FALSE;
	}
	
	return TRUE;
}

void WINAPI duListBox::Resize(LPRECT lpRect/*=NULL*/)
{
	duPlugin::Resize(lpRect);
	UpdateScroll();
}

void WINAPI duListBox::OnMouseWheel(UINT fwKeys, int zDelta, POINT pt)
{
	duScroll *pVScroll = (duScroll *)GetPluginByName(m_szVertScroll);
	if (pVScroll == NULL)
		return;

	if (m_fVert)
		pVScroll->OnMouseWheel(fwKeys, zDelta, pt);
}

void WINAPI duListBox::OnMouseLeave(POINT pt)
{
	if (m_pHotItem)
	{
		m_pHotItem = NULL;
		Plugin_Redraw(this, TRUE);
	}
}

void WINAPI duListBox::OnMouseLDown(POINT pt)
{
	if (m_pHotItem != m_pSelItem)
	{
		ListBoxItem *pTemp = m_pSelItem;
		m_pSelItem = m_pHotItem;
		Plugin_Redraw(this, TRUE);

		int nOldSelItemIndex = GetItemIndex(pTemp);
		int nNewSelItemIndex = GetItemIndex(m_pHotItem);
		NotifyUser(DUM_LBSELCHANGED, (WPARAM)nOldSelItemIndex, (LPARAM)nNewSelItemIndex);
	}
}

void WINAPI duListBox::OnMouseLUp(POINT pt)
{
}

void WINAPI duListBox::OnMouseRDown(POINT pt)
{
	if (m_pHotItem)
	{
		int nHotItemIndex = GetItemIndex(m_pHotItem);
		NotifyUser(DUM_LBRBUTTONDOWN, (WPARAM)nHotItemIndex, NULL);
	}
}

void WINAPI duListBox::OnMouseRUp(POINT pt)
{
}

void WINAPI duListBox::OnMouseHover(POINT pt)
{
	if (m_pHotItem)
	{
		int nHotItemIndex = GetItemIndex(m_pHotItem);
		NotifyUser(DUM_LBHOVER, (WPARAM)nHotItemIndex, NULL);
	}
}

void WINAPI duListBox::OnMouseMove(POINT pt)
{
	duRect rcListBox;
	Plugin_GetRect(this, &rcListBox);
	
	pt.x -= rcListBox.left;
	pt.y -= rcListBox.top;

	duPlugin *pScroll = GetPluginByName(m_szVertScroll);

	
	duRect rcItem;
	rcItem.SetRectEmpty();
	if(pScroll != NULL && Plugin_IsVisible(pScroll))
		rcItem.right = rcListBox.Width() - pScroll->GetPosWidth();
	else
		rcItem.right = rcListBox.Width();
	
	rcItem.OffsetRect(0, -m_nYOffset);
	
	int i;
	int nItemCount = GetItemCount();
	ListBoxItem *pItem = NULL;
	for (i = 0;i < nItemCount; i++)
	{
		rcItem.top = rcItem.bottom;
		rcItem.bottom = rcItem.top + m_nItemHeight;
		pItem = m_vtItem[i];
		
		if (rcItem.PtInRect(pt) && m_pHotItem != pItem)
		{
			if (pItem->fDisable)
			{
				m_pHotItem = NULL;
				Plugin_Redraw(this, TRUE);
			}
			else
			{
				m_pHotItem = pItem;
				if (m_pHotItem != m_pSelItem)
					FadeRedraw();
			}
			break;
		}
	}
}

void WINAPI duListBox::OnMouseDbClick(POINT pt)
{
	if (m_pHotItem)
	{
		int nHotItemIndex = GetItemIndex(m_pHotItem);
		NotifyUser(DUM_LBLBUTTONDBCLICK, (WPARAM)nHotItemIndex, NULL);
	}
}

void WINAPI duListBox::OnTimer(UINT nEventId)
{
	if (nEventId == m_nTimerFadeIn)
	{
		m_nAlphaFadeIn += 10;
		if (m_nAlphaFadeIn >= 255)
			m_nAlphaFadeIn = 255;
		
		Plugin_Redraw(this, TRUE);
		if (m_nAlphaFadeIn >= 255)
		{
			Plugin_KillTimer(this, m_nTimerFadeIn);
			m_fFadeIn = FALSE;
			m_nAlphaFadeIn = 0;
		}
	}
}

void WINAPI duListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int nItemCount = GetItemCount();
	if (nItemCount == 0)
		return;

	if (nChar == VK_UP)
	{
		int nSel = GetSel();
		nSel--;
		if (nSel < 0)
			nSel = 0;
		if (nSel >= nItemCount)
			nSel = nItemCount - 1;

		SetSel(nSel);
		duScroll *pScroll = (duScroll *)GetPluginByName(m_szVertScroll);
		if (pScroll)
		{
			pScroll->SetPos(m_nItemHeight * nSel);
			UpdateScroll();
		}
		if (!m_fCombobox)
		{
			m_pHotItem = NULL;
			Plugin_Redraw(this, TRUE);
		}
		else
		{
			FadeRedraw();
		}
	}
	else if (nChar == VK_DOWN)
	{
		int nSel = GetSel();
		_TRACE(_T("nSel :%d\n"), nSel);
		nSel++;
		if (nSel >= nItemCount)
			nSel = nItemCount - 1;
		if (nSel < 0)
			nSel = 0;

		SetSel(nSel);
		duScroll *pScroll = (duScroll *)GetPluginByName(m_szVertScroll);
		if (pScroll)
		{
			pScroll->SetPos(m_nItemHeight * nSel);
			UpdateScroll();
		}
		if (!m_fCombobox)
		{
			m_pHotItem = NULL;
			Plugin_Redraw(this, TRUE);
		}
		else
			FadeRedraw();
	}
}

int WINAPI duListBox::GetItemCount()
{
	return (int)m_vtItem.size();
}

BOOL WINAPI duListBox::InsertItem(int nIndex, LPCTSTR lpszText, LPCTSTR lpszIcon)
{
	int nItemCount = GetItemCount();
	if (nIndex == -1)
		nIndex = nItemCount;
		
	if (nIndex < 0 || nIndex > nItemCount)
		return FALSE;
	
	ListBoxItem *pItem = new ListBoxItem;
	pItem->uIconX = m_nItemIconX;
	pItem->uIconY = m_nItemIconY;
	pItem->uIconWidth = m_nItemIconWidth;
	pItem->uIconHeight = m_nItemIconHeight;
	if (lpszText)
		pItem->szText = lpszText;
	else
		pItem->szText = _T("");
		
	if (lpszIcon)
		pItem->szIcon = lpszIcon;
	else
		pItem->szIcon = _T("");

	pItem->fDisable = FALSE;
	
	vector<ListBoxItem *>::iterator iterInsert = m_vtItem.begin() + nIndex;
	m_vtItem.insert(iterInsert, pItem);
	UpdateScroll();
	return TRUE;
}

BOOL WINAPI duListBox::DeleteItem(int nIndex)
{
	int nItemCount = GetItemCount();
	if (nIndex < 0 || nIndex >= nItemCount)
		return FALSE;
	
	vector<ListBoxItem *>::iterator iterDelete = m_vtItem.begin() + nIndex;
	ListBoxItem *pItem = *iterDelete;
	if (pItem == m_pHotItem)
		m_pHotItem = NULL;
		
	if (pItem == m_pSelItem)
		m_pSelItem = NULL;

	SAFE_DELETE(pItem);
	m_vtItem.erase(iterDelete);
	UpdateScroll();
	return TRUE;
}

void WINAPI duListBox::DeleteAll()
{
	Destroy();
	UpdateScroll();
}

LPCTSTR WINAPI duListBox::GetItemText(int nIndex)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return NULL;
		
	return pItem->szText.c_str();
}

BOOL WINAPI duListBox::SetItemText(int nIndex, LPCTSTR lpszText)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return FALSE;
		
	if (lpszText)
		pItem->szText = lpszText;
	else
		pItem->szText = _T("");
	return TRUE;
}

LPCTSTR WINAPI duListBox::GetItemIcon(int nIndex)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return NULL;
		
	return pItem->szText.c_str();
}

BOOL WINAPI duListBox::SetItemIcon(int nIndex, LPCTSTR lpszIcon)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return FALSE;
		
	if (lpszIcon)
		pItem->szIcon = lpszIcon;
	else
		pItem->szIcon = _T("");
	return TRUE;
}

UINT WINAPI duListBox::GetItemIconX(int nIndex)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return m_nItemIconX;
		
	return pItem->uIconX;
}

BOOL WINAPI duListBox::SetItemIconX(int nIndex, UINT uIconX)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return FALSE;
		
	pItem->uIconX = uIconX;
	return TRUE;
}

UINT WINAPI duListBox::GetItemIconY(int nIndex)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return m_nItemIconY;
		
	return pItem->uIconY;
}

BOOL WINAPI duListBox::SetItemIconY(int nIndex, UINT uIconY)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return FALSE;
		
	pItem->uIconY = uIconY;
	return TRUE;
}

UINT WINAPI duListBox::GetItemIconWidth(int nIndex)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return m_nItemIconWidth;
		
	return pItem->uIconWidth;
}

BOOL WINAPI duListBox::SetItemIconWidth(int nIndex, UINT uIconWidth)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return FALSE;
		
	pItem->uIconWidth = uIconWidth;
	return TRUE;
}

UINT WINAPI duListBox::GetItemIconHeight(int nIndex)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return m_nItemIconHeight;
		
	return pItem->uIconHeight;
}

BOOL WINAPI duListBox::SetItemIconHeight(int nIndex, UINT uIconHeight)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return FALSE;
		
	pItem->uIconHeight = uIconHeight;
	return TRUE;
}

duScroll *WINAPI duListBox::GetVertScrollBar()
{
	return (duScroll *)GetPluginByName(m_szVertScroll);
}

ListBoxItem *duListBox::GetItem(int nIndex)
{
	int nItemCount = GetItemCount();
	if (nIndex < 0 || nIndex >= nItemCount)
		return NULL;
	
	return m_vtItem[nIndex];
}

void duListBox::FadeRedraw()
{
	if (m_fFade)
	{
		m_fFadeIn = TRUE;
		m_nAlphaFadeIn = 0;
	
		Plugin_KillTimer(this, m_nTimerFadeIn);
		Plugin_SetTimer(this, m_nTimerFadeIn, 30);
	}
	else
	{
		m_nAlphaFadeIn = 255;
		Plugin_Redraw(this, TRUE);
	}
}

int duListBox::GetItemIndex(ListBoxItem *pItem)
{
	if (pItem == NULL)
		return -1;

	vector<ListBoxItem *>::iterator Iter = m_vtItem.begin();
	ListBoxItem *pTemp = NULL;
	int i = 0;
	for (; Iter != m_vtItem.end(); Iter++)
	{
		pTemp = *Iter;
		if (pItem == pTemp)
			return i;
		i++;
	}

	return -1;
}

int WINAPI duListBox::GetSel()
{
	if (!m_fCombobox)
	{
		if (m_pSelItem == NULL)
			return -1;

		return GetItemIndex(m_pSelItem);
	}
	else
	{
		if (m_pHotItem == NULL)
			return -1;

		return GetItemIndex(m_pHotItem);
	}
}

void WINAPI duListBox::SetSel(int nIndex)
{
	if (!m_fCombobox)
	{
		ListBoxItem *pItem = GetItem(nIndex);
		if (pItem == NULL)
			return;

		m_pSelItem = pItem;
	}
	else
	{
		ListBoxItem *pItem = GetItem(nIndex);
		if (pItem == NULL)
			return;

		m_pHotItem = pItem;
	}
}

void WINAPI duListBox::SetVertScrollBar(LPCTSTR lpszScroll)
{
	if (lpszScroll)
		_tcsncpy(m_szVertScroll, lpszScroll, MAX_NAME);
	else
		ZeroMemory(m_szVertScroll, sizeof(TCHAR) * MAX_NAME);
}

BOOL WINAPI duListBox::IsItemDisable(int nIndex)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return FALSE;
		
	return pItem->fDisable;
}

BOOL WINAPI duListBox::SetItemDisable(int nIndex, BOOL fDisable)
{
	ListBoxItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return FALSE;
	
	pItem->fDisable = fDisable;
	if (pItem == m_pHotItem)
		m_pHotItem = NULL;
	return TRUE;
}
