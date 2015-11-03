//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duListCtrl.cpp
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "duListCtrl.h"
#include "duDrawHelper.h"
#include "duCtrlManager.h"
#include "duHeaderCtrl.h"

duListCtrl::duListCtrl() :
	 m_nCloneIndex(0)
	,m_nHotLine(0)
	,m_nFirstLine(0)
	,m_nFLTotalHeight(0)
	,m_nViewCy(0)
	,m_pSel(NULL)
	,m_pHot(NULL)
	,m_fDispMouseWheel(FALSE)
{
	ZeroMemory(m_szHeaderCtrl, MAX_NAME * sizeof(TCHAR));
	m_vtLines.clear();
}

duListCtrl::~duListCtrl()
{
	m_vtLines.clear();
	Plugin_UnHookWindowMessage(this);
}

void duListCtrl::RegisterControlProperty()
{
	//
	RegisterProperty(_T("headerctrl"), DU_PROPERTY_STRING, m_szHeaderCtrl);

	duScrollView::RegisterControlProperty();
}

void duListCtrl::OnCreate()
{
	duScrollView::OnCreate();

	Plugin_HookWindowMessage(this);
}

void duListCtrl::DrawObject(HDC hDC)
{
	
}

void duListCtrl::GetViewSize(LPSIZE lpSize)
{
	lpSize->cx = 0;
	duHeaderCtrl *pHeaderCtrl = (duHeaderCtrl *)GetPluginByName(m_szHeaderCtrl);
	if (pHeaderCtrl)
		lpSize->cx = pHeaderCtrl->GetTotalWidth();
	
	lpSize->cy = m_nViewCy;
}

