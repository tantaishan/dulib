//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duHeaderCtrl.cpp
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duHeaderCtrl.h"
#include "duDrawHelper.h"
#include "duListCtrl.h"

duHeaderCtrl::duHeaderCtrl() :
	 m_nIconX(0)
	,m_nIconY(0)
	,m_nIconWidth(0)
	,m_nIconHeight(0)
	,m_nSortX(0)
	,m_nSortY(0)
	,m_nSortWidth(0)
	,m_nSortHeight(0)
	,m_fFixed(FALSE)
	,m_nSepWidth(0)
	,m_nBorderWidth(1)
	,m_fShowCheckBox(FALSE)
	,m_nCheckBoxX(0)
	,m_nCheckBoxY(0)
	,m_nCheckBoxWidth(0)
	,m_nCheckBoxHeight(0)
{
	m_vtItems.clear();
	m_pHot = NULL;
	m_fTracking = FALSE;
	m_ptTrack.x = m_ptTrack.y = 0;
	m_nHotBorder = -1;
	m_nTrackWidth = 0;
	m_nOffsetX = 0;
	m_uCheckBoxState = HEADER_CHECKBOX_NORMAL;
	
	ZeroMemory(m_szItemStyle, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szSortStyle, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szSepImage, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szCheckBox, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szHorzScroll, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szListCtrl, MAX_NAME * sizeof(TCHAR));
}

duHeaderCtrl::~duHeaderCtrl()
{
	
}

void WINAPI duHeaderCtrl::RegisterControlProperty()
{
	RegisterProperty(_T("borderwidth"), DU_PROPERTY_LONG, &m_nBorderWidth);

	RegisterProperty(_T("itemstyle"), DU_PROPERTY_STYLEGROUP, m_szItemStyle);
	
	RegisterProperty(_T("itemiconx"), DU_PROPERTY_LONG, &m_nIconX);
	RegisterProperty(_T("itemicony"), DU_PROPERTY_LONG, &m_nIconY);
	RegisterProperty(_T("itemiconwidth"), DU_PROPERTY_LONG, &m_nIconWidth);
	RegisterProperty(_T("itemiconheight"), DU_PROPERTY_LONG, &m_nIconHeight);
	
	RegisterProperty(_T("sortstyle"), DU_PROPERTY_STYLEGROUP, m_szSortStyle);
	RegisterProperty(_T("sortx"), DU_PROPERTY_LONG, &m_nSortX);
	RegisterProperty(_T("sorty"), DU_PROPERTY_LONG, &m_nSortY);
	RegisterProperty(_T("sortwidth"), DU_PROPERTY_LONG, &m_nSortWidth);
	RegisterProperty(_T("sortheight"), DU_PROPERTY_LONG, &m_nSortHeight);
	
	//RegisterProperty(_T("fixed"), DU_PROPERTY_BOOL, &m_fFixed);
	RegisterProperty(_T("sepimage"), DU_PROPERTY_IMAGE, m_szSepImage);
	RegisterProperty(_T("sepwidth"), DU_PROPERTY_LONG, &m_nSepWidth);
	
	RegisterProperty(_T("showcheckbox"), DU_PROPERTY_BOOL, &m_fShowCheckBox);
	RegisterProperty(_T("checkboxx"), DU_PROPERTY_LONG, &m_nCheckBoxX);
	RegisterProperty(_T("checkboxy"), DU_PROPERTY_LONG, &m_nCheckBoxY);
	RegisterProperty(_T("checkboxwidth"), DU_PROPERTY_LONG, &m_nCheckBoxWidth);
	RegisterProperty(_T("checkboxheight"), DU_PROPERTY_LONG, &m_nCheckBoxHeight);
	RegisterProperty(_T("checkboximage"), DU_PROPERTY_IMAGE, m_szCheckBox);
	
	//RegisterProperty(_T("horzscroll"), DU_PROPERTY_STRING, m_szHorzScroll);

	RegisterProperty(_T("listctrl"), DU_PROPERTY_STRING, m_szListCtrl);
}

