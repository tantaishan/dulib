//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duPage.cpp
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "duPage.h"

duPage::duPage() :
	 m_nTotalCount(0)
	,m_nPerPage(0)
	,m_nArrowWidth(0)
	,m_nArrowHeight(0)
	,m_nArrowButtonSpace(0)
	,m_nButtonWidth(0)
	,m_nButtonHeight(0)
	,m_nButtonSpace(0)
	,m_uLeftArrowState(DU_STATE_NORMAL)
	,m_uRightArrowState(DU_STATE_NORMAL)
	,m_nHotButton(-1)

{
	ZeroMemory(m_szLeftArrowStyle, sizeof(TCHAR) * MAX_NAME);
	ZeroMemory(m_szRightArrowStyle, sizeof(TCHAR) * MAX_NAME);
	ZeroMemory(m_szButtonStyle, sizeof(TCHAR) * MAX_NAME);
	
	m_vtButtonState.clear();
}

duPage::~duPage()
{
	m_vtButtonState.clear();	
}

void duPage::RegisterControlProperty()
{
	RegisterProperty(_T("total"), DU_PROPERTY_LONG, &m_nTotalCount);
	RegisterProperty(_T("page"), DU_PROPERTY_LONG, &m_nPerPage);
	
	RegisterProperty(_T("arrowwidth"), DU_PROPERTY_LONG, &m_nArrowWidth);
	RegisterProperty(_T("arrowheight"), DU_PROPERTY_LONG, &m_nArrowHeight);
	RegisterProperty(_T("arrowbuttonspace"), DU_PROPERTY_LONG, &m_nArrowButtonSpace);
	
	RegisterProperty(_T("buttonwidth"), DU_PROPERTY_LONG, &m_nButtonWidth);
	RegisterProperty(_T("buttonheight"), DU_PROPERTY_LONG, &m_nButtonHeight);
	RegisterProperty(_T("buttonspace"), DU_PROPERTY_LONG, &m_nButtonSpace);
	
	RegisterProperty(_T("leftarrowstyle"), DU_PROPERTY_STYLEGROUP, m_szLeftArrowStyle);
	RegisterProperty(_T("rightarrowstyle"), DU_PROPERTY_STYLEGROUP, m_szRightArrowStyle);
	RegisterProperty(_T("buttonstyle"), DU_PROPERTY_STYLEGROUP, m_szButtonStyle);
}

void duPage::OnCreate()
{
	if (m_nPerPage)
	{
		int nPageCount = GetPageCount();
		int i;
		for (i = 0;i < nPageCount; i++)
			m_vtButtonState.push_back(DU_STATE_NORMAL);
	}
	
	AutoSize();
}

