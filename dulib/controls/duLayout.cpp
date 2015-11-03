//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duLayout.cpp
//
//  History:    JAN-09-2010   Denny Chen  Created
//
//--------------------------------------------------------------------------
#include "duLayout.h"

duLayout::duLayout() :
		 m_nLayoutType(LAYOUT_NORMAL)
		,m_nStartX(0)
		,m_nStartY(0)
		,m_nXSpace(0)
		,m_nYSpace(0)
		,m_fAlwaysHot(FALSE)
{

}

duLayout::~duLayout()
{
	Plugin_UnHookWindowMessage(this);
}

void duLayout::RegisterControlProperty()
{
	RegisterProperty(_T("type"), DU_PROPERTY_LONG, &m_nLayoutType);
	RegisterProperty(_T("startx"), DU_PROPERTY_LONG, &m_nStartX);
	RegisterProperty(_T("starty"), DU_PROPERTY_LONG, &m_nStartY);
	RegisterProperty(_T("xspace"), DU_PROPERTY_LONG, &m_nXSpace);
	RegisterProperty(_T("yspace"), DU_PROPERTY_LONG, &m_nYSpace);
	RegisterProperty(_T("alwayshot"), DU_PROPERTY_BOOL, &m_fAlwaysHot);
}

void duLayout::OnCreate()
{
	AdjustChilds();
	if (m_fAlwaysHot)
		Plugin_HookWindowMessage(this);
}

void duLayout::DrawObject(HDC hDC)
{
	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup == NULL)
		return;

	duRect rcCtrl;
	rcCtrl.SetRectEmpty();
	GetRect(&rcCtrl);
	rcCtrl.OffsetRect(-rcCtrl.left, -rcCtrl.top);
	
	pStyleGroup->Draw(hDC, &rcCtrl, GetState(), GetText(), GetAlpha());
}

LRESULT duLayout::OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!m_fAlwaysHot || !Plugin_IsVisible(this))
		return 0;

	if (uMsg == WM_MOUSEMOVE)
	{
		POINT pt;
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);

		duRect rcLayout;
		Plugin_GetRect(this, &rcLayout);

		if (rcLayout.PtInRect(pt))
		{
			if (GetState() == DU_STATE_NORMAL)
			{
				Plugin_SetState(this, DU_STATE_OVER);
				Plugin_Redraw(this, TRUE);
				return 0;
			}
		}
		else
		{
			if (GetState() == DU_STATE_OVER)
			{
				Plugin_SetState(this, DU_STATE_NORMAL);
				Plugin_Redraw(this, TRUE);
			}
		}
	}

	return 0;
}

void duLayout::OnChildShow(duPlugin *pChild, BOOL fVisible)
{
	if (!Plugin_IsValid(pChild))
		return;
	
	duPlugin *pParent = pChild->GetParent();
	if (pParent == this)
	{
		AdjustChilds();
		//Plugin_Redraw(this, TRUE);
	}
}

void duLayout::Resize(LPRECT lpRect)
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
	else
	{
		duRect rcClient;
		::GetClientRect(m_hWnd, &rcClient);
		::CopyRect(&m_rect, &rcClient);
	}

	switch(m_nLayoutType)
	{
	case LAYOUT_LEFTTORIGHT:
		AdjustLeftRight();
		break;
	case LAYOUT_RIGHTTOLEFT:
		AdjustRightLeft();
		break;
	case LAYOUT_TOPTOBOTTOM:
		AdjustTopBottom();
		break;
	case LAYOUT_BOTTOMTOTOP:
		AdjustBottomTop();
		break;
	case LAYOUT_HORZ_BRIMMING:
		AdjustHorzBrimming();
		break;
	case LAYOUT_VERT_BRIMMING:
		AdjustVertBrimming();
		break;
	case LAYOUT_GRID:
		AdjustGrid();
		break;
	default:
		{
			duPlugin *pChild = GetFirstChild();
			while (pChild)
			{
				pChild->Resize(NULL);
				pChild = pChild->GetNextSibling();
			}
		}
		break;
	}
}

void duLayout::OnChildResize(duPlugin *pChild)
{
	if (!Plugin_IsValid(pChild))
		return;

	duPlugin *pParent = pChild->GetParent();
	if (pParent == this)
	{
		AdjustChilds();
		Plugin_Redraw(this, TRUE);
	}
}

void duLayout::AdjustChilds()
{
	switch(m_nLayoutType)
	{
	case LAYOUT_LEFTTORIGHT:
		AdjustLeftRight();
		break;
	case LAYOUT_RIGHTTOLEFT:
		AdjustRightLeft();
		break;
	case LAYOUT_TOPTOBOTTOM:
		AdjustTopBottom();
		break;
	case LAYOUT_BOTTOMTOTOP:
		AdjustBottomTop();
		break;
	case LAYOUT_HORZ_BRIMMING:
		AdjustHorzBrimming();
		break;
	case LAYOUT_VERT_BRIMMING:
		AdjustVertBrimming();
		break;
	case LAYOUT_GRID:
		AdjustGrid();
		break;
	default:
		break;
	}
}

