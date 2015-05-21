//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duReportHeader.cpp
//
//  History:    Mar-08-2011   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duReportHeader.h"
#include "duReportView.h"

duReportHeader::duReportHeader() :
	 m_nRef(0)
	,m_pReportView(NULL)
	,m_nHot(-1)
	,m_fFixed(FALSE)
	,m_ptTrack(0,0)
	,m_fTracking(FALSE)
	,m_nHotBorder(-1)
	,m_nTrackWidth(0)
{
	ZeroMemory(m_szItemStyle, MAX_NAME * sizeof(TCHAR));
	m_vtHeaderItem.clear();


}

duReportHeader::~duReportHeader()
{
	Destroy();
}

void duReportHeader::Destroy()
{
	int nItemCount = GetItemCount();
	int i;
	ReportHeaderItem *pItem = NULL;
	for (i = 0;i < nItemCount; i++)
	{
		pItem = m_vtHeaderItem[i];
		delete pItem;
	}
	m_vtHeaderItem.clear();
}

ReportHeaderItem *duReportHeader::GetItem(int nIndex)
{
	int nItemCount = GetItemCount();
	if (nIndex < 0 || nIndex >= nItemCount)
		return NULL;
	
	return m_vtHeaderItem[nIndex];
}

void WINAPI duReportHeader::DrawHeader(HDC hDC)
{
	duStyleGroup *pStyleGroup = (duStyleGroup *)m_pReportView->GetResObj(m_szItemStyle, DU_RES_STYLEGROUP);
	if (pStyleGroup == NULL)
		return;

	duRect rcHeader;
	m_pReportView->GetHeaderRect(&rcHeader);
	
	int nItemCount = GetItemCount();
	int i;
	ReportHeaderItem *pItem = NULL;
	duRect rcItem = rcHeader;
	rcItem.right = rcItem.left;
	
	for (i = 0;i < nItemCount; i++)
	{
		pItem = m_vtHeaderItem[i];
		if (i == nItemCount - 1)
		{
			rcItem.left = rcItem.right;
			rcItem.right = rcHeader.right;
		}
		else
		{
			rcItem.left = rcItem.right;
			rcItem.right = rcItem.left + pItem->nWidth;
		}
		
		pStyleGroup->Draw(hDC, &rcItem, pItem->uState, pItem->strText.c_str(), m_pReportView->GetAlpha());
	}
}

int WINAPI duReportHeader::GetItemCount()
{
	return (int)m_vtHeaderItem.size();
}

int WINAPI duReportHeader::GetTotalWidth()
{
	duRect rcClient = m_pReportView->_GetClientRect();
	
	int nTotalWidth = 0;
	int nItemCount = GetItemCount();
	int i;
	ReportHeaderItem *pItem = NULL;
	for (i = 0;i < nItemCount; i++)
	{
		pItem = m_vtHeaderItem[i];
		nTotalWidth += pItem->nWidth;
	}
	
	if (nTotalWidth < rcClient.Width())
		nTotalWidth = rcClient.Width();
		
	return nTotalWidth;
}

void WINAPI duReportHeader::OnMouseLDown(POINT pt)
{
	if (m_nHotBorder != -1 && !m_fFixed)
	{
		ReportHeaderItem *pItem = GetItem(m_nHotBorder);
		if (pItem == NULL)
			return;
			
		m_nTrackWidth = pItem->nWidth;
		m_fTracking = TRUE;
		m_ptTrack = pt;
		Plugin_SetCapture(m_pReportView);
	}
}

void WINAPI duReportHeader::OnMouseLUp(POINT pt)
{
	if (m_fTracking)
	{
		m_nTrackWidth = 0;
		m_fTracking = FALSE;
		m_ptTrack.x = m_ptTrack.y = 0;
		Plugin_ReleaseCapture(m_pReportView);
	}
}

void WINAPI duReportHeader::OnMouseRDown(POINT pt)
{
}

void WINAPI duReportHeader::OnMouseRUp(POINT pt)
{
}

void WINAPI duReportHeader::OnMouseHover(POINT pt)
{
}

