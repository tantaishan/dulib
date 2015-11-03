//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duScrollView.cpp
//
//  History:    Mar-08-2011   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "duScrollView.h"

duScrollView::duScrollView(void) :
	 m_fHorz(FALSE)
	,m_fVert(FALSE)
	,m_nXOffset(0)
	,m_nYOffset(0)
{
	ZeroMemory(m_szVertScroll, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szHorzScroll, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szImage, MAX_NAME * sizeof(TCHAR));
	
}

duScrollView::~duScrollView(void)
{
}

void duScrollView::RegisterControlProperty()
{
	RegisterProperty(_T("vertscrollbar"),  DU_PROPERTY_STRING, m_szVertScroll);
	RegisterProperty(_T("horzscrollbar"),  DU_PROPERTY_STRING, m_szHorzScroll);
	RegisterProperty(_T("image"),  DU_PROPERTY_STRING, m_szImage);
}

void duScrollView::OnCreate()
{
	UpdateScroll();
	SetCached(TRUE);
}

void duScrollView::DrawObject(HDC hDC)
{
	POINT ptView = GetViewPoint();
	
	duRect rcClient = _GetClientRect();
	if (rcClient.IsRectEmpty())
		return;

	duRect rcScrollView;
	GetRect(&rcScrollView);
	
	HRGN hRgnClip = ::CreateRectRgn(rcScrollView.left + rcClient.left, rcScrollView.top + rcClient.top, rcScrollView.left + rcClient.right, rcScrollView.top + rcClient.bottom);
	::SelectClipRgn(hDC, hRgnClip);
	
	POINT ptOrg;
	::SetViewportOrgEx(hDC, -ptView.x, -ptView.y, &ptOrg);
	
	DrawView(hDC);
	
	::SetViewportOrgEx(hDC, ptOrg.x, ptOrg.y, &ptOrg);
	::SelectClipRgn(hDC, NULL);
	::DeleteObject(hRgnClip);
}

void duScrollView::Resize(LPRECT lpRect/*=NULL*/)
{
	duPlugin::Resize(lpRect);
	UpdateScroll();
}

void duScrollView::OnMouseLDown(POINT pt)
{
	duRect rcScrollView;
	rcScrollView.SetRectEmpty();
	GetRect(&rcScrollView);
	
	POINT ptView = GetViewPoint();
	pt.x += (ptView.x - rcScrollView.left);
	pt.y += (ptView.y - rcScrollView.top);
	OnViewMouseLDown(pt);
}

void duScrollView::OnMouseLUp(POINT pt)
{
	duRect rcScrollView;
	rcScrollView.SetRectEmpty();
	GetRect(&rcScrollView);
	
	POINT ptView = GetViewPoint();
	pt.x += (ptView.x - rcScrollView.left);
	pt.y += (ptView.y - rcScrollView.top);
	OnViewMouseLUp(pt);
}

void duScrollView::OnMouseRDown(POINT pt)
{
	duRect rcScrollView;
	rcScrollView.SetRectEmpty();
	GetRect(&rcScrollView);
	
	POINT ptView = GetViewPoint();
	pt.x += (ptView.x - rcScrollView.left);
	pt.y += (ptView.y - rcScrollView.top);
	OnViewMouseRDown(pt);
}

void duScrollView::OnMouseRUp(POINT pt)
{
	duRect rcScrollView;
	rcScrollView.SetRectEmpty();
	GetRect(&rcScrollView);
	
	POINT ptView = GetViewPoint();
	pt.x += (ptView.x - rcScrollView.left);
	pt.y += (ptView.y - rcScrollView.top);
	OnViewMouseRUp(pt);
}

void duScrollView::OnMouseHover(POINT pt)
{
	duRect rcScrollView;
	rcScrollView.SetRectEmpty();
	GetRect(&rcScrollView);
	
	POINT ptView = GetViewPoint();
	pt.x += (ptView.x - rcScrollView.left);
	pt.y += (ptView.y - rcScrollView.top);
	OnViewMouseHover(pt);
}

void duScrollView::OnMouseMove(POINT pt)
{
	duRect rcScrollView;
	rcScrollView.SetRectEmpty();
	GetRect(&rcScrollView);
	
	POINT ptView = GetViewPoint();
	pt.x += (ptView.x - rcScrollView.left);
	pt.y += (ptView.y - rcScrollView.top);
	OnViewMouseMove(pt);
}

void duScrollView::OnMouseDbClick(POINT pt)
{
	duRect rcScrollView;
	rcScrollView.SetRectEmpty();
	GetRect(&rcScrollView);
	
	POINT ptView = GetViewPoint();
	pt.x += (ptView.x - rcScrollView.left);
	pt.y += (ptView.y - rcScrollView.top);
	OnViewMouseDbClick(pt);
}