void WINAPI duHeaderCtrl::OnCreate()
{
	/*
	//640
	InsertItem(0, _T("Item00000"), FALSE, 200, _T("headericon"));
	InsertItem(1, _T("Item1"), FALSE, 200, _T("headericon2"));
	InsertItem(2, _T("Item2"), FALSE, 200, _T("headericon2"));
	InsertItem(3, _T("Item3"), FALSE, 200, _T("headericon"));

	CRect rc;
	Plugin_GetRect(this, &rc);
	_TRACE(_T("rc{%d, %d}\n"), rc.Width(), rc.Height());
	*/
}

void WINAPI duHeaderCtrl::DrawObject(HDC hDC)
{
	duRect rcHeader;
	Plugin_GetRect(this, &rcHeader);
	rcHeader.OffsetRect(-rcHeader.left, -rcHeader.top);

	duRect rcDraw(rcHeader);
	int nTotalWidth = GetTotalWidth();
	rcDraw.right = nTotalWidth;
	
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	BITMAPINFO bmInfo;
	RGBQUAD *pBits = NULL;
	
	InitBitmapInfo(&bmInfo, rcDraw.Width(), rcDraw.Height());
	HBITMAP hBmp = ::CreateDIBSection(hDC, &bmInfo, DIB_RGB_COLORS,(void**)&pBits, 0, 0);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBmp);
	
	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
	{
		pStyleGroup->Draw(hDC, &rcHeader, GetState(), GetText(), GetAlpha());
		pStyleGroup->Draw(hMemDC, &rcHeader, GetState(), GetText(), GetAlpha());
	}

	duRect rcItem(rcDraw);
	rcItem.left = rcItem.right = m_nBorderWidth;
	
	duStyleGroup *pStyleItem = (duStyleGroup *)GetResObj(m_szItemStyle, DU_RES_STYLEGROUP);
	if (pStyleItem)
	{
		duImage *pSepImage = (duImage *)GetResObj(m_szSepImage, DU_RES_IMAGE);
		duStyleGroup *pStyleSort = (duStyleGroup *)GetResObj(m_szSortStyle, DU_RES_STYLEGROUP);
		
		int i;
		int nItemCount = GetItemCount();
		for (i = 0;i < nItemCount; i++)
		{
			HeaderCtrlItem *pItem = m_vtItems[i];
			if (pItem == NULL)
				continue;
			
			//_TRACE(_T("i %d, nWidth %d\n"), i, pItem->nWidth);

			rcItem.right = rcItem.left + pItem->nWidth;
			//
			// draw item
			//
			pStyleItem->Draw(hMemDC, &rcItem, pItem->uState, pItem->strText.c_str(), GetAlpha());

			//
			// draw item icon
			//
			duImage *pIcon = (duImage *)GetResObj(pItem->strIcon.c_str(), DU_RES_IMAGE);
			if (pIcon)
			{
				duRect rcIcon(rcItem);
				rcIcon.left = rcItem.left + m_nIconX;
				rcIcon.right = rcIcon.left + m_nIconWidth;
				rcIcon.top = rcItem.top + m_nIconY;
				rcIcon.bottom = rcIcon.top + m_nIconHeight;

				duRect rcIconDraw;
				if (::IntersectRect(&rcIconDraw, &rcIcon, &rcItem))
				{
					DrawNormal(hMemDC, rcIconDraw.left, rcIconDraw.top, rcIconDraw.Width(), rcIconDraw.Height(), 
						pIcon, 0, 0, GetAlpha());
				}
			}

			//
			// draw item sort
			//
			if (pStyleSort && pItem->fShowSort)
			{
				duRect rcSort(rcItem);
				if (m_nSortX < 0)
					rcSort.left = rcItem.right + m_nSortX;
				else
					rcSort.left = rcItem.left + m_nSortX;
					
				rcSort.right = rcSort.left + m_nSortWidth;
				rcSort.top = rcItem.top + m_nSortY;
				rcSort.bottom = rcSort.top + m_nSortHeight;
				
				duRect rcSortDraw;
				if (::IntersectRect(&rcSortDraw, &rcSort, &rcItem))
					pStyleSort->Draw(hMemDC, &rcSortDraw, pItem->fSortUp ? DU_STATE_NORMAL : DU_STATE_CHECKED, NULL, GetAlpha());
			}

			//
			// draw checkbox
			//
			if (m_fShowCheckBox && i == 0)
			{
				duRect rcCheckBox;
				rcCheckBox.left = rcItem.left + m_nCheckBoxX;
				rcCheckBox.top = rcItem.top + m_nCheckBoxY;
				rcCheckBox.right = rcCheckBox.left + m_nCheckBoxWidth;
				rcCheckBox.bottom = rcCheckBox.top + m_nCheckBoxHeight;

				duImage *pImageCheckBox = (duImage *)GetResObj(m_szCheckBox, DU_RES_IMAGE);
				if (pImageCheckBox)
				{
					duRect rcCheckBoxDraw;
					if (::IntersectRect(&rcCheckBoxDraw, &rcCheckBox, &rcItem))
					{
						int nCheckBoxImageLeft = 0;
						if (m_uCheckBoxState == HEADER_CHECKBOX_CHECKED)
							nCheckBoxImageLeft = m_nCheckBoxWidth;
						else if (m_uCheckBoxState == HEADER_CHECKBOX_CHECKEDPART)
							nCheckBoxImageLeft = m_nCheckBoxWidth + m_nCheckBoxWidth;
						
						DrawNormal(hMemDC, rcCheckBoxDraw.left, rcCheckBoxDraw.top, rcCheckBoxDraw.Width(), rcCheckBoxDraw.Height(), 
							pImageCheckBox, nCheckBoxImageLeft, 0, GetAlpha());
					}
				}
			}

			if (pSepImage)
				DrawNormal(hMemDC, rcItem.right, rcItem.top, m_nSepWidth, rcItem.Height(), pSepImage, 0, 0, GetAlpha());
			
			rcItem.left = rcItem.right;
			rcItem.left += m_nSepWidth;
		}
	}
	
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	::AlphaBlend(hDC, 0, 0, rcDraw.Width() - m_nOffsetX, rcDraw.Height(), hMemDC, m_nOffsetX, 0, rcDraw.Width() - m_nOffsetX, rcDraw.Height(), bf);
	
	::SelectObject(hMemDC, hOldBitmap);
	::DeleteObject(hBmp);
	::DeleteDC(hMemDC);
}