void duLayout::SetAlwaysHot(BOOL fAlwaysHot)
{
	if (fAlwaysHot != m_fAlwaysHot)
	{
		if (m_fAlwaysHot)
			Plugin_UnHookWindowMessage(this);
		else
			Plugin_HookWindowMessage(this);

		m_fAlwaysHot = fAlwaysHot;
	}
}

void duLayout::AdjustLeftRight()
{
	duRect rectLayout;
	rectLayout.SetRectEmpty();
	GetRect(&rectLayout);
	
	duPlugin *pNow = GetFirstChild();
	while(pNow)
	{
		if (!pNow->IsVisible())
		{
			pNow = pNow->GetNextSibling();
			continue;
		}
		
		duPlugin *pPre = GetPreVisible(pNow);
		if (pPre)
		{
			duRect rectPre;
			pPre->GetRect(&rectPre);
			rectPre.OffsetRect(-rectLayout.left, -rectLayout.top);
			pNow->SetPosLeft(rectPre.right + m_nXSpace);
		}
		else
		{
			pNow->SetPosLeft(m_nStartX);
		}
		pNow->SetPosHorzAnchor(0);
		pNow->Resize(&rectLayout);
		pNow = pNow->GetNextSibling();
	}
}

void duLayout::AdjustRightLeft()
{
	duRect rectLayout;
	rectLayout.SetRectEmpty();
	GetRect(rectLayout);
	
	duPlugin *pNow = GetLastChild();
	while(pNow)
	{
		if (!pNow->IsVisible())
		{
			pNow = pNow->GetPreSibling();
			continue;
		}
		
		duRect rectNow;
		rectNow.SetRectEmpty();
		pNow->GetRect(&rectNow);
		duPlugin *pNext = GetNextVisible(pNow);
		if (pNext)
		{
			int Nextleft = pNext->GetPosLeft();
			pNow->SetPosLeft(Nextleft - m_nXSpace - rectNow.Width());
		}
		else
		{
			pNow->SetPosLeft(-m_nStartX-rectNow.Width());
		}
		pNow->SetPosHorzAnchor(0);
		pNow->Resize(&rectLayout);
		pNow = pNow->GetPreSibling();
	}
}

void duLayout::AdjustTopBottom()
{
	duRect rectLayout;
	rectLayout.SetRectEmpty();
	GetRect(rectLayout);
	
	duPlugin *pNow = GetFirstChild();
	while(pNow)
	{
		if (!pNow->IsVisible())
		{
			pNow = pNow->GetNextSibling();
			continue;
		}
		
		duPlugin *pPre = GetPreVisible(pNow);
		if (pPre)
		{
			duRect rectPre;
			rectPre.SetRectEmpty();
			pPre->GetRect(&rectPre);
			rectPre.OffsetRect(-rectLayout.left, -rectLayout.top);
			pNow->SetPosTop(rectPre.bottom + m_nYSpace);
		}
		else
		{
			pNow->SetPosTop(m_nStartY);
		}
		pNow->SetPosVertAnchor(0);
		pNow->Resize(&rectLayout);
		pNow = pNow->GetNextSibling();
	}
}

void duLayout::AdjustBottomTop()
{
	duRect rectLayout;
	rectLayout.SetRectEmpty();
	GetRect(rectLayout);

	duPlugin *pNow = GetLastChild();
	while(pNow)
	{
		if (!pNow->IsVisible())
		{
			pNow = pNow->GetPreSibling();
			continue;
		}
		
		duRect rectNow;
		rectNow.SetRectEmpty();
		pNow->GetRect(&rectNow);
		duPlugin *pNext = GetNextVisible(pNow);
		if (pNext)
		{
			int NextTop = pNext->GetPosTop();
			pNow->SetPosTop(NextTop - m_nYSpace - rectNow.Height());
		}
		else
		{
			pNow->SetPosTop(-m_nStartY-rectNow.Height());
		}
		pNow->SetPosHorzAnchor(0);
		pNow->Resize(&rectLayout);
		pNow = pNow->GetPreSibling();
	}
}

void duLayout::AdjustHorzBrimming()
{
	int nChildCount = GetVisibleChildCount(this);
	if (nChildCount == 0)
		return;
		
	duRect rectLayout;
	rectLayout.SetRectEmpty();
	GetRect(rectLayout);

	int nWidth = rectLayout.Width() - m_nStartX - (nChildCount - 1) * m_nXSpace;
	int iWidth = nWidth / nChildCount;
	int nPxCount = nWidth - iWidth * nChildCount; // Æ«²îµÄÏñËØÊý

	duPlugin *pNow = GetFirstChild();
	while(pNow)
	{
		if (!pNow->IsVisible())
		{
			pNow = pNow->GetNextSibling();
			continue;
		}
		
		duPlugin *pPre = GetPreVisible(pNow);
		if (pPre)
		{
			duRect rectPre;
			rectPre.SetRectEmpty();
			pPre->GetRect(&rectPre);
			rectPre.OffsetRect(-rectLayout.left, -rectLayout.top);
			pNow->SetPosLeft(rectPre.right + m_nXSpace);
		}
		else
		{
			pNow->SetPosLeft(m_nStartX);
		}

		pNow->SetPosHorzAnchor(0);
		if (nPxCount > 0)
		{
			pNow->SetPosWidth(iWidth+1);
			nPxCount--;
		}
		else
		{
			pNow->SetPosWidth(iWidth);
		}

		pNow->Resize(&rectLayout);
		pNow = pNow->GetNextSibling();
	}
}

