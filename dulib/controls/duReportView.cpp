//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duReportView.cpp
//
//  History:    Mar-08-2011   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duReportView.h"

duReportView::duReportView() :
	 m_nHotLine(-1)
	,m_nHeaderHeight(0)
	,m_nLineCount(0)
	,m_fGridLine(FALSE)
	,m_fMultiSel(FALSE)
{
	m_vtColumn.clear();
	m_vtLine.clear();
	m_pHeaderCtrl = new duReportHeader;
	m_pHeaderCtrl->SetReportView(this);
	ZeroMemory(m_szLineStyle, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szCellStyle, MAX_NAME * sizeof(TCHAR));
}

duReportView::~duReportView()
{
	int nColCount = (int)m_vtColumn.size();
	int i;
	for (i = 0;i < nColCount; i++)
	{
		duReportColumn *pColumn = m_vtColumn[i];
		delete pColumn;
	}
	m_vtColumn.clear();
	
	int nLineCount = (int)m_vtLine.size();
	for (i = 0;i < nLineCount; i++)
	{
		duReportLine *pLine = m_vtLine[i];
		delete pLine;
	}
	m_vtLine.clear();
	
	if (m_pHeaderCtrl)
	{
		delete m_pHeaderCtrl;
		m_pHeaderCtrl = NULL;
	}
}

void WINAPI duReportView::RegisterControlProperty()
{
	RegisterProperty(_T("headerheight"),  DU_PROPERTY_LONG, &m_nHeaderHeight);
	RegisterProperty(_T("headerstyle"),  DU_PROPERTY_STRING, m_pHeaderCtrl->GetStyle());
	//RegisterProperty(_T("gridline"),  DU_PROPERTY_BOOL, &m_fGridLine);
	RegisterProperty(_T("linestyle"),  DU_PROPERTY_STRING, m_szLineStyle);
	RegisterProperty(_T("multiselect"),  DU_PROPERTY_BOOL, &m_fMultiSel);
	RegisterProperty(_T("cellstyle"),  DU_PROPERTY_STRING, m_szCellStyle);
	
	duScrollView::RegisterControlProperty();
}

void WINAPI duReportView::OnCreate()
{
	duScrollView::OnCreate();
}

