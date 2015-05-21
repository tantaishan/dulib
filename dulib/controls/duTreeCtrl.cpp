//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duTreeCtrl.cpp
//
//  History:    Feb-21-2011   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duTreeCtrl.h"
#include "duDrawHelper.h"
#include "duCtrlManager.h"

duTreeCtrl::duTreeCtrl() :
	 m_nItemHeight(18)
	,m_nIndentWidth(0)
	,m_nIndentHeight(0)
	,m_nIconWidth(16)
	,m_nIconHeight(16)
	,m_nIndentIconSpace(0)
	,m_nIconTextSpace(0)
	,m_pHot(NULL)
	,m_pSelect(NULL)
	,m_nItems(0)
	,m_clrText(RGB(0,0,0))
	,m_fShowTooltip(FALSE)
	,m_fAutoShowToolTip(FALSE)
{
	m_pRoot = new TreeCtrlItem;
	m_pRoot->strText = _T("ROOT");
	m_pRoot->nTextWidth = 0;
	m_pRoot->strImage = _T("");
	m_pRoot->nLevel = 0;
	m_pRoot->fExpand = TRUE;
	m_pRoot->pParent = NULL;
	m_pRoot->pNextSibling = NULL;
	m_pRoot->pFirstChild = NULL;
	m_pRoot->lParam = NULL;
	
	ZeroMemory(m_szItemStyle, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szIndentStyle, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szFont, MAX_NAME * sizeof(TCHAR));
}

duTreeCtrl::~duTreeCtrl()
{
	DeleteItem(m_pRoot);
	if (m_pRoot)
	{
		delete m_pRoot;
		m_pRoot = NULL;
	}
}

void WINAPI duTreeCtrl::RegisterControlProperty()
{
	RegisterProperty(_T("itemstyle"), DU_PROPERTY_STYLEGROUP, m_szItemStyle);
	RegisterProperty(_T("font"), DU_PROPERTY_FONT, m_szFont);
	RegisterProperty(_T("indentstyle"), DU_PROPERTY_STYLEGROUP, m_szIndentStyle);
	RegisterProperty(_T("itemheight"), DU_PROPERTY_LONG, &m_nItemHeight);
	RegisterProperty(_T("indentwidth"), DU_PROPERTY_LONG, &m_nIndentWidth);
	RegisterProperty(_T("indentheight"), DU_PROPERTY_LONG, &m_nIndentHeight);
	RegisterProperty(_T("iconwidth"), DU_PROPERTY_LONG, &m_nIconWidth);
	RegisterProperty(_T("iconheight"), DU_PROPERTY_LONG, &m_nIconHeight);
	RegisterProperty(_T("indenticonspace"), DU_PROPERTY_LONG, &m_nIndentIconSpace);
	RegisterProperty(_T("icontextspace"), DU_PROPERTY_LONG, &m_nIconTextSpace);
	RegisterProperty(_T("btooltip"), DU_PROPERTY_BOOL, &m_fAutoShowToolTip);

	duScrollView::RegisterControlProperty();
}

void WINAPI duTreeCtrl::OnCreate()
{
	duScrollView::OnCreate();
}