LRESULT duListCtrl::OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

		duRect rcListCtrl;
		GetRect(&rcListCtrl);

		if (!rcListCtrl.PtInRect(pointClient))
		{
			m_fDispMouseWheel = FALSE;
			return 0;
		}

		m_fDispMouseWheel = TRUE;

		//pointClient.Offset(-rcListCtrl.left, -rcListCtrl.top);
		duScroll *pVertScroll = (duScroll *)GetPluginByName(m_szVertScroll);
		if (pVertScroll && pVertScroll->IsVisible())
		{
			duRect rcVertScroll;
			pVertScroll->GetRect(&rcVertScroll);

			if (rcVertScroll.PtInRect(pointClient))
			{
				m_fDispMouseWheel = FALSE;
				return 0;
			}
		}

		duScroll *pHorzScroll = (duScroll *)GetPluginByName(m_szHorzScroll);
		if (pHorzScroll && pHorzScroll->IsVisible())
		{
			duRect rcHorzScroll;
			pHorzScroll->GetRect(&rcHorzScroll);

			if (rcHorzScroll.PtInRect(pointClient))
			{
				m_fDispMouseWheel = FALSE;
				return 0;
			}
		}

		int nHeight = rcListCtrl.Height();
		int i;
		int nLineCount = GetLineCount();
		for (i = m_nFirstLine;i < nLineCount; i++)
		{
			duPlugin *pTemp = m_vtLines[i];
			if (pTemp)
			{
				duRect rc;
				pTemp->GetRect(&rc);

				if (rc.PtInRect(pointClient))
				{
					if (m_pSel != pTemp)
					{
						if (Plugin_IsValid(m_pSel))
						{
							m_pSel->SetState(DU_STATE_NORMAL);
							Plugin_Redraw(m_pSel, TRUE);
						}

						pTemp->SetState(DU_STATE_PRESS);
						m_pSel = pTemp;
						Plugin_Redraw(pTemp, TRUE);
					}	
					break;
				}

				if (rc.top >= rcListCtrl.bottom)
					break;
			}
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
		if (Plugin_IsValid(pVertScrollBar) && pVertScrollBar->IsVisible() && m_fDispMouseWheel)
			pVertScrollBar->OnMouseWheel(fwKeys, zDelta, pt);
	}
	else if (uMsg == WM_MOUSEMOVE)
	{
		POINT pointClient; 
		pointClient.x = GET_X_LPARAM(lParam);
		pointClient.y = GET_Y_LPARAM(lParam);

		duRect rcListCtrl;
		GetRect(&rcListCtrl);

		if (!rcListCtrl.PtInRect(pointClient))
		{
			if (Plugin_IsValid(m_pHot))
			{
				if (m_pHot != m_pSel)
				{
					m_pHot->SetState(DU_STATE_NORMAL);
					Plugin_Redraw(m_pHot, TRUE);
					m_pHot = NULL;
				}
			}

			return 0;
		}

		duScroll *pVertScroll = (duScroll *)GetPluginByName(m_szVertScroll);
		if (pVertScroll && pVertScroll->IsVisible())
		{
			duRect rcVertScroll;
			pVertScroll->GetRect(&rcVertScroll);

			if (rcVertScroll.PtInRect(pointClient))
			{
				if (Plugin_IsValid(m_pHot))
				{
					if (m_pHot != m_pSel)
					{
						m_pHot->SetState(DU_STATE_NORMAL);
						Plugin_Redraw(m_pHot, TRUE);
						m_pHot = NULL;
					}
				}

				return 0;
			}
		}

		duScroll *pHorzScroll = (duScroll *)GetPluginByName(m_szHorzScroll);
		if (pHorzScroll && pHorzScroll->IsVisible())
		{
			duRect rcHorzScroll;
			pHorzScroll->GetRect(&rcHorzScroll);

			if (rcHorzScroll.PtInRect(pointClient))
			{
				if (Plugin_IsValid(m_pHot))
				{
					if (m_pHot != m_pSel)
					{
						m_pHot->SetState(DU_STATE_NORMAL);
						Plugin_Redraw(m_pHot, TRUE);
						m_pHot = NULL;
					}
				}

				return 0;
			}
		}

		BOOL fNeedRedraw = FALSE;
		BOOL fMouseIn = FALSE;

		int nHeight = rcListCtrl.Height();
		int i;
		int nLineCount = GetLineCount();
		for (i = m_nFirstLine;i < nLineCount; i++)
		{
			duPlugin *pTemp = m_vtLines[i];
			if (pTemp)
			{
				duRect rc;
				pTemp->GetRect(&rc);

				if (rc.PtInRect(pointClient))
				{
					fMouseIn = TRUE;

					if (pTemp == m_pSel)
					{
						if (Plugin_IsValid(m_pHot))
						{
							if (m_pHot == m_pSel)
								break;

							m_pHot->SetState(DU_STATE_NORMAL);
							m_pHot = NULL;
							fNeedRedraw = TRUE;
						}
						
						m_pHot = NULL;
					}
					else
					{
						if (m_pHot != pTemp)
						{
							if (Plugin_IsValid(m_pHot))
							{
								if (m_pHot != m_pSel)
								{
									m_pHot->SetState(DU_STATE_NORMAL);
									m_pHot = NULL;
									fNeedRedraw = TRUE;
								}
							}

							pTemp->SetState(DU_STATE_OVER);
							m_pHot = pTemp;
							fNeedRedraw = TRUE;
						}
					}
					break;
				}

				if (rc.top >= rcListCtrl.bottom)
					break;
			}
		}

		if (!fMouseIn)
		{
			if (Plugin_IsValid(m_pHot))
			{
				if (m_pHot != m_pSel)
				{
					m_pHot->SetState(DU_STATE_NORMAL);
					m_pHot = NULL;
					fNeedRedraw = TRUE;
				}
			}
		}

		if (fNeedRedraw)
			Plugin_Redraw(this, TRUE);
	}

	return 0;
}

void duListCtrl::Resize(LPRECT lpRect/*=NULL*/)
{
	duScrollView::Resize(NULL);
	
	if (m_vtLines.size() > 0)
		VertScroll(0);
}

void duListCtrl::OnVScroll(ScrollBarAction sbAction, int nPos)
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
				VertScroll(nPos - m_nYOffset);
				m_nYOffset = nPos;
				
				Plugin_Redraw(this, TRUE); // 重绘
			}
		}
		break;
	
	default:
		_TRACE(_T("OnVScroll Error pos[%d]\n"), nPos);
		break;
	}
}

void duListCtrl::OnHScroll(ScrollBarAction sbAction, int nPos)
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
				HorzScroll(nPos);
				m_nXOffset = nPos;
				
				Plugin_Redraw(this, TRUE);
				//::InvalidateRect(m_hWnd, NULL, TRUE);
			}
		}
		break;

	default:
		break;
	}
}