void duPage::DrawObject(HDC hDC)
{
	duRect rcPage;
	Plugin_GetRect(this, &rcPage);
	rcPage.OffsetRect(-rcPage.left, -rcPage.top);
	
	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
		pStyleGroup->Draw(hDC, &rcPage, GetState(), NULL, GetAlpha());
		
	// Draw Left Arrow
	duRect rcLeftArrow;
	rcLeftArrow.left = rcPage.left;
	rcLeftArrow.top = (rcPage.Height() - m_nArrowHeight) / 2;
	rcLeftArrow.right = rcLeftArrow.left + m_nArrowWidth;
	rcLeftArrow.bottom = rcLeftArrow.top + m_nArrowHeight;

	m_rcLeftArrowBtn.left = rcLeftArrow.left;
	m_rcLeftArrowBtn.right = rcLeftArrow.right;
	m_rcLeftArrowBtn.top = rcLeftArrow.top;
	m_rcLeftArrowBtn.bottom = rcLeftArrow.bottom;
	
	duStyleGroup *pLeftArrowStyle = (duStyleGroup *)GetResObj(m_szLeftArrowStyle, DU_RES_STYLEGROUP);
	if (pLeftArrowStyle)
		pLeftArrowStyle->Draw(hDC, &rcLeftArrow, m_uLeftArrowState, NULL, GetAlpha());
		
	// Draw page Buttons
	duRect rcButton;
	rcButton.left = rcLeftArrow.right + m_nArrowButtonSpace;
	rcButton.top = (rcPage.Height() - m_nButtonHeight) / 2;
	rcButton.right = rcButton.left + m_nButtonWidth;
	rcButton.bottom = rcButton.top + m_nButtonHeight;
	
	rcButton.OffsetRect(-(m_nButtonWidth + m_nButtonSpace), 0);
	
	duStyleGroup *pButtonStyle = (duStyleGroup *)GetResObj(m_szButtonStyle, DU_RES_STYLEGROUP);
	if (pButtonStyle)
	{
		int i;
		int nButtonCount = m_vtButtonState.size();
		for (i = 0;i < nButtonCount; i++)
		{
			rcButton.OffsetRect(m_nButtonWidth + m_nButtonSpace, 0);
			
			TCHAR szText[128];
			_stprintf(szText, _T("%d"), i + 1);
			pButtonStyle->Draw(hDC, &rcButton, m_vtButtonState[i], szText, GetAlpha());
		}
	}
	
	// Draw Right Arrow
	duRect rcRightArrow(rcLeftArrow);
	rcRightArrow.left = rcPage.right - m_nArrowWidth;
	rcRightArrow.right = rcPage.right;

	m_rcRightArrowBtn.left = rcRightArrow.left;
	m_rcRightArrowBtn.right = rcRightArrow.right;
	m_rcRightArrowBtn.top = rcRightArrow.top;
	m_rcRightArrowBtn.bottom = rcRightArrow.bottom;
	
	duStyleGroup *pRightArrowStyle = (duStyleGroup *)GetResObj(m_szRightArrowStyle, DU_RES_STYLEGROUP);
	if (pRightArrowStyle)
		pRightArrowStyle->Draw(hDC, &rcRightArrow, m_uRightArrowState, NULL, GetAlpha());
}

void duPage::OnMouseMove(POINT point)
{
	BOOL fNeedRedraw = FALSE;
	duRect rcButton;
	duRect rcRightArrow;

	duPoint pt(point);
	duRect rcPage;
	Plugin_GetRect(this, &rcPage);
	pt.Offset(-rcPage.left, -rcPage.top);
	rcPage.OffsetRect(-rcPage.left, -rcPage.top);

	// Hit Left Arrow
	duRect rcLeftArrow;
	rcLeftArrow.left = rcPage.left;
	rcLeftArrow.top = (rcPage.Height() - m_nArrowHeight) / 2;
	rcLeftArrow.right = rcLeftArrow.left + m_nArrowWidth;
	rcLeftArrow.bottom = rcLeftArrow.top + m_nArrowHeight;

	
	
	if (rcLeftArrow.PtInRect(pt))
	{
		if (m_nHotButton != -1)
		{
			m_vtButtonState[m_nHotButton] = IsStateChecked(m_vtButtonState[m_nHotButton]) ? DU_STATE_CHECKED : DU_STATE_NORMAL;
			m_nHotButton = -1;
			fNeedRedraw = TRUE;
		}
	
		if (m_uLeftArrowState == DU_STATE_NORMAL)
		{
			m_uLeftArrowState = DU_STATE_OVER;
			m_uRightArrowState = DU_STATE_NORMAL;
			fNeedRedraw = TRUE;
			goto PageMouseMove_end;
		}
	}
	
	// Draw Right Arrow
	rcRightArrow = rcLeftArrow;
	rcRightArrow.left = rcPage.right - m_nArrowWidth;
	rcRightArrow.right = rcPage.right;

	

	if (rcRightArrow.PtInRect(pt))
	{
		if (m_nHotButton != -1)
		{
			m_vtButtonState[m_nHotButton] = IsStateChecked(m_vtButtonState[m_nHotButton]) ? DU_STATE_CHECKED : DU_STATE_NORMAL;
			m_nHotButton = -1;
			fNeedRedraw = TRUE;
		}
		
		if (m_uRightArrowState == DU_STATE_NORMAL)
		{
			m_uRightArrowState = DU_STATE_OVER;
			m_uLeftArrowState = DU_STATE_NORMAL;
			fNeedRedraw = TRUE;
			goto PageMouseMove_end;
		}
	}
	
	// Hit page Buttons
	rcButton.left = rcLeftArrow.right + m_nArrowButtonSpace;
	rcButton.top = (rcPage.Height() - m_nButtonHeight) / 2;
	rcButton.right = rcButton.left + m_nButtonWidth;
	rcButton.bottom = rcButton.top + m_nButtonHeight;
	
	rcButton.OffsetRect(-(m_nButtonWidth + m_nButtonSpace), 0);
	
	int i;
	int nButtonCount = m_vtButtonState.size();
	for (i = 0;i < nButtonCount; i++)
	{
		rcButton.OffsetRect(m_nButtonWidth + m_nButtonSpace, 0);
		if (rcButton.PtInRect(pt))
		{
			if (m_uLeftArrowState != DU_STATE_NORMAL)
			{
				m_uLeftArrowState = DU_STATE_NORMAL;
				fNeedRedraw = TRUE;
			}
			
			if (m_uRightArrowState != DU_STATE_NORMAL)
			{
				m_uRightArrowState = DU_STATE_NORMAL;
				fNeedRedraw = TRUE;
			}

			if (m_nHotButton != i)
			{
				if (m_nHotButton != -1)
				{
					m_vtButtonState[m_nHotButton] = IsStateChecked(m_vtButtonState[m_nHotButton]) ? DU_STATE_CHECKED : DU_STATE_NORMAL;
					fNeedRedraw = TRUE;
				}
				
				m_nHotButton = i;
				if (m_vtButtonState[i] == DU_STATE_NORMAL)
				{
					m_vtButtonState[i] = DU_STATE_OVER;
					fNeedRedraw = TRUE;
				}
				
				if (m_vtButtonState[i] == DU_STATE_CHECKED)
				{
					m_vtButtonState[i] = DU_STATE_CHECKEDOVER;
					fNeedRedraw = TRUE;
				}
			}
			break;
		}
	}
	

PageMouseMove_end:
	if (fNeedRedraw)
		Plugin_Redraw(this, TRUE);
}