void WINAPI duTreeCtrl::DrawObject(HDC hDC)
{
	duRect rcTreeCtrl;
	Plugin_GetRect(this, &rcTreeCtrl);
	rcTreeCtrl.OffsetRect(-rcTreeCtrl.left, -rcTreeCtrl.top);

	BITMAPINFO bmInfo;
	BYTE *pBits;

	HDC hMemDC = ::CreateCompatibleDC(hDC);
	InitBitmapInfo(&bmInfo, rcTreeCtrl.Width(), rcTreeCtrl.Height());
	HBITMAP hBmp = ::CreateDIBSection(hDC, &bmInfo, DIB_RGB_COLORS,(void**)&pBits, 0, 0);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBmp);

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	::AlphaBlend(hMemDC, 0, 0, rcTreeCtrl.Width(), rcTreeCtrl.Height(), hDC, 0, 0, rcTreeCtrl.Width(), rcTreeCtrl.Height(), bf);


	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
		pStyleGroup->Draw(hMemDC, &rcTreeCtrl, GetState(), NULL, GetAlpha());
	
	int nOffsetY = 0;
	TreeCtrlItem *pFirstVisible = GetFirstVisibleItem(nOffsetY);
	if (pFirstVisible == NULL || pFirstVisible == m_pRoot)
		return;
		
	duRect rcClient = _GetClientRect();
	
	POINT ptView = GetViewPoint();
	duFont *pFont = (duFont *)GetResObj(m_szFont, DU_RES_FONT);
	
	duSize sizeView;
	GetViewSize(&sizeView);
	
	int nDrawHeight = -nOffsetY;
	TreeCtrlItem *pItem = pFirstVisible;
	while (pItem)
	{
		duRect rcItem;
		
		rcItem.left   = -ptView.x;
		rcItem.right  = rcClient.right;
		rcItem.top    = nDrawHeight;
		rcItem.bottom = rcItem.top + m_nItemHeight;

		UINT uItemState = DU_STATE_NORMAL;
		if (pItem == m_pHot)
			uItemState = DU_STATE_OVER;
		
		if (pItem == m_pSelect)
			uItemState = DU_STATE_PRESS;
		
		DrawByStyle(this, m_szItemStyle, hMemDC, &rcItem, uItemState, NULL, GetAlpha());

		// draw indent[-+]
		int nLeft = (pItem->nLevel - 1) * m_nIndentWidth;
		duRect rcIndent = CalcVCenterRect(rcItem, nLeft, m_nIndentWidth, m_nIndentHeight);
		if (ItemHasChildren(pItem))
		{
			UINT nIndentState = pItem->fExpand ? DU_STATE_NORMAL : DU_STATE_CHECKED;
			DrawByStyle(this, m_szIndentStyle, hMemDC, &rcIndent, nIndentState, NULL, GetAlpha());
		}

		// draw icon
		nLeft += (m_nIndentWidth + m_nIndentIconSpace);
		duRect rcIcon = CalcVCenterRect(rcItem, nLeft, m_nIconWidth, m_nIconHeight);
		duImage *pIcon = (duImage *)GetResObj(pItem->strImage.c_str(), DU_RES_IMAGE);
		if (pIcon)
			DrawNormal(hMemDC, rcIcon.left, rcIcon.top, rcIcon.Width(), rcIcon.Height(), pIcon, 0, 0, GetAlpha());

		// draw text
		duRect rcText;
		nLeft += (m_nIconWidth + m_nIconTextSpace);
		rcText = rcItem;
		rcText.left = rcItem.left + nLeft;
		if (pItem->strText.length() > 0)
			DrawText32Bpp(hMemDC, pFont, m_clrText, pItem->strText.c_str(), pItem->strText.length(), &rcText, DT_LEFT|DT_VCENTER|DT_SINGLELINE, GetAlpha());

		if (nDrawHeight - nOffsetY > rcTreeCtrl.Height())
			break;

		nDrawHeight += m_nItemHeight;
		pItem = GetNextVisibleItem(pItem);
	}

	::AlphaBlend(hDC, 0, 0, rcTreeCtrl.Width(), rcTreeCtrl.Height(), hMemDC, 0, 0, rcTreeCtrl.Width(), rcTreeCtrl.Height(), bf);

	::SelectObject(hMemDC, hOldBitmap);
	::DeleteObject(hBmp);
	::DeleteDC(hMemDC);
}

void WINAPI duTreeCtrl::OnMouseMove(POINT pt)
{
	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	duRect rcTreeCtrl;
	Plugin_GetRect(this, &rcTreeCtrl);

	duRect rcItem;
	rcItem.SetRectEmpty();
	TreeCtrlItem *pItem = yHitTest(rcTreeCtrl, pt, rcItem);
	if (m_pHot != pItem)
	{
		m_pHot = pItem;
		Plugin_Redraw(this, TRUE);
		if(m_fShowTooltip)
		{
			pCtrlManager->DelToolTip();
			m_fShowTooltip = FALSE;
		}
	}
	
	if (pItem == NULL)
	{
		m_hitCode = tc_hit_empty;
		if(m_fShowTooltip)
		{
			pCtrlManager->DelToolTip();
			m_fShowTooltip = FALSE;
		}
	}
	else
	{
		POINT ptClient(pt);
		ptClient.x -= rcTreeCtrl.left;
		ptClient.y -= rcTreeCtrl.top;
		
		int nLeft = (pItem->nLevel - 1) * m_nIndentWidth;
		duRect rcIndent = CalcVCenterRect(rcItem, nLeft, m_nIndentWidth, m_nIndentHeight);
		if (ItemHasChildren(pItem) && (rcIndent.PtInRect(ptClient)))
			m_hitCode = tc_hit_indent;

		nLeft += (m_nIndentWidth + m_nIndentIconSpace);
		duRect rcIcon = CalcVCenterRect(rcItem, nLeft, m_nIconWidth, m_nIconHeight);
		if (rcIcon.PtInRect(ptClient))
			m_hitCode = tc_hit_icon;

		duRect rcText;
		nLeft += (m_nIconWidth + m_nIconTextSpace);
		rcText = rcItem;
		rcText.left = rcItem.left + nLeft;
		if (rcText.PtInRect(ptClient))
			m_hitCode = tc_hit_text;
	}
}