void duListCtrl::HorzScroll(int nPos)
{
	int i;
	int nLineCount = m_vtLines.size();
	
	for (i = 0;i < nLineCount; i++)
	{
		duPlugin *pTemp = m_vtLines[i];
		pTemp->SetPosLeft( - nPos );
	}

	duHeaderCtrl *pHeaderCtrl = (duHeaderCtrl *)GetPluginByName(m_szHeaderCtrl);
	if (pHeaderCtrl)
	{
		//CRect rc;
		//GetRect(&rc);

		//_TRACE(_T("HorzScroll %d TotalWidth %d ListCtrlWidth :%d\n"), nPos, pHeaderCtrl->GetTotalWidth(), rc.Width());


		//duScroll *p = (duScroll *)GetPluginByName(m_szHorzScroll);
		//_TRACE(_T("maxpos :%d page :%d pos :%d\n"), p->GetMaxPos(), p->GetPage(), p->GetPos());

		pHeaderCtrl->SetOffsetX(nPos);
		Plugin_Redraw(pHeaderCtrl, TRUE);
	}

	Resize(NULL);
}

void duListCtrl::VertScroll(int nPos)
{
	int nLineCount = m_vtLines.size();
	if (nLineCount <= 0)
		return;

	// 判断nPos正确性
	duPlugin *pFirstLine = m_vtLines[m_nFirstLine];
	duRect rcFirstLine;
	pFirstLine->GetRect(&rcFirstLine);

	int i;
	for (i = 0;i < nLineCount; i++)
	{
		duPlugin *pTemp = m_vtLines[i];
		pTemp->SetVisible(FALSE);
	}
	
	int nFirstLineOffset = m_nYOffset - m_nFLTotalHeight + nPos;
	int nTop = 0;
	if (rcFirstLine.Height() <= nFirstLineOffset) // 计算滚动几个控件 <= ?
	{
		int nNewFirstLine = m_nFirstLine;
		int nNewFLTotalHeight = m_nFLTotalHeight;
		int i;
		for (i = nNewFirstLine; i < m_vtLines.size(); i++)
		{
			duPlugin *pTemp = m_vtLines[i];
			duRect rcTemp;
			pTemp->GetRect(&rcTemp);
			
			nFirstLineOffset -= rcTemp.Height();
			if (nFirstLineOffset < 0)
			{
				nFirstLineOffset += rcTemp.Height();
				break;
			}
			
			nNewFLTotalHeight += rcTemp.Height();
			nNewFirstLine++;
		}
		
		m_nFirstLine = nNewFirstLine;
		m_nFLTotalHeight = nNewFLTotalHeight;
	}
	else if (nFirstLineOffset < 0)
	{
		int nNewFirstLine = m_nFirstLine - 1;
		int nNewFLTotalHeight = m_nFLTotalHeight;
		int i;
		for (i = nNewFirstLine; i >= 0; i--)
		{
			duPlugin *pTemp = m_vtLines[i];
			duRect rcTemp;
			pTemp->GetRect(&rcTemp);
			
			nFirstLineOffset += rcTemp.Height();
			nNewFLTotalHeight -= rcTemp.Height();
			if (nFirstLineOffset > 0)
			{
				//nFirstLineOffset -= rcTemp.Height();
				break;
			}
			
			nNewFirstLine--;
		}
		
		if (nNewFirstLine < 0)
		{
			m_nFirstLine = 0;
			m_nFLTotalHeight = 0;
			nFirstLineOffset = 0;
		}
		else
		{
			m_nFirstLine = nNewFirstLine;
			m_nFLTotalHeight = nNewFLTotalHeight;
		}
	}
	
	//_TRACE(_T("nFirstLineOffset : %d\n"), nFirstLineOffset);
	AdjustVisibleLine(-nFirstLineOffset);
}