void duLayout::AdjustVertBrimming()
{
	int nChildCount = GetVisibleChildCount(this);
	if (nChildCount == 0)
		return;
		
	duRect rectLayout;
	rectLayout.SetRectEmpty();
	GetRect(rectLayout);

	int nHeight = rectLayout.Height() - m_nStartY - (nChildCount - 1) * m_nYSpace;
	int iHeight = nHeight / nChildCount;
	int nPxCount = nHeight - iHeight * nChildCount;

	duPlugin *pNow = GetFirstChild();
	while(pNow)
	{
		if (!pNow->IsVisible())
		{
			pNow = pNow->GetNextSibling();
			continue;
		}
		
		duPlugin *pPre = GetPreVisible(pNow);
		if (pPre)
		{
			duRect rectPre;
			rectPre.SetRectEmpty();
			pPre->GetRect(&rectPre);
			rectPre.OffsetRect(-rectLayout.left, -rectLayout.top);
			pNow->SetPosTop(rectPre.bottom + m_nYSpace);
		}
		else
		{
			pNow->SetPosTop(m_nStartY);
		}

		pNow->SetPosVertAnchor(0);
		if (nPxCount > 0)
		{
			pNow->SetPosHeight(iHeight+1);
			nPxCount--;
		}
		else
		{
			pNow->SetPosHeight(iHeight);
		}

		pNow->Resize(&rectLayout);
		pNow = pNow->GetNextSibling();
	}
}

void duLayout::AdjustGrid()
{
	duRect rectLayout;
	Plugin_GetRect(this, rectLayout);
	
	int nMaxRowHeight = rectLayout.top + m_nStartY;

	duPlugin *pNow = GetFirstChild();
	while(pNow)
	{
		if (!pNow->IsVisible())
		{
			pNow = pNow->GetNextSibling();
			continue;
		}
		
		duPlugin *pPre = GetPreVisible(pNow);
		if (pPre)
		{
			duRect rectPre;
			rectPre.SetRectEmpty();
			pPre->GetRect(&rectPre);
			rectPre.OffsetRect(-rectLayout.left, -rectLayout.top);
			pNow->SetPosTop(rectPre.top);
			pNow->SetPosLeft(rectPre.right + m_nXSpace);
		}
		else
		{
			pNow->SetPosTop(m_nStartY);
			pNow->SetPosLeft(m_nStartX);
		}
		pNow->Resize(&rectLayout);
		
		duRect rectNow;
		rectNow.SetRectEmpty();
		pNow->GetRect(&rectNow);
        if (rectNow.right > rectLayout.right)
		{
			pNow->SetPosHorzAnchor(0);
			pNow->SetPosVertAnchor(0);

			if (pNow == GetFirstChild())
                pNow->SetPosTop(nMaxRowHeight - rectLayout.top);
			else
				pNow->SetPosTop(nMaxRowHeight - rectLayout.top + m_nYSpace);

			pNow->SetPosLeft(m_nStartX);
			pNow->Resize(&rectLayout);
			
			rectNow.SetRectEmpty();
			pNow->GetRect(&rectNow);
		}

		if (rectNow.bottom > nMaxRowHeight)
			nMaxRowHeight = rectNow.bottom;
		
		pNow = pNow->GetNextSibling();
	}
}

duPlugin *GetPreVisible(duPlugin *pPlugin)
{
	if (pPlugin == NULL)
		return NULL;
	
	duPlugin *pTemp = NULL;
	if (pPlugin)
	{
		pTemp = pPlugin->GetPreSibling();
		while(pTemp)
		{
			if (pTemp->IsVisible())
				break;
			pTemp = pTemp->GetPreSibling();
		}
	}

	return pTemp;
}

duPlugin *GetNextVisible(duPlugin *pPlugin)
{
	if (pPlugin == NULL)
		return NULL;
	
	duPlugin *pTemp = NULL;
	if (pPlugin)
	{
		pTemp = pPlugin->GetNextSibling();
		while(pTemp)
		{
			if (pTemp->IsVisible())
				break;
			pTemp = pTemp->GetNextSibling();
		}
	}

	return pTemp;
}

int GetVisibleChildCount(duPlugin *pPlugin)
{
	if (pPlugin == NULL)
		return 0;
	
	int nCount = 0;
	duPlugin *pTemp = pPlugin->GetFirstChild();
	while(pTemp)
	{
		if (pTemp->IsVisible())
			nCount++;

		pTemp = pTemp->GetNextSibling();
	}

	return nCount;
}