void WINAPI duReportView::DrawObject(HDC hDC)
{
	duRect rcReportView;
	rcReportView.SetRectEmpty();
	GetRect(&rcReportView);

	BITMAPINFO bmInfo;
	BYTE *pBits;

	HDC hMemDC = ::CreateCompatibleDC(hDC);
	InitBitmapInfo(&bmInfo, rcReportView.Width(), rcReportView.Height());
	HBITMAP hBmp = ::CreateDIBSection(hDC, &bmInfo, DIB_RGB_COLORS,(void**)&pBits, 0, 0);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBmp);

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	::AlphaBlend(hMemDC, 0, 0, rcReportView.Width(), rcReportView.Height(), hDC, 0, 0, rcReportView.Width(), rcReportView.Height(), bf);


	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
	{
		duRect rcStyle = rcReportView;
		rcStyle.OffsetRect(-rcStyle.left, -rcStyle.top);
		pStyleGroup->Draw(hMemDC, &rcStyle, GetState(), GetText(), GetAlpha());
	}
	
	POINT ptView = GetViewPoint();
	duRect rcHeader;
	GetHeaderRect(&rcHeader);

	POINT ptOrg;
	HRGN hRgnClip = NULL;
	
	int i, j;
	duReportLine *pLine = NULL;
	duRect rcLine;
	rcLine = rcHeader;
	int nColCount = GetColumnCount();
	duReportCell *pCell = NULL;
	duRect rcCell;
	int nTotalHeight = 0;
	int nFirstHeight = 0;
	i = GetFirstVisibleLine(nFirstHeight);
	rcLine.top = rcLine.bottom = rcHeader.bottom + nFirstHeight;
	
	::SetViewportOrgEx(hMemDC, -ptView.x, -ptView.y, &ptOrg);
	for (; i < m_nLineCount; i++)
	{
		pLine = m_vtLine[i];
		rcLine.top = rcLine.bottom;
		rcLine.bottom = rcLine.top + pLine->GetHeight();
		pLine->DrawLine(hMemDC, &rcLine, i);
		
		rcCell = rcLine;
		rcCell.right = rcCell.left;
		for (j = 0;j < nColCount; j++)
		{
			pCell = GetCell(i, j);
			if (pCell == NULL)
				continue;
				
			if (j == nColCount - 1)
			{
				rcCell.left = rcCell.right;
				rcCell.right = rcHeader.right;
			}
			else
			{
				rcCell.left = rcCell.right;
				rcCell.right = rcCell.left + GetColumnWidth(j);
			}
			
			pCell->DrawCell(hMemDC, &rcCell, i, j);
		}

		if (rcLine.bottom - ptView.y > rcReportView.bottom)
			break;
	}
	
	::SetViewportOrgEx(hMemDC, ptOrg.x, ptOrg.y, &ptOrg);
	
	// DrawHeader
	hRgnClip = ::CreateRectRgn(rcHeader.left, rcHeader.top, rcHeader.right, rcHeader.bottom);
	::SelectClipRgn(hMemDC, hRgnClip);
	::SetViewportOrgEx(hMemDC, -ptView.x, 0, &ptOrg);
	m_pHeaderCtrl->DrawHeader(hMemDC);	
	::SetViewportOrgEx(hMemDC, ptOrg.x, ptOrg.y, &ptOrg);
	::SelectClipRgn(hMemDC, NULL);
	::DeleteObject(hRgnClip);

	::AlphaBlend(hDC, 0, 0, rcReportView.Width(), rcReportView.Height(), hMemDC, 0, 0, rcReportView.Width(), rcReportView.Height(), bf);

	::SelectObject(hMemDC, hOldBitmap);
	::DeleteObject(hBmp);
	::DeleteDC(hMemDC);
}

void WINAPI duReportView::DrawView(HDC hDC)
{
}

void WINAPI duReportView::GetViewSize(LPSIZE lpSize)
{
	duRect rcClient = _GetClientRect();
	lpSize->cx = m_pHeaderCtrl->GetTotalWidth();
	lpSize->cy = GetTotalHeight();
}

void WINAPI duReportView::OnMouseLDown(POINT pt)
{
	duRect rcReportView;
	rcReportView.SetRectEmpty();
	GetRect(&rcReportView);
	
	if (pt.y - rcReportView.top <= m_nHeaderHeight)
	{
		POINT ptView = GetViewPoint();
		pt.x += (ptView.x - rcReportView.left);
		pt.y -= rcReportView.top;
		m_pHeaderCtrl->OnMouseLDown(pt);
		return;
	}
	
	duScrollView::OnMouseLDown(pt);
}

void WINAPI duReportView::OnMouseLUp(POINT pt)
{
	duRect rcReportView;
	rcReportView.SetRectEmpty();
	GetRect(&rcReportView);
	

	POINT pt2 = pt;
	POINT ptView = GetViewPoint();
	pt2.x += (ptView.x - rcReportView.left);
	pt2.y -= rcReportView.top;
	m_pHeaderCtrl->OnMouseLUp(pt2);
	
	if (pt.y - rcReportView.top > m_nHeaderHeight)
		duScrollView::OnMouseLUp(pt);
}

void WINAPI duReportView::OnMouseRDown(POINT pt)
{
	duRect rcReportView;
	rcReportView.SetRectEmpty();
	GetRect(&rcReportView);
	
	if (pt.y - rcReportView.top <= m_nHeaderHeight)
	{
		POINT ptView = GetViewPoint();
		pt.x += (ptView.x - rcReportView.left);
		pt.y -= rcReportView.top;
		m_pHeaderCtrl->OnMouseRDown(pt);
		return;
	}
	
	duScrollView::OnMouseRDown(pt);
}

