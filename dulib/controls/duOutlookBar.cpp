//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duOutlookBar.cpp
//
//  History:    Jun-20-11   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "duOutlookBar.h"

duOutlookBar::duOutlookBar() :
	 m_nItemHeight(0)
{
	ZeroMemory(m_szItemStyle, sizeof(TCHAR) * MAX_NAME);
	m_vtItems.clear();
}

duOutlookBar::~duOutlookBar()
{
	int i;
	for (i = 0; i < (int)m_vtItems.size(); i++)
	{
		OutlookBarItem *pOutlookBarItem = m_vtItems[i];
		if (pOutlookBarItem)
		{
			delete pOutlookBarItem;
		}
	}
	m_vtItems.clear();
}

void duOutlookBar::RegisterControlProperty()
{
	RegisterProperty(_T("itemstyle"), DU_PROPERTY_STYLEGROUP, m_szItemStyle);
	RegisterProperty(_T("itemheight"), DU_PROPERTY_LONG, &m_nItemHeight);
}

void duOutlookBar::OnCreate()
{
}

void duOutlookBar::DrawObject(HDC hDC)
{
	duRect rect;
	GetRect(&rect);
	rect.OffsetRect(-rect.left, -rect.top);

	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
		pStyleGroup->Draw(hDC, &rect, DU_STATE_ALL, GetText(), GetAlpha());

	duStyleGroup *pItemStyleGroup = (duStyleGroup *)GetResObj(m_szItemStyle, DU_RES_STYLEGROUP);
	if (pItemStyleGroup == NULL)
		return ;

	int i;
	OutlookBarItem *pOutlookBarItem = NULL;
	int nItemCount = GetItemCount();
	duRect rectItem = rect;
	rectItem.bottom = rectItem.top + m_nItemHeight;

	for (i = 0;i < nItemCount; i++)
	{
		pOutlookBarItem = m_vtItems[i];
		pItemStyleGroup->Draw(hDC, &rectItem, pOutlookBarItem->iState, pOutlookBarItem->strText.c_str(), GetAlpha());

		if (pOutlookBarItem->fSelected)
		{
			rectItem.top = rect.bottom - (nItemCount - i - 1) * m_nItemHeight;
			rectItem.bottom = rectItem.top + m_nItemHeight;
		}
		else
			rectItem.OffsetRect(0, m_nItemHeight);
	}
}

void duOutlookBar::OnMouseLeave(POINT pt)
{
	int i;
	OutlookBarItem *pOutlookBarItem = NULL;
	BOOL fRedraw = FALSE;
	int nItemCount = GetItemCount();

	for (i = 0;i < nItemCount; i++)
	{
		pOutlookBarItem = m_vtItems[i];

		if (pOutlookBarItem->iState != DU_STATE_NORMAL)
		{
			pOutlookBarItem->iState = DU_STATE_NORMAL;
			fRedraw = TRUE;
		}
	}

	if (fRedraw)
		Plugin_Redraw(this, TRUE);
}

void duOutlookBar::OnMouseLDown(POINT pt)
{
	duRect rect;
	GetRect(&rect);

	int i;
	OutlookBarItem *pOutlookBarItem = NULL;
	int nItemCount = GetItemCount();
	duRect rectItem = rect;
	rectItem.bottom = rectItem.top + m_nItemHeight;
	BOOL fRedraw = FALSE;

	for (i = 0;i < nItemCount; i++)
	{
		pOutlookBarItem = m_vtItems[i];
		if (rectItem.PtInRect(pt))
		{
			if (pOutlookBarItem->iState == DU_STATE_OVER)
			{
				pOutlookBarItem->iState = DU_STATE_PRESS;
				fRedraw = TRUE;
			}
		}

		if (pOutlookBarItem->fSelected)
		{
			rectItem.top = rect.bottom - (nItemCount - i - 1) * m_nItemHeight;
			rectItem.bottom = rectItem.top + m_nItemHeight;
		}
		else
		{
			rectItem.OffsetRect(0, m_nItemHeight);
		}
	}

	if (fRedraw)
		Plugin_Redraw(this, TRUE);

}

void duOutlookBar::OnMouseLUp(POINT pt)
{
	duRect rect;
	GetRect(&rect);

	int i;
	OutlookBarItem *pOutlookBarItem = NULL;
	int nItemCount = GetItemCount();
	duRect rectItem = rect;
	rectItem.bottom = rectItem.top + m_nItemHeight;
	BOOL fRedraw = FALSE;

	for (i = 0;i < nItemCount; i++)
	{
		pOutlookBarItem = m_vtItems[i];
		if (rectItem.PtInRect(pt))
		{
			if (pOutlookBarItem->iState == DU_STATE_PRESS)
			{
				pOutlookBarItem->iState = DU_STATE_NORMAL;
				fRedraw = TRUE;

				if (!pOutlookBarItem->fSelected)
					SetSelectedItem(i);
			}
		}

		if (pOutlookBarItem->fSelected)
		{
			rectItem.top = rect.bottom - (nItemCount - i - 1) * m_nItemHeight;
			rectItem.bottom = rectItem.top + m_nItemHeight;
		}
		else
		{
			rectItem.OffsetRect(0, m_nItemHeight);
		}
	}

	if (fRedraw)
		Plugin_Redraw(this, TRUE);

}