void WINAPI duHeaderCtrl::OnMouseMove(POINT point)
{
	duPoint pt(point);
	if (m_fTracking)
	{
		duListCtrl *pListCtrl = (duListCtrl *)GetPluginByName(m_szListCtrl);
		if (pListCtrl == NULL)
			return;

		HeaderCtrlItem *pItem = GetItem(m_nHotBorder);
		if (pItem)
		{
			int nCurWidth = m_nTrackWidth + (pt.x - m_ptTrack.x);
			pItem->nWidth = max(nCurWidth, 0);
			
			int nTotalWidth = GetTotalWidth();
			duRect rcHeader;
			Plugin_GetRect(this, &rcHeader);

			//_TRACE(_T("TotalWidth :%d, rcHeader.Width : %d\n"), nTotalWidth, rcHeader.Width());
			if (nTotalWidth <= (rcHeader.Width() + m_nOffsetX))
			{
				if (m_nOffsetX != 0)
				{
					//_TRACE(_T("nWidth : %d\n"), pItem->nWidth);

					nCurWidth -= m_nOffsetX;
					pItem->nWidth = max(nCurWidth, 0);
					m_nTrackWidth = max(m_nTrackWidth - m_nOffsetX, 0);
					m_nOffsetX = 0;
					
					pListCtrl->HorzScroll(0);
				}
			}

			pListCtrl->AdjustColumnWidth(m_nHotBorder, pItem->nWidth);

			Plugin_Redraw(this, TRUE);
		}
		return;
	}

	duRect rcHeader;
	Plugin_GetRect(this, &rcHeader);
	pt.Offset(-rcHeader.left, -rcHeader.top);
	pt.Offset(m_nOffsetX, 0);
	
	rcHeader.OffsetRect(-rcHeader.left, -rcHeader.top);
	
	duRect rcDraw(rcHeader);	
	int nTotalWidth = GetTotalWidth();
	if (nTotalWidth > rcHeader.Width())
		rcDraw.right = nTotalWidth;
		
	duRect rcItem(rcDraw);
	rcItem.left = rcItem.right = m_nBorderWidth;
	
	int i;
	int nItemCount = GetItemCount();
	for (i = 0;i < nItemCount; i++)
	{
		HeaderCtrlItem *pItem = m_vtItems[i];
		if (pItem == NULL)
			continue;
		
		rcItem.right = rcItem.left + pItem->nWidth;
		if (rcItem.PtInRect(pt))
		{
			if (m_pHot != pItem)
			{
				if (m_pHot)
					m_pHot->uState = DU_STATE_NORMAL;
				
				pItem->uState = DU_STATE_OVER;
				m_pHot = pItem;
				Plugin_Redraw(this, TRUE);
			}

			return;
		}
		
		duRect rcSep(rcItem);
		rcSep.left = rcItem.right;
		rcSep.right = rcSep.left + m_nSepWidth;
		if (rcSep.PtInRect(pt))
		{
			if (m_pHot)
			{
				m_pHot->uState = DU_STATE_NORMAL;
				m_pHot = NULL;
				Plugin_Redraw(this, TRUE);
			}
			
			m_nHotBorder = i;
			return;
		}
		
		rcItem.left = rcItem.right;
		rcItem.left += m_nSepWidth;
	}
}