void WINAPI duTreeCtrl::OnMouseLeave(POINT pt)
{
	if (m_pHot)
	{
		m_pHot = NULL;
		Plugin_Redraw(this, TRUE);
	}
	
	m_hitCode = tc_hit_empty;
}

void WINAPI duTreeCtrl::OnMouseHover(POINT pt)
{
	if(m_pHot)
	{
		NotifyUser(DUM_TREEITEMHOVER, (WPARAM)m_pHot, (LPARAM)m_hitCode);
		if(m_fAutoShowToolTip)
		{
			if(!m_fShowTooltip)
			{
				LPTSTR strText = new TCHAR[m_pHot->strText.size() + 1];
				_tcscpy(strText, m_pHot->strText.c_str());
				duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
				pCtrlManager->AddToolTip(strText);
				m_fShowTooltip = TRUE;
			}
		}	
	}
}

void WINAPI duTreeCtrl::OnMouseLDown(POINT pt)
{
	BOOL fRedraw = FALSE;

	if ((m_pHot) && (m_hitCode == tc_hit_indent))
	{
		m_pHot->fExpand = !m_pHot->fExpand;
		fRedraw = TRUE;
		UpdateScroll();
	}

	if (m_pSelect != m_pHot)
	{
		m_pSelect = m_pHot;
		fRedraw = TRUE;
	}

	if (m_pSelect)
		NotifyUser(DUM_TREEITEMCLICK, (WPARAM)m_pSelect, (LPARAM)m_hitCode);

	if (fRedraw)
		Plugin_Redraw(this, TRUE);
}

void WINAPI duTreeCtrl::OnMouseLUp(POINT pt)
{
}

void WINAPI duTreeCtrl::OnMouseRDown(POINT pt)
{
	BOOL fRedraw = FALSE;
	if ((m_pHot) && (m_hitCode == tc_hit_indent))
	{
		m_pHot->fExpand = !m_pHot->fExpand;
		fRedraw = TRUE;
		UpdateScroll();
	}

	if (m_pSelect != m_pHot)
	{
		m_pSelect = m_pHot;
		fRedraw = TRUE;
	}

	if (m_pSelect)
		NotifyUser(DUM_TREEITEMRBUTTONCLICK, (WPARAM)m_pSelect, (LPARAM)m_hitCode);

	if (fRedraw)
		Plugin_Redraw(this, TRUE);
}

void WINAPI duTreeCtrl::OnMouseRUp(POINT pt)
{
}

void WINAPI duTreeCtrl::OnMouseDbClick(POINT pt)
{
	BOOL fRedraw = FALSE;

	if (m_pSelect != m_pHot)
	{
		m_pSelect = m_pHot;
		fRedraw = TRUE;
	}

	if (m_pSelect && ItemHasChildren(m_pSelect))
	{
		Expand(m_pSelect, !m_pSelect->fExpand);
		fRedraw = TRUE;
	}

	if (m_pSelect)
		NotifyUser(DUM_TREEITEMDBCLICK, (WPARAM)m_pSelect, (LPARAM)NULL);

	if (fRedraw)
		Plugin_Redraw(this, TRUE);
}

void WINAPI duTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

void WINAPI duTreeCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

void WINAPI duTreeCtrl::GetViewSize(LPSIZE lpSize)
{
	if (lpSize == NULL)
		return;

	lpSize->cx = lpSize->cy = 0;

	int nMaxWidth = 0;
	int nTotalHeight = 0;
	TreeCtrlItem *pItem = m_pRoot->pFirstChild;
	while (pItem)
	{
		int nItemWidth = CalcItemWidth(pItem);
		nMaxWidth = max(nMaxWidth, nItemWidth);
		nTotalHeight += m_nItemHeight;
		pItem = GetNextVisibleItem(pItem);
	}
	
	lpSize->cx = nMaxWidth;
	lpSize->cy = nTotalHeight;
}