void duScrollView::OnMouseWheel(UINT fwKeys, int zDelta, POINT pt)
{
	duScroll *pVScroll = (duScroll *)GetPluginByName(m_szVertScroll);
	if (pVScroll == NULL)
		return;

	if (m_fVert)
		pVScroll->OnMouseWheel(fwKeys, zDelta, pt);
}

BOOL duScrollView::OnSetCursor()
{
	POINT pt;
	::GetCursorPos(&pt);
	
	duRect rcWindow;
	::GetWindowRect(m_hWnd, &rcWindow);
	
	pt.x -= rcWindow.left;
	pt.y -= rcWindow.top;
	
	duRect rcScrollView;
	rcScrollView.SetRectEmpty();
	GetRect(&rcScrollView);
	
	pt.x -= rcScrollView.left;
	pt.y -= rcScrollView.top;
	
	POINT ptView = GetViewPoint();
	pt.x += ptView.x;
	pt.y += ptView.y;
	
	if (OnViewSetCursor(pt))
		return TRUE;

	return duPlugin::OnSetCursor();
}

void duScrollView::OnVScroll(ScrollBarAction sbAction, int nPos)
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

void duScrollView::OnHScroll(ScrollBarAction sbAction, int nPos)
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
				m_nXOffset = nPos;
				Plugin_Redraw(this, TRUE);
			}
		}
		break;

	default:
		break;
	}
}

void duScrollView::OnChildShow(duPlugin *pChild, BOOL fVisible)
{
	if (!Plugin_IsValid(pChild))
		return;

	if (lstrcmpi(pChild->GetName(), m_szVertScroll) == 0)
	{
		_TRACE(_T("VertScrollBar Show[%d]\n"), fVisible);
	}
	else if (lstrcmpi(pChild->GetName(), m_szHorzScroll) == 0)
	{
		_TRACE(_T("HorzScrollBar Show[%d]\n"), fVisible);
	}
}

void duScrollView::GetViewSize(LPSIZE lpSize)
{
	if (lpSize == NULL)
		return;
	
	lpSize->cx = lpSize->cy = 0;
	duImage *pImage = (duImage *)GetResObj(m_szImage, DU_RES_IMAGE);
	lpSize->cx = pImage->GetWidth();
	lpSize->cy = pImage->GetHeight();
}

POINT duScrollView::GetViewPoint()
{
	POINT pt = {m_nXOffset, m_nYOffset};
	return pt;
}

void duScrollView::DrawView(HDC hDC)
{
	duImage *pImage = (duImage *)GetResObj(m_szImage, DU_RES_IMAGE);
	if (pImage == NULL)
		return;

	DrawNormal(hDC, 0, 0, pImage->GetWidth(), pImage->GetHeight(), pImage, 0, 0, GetAlpha());
}