void WINAPI duReportView::OnMouseRUp(POINT pt)
{
	duRect rcReportView;
	rcReportView.SetRectEmpty();
	GetRect(&rcReportView);
	
	if (pt.y - rcReportView.top <= m_nHeaderHeight)
	{
		POINT ptView = GetViewPoint();
		pt.x += (ptView.x - rcReportView.left);
		pt.y -= rcReportView.top;
		m_pHeaderCtrl->OnMouseRUp(pt);
		return;
	}
	
	duScrollView::OnMouseRUp(pt);
}

void WINAPI duReportView::OnMouseHover(POINT pt)
{
	duRect rcReportView;
	rcReportView.SetRectEmpty();
	GetRect(&rcReportView);
	
	if (pt.y - rcReportView.top <= m_nHeaderHeight)
	{
		POINT ptView = GetViewPoint();
		pt.x += (ptView.x - rcReportView.left);
		pt.y -= rcReportView.top;
		m_pHeaderCtrl->OnMouseHover(pt);
		return;
	}
	
	duScrollView::OnMouseHover(pt);
}

void WINAPI duReportView::OnMouseMove(POINT pt)
{
	duRect rcReportView;
	rcReportView.SetRectEmpty();
	GetRect(&rcReportView);
	
	POINT pt2 = pt;
	POINT ptView = GetViewPoint();
	pt2.x += (ptView.x - rcReportView.left);
	pt2.y -= rcReportView.top;
	m_pHeaderCtrl->OnMouseMove(pt2);

	if (pt.y - rcReportView.top > m_nHeaderHeight)
	{
		m_pHeaderCtrl->OnMouseLeave();
		duScrollView::OnMouseMove(pt);
	}
	else if (m_nHotLine != -1)
	{
		duReportLine *pOldLine = GetLine(m_nHotLine);
		if (pOldLine)
		{
			pOldLine->SetState(DU_STATE_NORMAL);
			m_nHotLine = -1;
			Plugin_Redraw(this, TRUE);
		}
	}
}

void WINAPI duReportView::OnMouseDbClick(POINT pt)
{
	duRect rcReportView;
	rcReportView.SetRectEmpty();
	GetRect(&rcReportView);
	
	if (pt.y - rcReportView.top <= m_nHeaderHeight)
	{
		POINT ptView = GetViewPoint();
		pt.x += (ptView.x - rcReportView.left);
		pt.y -= rcReportView.top;
		m_pHeaderCtrl->OnMouseDbClick(pt);
		return;
	}
	
	duScrollView::OnMouseDbClick(pt);
}

void WINAPI duReportView::OnViewMouseLDown(POINT pt)
{
	if (m_nHotLine == -1)
		return;
	
	duReportLine *pHotLine = GetLine(m_nHotLine);
	if (pHotLine == NULL)
		return;
	
	SelectLine(m_nHotLine);
	Plugin_Redraw(this, TRUE);
}

void WINAPI duReportView::OnViewMouseLUp(POINT pt)
{
/*
	m_pHeaderCtrl->OnMouseLUp(pt);
*/
}

void WINAPI duReportView::OnViewMouseRDown(POINT pt)
{
/*
	CRect rcHeader;
	GetHeaderRect(&rcHeader);
	if (rcHeader.PtInRect(pt))
	{
		m_pHeaderCtrl->OnMouseRDown(pt);
		return;
	}
*/
}

void WINAPI duReportView::OnViewMouseRUp(POINT pt)
{
/*
	CRect rcHeader;
	GetHeaderRect(&rcHeader);
	if (rcHeader.PtInRect(pt))
	{
		m_pHeaderCtrl->OnMouseRUp(pt);
		return;
	}
*/
}

void WINAPI duReportView::OnViewMouseHover(POINT pt)
{
/*
	CRect rcHeader;
	GetHeaderRect(&rcHeader);
	if (rcHeader.PtInRect(pt))
	{
		m_pHeaderCtrl->OnMouseHover(pt);
		return;
	}
*/
}