duPlugin *duListCtrl::InsertLine(int nIndex, LPCTSTR lpszTemplate)
{
	int nLineCount = m_vtLines.size();
	if (nIndex == -1)
		nIndex = nLineCount;

	if (nIndex < 0 || nIndex > nLineCount || lpszTemplate == NULL)
		return NULL;
	
	duPlugin *pTemplate = GetPluginByName(lpszTemplate);
	if (pTemplate == NULL)
		return NULL;

	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	duPlugin *pNewPlugin = pCtrlManager->Clone(pTemplate, this, NULL, m_nCloneIndex);
	if (pNewPlugin == NULL)
		return NULL;

	m_nCloneIndex++;
	pNewPlugin->SetVisible(FALSE); // 初始化隐藏
	m_vtLines.insert(m_vtLines.begin() + nIndex, pNewPlugin);
	
	duRect rcNewPlugin;
	pNewPlugin->GetRect(&rcNewPlugin);
	m_nViewCy += rcNewPlugin.Height();
	nLineCount = m_vtLines.size();
	
	duRect rcListCtrl;
	GetRect(&rcListCtrl);
	
	if (nIndex < m_nFirstLine)
	{
		int nInsertHeight = rcNewPlugin.Height();
		int nNewFirstLine = m_nFirstLine + 1;
		int nFLTotalHeight = m_nFLTotalHeight;
		
		duPlugin *pOldFirstLine = m_vtLines[nNewFirstLine];
		duRect rcOld;
		pOldFirstLine->GetRect(&rcOld);

		int i;
		for (i = nNewFirstLine; i >= 0; i--)
		{
			duPlugin *pTemp = m_vtLines[i];
			wstring strText = pTemp->GetText();
			if (pTemp)
			{
				duRect rcTemp;
				pTemp->GetRect(&rcTemp);
				
				nNewFirstLine--;
				nInsertHeight -= rcTemp.Height();
				if (nInsertHeight <= rcOld.Height())
				{
					nFLTotalHeight += nInsertHeight;
					break;
				}
			}
		}
		
		m_nFLTotalHeight = nFLTotalHeight;
		m_nFirstLine = nNewFirstLine;
	}

	UpdateScroll();
	
	int nTop = m_nFLTotalHeight - m_nYOffset;
	AdjustVisibleLine(nTop);
	return pNewPlugin;
}

void duListCtrl::AdjustVisibleLine(int nTop)
{
	duRect rcListCtrl;
	GetRect(&rcListCtrl);

	int i;
	for (i = m_nFirstLine; i < m_vtLines.size(); i++)
	{
		duPlugin *pTemp = m_vtLines[i];
		if (pTemp)
		{
			duRect rcTemp;
			pTemp->GetRect(&rcTemp);
			
			pTemp->SetPosTop(nTop);
			pTemp->Resize(NULL);
			pTemp->SetVisible(TRUE); // 后面的控件还需要SetVisible(FALSE);
			
			nTop += rcTemp.Height();
			if (nTop >= rcListCtrl.Height())
				break;
		}
	}
}

void duListCtrl::AdjustColumn()
{
	duHeaderCtrl *pHeaderCtrl = (duHeaderCtrl *)GetPluginByName(m_szHeaderCtrl);
	if (pHeaderCtrl == NULL)
		return;

	int nItemCount = pHeaderCtrl->GetItemCount();
	if (nItemCount <= 0)
		return;

	int i, j;
	vector<int> vtColWidth;
	vtColWidth.clear();
	vector<HeaderCtrlItem *> &vtItems = pHeaderCtrl->m_vtItems;
	for (i = 0;i < nItemCount; i++)
	{
		HeaderCtrlItem *pItem = vtItems[i];
		if (pItem)
			vtColWidth.push_back(pItem->nWidth);
	}

	int nSepWidth = pHeaderCtrl->GetSepWidth();
	int nLineWidth = pHeaderCtrl->GetTotalWidth();
	int nLineCount = m_vtLines.size();
	for (i = 0;i < nLineCount; i++)
	{
		duPlugin *pTemp = m_vtLines[i];
		if (pTemp == NULL)
			continue;

		pTemp->SetPosWidth(nLineWidth);

		j = 0;
		duPlugin *pColumn = pTemp->GetFirstChild();
		while (pColumn)
		{
			pColumn->SetPosWidth(vtColWidth[j] + nSepWidth);

			j++;
			pColumn = pColumn->GetNextSibling();
		}
	}

	vtColWidth.clear();
	Resize(NULL);
	
	VertScroll(0);
}