TreeCtrlItem *WINAPI duTreeCtrl::InsertItem(LPCTSTR lpszText, LPCTSTR lpszImage, TreeCtrlItem *pParent, TreeCtrlItem *pInsertAfter)
{
	TreeCtrlItem *pNewItem = new TreeCtrlItem;
	pNewItem->strText = lpszText;
	pNewItem->strImage = lpszImage;
	pNewItem->fExpand = TRUE;
	pNewItem->nTextWidth = 0;
	pNewItem->lParam = NULL;	
	pNewItem->pParent = NULL;
	pNewItem->pNextSibling = NULL;
	pNewItem->pFirstChild = NULL;
	
	if ((pParent == NULL) || (pParent == TCVI_ROOT))
		pParent = m_pRoot;
	else if (!m_pRoot->pFirstChild)
	{
		delete pNewItem;
		return NULL;
	}

	if ((pInsertAfter == TCVI_FIRST) || !pParent->pFirstChild)
	{
		pNewItem->pNextSibling = pParent->pFirstChild;
		pParent->pFirstChild = pNewItem;
	}
	else
	{
		TreeCtrlItem *pItem = NULL;
		if (pInsertAfter == TCVI_LAST)
		{
			for (pItem = pParent->pFirstChild; pItem->pNextSibling; pItem = pItem->pNextSibling)
				;
		}
		else
		{
			for (pItem = pParent->pFirstChild; pItem->pNextSibling; pItem = pItem->pNextSibling)
				if (pItem == pInsertAfter)
					break;
		}

		pNewItem->pNextSibling = pItem->pNextSibling;
		pItem->pNextSibling = pNewItem;
	}

	pNewItem->pParent = pParent;
	pNewItem->nLevel = pParent->nLevel + 1;
	m_nItems++;

	UpdateScroll();
	return pNewItem;
}

BOOL WINAPI duTreeCtrl::DeleteItem(TreeCtrlItem *pItem)
{
	if (pItem == TCVI_ROOT || !pItem)
		pItem = m_pRoot;

	if (pItem != m_pRoot)
	{
		TreeCtrlItem *pPreItem = NULL;
		TreeCtrlItem *pParent = pItem->pParent;
		pPreItem = pParent->pFirstChild;
		for (; pPreItem->pNextSibling; pPreItem = pPreItem->pNextSibling)
			if (pPreItem->pNextSibling == pItem)
				break;

		if (pParent->pFirstChild == pItem)
			pParent->pFirstChild = pItem->pNextSibling;
		else
			pPreItem->pNextSibling = pItem->pNextSibling;

		DeleteItemT(pItem);
	}
	else
	{
		DeleteItemT(pItem);
		m_pRoot->pFirstChild = NULL;
	}

	UpdateScroll();
	return TRUE;
}

BOOL duTreeCtrl::DeleteItemT(TreeCtrlItem *pItem)
{
	if (pItem == NULL)
		return FALSE;

	TreeCtrlItem *pChildItem = pItem->pFirstChild;
	while (pChildItem)
	{
		TreeCtrlItem *pDelItem = pChildItem;
		pChildItem = pChildItem->pNextSibling;
		DeleteItemT(pDelItem);
	}

	if (pItem != m_pRoot)
	{
		if (pItem == m_pHot)
			m_pHot = NULL;
		if (pItem == m_pSelect)
			m_pSelect = NULL;
		delete pItem;

		m_nItems--;
	}

	return TRUE;
}

BOOL WINAPI duTreeCtrl::ItemHasChildren(TreeCtrlItem *pItem)
{
	if (pItem == NULL || pItem == TCVI_ROOT)
		pItem = m_pRoot;
	
	return pItem->pFirstChild ? TRUE : FALSE;
}

TreeCtrlItem *duTreeCtrl::GetNextVisibleItem(TreeCtrlItem *pItem)
{
	if (pItem->pFirstChild && pItem->fExpand)
		return pItem->pFirstChild;

checkNext:
	if (pItem->pNextSibling)
		return pItem->pNextSibling;

	pItem = pItem->pParent;
	if (pItem)
		goto checkNext;

	return NULL;
}