void WINAPI duReportView::OnViewMouseMove(POINT pt)
{
	duRect rcReportView;
	rcReportView.SetRectEmpty();
	GetRect(&rcReportView);
	
	POINT ptView = GetViewPoint();
	duRect rcHeader;
	GetHeaderRect(&rcHeader);
	
	int i;
	duRect rcLine;
	rcLine = rcHeader;
	int nColCount = GetColumnCount();
	duRect rcCell;
	int nTotalHeight = 0;
	int nFirstHeight = 0;
	i = GetFirstVisibleLine(nFirstHeight);
	rcLine.top = rcLine.bottom = rcHeader.bottom + nFirstHeight;
	
	duReportCell *pCell = NULL;
	duReportLine *pLine = NULL;
	duReportLine *pOldLine = NULL;
	for (; i < m_nLineCount; i++)
	{
		pLine = m_vtLine[i];
		rcLine.top = rcLine.bottom;
		rcLine.bottom = rcLine.top + pLine->GetHeight();
		
		if (rcLine.PtInRect(pt))
		{
			if (m_nHotLine != i)
			{
				pLine->SetState(DU_STATE_OVER);
				pOldLine = GetLine(m_nHotLine);
				if (pOldLine)
					pOldLine->SetState(DU_STATE_NORMAL);
			
				m_nHotLine = i;
				Plugin_Redraw(this, TRUE);
			}
			return;
		}
		
		//rcCell = rcLine;
		//rcCell.right = rcCell.left;
		//for (j = 0;j < nColCount; j++)
		//{
		//	pCell = GetCell(i, j);
		//	if (pCell == NULL)
		//		continue;
		//		
		//	if (j == nColCount - 1)
		//	{
		//		rcCell.left = rcCell.right;
		//		rcCell.right = rcHeader.right;
		//	}
		//	else
		//	{
		//		rcCell.left = rcCell.right;
		//		rcCell.right = rcCell.left + GetColumnWidth(j);
		//	}
		//}

		if (rcLine.bottom - ptView.y > rcReportView.bottom)
			break;
	}

	if (m_nHotLine != -1)
	{
		pOldLine = GetLine(m_nHotLine);
		if (pOldLine)
			pOldLine->SetState(DU_STATE_NORMAL);
	
		m_nHotLine = -1;
		Plugin_Redraw(this, TRUE);
	}
}

void WINAPI duReportView::OnViewMouseDbClick(POINT pt)
{
/*
	CRect rcHeader;
	GetHeaderRect(&rcHeader);
	if (rcHeader.PtInRect(pt))
	{
		m_pHeaderCtrl->OnMouseDbClick(pt);
		return;
	}
*/
}

void WINAPI duReportView::OnMouseLeave(POINT pt)
{
	m_pHeaderCtrl->OnMouseLeave();
	
	if (m_nHotLine != -1)
	{
		duReportLine *pOldLine = GetLine(m_nHotLine);
		if (pOldLine)
		{
			pOldLine->SetState(DU_STATE_NORMAL);
			m_nHotLine = -1;
			Plugin_Redraw(this, TRUE);
		}
	}
}

BOOL WINAPI duReportView::OnViewSetCursor(POINT pt)
{
	POINT ptView = GetViewPoint();
	if (pt.y - ptView.y <= m_nHeaderHeight)
	{
		pt.y -= ptView.y;
		return m_pHeaderCtrl->OnSetCursor(pt);
	}
	
	return FALSE;
}

BOOL WINAPI duReportView::InsertColumn(int nCol, int nWidth, LPCTSTR lpszText)
{
	int nNewCol = m_pHeaderCtrl->InsertItem(nCol, nWidth, lpszText);
	if (nNewCol == -1)
		return FALSE;
	
	vector<duReportColumn *>::iterator iterInsert = m_vtColumn.begin() + nNewCol;
	duReportColumn *pColumn = new duReportColumn;
	pColumn->SetReportView(this);
	pColumn->Init();
	m_vtColumn.insert(iterInsert, pColumn);
	return TRUE;
}