void duPage::OnMouseLeave(POINT pt)
{
	BOOL fNeedRedraw = FALSE;
	
	if (m_uLeftArrowState != DU_STATE_NORMAL)
	{
		m_uLeftArrowState = DU_STATE_NORMAL;
		fNeedRedraw = TRUE;
	}
	
	if (m_uRightArrowState != DU_STATE_NORMAL)
	{
		m_uRightArrowState = DU_STATE_NORMAL;
		fNeedRedraw = TRUE;
	}
	
	if (m_nHotButton != -1)
	{
		m_vtButtonState[m_nHotButton] = IsStateChecked(m_vtButtonState[m_nHotButton]) ? DU_STATE_CHECKED : DU_STATE_NORMAL;
		m_nHotButton = -1;
		fNeedRedraw = TRUE;
	}
	
	if (fNeedRedraw)
		Plugin_Redraw(this, TRUE);
}

void duPage::OnMouseLDown(POINT pt)
{
	BOOL fNeedRedraw = FALSE;
	duRect rcRect;
	GetRect(&rcRect);
	pt.x = pt.x - rcRect.left;
	pt.y = pt.y - rcRect.top;
	if (PtInRect(&m_rcLeftArrowBtn,pt))
	{
		if (m_uLeftArrowState == DU_STATE_OVER)
		{
			m_uLeftArrowState = DU_STATE_PRESS;
			fNeedRedraw = TRUE;
		}
	}
	
	if (PtInRect(&m_rcRightArrowBtn,pt))
	{
		if (m_uRightArrowState == DU_STATE_OVER)
		{
			m_uRightArrowState = DU_STATE_PRESS;
			fNeedRedraw = TRUE;
		}
	}
	
	
	 
		if (m_nHotButton != -1)
		{
			if (m_vtButtonState[m_nHotButton] == DU_STATE_OVER)
			{
				m_vtButtonState[m_nHotButton] = DU_STATE_PRESS;
				fNeedRedraw = TRUE;
			}

			if (m_vtButtonState[m_nHotButton] == DU_STATE_CHECKEDOVER)
			{
				m_vtButtonState[m_nHotButton] = DU_STATE_CHECKEDPRESS;
				fNeedRedraw = TRUE;
			}
		}

	
	
	
	if (fNeedRedraw)
		Plugin_Redraw(this, TRUE);
}