void duOutlookBar::OnMouseMove(POINT pt)
{
	duRect rect;
	GetRect(&rect);

	int i;
	OutlookBarItem *pOutlookBarItem = NULL;
	int nItemCount = GetItemCount();
	duRect rectItem = rect;
	rectItem.bottom = rectItem.top + m_nItemHeight;
	BOOL fRedraw = FALSE;

	for (i = 0;i < nItemCount; i++)
	{
		pOutlookBarItem = m_vtItems[i];
		if (rectItem.PtInRect(pt))
		{
			if (pOutlookBarItem->iState == DU_STATE_NORMAL)
			{
				pOutlookBarItem->iState = DU_STATE_OVER;
				fRedraw = TRUE;
			}
		}
		else
		{
			if (pOutlookBarItem->iState != DU_STATE_NORMAL)
			{
				pOutlookBarItem->iState = DU_STATE_NORMAL;
				fRedraw = TRUE;
			}
		}

		if (pOutlookBarItem->fSelected)
		{
			rectItem.top = rect.bottom - (nItemCount - i - 1) * m_nItemHeight;
			rectItem.bottom = rectItem.top + m_nItemHeight;
		}
		else
		{
			rectItem.OffsetRect(0, m_nItemHeight);
		}
	}

	if (fRedraw)
		Plugin_Redraw(this, TRUE);

}

int duOutlookBar::GetItemCount()
{
	return (int)m_vtItems.size();
}

BOOL duOutlookBar::InsertItem(int nIndex, LPCTSTR lpszText, duPlugin *pChild)
{
	int nItemCount = GetItemCount();
	if (nIndex < 0 || nIndex > nItemCount)
		return FALSE;

	OutlookBarItem *pOutlookBarItem = new OutlookBarItem();
	pOutlookBarItem->strText = lpszText;
	pOutlookBarItem->iState = DU_STATE_NORMAL;
	pOutlookBarItem->pChild = pChild;
	pOutlookBarItem->fSelected = FALSE;

	vector<OutlookBarItem *>::iterator iter = m_vtItems.begin();
	iter += nIndex;
	m_vtItems.insert(iter, pOutlookBarItem);
	return TRUE;
}

LPCTSTR duOutlookBar::GetItemText(int nIndex)
{
	if (nIndex < 0 || nIndex >= GetItemCount())
		return NULL;

	OutlookBarItem *pOutlookBarItem = m_vtItems[nIndex];
	if (pOutlookBarItem)
		return pOutlookBarItem->strText.c_str();

	return NULL;
}

void duOutlookBar::SetItemText(int nIndex, LPCTSTR lpszText)
{
	if (nIndex < 0 || nIndex >= GetItemCount())
		return;

	OutlookBarItem *pOutlookBarItem = m_vtItems[nIndex];
	if (pOutlookBarItem)
		pOutlookBarItem->strText = lpszText;
}

duPlugin *duOutlookBar::GetItem(int nIndex)
{
	if (nIndex < 0 || nIndex >= GetItemCount())
		return NULL;

	OutlookBarItem *pOutlookBarItem = m_vtItems[nIndex];
	if (pOutlookBarItem)
		return pOutlookBarItem->pChild;

	return NULL;
}

void duOutlookBar::SetItem(int nIndex, duPlugin *pChild)
{
	if (nIndex < 0 || nIndex >= GetItemCount())
		return;

	OutlookBarItem *pOutlookBarItem = m_vtItems[nIndex];
	if (pOutlookBarItem)
		pOutlookBarItem->pChild = pChild;
}

BOOL duOutlookBar::DeleteItem(int nIndex)
{
	if (nIndex < 0 || nIndex >= GetItemCount())
		return FALSE;

	vector<OutlookBarItem *>::iterator iter = m_vtItems.begin() + nIndex;
	if (iter != m_vtItems.end())
	{
		OutlookBarItem *pOutlookBarItem = *iter;

		if (pOutlookBarItem)
		{
			if (pOutlookBarItem->pChild)
				pOutlookBarItem->pChild->SetVisible(FALSE);
			delete pOutlookBarItem;
		}
		m_vtItems.erase(iter);
		return TRUE;
	}

	return FALSE;
}

int duOutlookBar::GetSelectedItem()
{
	int i;
	for (i = 0; i < (int)m_vtItems.size(); i++)
	{
		OutlookBarItem *pOutlookBarItem = m_vtItems[i];
		if (pOutlookBarItem->fSelected)
			return i;
	}
	return -1;
}

void duOutlookBar::SetSelectedItem(int nIndex)
{
	duRect rect;
	GetRect(&rect);

	int i;
	for (i = 0; i < (int)m_vtItems.size(); i++)
	{	
		OutlookBarItem *pOutlookBarItem = m_vtItems[i];
		pOutlookBarItem->fSelected = FALSE;
		if (pOutlookBarItem->pChild)
			pOutlookBarItem->pChild->SetVisible(FALSE);
		
		if (i == nIndex)
		{
			pOutlookBarItem->fSelected = TRUE;
			if (pOutlookBarItem->pChild)
			{
				duPlugin *pChild = pOutlookBarItem->pChild;
				pChild->SetPosTop((i + 1) * m_nItemHeight);
				pChild->SetPosHeight(rect.Height() - GetItemCount() * m_nItemHeight);
				pChild->Resize(&rect);
				pChild->SetVisible(TRUE);
			}
		}
	}
}