int duTreeCtrl::CalcItemWidth(TreeCtrlItem *pItem)
{
	if (pItem == NULL)
		return 0;

	HDC hDC = GetDC(NULL);
	HFONT hFont = (HFONT)GetStockObject(SYSTEM_FONT);
	duFont *pFont = (duFont *)GetResObj(m_szFont, DU_RES_FONT);
	if (pFont)
		hFont = pFont->GetHFont();
	
	HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);
	int nItemWidth = (pItem->nLevel * m_nIndentWidth) + m_nIndentIconSpace + m_nIconTextSpace + m_nIconWidth;
	if (pItem->strText.c_str() > 0 && !pItem->nTextWidth)
	{
		duRect rcText;
		rcText.SetRectEmpty();
		::DrawText(hDC, pItem->strText.c_str(), pItem->strText.length(), &rcText, DT_TOP|DT_LEFT|DT_SINGLELINE|DT_CALCRECT);
		
		pItem->nTextWidth = rcText.Width();
	}
	nItemWidth += pItem->nTextWidth;
	::SelectObject(hDC, hOldFont);
	ReleaseDC(NULL, hDC);
	return nItemWidth;
}

TreeCtrlItem *duTreeCtrl::GetFirstVisibleItem(int &nOffset)
{
	nOffset = 0;
	if (m_nItems == 0)
		return m_pRoot;
	
	POINT ptScroll = GetViewPoint();
	int nCalcHeight = 0;
	TreeCtrlItem *pTreeItem = GetNextVisibleItem(m_pRoot);
	while (pTreeItem)
	{
		nCalcHeight += m_nItemHeight;
		if (nCalcHeight > ptScroll.y)
		{
			nOffset = (ptScroll.y + m_nItemHeight) - nCalcHeight;
			return pTreeItem;
		}

		pTreeItem = GetNextVisibleItem(pTreeItem);
	}

	return NULL;
}

duRect duTreeCtrl::CalcVCenterRect(duRect &rcItemRect, int nLeft, int nWidth, int nHeight)
{
	duRect rcVCenter;
	
	rcVCenter.left   = rcItemRect.left + nLeft;
	rcVCenter.right  = rcVCenter.left + nWidth;
	rcVCenter.top    = rcItemRect.top + (rcItemRect.Height() - nHeight) / 2;
	rcVCenter.bottom = rcVCenter.top + nHeight;
	
	return rcVCenter;
}

TreeCtrlItem *duTreeCtrl::yHitTest(const duRect &rcTreeCtrl, const POINT pt, duRect &rcItem)
{
	POINT ptClient(pt);
	ptClient.x -= rcTreeCtrl.left;
	ptClient.y -= rcTreeCtrl.top;
	
	int nOffsetY = 0;
	TreeCtrlItem *pFirstVisible = GetFirstVisibleItem(nOffsetY);
	if (pFirstVisible == NULL || pFirstVisible == m_pRoot)
		return NULL;
	
	POINT ptView = GetViewPoint();
	duSize sizeView;
	GetViewSize(&sizeView);
	int nDrawHeight = -nOffsetY;
	TreeCtrlItem *pItem = pFirstVisible;
	while (pItem)
	{
		rcItem.SetRectEmpty();
		rcItem.left = -ptView.x;
		rcItem.right = rcItem.left + sizeView.cx;
		rcItem.top = nDrawHeight;
		rcItem.bottom = rcItem.top + m_nItemHeight;

		if (rcItem.PtInRect(ptClient))
			return pItem;

		if (nDrawHeight - nOffsetY > rcTreeCtrl.Height())
			break;

		nDrawHeight += m_nItemHeight;
		pItem = GetNextVisibleItem(pItem);
	}
	
	return NULL;
}

void WINAPI duTreeCtrl::SetItemStyle(LPCTSTR lpszStyle)
{
	if (lpszStyle)
		_tcsncpy(m_szItemStyle, lpszStyle, MAX_NAME);
	else
		ZeroMemory(m_szItemStyle, MAX_NAME * sizeof(TCHAR));
}

void WINAPI duTreeCtrl::SetIndentStyle(LPCTSTR lpszStyle)
{
	if (lpszStyle)
		_tcsncpy(m_szIndentStyle, lpszStyle, MAX_NAME);
	else
		ZeroMemory(m_szIndentStyle, MAX_NAME * sizeof(TCHAR));
}

void WINAPI duTreeCtrl::SetFont(LPCTSTR lpszFont)
{
	if (lpszFont)
	{
		if (!GetResObj(lpszFont, DU_RES_FONT))
			return;
		
		_tcsncpy(m_szFont, lpszFont, MAX_NAME);
		TreeCtrlItem *pItem = m_pRoot;
		while (pItem)
		{
			pItem->nTextWidth = 0;
			pItem = GetNextItem(pItem);
		}
	
		UpdateScroll();
		Plugin_Redraw(this, TRUE);
	}
}