void duPage::OnMouseLUp(POINT pt)
{
	BOOL fNeedRedraw = FALSE;
	
	if (m_uLeftArrowState == DU_STATE_PRESS)
	{
		m_uLeftArrowState = DU_STATE_NORMAL;
		fNeedRedraw = TRUE;
		MoveLeft(FALSE);
	}
	
	if (m_uRightArrowState == DU_STATE_PRESS)
	{
		m_uRightArrowState = DU_STATE_NORMAL;
		fNeedRedraw = TRUE;
		MoveRight(FALSE);
	}
	
	
		if (m_nHotButton != -1)
			{
			if (m_vtButtonState[m_nHotButton] == DU_STATE_PRESS)
			{
				int i;
				int nButtonCount = m_vtButtonState.size();
				int nOldSel = -1;
				for (i = 0;i < nButtonCount; i++)
				{
					if (IsStateChecked(m_vtButtonState[i]))
					{
						if (i != m_nHotButton)
						{
							nOldSel = i;
							m_vtButtonState[i] = DU_STATE_NORMAL;
							fNeedRedraw = TRUE;
						}
					}
				}
			
				m_vtButtonState[m_nHotButton] = DU_STATE_CHECKED;
				NotifyUser(DUM_PAGECHANGED, nOldSel, m_nHotButton);
				fNeedRedraw = TRUE;
			}
			
			if (m_vtButtonState[m_nHotButton] == DU_STATE_CHECKEDPRESS)
			{
				m_vtButtonState[m_nHotButton] = DU_STATE_CHECKED;
				fNeedRedraw = TRUE;
			}
		}
	
	if (fNeedRedraw)
		Plugin_Redraw(this, TRUE);
}

void duPage::OnMouseDbClick(POINT pt)
{
	
}

void duPage::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
/*
	switch (nChar)
	{
	case VK_LEFT:
		MoveLeft(TRUE);
		break;

	case VK_RIGHT:
		MoveRight(TRUE);
		break;

	default:
		break;
	}
*/
}

BOOL duPage::OnSetCursor()
{
	duPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd, &pt);
	
	
	duRect rcPage;
	Plugin_GetRect(this, &rcPage);
	pt.Offset(-rcPage.left, -rcPage.top);
	rcPage.OffsetRect(-rcPage.left, -rcPage.top);
	
	duRect rcButton;
	rcButton.left = rcPage.left + m_nArrowWidth + m_nArrowButtonSpace;
	rcButton.top = (rcPage.Height() - m_nButtonHeight) / 2;
	rcButton.right = rcButton.left + m_nButtonWidth;
	rcButton.bottom = rcButton.top + m_nButtonHeight;
	
	rcButton.OffsetRect(-(m_nButtonWidth + m_nButtonSpace), 0);
	
	int i;
	int nButtonCount = m_vtButtonState.size();
	for (i = 0;i < nButtonCount; i++)
	{
		rcButton.OffsetRect(m_nButtonWidth + m_nButtonSpace, 0);
		if (rcButton.PtInRect(pt))
		{
			return duPlugin::OnSetCursor();
		}
	}
	if (m_rcLeftArrowBtn.PtInRect(pt)||m_rcRightArrowBtn.PtInRect(pt))
		return duPlugin::OnSetCursor();
	
	
	::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
	return TRUE;
}

int duPage::CalcPageWidth()
{
	if (!m_nPerPage)
		return 0;
	
	int nWidth = (m_nArrowWidth + m_nArrowButtonSpace) * 2;
	int nPageCount = GetPageCount();
	if (nPageCount)
		nWidth += (nPageCount * m_nButtonWidth) + (nPageCount - 1) * m_nButtonSpace;
	
	return nWidth;
}

BOOL duPage::MoveLeft(BOOL fRedraw)
{
	int i;
	int nButtonCount = m_vtButtonState.size();
	for (i = 0;i < nButtonCount; i++)
	{
		if (IsStateChecked(m_vtButtonState[i]))
		{
			if (!i)
				return FALSE;
			
			m_vtButtonState[i] = DU_STATE_NORMAL;
			m_vtButtonState[i - 1] = DU_STATE_CHECKED;
			
			NotifyUser(DUM_PAGECHANGED, i, i - 1);
			
			if (fRedraw)
				Plugin_Redraw(this, TRUE);

			return TRUE;
		}
	}
	
	if (nButtonCount)
	{
		m_vtButtonState[nButtonCount - 1] = DU_STATE_CHECKED;
		NotifyUser(DUM_PAGECHANGED, -1, nButtonCount - 1);
		if (fRedraw)
			Plugin_Redraw(this, TRUE);
	}
	
	return TRUE;
}