void duListCtrl::AdjustColumnWidth(int nCol, int nWidth)
{
	duHeaderCtrl *pHeaderCtrl = (duHeaderCtrl *)GetPluginByName(m_szHeaderCtrl);
	if (pHeaderCtrl == NULL)
		return;

	int nSepWidth = pHeaderCtrl->GetSepWidth();
	int i,j;
	int nLineWidth = pHeaderCtrl->GetTotalWidth();
	int nLineCount = m_vtLines.size();
	for (i = 0;i < nLineCount; i++)
	{
		duPlugin *pTemp = m_vtLines[i];
		if (pTemp == NULL)
			continue;

		pTemp->SetPosWidth(nLineWidth);

		j = 0;
		duPlugin *pColumn = pTemp->GetFirstChild();
		while (pColumn)
		{
			if (j == nCol)
			{
				pColumn->SetPosWidth(nWidth + nSepWidth);
				break;
			}

			j++;
			pColumn = pColumn->GetNextSibling();
		}
	}
	
	Resize(NULL);
	UpdateScroll();
	Plugin_Redraw(this, TRUE);
}

duPlugin *duListCtrl::GetLine(int nIndex)
{
	int nLineCount = GetLineCount();
	if (nIndex < 0 || nIndex >= nLineCount)
		return NULL;

	return m_vtLines[nIndex];
}

int duListCtrl::GetLineCount()
{
	return (int)m_vtLines.size();
}

int duListCtrl::GetTotalHeight()
{
	return m_nViewCy;
}

duPlugin *duListCtrl::GetFirstVisibleLine()
{
	int nLineCount = GetLineCount();
	if (m_nFirstLine < 0 || m_nFirstLine >= nLineCount)
		return NULL;

	return m_vtLines[m_nFirstLine];
}

BOOL duListCtrl::MoveLine(int nIndex, int nNewIndex)
{
	int nLineCount = GetLineCount();
	if (nIndex < 0 || nIndex >= nLineCount)
		return FALSE;

	if (nNewIndex < 0 || nNewIndex >= nLineCount)
		return FALSE;

	if (nIndex == nNewIndex)
		return FALSE;

	duPlugin *pTemp = m_vtLines[nNewIndex];
	m_vtLines[nNewIndex] = m_vtLines[nIndex];
	m_vtLines[nIndex] = pTemp;

	return TRUE;
}

BOOL duListCtrl::DeleteLine(duPlugin* pItem, BOOL fAjust)
{
	for (unsigned int i = 0; i < m_vtLines.size(); i++)
	{
		if (m_vtLines[i] == pItem)
		{
			return DeleteLine(i, fAjust);
		}
	}
	return FALSE;
}

BOOL duListCtrl::DeleteLine(int nIndex, BOOL fAdjust)
{
	int nLineCount = GetLineCount();
	if (nIndex < 0 || nIndex >= nLineCount)
		return FALSE;

	duPlugin *pDelete = m_vtLines[nIndex];
	if (pDelete == NULL)
		return FALSE;

	duRect rcDelete;
	pDelete->GetRect(&rcDelete);

	if (nIndex < m_nFirstLine)
		m_nFLTotalHeight -= rcDelete.Height();

	m_nViewCy -= rcDelete.Height();
	
	vector<duPlugin *>::iterator iterDelete = m_vtLines.begin() + nIndex;
	m_vtLines.erase(iterDelete);
	UpdateScroll();

	if (pDelete == m_pSel)
		m_pSel = NULL;

	if (pDelete == m_pHot)
		m_pHot = NULL;
	
	Plugin_Delete(pDelete);
	CompleteAdjustLine();

	return TRUE;
}

void duListCtrl::DeleteAll()
{
	for (unsigned int i = 0; i < m_vtLines.size(); i++)
	{
		duPlugin *pDelete = m_vtLines[i];
		Plugin_Delete(pDelete);
	}

	m_vtLines.clear();

	m_nCloneIndex = 0;
	m_nHotLine = 0;
	m_nFirstLine = 0;
	m_nFLTotalHeight = 0;
	m_nViewCy = 0;
	m_pSel = NULL;
	m_pHot = NULL;
}