void WINAPI duTreeCtrl::SetItemHeight(int nItemHeight)
{
	if (nItemHeight > 0)
	{
		m_nItemHeight = nItemHeight;
		UpdateScroll();
		Plugin_Redraw(this, TRUE);
	}
}

TreeCtrlItem *WINAPI duTreeCtrl::GetNextItem(TreeCtrlItem *pItem)
{
	if (pItem->pFirstChild)
		return pItem->pFirstChild;

checkNext:
	if (pItem->pNextSibling)
		return pItem->pNextSibling;

	pItem = pItem->pParent;
	if (pItem)
		goto checkNext;

	return NULL;
}

LPCTSTR WINAPI duTreeCtrl::GetItemText(TreeCtrlItem *pItem)
{
	if (pItem == NULL)
		return NULL;
	
	return pItem->strText.c_str();
}

void WINAPI duTreeCtrl::SetItemText(TreeCtrlItem *pItem, LPCTSTR lpszText)
{
	if (pItem == NULL)
		return;
	
	pItem->strText = lpszText;
}

LPCTSTR WINAPI duTreeCtrl::GetItemIcon(TreeCtrlItem *pItem)
{
	if (pItem == NULL)
		return NULL;
	
	return pItem->strImage.c_str();
}

void WINAPI duTreeCtrl::SetItemIcon(TreeCtrlItem *pItem, LPCTSTR lpszIcon)
{
	if (pItem == NULL)
		return;
	
	pItem->strImage = lpszIcon;
}

int WINAPI duTreeCtrl::GetItemLevel(TreeCtrlItem *pItem)
{
	if (pItem == NULL)
		return -1;
	
	return pItem->nLevel;
}

BOOL WINAPI duTreeCtrl::IsExpand(TreeCtrlItem *pItem)
{
	if (pItem == NULL)
		return FALSE;
	
	return pItem->fExpand;
}

void WINAPI duTreeCtrl::Expand(TreeCtrlItem *pItem, BOOL fExpand)
{
	if (pItem == NULL || pItem->fExpand == fExpand)
		return;
	
	pItem->fExpand = fExpand;
	UpdateScroll();
}

TreeCtrlItem *WINAPI duTreeCtrl::GetParent(TreeCtrlItem *pItem)
{
	if (pItem == NULL)
		return NULL;
	
	return pItem->pParent;
}

TreeCtrlItem *WINAPI duTreeCtrl::GetNextSibling(TreeCtrlItem *pItem)
{
	if (pItem == NULL)
		return NULL;
	
	return pItem->pNextSibling;
}

TreeCtrlItem *WINAPI duTreeCtrl::GetFirstChild(TreeCtrlItem *pItem)
{
	if (pItem == NULL)
		return NULL;
	
	return pItem->pFirstChild;
}

void WINAPI duTreeCtrl::DeleteAllItems()
{
	DeleteItem(m_pRoot);
}

INT_PTR WINAPI duTreeCtrl::GetItemData(TreeCtrlItem *pItem)
{
	if(pItem)
		return pItem->lParam;
	return NULL;
}

BOOL WINAPI duTreeCtrl::SetItemData(TreeCtrlItem *pItem, INT_PTR pData)
{
	if(pItem)
	{
		pItem->lParam = pData;
		return TRUE;
	}
	return FALSE;
}

void WINAPI duTreeCtrl::SetAutoShowTooltip(BOOL fAutoShowTooltip)
{
	m_fAutoShowToolTip = fAutoShowTooltip;
	if(m_fAutoShowToolTip)
	{
		if(m_fShowTooltip)
		{
			duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
			pCtrlManager->DelToolTip();
			m_fShowTooltip = FALSE;
		}
	}
}

TreeCtrlItem * WINAPI duTreeCtrl::HitTest(LPPOINT lppt)
{
	duRect rcTreeCtrl;
	Plugin_GetRect(this, &rcTreeCtrl);

	duRect rcItem;
	rcItem.SetRectEmpty();
	POINT pt;
	pt.x = lppt->x;
	pt.y = lppt->y;
	TreeCtrlItem *pItem = yHitTest(rcTreeCtrl, pt, rcItem);
	return pItem;
}