int WINAPI duReportView::InsertLine(int nLine, int nHeight)
{
	if (nLine == -1)
		nLine = m_nLineCount;
		
	if (nLine < 0 || nLine > m_nLineCount)
		return -1;
	
	int nColCount = GetColumnCount();
	int i;
	duReportColumn *pColumn = NULL;
	for (i = 0;i < nColCount; i++)
	{
		pColumn = m_vtColumn[i];
		pColumn->InsertLine(nLine);
	}
	
	vector<duReportLine *>::iterator iterInsert = m_vtLine.begin() + nLine;
	duReportLine *pLine = new duReportLine;
	pLine->SetReportView(this);
	pLine->SetStyle(m_szLineStyle);
	if (nHeight <= 0)
		nHeight = REPORTVIEW_DEFAULT_HEIGHT;
	
	pLine->SetHeight(nHeight);
	pLine->SetState(DU_STATE_NORMAL);
	m_vtLine.insert(iterInsert, pLine);
	m_nLineCount++;

	UpdateScroll();
	return nLine;
}

void WINAPI duReportView::SetCellStyle(LPCTSTR lpszStyle)
{
	if (lpszStyle)
		_tcsncpy(m_szCellStyle, lpszStyle, MAX_NAME);
	else
		ZeroMemory(m_szCellStyle, MAX_NAME * sizeof(TCHAR));
}

void WINAPI duReportView::SetLineStyle(LPCTSTR lpszStyle)
{
	if (lpszStyle)
		_tcsncpy(m_szLineStyle, lpszStyle, MAX_NAME);
	else
		ZeroMemory(m_szLineStyle, MAX_NAME * sizeof(TCHAR));
}

LPCTSTR WINAPI duReportView::GetCellText(int nLine, int nCol)
{
	duReportCell *pCell = GetCell(nLine, nCol);
	if (pCell)
		return pCell->GetText();
	return NULL;
}

void WINAPI duReportView::SetCellText(int nLine, int nCol, LPCTSTR lpszText)
{
	duReportCell *pCell = GetCell(nLine, nCol);
	if (pCell)
		pCell->SetText(lpszText);
}

BOOL WINAPI duReportView::DeleteColumn(int nCol)
{
	if (m_pHeaderCtrl->DeleteColumn(nCol))
	{
		vector<duReportColumn *>::iterator iter = m_vtColumn.begin() + nCol;
		duReportColumn *pColumn = *iter;
		if (pColumn)
			delete pColumn;
		m_vtColumn.erase(iter);
		return TRUE;
	}
	return FALSE;
}

BOOL WINAPI duReportView::DeleteLine(int nLine)
{
	int nLineCount = GetLineCount();
	if (nLine < 0 || nLine >= nLineCount)
		return FALSE;
	
	vector<duReportLine *>::iterator iter = m_vtLine.begin() + nLine;
	duReportLine *pLine = *iter;
	if (pLine)
		delete pLine;
	m_vtLine.erase(iter);

	int nColCount = GetColumnCount();
	int i;
	duReportColumn *pColumn = NULL;
	for (i = 0;i < nColCount; i++)
	{
		pColumn = m_vtColumn[i];
		pColumn->DeleteLine(nLine);
	}
	
	m_nLineCount--;
	
	return TRUE;
}

void WINAPI duReportView::GetHeaderRect(LPRECT lpRect)
{
	if (lpRect)
		::SetRectEmpty(lpRect);
		
	duRect rcClient = _GetClientRect();
	
	duSize sizeView;
	GetViewSize(&sizeView);
	
	lpRect->left = 0;
	lpRect->right = max(sizeView.cx, rcClient.Width());
	lpRect->top = 0;
	lpRect->bottom = lpRect->top + m_nHeaderHeight;
}

int duReportView::GetTotalHeight()
{
	int nTotalHeight = 0;
	int i;
	duReportLine *pLine = NULL;
	for (i = 0;i < m_nLineCount; i++)
	{
		pLine = m_vtLine[i];
		nTotalHeight += pLine->GetHeight();
	}
	
	return nTotalHeight + m_nHeaderHeight;
}