BOOL duPage::MoveRight(BOOL fRedraw)
{
	int i;
	int nButtonCount = m_vtButtonState.size();
	for (i = 0;i < nButtonCount; i++)
	{
		if (IsStateChecked(m_vtButtonState[i]))
		{
			if (i == nButtonCount - 1)
				return FALSE;
				
			m_vtButtonState[i] = DU_STATE_NORMAL;
			m_vtButtonState[i + 1] = DU_STATE_CHECKED;
			
			NotifyUser(DUM_PAGECHANGED, i, i + 1);
			
			if (fRedraw)
				Plugin_Redraw(this, TRUE);
				
			return TRUE;
		}
	}
	
	if (nButtonCount)
	{
		m_vtButtonState[0] = DU_STATE_CHECKED;
		NotifyUser(DUM_PAGECHANGED, -1, 0);
		if (fRedraw)
			Plugin_Redraw(this, TRUE);
	}
	
	return TRUE;
}

int duPage::GetPageCount()
{
	if (!m_nPerPage || !m_nTotalCount)
		return 0;
	
	return (m_nTotalCount / m_nPerPage) + ((m_nTotalCount % m_nPerPage) ? 1 : 0);
}

int duPage::GetSel()
{
	int i;
	int nButtonCount = m_vtButtonState.size();
	for (i = 0;i < nButtonCount; i++)
	{
		if (IsStateChecked(m_vtButtonState[i]))
			return i;
	}
	
	return -1;
}

void duPage::SetSel(int nSel)
{
	int i;
	int nButtonCount = m_vtButtonState.size();
	if (nSel < 0 || nSel >= nButtonCount)
		return;
	
	for (i = 0;i < nButtonCount; i++)
	{
		if (IsStateChecked(m_vtButtonState[i]))
		{
			if (i != nSel)
			{
				m_vtButtonState[i] = DU_STATE_NORMAL;
				m_vtButtonState[nSel] = DU_STATE_CHECKED;
				
				NotifyUser(DUM_PAGECHANGED, i, nSel);
				return;
			}
		}
	}

	m_vtButtonState[nSel] = DU_STATE_CHECKED;
	NotifyUser(DUM_PAGECHANGED, -1, nSel);
}

void duPage::SetTotal(int nTotal)
{
	m_nTotalCount = nTotal;
	m_vtButtonState.clear();
	int nPageCount = GetPageCount();
	int i;
	for (i = 0;i < nPageCount; i++)
		m_vtButtonState.push_back(DU_STATE_NORMAL);

	AutoSize();
}

void duPage::SetPerPage(int nPerPage)
{
	if (nPerPage < 0)
		return;
	
	m_nPerPage = nPerPage;
	m_vtButtonState.clear();
	int nPageCount = GetPageCount();
	int i;
	for (i = 0;i < nPageCount; i++)
		m_vtButtonState.push_back(DU_STATE_NORMAL);
		
	AutoSize();
}

void duPage::SetLeftArrowStyle(LPCTSTR lpszStyle)
{
	if (lpszStyle)
		_tcsncpy(m_szLeftArrowStyle, lpszStyle, MAX_NAME);
	else
		ZeroMemory(m_szLeftArrowStyle, MAX_NAME * sizeof(TCHAR));
}

void duPage::SetRightArrowStyle(LPCTSTR lpszStyle)
{
	if (lpszStyle)
		_tcsncpy(m_szRightArrowStyle, lpszStyle, MAX_NAME);
	else
		ZeroMemory(m_szRightArrowStyle, MAX_NAME * sizeof(TCHAR));	
}

void duPage::SetButtonStyle(LPCTSTR lpszStyle)
{
	if (lpszStyle)
		_tcsncpy(m_szButtonStyle, lpszStyle, MAX_NAME);
	else
		ZeroMemory(m_szButtonStyle, MAX_NAME * sizeof(TCHAR));
}

void duPage::AutoSize()
{
	SetPosHorzAnchor(0);
	SetPosWidth(CalcPageWidth());
	
	SetPosVertAnchor(0);
	SetPosHeight(max(m_nArrowHeight, m_nButtonHeight));
	
	Resize(NULL);
}