void WINAPI duHeaderCtrl::OnMouseLeave(POINT pt)
{
	if (m_pHot)
	{
		m_pHot->uState = DU_STATE_NORMAL;
		m_pHot = NULL;
		
		Plugin_Redraw(this, TRUE);
	}
}

BOOL WINAPI duHeaderCtrl::OnSetCursor()
{
	duPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd, &pt);
	
	duRect rcHeader;
	Plugin_GetRect(this, &rcHeader);
	pt.Offset(-rcHeader.left, -rcHeader.top);
	pt.Offset(m_nOffsetX, 0);
	
	rcHeader.OffsetRect(-rcHeader.left, -rcHeader.top);

	duRect rcDraw(rcHeader);	
	int nTotalWidth = GetTotalWidth();
	if (nTotalWidth > rcHeader.Width())
		rcDraw.right = nTotalWidth;

	duRect rcItem(rcDraw);
	rcItem.left = rcItem.right = m_nBorderWidth;
	
	int i;
	int nItemCount = GetItemCount();
	for (i = 0;i < nItemCount; i++)
	{
		HeaderCtrlItem *pItem = m_vtItems[i];
		if (pItem == NULL)
			continue;
		
		rcItem.right = rcItem.left + pItem->nWidth;
		if (rcItem.PtInRect(pt))
			return duPlugin::OnSetCursor();
		
		duRect rcSep(rcItem);
		rcSep.left = rcItem.right;
		rcSep.right = rcSep.left + m_nSepWidth;
		if (rcSep.PtInRect(pt))
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
			return TRUE;
		}
		
		rcItem.left = rcItem.right;
		rcItem.left += m_nSepWidth;
	}

	return duPlugin::OnSetCursor();
}