void WINAPI duReportHeader::OnMouseMove(POINT pt)
{
	if (m_fTracking)
	{
		ReportHeaderItem *pItem = GetItem(m_nHotBorder);
		if (pItem == NULL)
			return;
		
		int nNewWidth = m_nTrackWidth + (pt.x - m_ptTrack.x);
		nNewWidth = max(pItem->nMinWidth, min(nNewWidth, pItem->nMaxWidth));
		if (pItem->nWidth != nNewWidth)
		{
			pItem->nWidth = nNewWidth;
			m_pReportView->UpdateScroll();
			Plugin_Redraw(m_pReportView, TRUE);
		}
		return;
	}
	
	duRect rcHeader;
	m_pReportView->GetHeaderRect(&rcHeader);
	
	int nItemCount = GetItemCount();
	int i;
	ReportHeaderItem *pItem = NULL;
	duRect rcItem = rcHeader;
	rcItem.right = rcItem.left;
	
	for (i = 0;i < nItemCount; i++)
	{
		pItem = m_vtHeaderItem[i];
		if (i == nItemCount - 1)
		{
			rcItem.left = rcItem.right;
			rcItem.right = rcHeader.right;
		}
		else
		{
			rcItem.left = rcItem.right;
			rcItem.right = rcItem.left + pItem->nWidth;
		}
		
		if (rcItem.PtInRect(pt) && m_nHot != i)
		{
			ReportHeaderItem *pOldItem = GetItem(m_nHot);
			if (pOldItem)
				pOldItem->uState = DU_STATE_NORMAL;
			pItem->uState = DU_STATE_OVER;
			m_nHot = i;
			Plugin_Redraw(m_pReportView, TRUE);
			break;
		}
	}
	
	
	int nDis = rcHeader.left;
	for (i = 0;i < nItemCount; i++)
	{
		if (i == nItemCount - 1)
			break;

		pItem = m_vtHeaderItem[i];
		nDis += pItem->nWidth;
		if (pt.x > nDis - HEADER_BORDER_WIDTH && pt.x < nDis + HEADER_BORDER_WIDTH)
		{
			m_nHotBorder = i;
			return;
		}
	}
	
	m_nHotBorder = -1;
}

void WINAPI duReportHeader::OnMouseDbClick(POINT pt)
{
}

void WINAPI duReportHeader::OnMouseLeave()
{
	if (m_fTracking)
		return;

	if (m_nHot != -1)
	{
		ReportHeaderItem *pOldItem = GetItem(m_nHot);
		if (pOldItem)
		{
			pOldItem->uState = DU_STATE_NORMAL;
			Plugin_Redraw(m_pReportView, TRUE);
		}
		m_nHot = -1;
	}
}

BOOL WINAPI duReportHeader::OnSetCursor(POINT pt)
{
	duRect rcHeader;
	m_pReportView->GetHeaderRect(&rcHeader);
	
	if (!rcHeader.PtInRect(pt))
		return FALSE;
	
	int nItemCount = GetItemCount();
	int i;
	ReportHeaderItem *pItem = NULL;
	int nDis = rcHeader.left;
	
	for (i = 0;i < nItemCount; i++)
	{
		if (i == nItemCount - 1)
			break;

		pItem = m_vtHeaderItem[i];
		nDis += pItem->nWidth;
		if (pt.x > nDis - HEADER_BORDER_WIDTH && pt.x < nDis + HEADER_BORDER_WIDTH)
		{
			::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL WINAPI duReportHeader::InsertItem(int nCol, int nWidth, LPCTSTR lpszText)
{
	int nItemCount = GetItemCount();
	if (nCol == -1)
		nCol = nItemCount;

	if (nCol < 0 || nCol > nItemCount)
		return -1;
	
	vector<ReportHeaderItem *>::iterator iterItem = m_vtHeaderItem.begin() + nCol;
	ReportHeaderItem *pItem = new ReportHeaderItem;
	if (lpszText)
		pItem->strText = lpszText;
	else
		pItem->strText = _T("");
	
	if (nWidth <= 0)
		pItem->nWidth = HEADER_BORDER_WIDTH * 2;
	else
		pItem->nWidth = nWidth;
	
	pItem->uState = DU_STATE_NORMAL;
	pItem->nMinWidth = HEADER_BORDER_WIDTH * 2;
	pItem->nMaxWidth = HEADER_ITEM_MAXWIDTH;
	
	m_vtHeaderItem.insert(iterItem, pItem);
	return nCol;
}

int WINAPI duReportHeader::GetColumnWidth(int nCol)
{
	ReportHeaderItem *pItem = GetItem(nCol);
	if (pItem == NULL)
		return 0;

	return pItem->nWidth;
}

BOOL WINAPI duReportHeader::DeleteColumn(int nCol)
{
	int nItemCount = GetItemCount();
	if (nCol < 0 || nCol >= nItemCount)
		return FALSE;
	
	vector<ReportHeaderItem *>::iterator iter = m_vtHeaderItem.begin() + nCol;
	ReportHeaderItem *pItem = *iter;
	if (pItem)
		delete pItem;
	m_vtHeaderItem.erase(iter);
	return TRUE;
}