void duListCtrl::ResetLine()
{
	m_nYOffset = 0;
	m_nFirstLine = 0;
	m_nFLTotalHeight = 0;
	m_nHotLine = 0;
	if (Plugin_IsValid(m_pHot))
	{
		m_pHot->SetState(DU_STATE_NORMAL);
		Plugin_Redraw(m_pHot, TRUE);
	}

	UpdateScroll();
	AdjustVisibleLine(0);
}

void duListCtrl::CompleteAdjustLine()
{
	int nLineCount = GetLineCount();
	duRect rcListCtrl;
	GetRect(&rcListCtrl);
	int nHeight = rcListCtrl.Height();
	int nTop = -m_nYOffset;
	int i;

	for (i = 0;i < nLineCount; i++)
	{
		duPlugin *pTemp = m_vtLines[i];
		pTemp->SetVisible(FALSE);
	}

	duHeaderCtrl *pHeaderCtrl = (duHeaderCtrl *)GetPluginByName(m_szHeaderCtrl);
	if (pHeaderCtrl == NULL)
		return;

	int nItemCount = pHeaderCtrl->GetItemCount();
	if (nItemCount <= 0)
		return;

	vector<int> vtColWidth;
	vtColWidth.clear();
	vector<HeaderCtrlItem *> &vtItems = pHeaderCtrl->m_vtItems;
	for (i = 0;i < nItemCount; i++)
	{
		HeaderCtrlItem *pItem = vtItems[i];
		if (pItem)
			vtColWidth.push_back(pItem->nWidth);
	}

	int nLineWidth = pHeaderCtrl->GetTotalWidth();
	BOOL fFirstLine = FALSE;
	int nTotalHeight2 = 0;
	for (i = 0;i < nLineCount; i++)
	{
		duPlugin *pTemp = m_vtLines[i];
	
		duRect rcTemp;
		pTemp->GetRect(&rcTemp);

		nTotalHeight2 += rcTemp.Height();
		nTop += rcTemp.Height();
		if (nTop >= 0)
		{
			if (!fFirstLine)
			{
				fFirstLine = TRUE;
				m_nFirstLine = i;
			}

			pTemp->SetVisible(TRUE);

			int nTop2 = nTop - rcTemp.Height();
			pTemp->SetPosWidth(nLineWidth);
			pTemp->SetPosTop(nTop2);

			//
			//
			int j = 0;
			duPlugin *pColumn = pTemp->GetFirstChild();
			while (pColumn)
			{
				pColumn->SetPosWidth(vtColWidth[j]);

				j++;
				pColumn = pColumn->GetNextSibling();
			}

			pTemp->Resize(NULL);
		}
		if (nTop >= nHeight)
			break;
	}

	if (nLineCount == 0)
	{
		m_nFirstLine = 0;
		m_nFLTotalHeight = 0;
	}
}

int duListCtrl::GetLinePluginIndex(duPlugin *pPlugin)
{
	if (pPlugin == NULL)
		return -1;

	int nLineCount = m_vtLines.size();
	int i;
	for (i = 0;i < nLineCount; i++)
	{
		if (m_vtLines[i] == pPlugin)
			return i;
	}
	
	return -1;
}

int duListCtrl::GetSel()
{
	return GetLinePluginIndex(m_pSel);
}

void duListCtrl::SetSel(int nSel)
{
	int nLineCount = GetLineCount();
	if (nSel < 0 || nSel >= nLineCount)
		return;

	duPlugin *pNewSel = m_vtLines[nSel];
	if (m_pSel)
	{
		m_pSel->SetState(DU_STATE_NORMAL);
		if (m_pSel->IsVisible())
			Plugin_Redraw(m_pSel, TRUE);
	}

	pNewSel->SetState(DU_STATE_OVER);
	m_pSel = pNewSel;
	if (pNewSel->IsVisible())
		Plugin_Redraw(m_pSel, TRUE);
}

void duListCtrl::SetHeaderCtrl(LPCTSTR lpszHeader)
{
	if (lpszHeader)
		_tcscpy(m_szHeaderCtrl, lpszHeader);
	else
		ZeroMemory(m_szHeaderCtrl, MAX_NAME * sizeof(TCHAR));
}