BOOL duScrollView::UpdateScroll()
{	
	duScroll *pVScroll = (duScroll *)GetPluginByName(m_szVertScroll);
	duScroll *pHScroll = (duScroll *)GetPluginByName(m_szHorzScroll);

	if (pVScroll == NULL || pHScroll == NULL)
		return FALSE;

	duSize sizeView;
	GetViewSize(&sizeView);
	
	duRect rcScrollView;
	GetRect(&rcScrollView);
	duRect rcHScroll;
	pHScroll->GetRect(&rcHScroll);
	duRect rcVScroll;
	pVScroll->GetRect(&rcVScroll);

	if (rcScrollView.Width() >= sizeView.cx && rcScrollView.Height() >= sizeView.cy)
	{
		pHScroll->SetVisible(FALSE);
		m_fHorz = FALSE;
		pVScroll->SetVisible(FALSE);
		m_fVert = FALSE;

		pHScroll->SetPos(0);
		pVScroll->SetPos(0);
	}
	else if (rcScrollView.Width() < sizeView.cx && rcScrollView.Height() < sizeView.cy)
	{
		pHScroll->SetVisible(TRUE);
		m_fHorz = TRUE;
		pVScroll->SetVisible(TRUE);
		m_fVert = TRUE;
	}
	else if (rcScrollView.Width() >= sizeView.cx && rcScrollView.Height() < sizeView.cy)
	{
		pVScroll->SetVisible(TRUE);
		m_fVert = TRUE;

		if (sizeView.cx + rcVScroll.Width() > rcScrollView.Width())
		{
			pHScroll->SetVisible(TRUE);
			m_fHorz = TRUE;
		}
		else
		{
			pHScroll->SetVisible(FALSE);
			m_fHorz = FALSE;
			pHScroll->SetPos(0);
		}
	}
	else if (rcScrollView.Width() < sizeView.cx && rcScrollView.Height() >= sizeView.cy)
	{
		pHScroll->SetVisible(TRUE);
		m_fHorz = TRUE;
		
		if (sizeView.cy + rcHScroll.Height() > rcScrollView.Height())
		{
			pVScroll->SetVisible(TRUE);
			m_fVert = TRUE;
		}
		else
		{
			pVScroll->SetVisible(FALSE);
			m_fVert = FALSE;
			pVScroll->SetPos(0);
		}
	}
	
	int nClientHeight = rcScrollView.Height() - rcHScroll.Height();
	int nClientWidth = rcScrollView.Width() - rcVScroll.Width();
	int nVMaxPosValue = sizeView.cy - nClientHeight;
	int nHMaxPosValue = sizeView.cx - nClientWidth;
		
	if (m_fVert && m_fHorz)
	{
		pVScroll->SetMaxPos(sizeView.cy);
		pVScroll->SetPage(nClientHeight);
		
		if (pVScroll->GetPos() > nVMaxPosValue)
		{
			pVScroll->SetPos(nVMaxPosValue);
			//m_nYOffset = pVScroll->GetPos();
		}

		pVScroll->SetPosBottom(0);
		pVScroll->Resize(NULL);

		pHScroll->SetMaxPos(sizeView.cx);
		pHScroll->SetPage(nClientWidth);
		if (pHScroll->GetPos() > nHMaxPosValue)
		{
			pHScroll->SetPos(nHMaxPosValue);
			//m_nXOffset = pHScroll->GetPos();
		}
		
		pHScroll->SetPosRight(rcVScroll.Width());
		pHScroll->Resize(NULL);
	}
	else if (m_fVert)
	{
		pVScroll->SetMaxPos(sizeView.cy);
		pVScroll->SetPage(rcScrollView.Height());
		if (pVScroll->GetPos() > (sizeView.cy-rcScrollView.Height()))
		{
			pVScroll->SetPos(sizeView.cy-rcScrollView.Height());
			//m_nYOffset = pVScroll->GetPos();
		}

		pVScroll->SetPosBottom(0);
		pVScroll->Resize(NULL);
	}
	else if (m_fHorz)
	{
		pHScroll->SetMaxPos(sizeView.cx);
		pHScroll->SetPage(rcScrollView.Width());
		if (pHScroll->GetPos() > (sizeView.cx-rcScrollView.Width()))
		{
			pHScroll->SetPos(sizeView.cx-rcScrollView.Width());
			//m_nXOffset = pHScroll->GetPos();
		}

		pHScroll->SetPosRight(0);
		pHScroll->Resize(NULL);
	}

	m_nXOffset = pHScroll->GetPos();
	m_nYOffset = pVScroll->GetPos();

	return TRUE;
}

duRect duScrollView::_GetClientRect()
{
	duScroll *pVScroll = (duScroll *)GetPluginByName(m_szVertScroll);
	duScroll *pHScroll = (duScroll *)GetPluginByName(m_szHorzScroll);

	if (pVScroll == NULL || pHScroll == NULL)
		return duRect(0, 0, 0, 0);
		
	duRect rcScrollView;
	rcScrollView.SetRectEmpty();
	GetRect(&rcScrollView);
	rcScrollView.OffsetRect(-rcScrollView.left, -rcScrollView.top);
	
	duRect rcHScroll;
	rcHScroll.SetRectEmpty();
	pHScroll->GetRect(&rcHScroll);
	duRect rcVScroll;
	rcVScroll.SetRectEmpty();
	pVScroll->GetRect(&rcVScroll);
	
	if (m_fHorz)
		rcScrollView.bottom = max(0, rcScrollView.bottom - rcHScroll.Height());

	if (m_fVert)
		rcScrollView.right = max(0, rcScrollView.right - rcVScroll.Width());
		
	return rcScrollView;
}

void duScrollView::SetHorzScrollBar(LPCTSTR lpszScroll)
{
	if (lpszScroll)
		_tcsncpy(m_szHorzScroll, lpszScroll, MAX_NAME);
	else
		ZeroMemory(m_szHorzScroll, sizeof(TCHAR) * MAX_NAME);
}

void duScrollView::SetVertScrollBar(LPCTSTR lpszScroll)
{
	if (lpszScroll)
		_tcsncpy(m_szVertScroll, lpszScroll, MAX_NAME);
	else
		ZeroMemory(m_szVertScroll, sizeof(TCHAR) * MAX_NAME);
}