void WINAPI duHeaderCtrl::OnMouseLDown(POINT point)
{
	duPoint pt(point);
	if (m_pHot)
	{
		if (m_vtItems[0] == m_pHot)
		{
			duRect rcHeader;
			Plugin_GetRect(this, &rcHeader);

			pt.Offset(-rcHeader.left, -rcHeader.top);
			pt.Offset(m_nOffsetX, 0);
			rcHeader.OffsetRect(-rcHeader.left, -rcHeader.top);
			
			duRect rcCheckBox;
			rcCheckBox.left = rcHeader.left + m_nBorderWidth + m_nCheckBoxX;
			rcCheckBox.right = rcCheckBox.left + m_nCheckBoxWidth;
			rcCheckBox.top = rcHeader.top + m_nCheckBoxY;
			rcCheckBox.bottom = rcCheckBox.top + m_nCheckBoxHeight;

			if (rcCheckBox.PtInRect(pt))
			{
				if (m_uCheckBoxState == HEADER_CHECKBOX_NORMAL)
					m_uCheckBoxState = HEADER_CHECKBOX_CHECKED;
				else
					m_uCheckBoxState = HEADER_CHECKBOX_NORMAL;

				//
				int nIndex = GetItemIndex(m_pHot);
				if (nIndex != -1)
					NotifyUser(DUM_HEADERCKCLICK, m_uCheckBoxState, 0);
			}
			else
				m_pHot->uState = DU_STATE_PRESS;
		}
		else
			m_pHot->uState = DU_STATE_PRESS;
		
		Plugin_Redraw(this, TRUE);
	}
	else if (m_nHotBorder != -1)
	{
		HeaderCtrlItem *pItem = GetItem(m_nHotBorder);
		if (pItem)
		{
			m_fTracking = TRUE;
			m_ptTrack = pt;
			m_nTrackWidth = pItem->nWidth;
		
			Plugin_SetCapture(this);
		}
	}
}

void WINAPI duHeaderCtrl::OnMouseLUp(POINT pt)
{
	Plugin_ReleaseCapture(this);
	m_fTracking = FALSE;
	
	if (m_pHot)
	{
		if (m_pHot->uState == DU_STATE_PRESS)
		{
			if (m_pHot->fShowSort)
				m_pHot->fSortUp = !m_pHot->fSortUp;
			else
				m_pHot->fShowSort = TRUE;

			int nIndex = GetItemIndex(m_pHot);
			if (nIndex != -1)
				NotifyUser(DUM_HEADERSORT, nIndex, m_pHot->fSortUp);
			
			m_pHot->uState = DU_STATE_NORMAL;
			Plugin_Redraw(this, TRUE);
		}
	}
}

BOOL WINAPI duHeaderCtrl::InsertItem(int nIndex, LPCTSTR lpszText, BOOL bPercent, int nWidthOrPercent, LPCTSTR lpszIcon)
{
	int nItemCount = GetItemCount();
	if (nIndex == -1)
		nIndex = nItemCount;

	if (nIndex < 0 || nIndex > nItemCount)
		return FALSE;
	
	HeaderCtrlItem *pHeaderItem = new HeaderCtrlItem;
	
	pHeaderItem->bPercent = bPercent;
	if (bPercent)
	{
		pHeaderItem->nPercent = nWidthOrPercent;
		pHeaderItem->nWidth = 0;
	}
	else
	{
		pHeaderItem->nPercent = 0;
		pHeaderItem->nWidth = nWidthOrPercent;
	}
	
	pHeaderItem->fShowSort = FALSE;
	pHeaderItem->fSortUp = FALSE;
	if (lpszText)
		pHeaderItem->strText = lpszText;
	else
		pHeaderItem->strText = _T("");

	if (lpszIcon)
		pHeaderItem->strIcon = lpszIcon;
	else
		pHeaderItem->strIcon = _T("");
		
	pHeaderItem->uState = DU_STATE_NORMAL;

	m_vtItems.push_back(pHeaderItem);
	return TRUE;
}

int WINAPI duHeaderCtrl::GetItemCount()
{
	return (int)m_vtItems.size();
}

int WINAPI duHeaderCtrl::GetTotalWidth()
{
	int nTotalWidth = 0;
	int i;
	int nItemCount = GetItemCount();
	for (i = 0;i < nItemCount; i++)
	{
		HeaderCtrlItem *pItem = m_vtItems[i];
		if (pItem)
			nTotalWidth += pItem->nWidth;
	}

	if (nItemCount)
		nTotalWidth += (m_nBorderWidth * 2 + nItemCount * m_nSepWidth);
	
	return nTotalWidth;
}