duReportCell *duReportView::GetCell(int nLine, int nCol)
{
	if (nLine < 0 || nLine >= m_nLineCount || nCol < 0 || nCol >= GetColumnCount())
		return NULL;
	
	duReportColumn *pColumn = GetColumn(nCol);
	if (pColumn == NULL)
		return NULL;
	
	return pColumn->GetCell(nLine);
}

duReportColumn *duReportView::GetColumn(int nCol)
{
	if (nCol < 0 || nCol >= GetColumnCount())
		return NULL;
		
	return m_vtColumn[nCol];
}

duReportLine *duReportView::GetLine(int nLine)
{
	if (nLine < 0 || nLine >= m_nLineCount)
		return NULL;
		
	return m_vtLine[nLine];
}

int WINAPI duReportView::GetColumnWidth(int nCol)
{
	return m_pHeaderCtrl->GetColumnWidth(nCol);
}

int duReportView::GetFirstVisibleLine(int &nFirstHeight)
{
	POINT ptView = GetViewPoint();

	nFirstHeight = 0;
	int i;
	duReportLine *pLine = NULL;
	for (i = 0;i < m_nLineCount; i++)
	{
		pLine = m_vtLine[i];
		nFirstHeight += pLine->GetHeight();

		if (nFirstHeight > ptView.y)
		{
			nFirstHeight -= pLine->GetHeight();
			return i;
		}
	}
	
	return 0;
}

void duReportView::SelectLine(int nLine)
{
	duReportLine *pNewLine = GetLine(nLine);
	if (pNewLine == NULL)
		return;

	BOOL fKeyShift = 0x8000 & ::GetKeyState(VK_SHIFT);
	BOOL fKeyCtrl = 0x8000 & ::GetKeyState(VK_CONTROL);

	if (m_fMultiSel && (fKeyShift || fKeyCtrl))
	{
		if (fKeyShift)
		{
			int nPreSelLine = GetPreSelLine(nLine);
			if (nPreSelLine == -1)
			{
				pNewLine->SetSelected(TRUE);
				pNewLine->SetState(DU_STATE_PRESS);
			}
			else
				SelectRange(nPreSelLine, nLine);
		}
		else if (fKeyCtrl)
		{
			if (pNewLine->IsSelected())
			{
				pNewLine->SetSelected(FALSE);
				pNewLine->SetState(DU_STATE_NORMAL);
			}
			else
			{
				pNewLine->SetSelected(TRUE);
				pNewLine->SetState(DU_STATE_PRESS);			
			}
		}
	}
	else
	{
		int i;
		duReportLine *pLine = NULL;
		for (i = 0;i < m_nLineCount; i++)
		{
			pLine = m_vtLine[i];
			if (pLine->IsSelected())
			{					
				pLine->SetSelected(FALSE);
				pLine->SetState(DU_STATE_NORMAL);
			}
		}
		
		pNewLine->SetSelected(TRUE);
		pNewLine->SetState(DU_STATE_PRESS);
	}
}

void duReportView::SelectRange(int nStart, int nEnd)
{
	if (nStart < 0 || nEnd < 0 || nStart >= m_nLineCount || nEnd >= m_nLineCount || nStart >= nEnd)
		return;
	
	int i;
	duReportLine *pLine = NULL;
	for (i = nStart; i <= nEnd; i++)
	{
		pLine = m_vtLine[i];
		pLine->SetSelected(TRUE);
		pLine->SetState(DU_STATE_PRESS);
	}
}

int duReportView::GetPreSelLine(int nLine)
{
	if (nLine < 0 || nLine >= m_nLineCount)
		return -1;
	
	int i;
	duReportLine *pLine = NULL;
	for (i = nLine; i >= 0; i--)
	{
		pLine = m_vtLine[i];
		if (pLine->IsSelected())
			return i;
	}
	
	return -1;
}