void WINAPI duHeaderCtrl::SetOffsetX(int nOffsetX)
{
	m_nOffsetX = nOffsetX;
	//CRect rc;
	//GetRect(&rc);
	//int nMax = GetTotalWidth() - rc.Width();
	//if (m_nOffsetX > nMax)
	//	m_nOffsetX = nMax;
}

HeaderCtrlItem *duHeaderCtrl::GetItem(int nItem)
{
	int nItemCount = GetItemCount();
	if (nItem < 0 || nItem >= nItemCount)
		return NULL;
		
	return m_vtItems[nItem];
}

int duHeaderCtrl::GetItemIndex(HeaderCtrlItem *pHeaderItem)
{
	if (pHeaderItem == NULL)
		return -1;

	int i;
	int nItemCount = GetItemCount();
	for (i = 0;i < nItemCount; i++)
	{
		HeaderCtrlItem *pItem = m_vtItems[i];
		if (pItem == pHeaderItem)
			return i;
	}

	return -1;
}

int WINAPI duHeaderCtrl::GetItemWidth(int nIndex)
{
	HeaderCtrlItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return 0;

	return pItem->nWidth;
}

void WINAPI duHeaderCtrl::SetItemWidth(int nIndex, int nWidth)
{
	HeaderCtrlItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return;

	pItem->nWidth = nWidth;

	duListCtrl *pListCtrl = (duListCtrl *)GetPluginByName(m_szListCtrl);
	if (pListCtrl)
		pListCtrl->AdjustColumnWidth(nIndex, nWidth);
}

LPCTSTR WINAPI duHeaderCtrl::GetItemText(int nIndex)
{
	HeaderCtrlItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return NULL;

	return pItem->strText.c_str();
}

void WINAPI duHeaderCtrl::SetItemText(int nIndex, LPCTSTR lpszText)
{
	HeaderCtrlItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return;

	if (lpszText)
		pItem->strText = lpszText;
	else
		pItem->strText = _T("");
}

LPCTSTR WINAPI duHeaderCtrl::GetItemIcon(int nIndex)
{
	HeaderCtrlItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return NULL;

	return pItem->strIcon.c_str();
}

void WINAPI duHeaderCtrl::SetItemIcon(int nIndex, LPCTSTR lpszIcon)
{
	HeaderCtrlItem *pItem = GetItem(nIndex);
	if (pItem == NULL)
		return;

	if (lpszIcon)
		pItem->strIcon = lpszIcon;
	else
		pItem->strIcon = _T("");
}

void WINAPI duHeaderCtrl::SetCheckBoxImage(LPCTSTR lpszImage)
{
	if (lpszImage)
		_tcscpy(m_szCheckBox, lpszImage);
	else
		ZeroMemory(m_szCheckBox, MAX_NAME * sizeof(TCHAR));
}

void WINAPI duHeaderCtrl::SetListCtrl(LPCTSTR lpszCtrl)
{
	if (lpszCtrl)
		_tcscpy(m_szListCtrl, lpszCtrl);
	else
		ZeroMemory(m_szListCtrl, MAX_NAME * sizeof(TCHAR));
}

void WINAPI duHeaderCtrl::SetItemStyle(LPCTSTR lpszStyle)
{
	if (lpszStyle)
		_tcscpy(m_szItemStyle, lpszStyle);
	else
		ZeroMemory(m_szItemStyle, MAX_NAME * sizeof(TCHAR));
}

void WINAPI duHeaderCtrl::SetSortStyle(LPCTSTR lpszStyle)
{
	if (lpszStyle)
		_tcscpy(m_szSortStyle, lpszStyle);
	else
		ZeroMemory(m_szSortStyle, MAX_NAME * sizeof(TCHAR));
}

void WINAPI duHeaderCtrl::SetSepImage(LPCTSTR lpszImage)
{
	if (lpszImage)
		_tcscpy(m_szSepImage, lpszImage);
	else
		ZeroMemory(m_szSepImage, MAX_NAME * sizeof(TCHAR));
}

