//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duScroll.cpp
//
//  History:    Dec-13-2009   Steven Li  Created
//
//--------------------------------------------------------------------------
#include "duScroll.h"

#define DEFAULT_MIN_THUMB_SIZE 10

duScroll::duScroll(void) :
	 m_nMaxPos(0)
	,m_nPos(0)
	,m_nPage(0)
	,m_nArrowHeight(0)
	,m_fVert(TRUE)
	,m_fTracking(FALSE)
	,m_uUpState(DU_STATE_NORMAL)
	,m_uDownState(DU_STATE_NORMAL)
	,m_uThumbState(DU_STATE_NORMAL)
	,m_nLastActiveRect(NO_WHERE)
	,m_nLimitPage(0)
	,m_ptTrack(0,0)
	,m_nOldPos(0)
	,m_nStep(1)
	,m_nTimerid(0)
	,m_nTrackPos(0)
	,m_nMinThumbSize(0)
{
	m_rectUp.SetRectEmpty();
	m_rectDown.SetRectEmpty();
	m_rectPage1.SetRectEmpty();
	m_rectPage2.SetRectEmpty();
	m_rectThumb.SetRectEmpty();
	
	ZeroMemory(m_szUpStyle, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szDownStyle, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szThumbStyle, MAX_NAME * sizeof(TCHAR));
}

duScroll::~duScroll(void)
{
}

//////////////////////////////////////////////////////////////////////////
void duScroll::RegisterControlProperty()
{
	RegisterProperty(_T("maxpos"), DU_PROPERTY_LONG, &m_nMaxPos);
	RegisterProperty(_T("pos"), DU_PROPERTY_LONG, &m_nPos);
	RegisterProperty(_T("page"), DU_PROPERTY_LONG, &m_nPage);
	RegisterProperty(_T("arrowheight"), DU_PROPERTY_LONG, &m_nArrowHeight);
	RegisterProperty(_T("upstyle"), DU_PROPERTY_STYLEGROUP, &m_szUpStyle);
	RegisterProperty(_T("downstyle"), DU_PROPERTY_STYLEGROUP, &m_szDownStyle);
	RegisterProperty(_T("thumbstyle"), DU_PROPERTY_STYLEGROUP, &m_szThumbStyle);
	RegisterProperty(_T("vert"), DU_PROPERTY_BOOL, &m_fVert);
	RegisterProperty(_T("step"), DU_PROPERTY_LONG, &m_nStep);
	RegisterProperty(_T("minthumbsize"), DU_PROPERTY_LONG, &m_nMinThumbSize);
}

void duScroll::OnCreate()
{
	m_nOldPos = m_nPos;
	RefreshScrollBar();
	m_nTimerid = GetUniTimer();

	if (m_nMinThumbSize == 0)
		m_nMinThumbSize = DEFAULT_MIN_THUMB_SIZE;
}

void duScroll::OnTimer(UINT nEventId)
{
	if (m_nTimerid == nEventId)
		_OnTimer();
}

void duScroll::OnMouseMove(POINT pt)
{
	_MouseMove(pt);
}

void duScroll::OnMouseLDown(POINT pt)
{
	_MouseDown(pt);
}

void duScroll::OnMouseLUp(POINT pt)
{
	_MouseUp(pt);
}

void duScroll::OnMouseLeave(POINT pt)
{
	Plugin_KillTimer(this, m_nTimerid);

	if (!m_fTracking)
	{
		m_uUpState = m_uDownState = m_uThumbState = DU_STATE_NORMAL;
		m_nLastActiveRect = NO_WHERE;

		Plugin_Redraw(this, TRUE);
	}
}

void duScroll::OnMouseWheel(UINT fwKeys, int zDelta, POINT pt)
{
	_MouseWheel(zDelta);
}

void duScroll::DrawObject(HDC hDC)
{	
	duRect rcScroll;
	rcScroll.SetRectEmpty();
	GetRect(&rcScroll);
	rcScroll.OffsetRect(-rcScroll.left, -rcScroll.top);
	DrawByStyle(this, GetStyle(), hDC, &rcScroll, GetState(), NULL, GetAlpha());
	
	duRect rcUp = m_rectUp;
	DrawByStyle(this, m_szUpStyle, hDC, &rcUp, m_uUpState, NULL, GetAlpha());
	
	duRect rcDown = m_rectDown;
	DrawByStyle(this, m_szDownStyle, hDC, &rcDown, m_uDownState, NULL, GetAlpha());
	
	duRect rcThumb = m_rectThumb;
	DrawByStyle(this, m_szThumbStyle, hDC, &rcThumb, m_uThumbState, NULL, GetAlpha());
}

void duScroll::Resize(LPRECT lpNewRect)
{
	duPlugin::Resize(lpNewRect);
	RefreshScrollBar();
}

void duScroll::RefreshScrollBar()
{
	duRect rcScroll;
	rcScroll.SetRectEmpty();
	GetRect(&rcScroll);
	rcScroll.OffsetRect(-rcScroll.left, -rcScroll.top);
	CalcRect(rcScroll);
	//Plugin_Redraw(this, TRUE);
}

//////////////////////////////////////////////////////////////////////////
void duScroll::CalcRect(duRect rcScroll)
{
	m_rectUp = m_rectDown = m_rectThumb = m_rectPage1 = m_rectPage2 = rcScroll;

	if (m_nMaxPos == 0)
	{
		if (m_fVert)
		{
			m_rectUp.bottom    = m_rectUp.top + m_nArrowHeight;
			m_rectDown.top     = m_rectDown.bottom - m_nArrowHeight;
		}
		else
		{
			m_rectUp.right    = m_rectUp.left + m_nArrowHeight;
			m_rectDown.left   = m_rectDown.right - m_nArrowHeight;
		}

		m_rectPage1.SetRectEmpty();
		m_rectPage2.SetRectEmpty();
		m_rectThumb.SetRectEmpty();
		return;
	}

	if (m_fVert)
	{
		float H = (float) (rcScroll.Height() - m_nArrowHeight * 2);
		float pageH = m_nPage / (float)(m_nMaxPos)  * H;
		int posY = (int)(m_nPos / (float) (m_nMaxPos - m_nPage) * (H - pageH));

		m_rectUp.bottom    = m_rectUp.top + m_nArrowHeight;
		m_rectDown.top     = m_rectDown.bottom - m_nArrowHeight;
		m_rectThumb.top    = m_rectUp.bottom + posY;
		m_rectThumb.bottom = m_rectThumb.top + (long)ceil(pageH);
		if (m_rectThumb.Height() < m_nMinThumbSize)
			m_rectThumb.bottom = m_rectThumb.top + m_nMinThumbSize;
		if (m_rectThumb.bottom > m_rectDown.top)
		{
			m_rectThumb.bottom = m_rectDown.top;
			m_rectThumb.top = m_rectThumb.bottom - m_nMinThumbSize;
		}

		m_rectPage1.top    = m_rectUp.bottom;
		m_rectPage1.bottom = m_rectThumb.top;
		m_rectPage2.top    = m_rectThumb.bottom;
		m_rectPage2.bottom = m_rectDown.top;
	}
	else
	{
		float H = (float) (rcScroll.Width() - m_nArrowHeight * 2);
		float pageH = m_nPage / (float)(m_nMaxPos)  * H;
		int posY = (int)(m_nPos / (float) (m_nMaxPos - m_nPage) * (H - pageH));

		m_rectUp.right    = m_rectUp.left + m_nArrowHeight;
		m_rectDown.left   = m_rectDown.right - m_nArrowHeight;
		m_rectThumb.left  = m_rectUp.right + posY;
		m_rectThumb.right = m_rectThumb.left + (long)ceil(pageH);
		if (m_rectThumb.Width() < m_nMinThumbSize)
			m_rectThumb.right = m_rectThumb.left + m_nMinThumbSize;
		if (m_rectThumb.right > m_rectDown.left)
		{
			m_rectThumb.right = m_rectDown.left;
			m_rectThumb.left = m_rectThumb.right - m_nMinThumbSize;
		}

		m_rectPage1.left  = m_rectUp.right;
		m_rectPage1.right = m_rectThumb.left;
		m_rectPage2.left  = m_rectThumb.right;
		m_rectPage2.right = m_rectDown.left;
	}

	if (m_nPage >= m_nMaxPos)
	{
		m_rectPage1.SetRectEmpty();
		m_rectPage2.SetRectEmpty();
		m_rectThumb.SetRectEmpty();
	}
}

int duScroll::PtToPage(POINT pt, duRect rcScroll)
{
	if (m_nPage == 0)
		return 0;

	if (m_fVert)
	{
		float H = (float)(rcScroll.Height() - m_nArrowHeight * 2);
		float pageH = m_nPage / (float)(m_nMaxPos)  * H;
		float pH = (float)(pt.y - m_rectUp.bottom);

		return (int)(pH / pageH);
	}
	else
	{
		float H = (float)(rcScroll.Width() - m_nArrowHeight * 2);
		float pageH = m_nPage / (float)(m_nMaxPos)  * H;
		float pH = (float)(pt.x - m_rectUp.right);

		return (int)(pH / pageH);
	}

	return 0;
}

void duScroll::CalcRectByPt(POINT pt, duRect rcScroll)
{
	if (m_fVert)
	{
		float H = (float)(rcScroll.Height() - m_nArrowHeight * 2);
		float pageH = m_nPage / (float)(m_nMaxPos)  * H;
		int posY = (int)(m_nTrackPos / (float) (m_nMaxPos - m_nPage) * (H - pageH));
		posY += (pt.y - m_ptTrack.y);

		m_nOldPos = m_nPos;
		m_nPos = (int) min(m_nMaxPos-m_nPage, max(0,posY * (float) (m_nMaxPos - m_nPage) / (H - pageH)));

		CalcRect(rcScroll);
		NotifyParent(DUMI_VSCROLL, SCROLL_TRACK, m_nPos);
	}
	else 
	{
		float H = (float)(rcScroll.Width() - m_nArrowHeight * 2);
		float pageH = m_nPage / (float)(m_nMaxPos)  * H;
		int posX = (int)(m_nTrackPos / (float) (m_nMaxPos - m_nPage) * (H - pageH));
		posX += (pt.x - m_ptTrack.x);

		m_nOldPos = m_nPos;
		m_nPos = (int) min(m_nMaxPos-m_nPage, max(0,posX * (float) (m_nMaxPos - m_nPage) / (H - pageH)));
		
		int nPos0 = m_nMaxPos-m_nPage;
		int nPos1 =  posX * (float) (m_nMaxPos - m_nPage) / (H - pageH);
	
		CalcRect(rcScroll);
		NotifyParent(DUMI_HSCROLL, SCROLL_TRACK, m_nPos);
	}
	Plugin_Redraw(this, TRUE);
}

ScrollBarHit duScroll::HitTest(POINT pt)
{
	ScrollBarHit sbHit = NO_WHERE;
	if (m_rectUp.PtInRect(pt)) 
		sbHit = UP_RECT;
	else if (m_rectPage1.PtInRect(pt)) 
		sbHit = PAGE1_RECT;
	else if (m_rectPage2.PtInRect(pt)) 
		sbHit = PAGE2_RECT;
	else if (m_rectThumb.PtInRect(pt)) 
		sbHit = THUMB_RECT;
	else if (m_rectDown.PtInRect(pt)) 
		sbHit =DOWN_RECT;

	return sbHit;
}

void duScroll::_MouseIn(ScrollBarHit sbHit)
{
	switch(sbHit)
	{
	case UP_RECT:
		m_uUpState = DU_STATE_OVER;
		break;
	case DOWN_RECT:
		m_uDownState = DU_STATE_OVER;
		break;
	case THUMB_RECT:
		m_uThumbState = DU_STATE_OVER;
		break;
	}
	
	Plugin_Redraw(this, TRUE);
}

void duScroll::_MouseMove(POINT pt)
{
	duRect rcScroll;
	rcScroll.SetRectEmpty();
	GetRect(&rcScroll);

	duRect rcScollClient = rcScroll;
	rcScollClient.OffsetRect(-rcScollClient.left, -rcScollClient.top);

	POINT ptClient(pt);
	ptClient.x -= rcScroll.left;
	ptClient.y -= rcScroll.top;

	if (rcScroll.PtInRect(pt) && !m_fTracking)
	{
		m_nLimitPage = PtToPage(ptClient, rcScollClient);

		ScrollBarHit sbhit = HitTest(ptClient);
		if (sbhit == m_nLastActiveRect)
			return;

		_MouseLeave(m_nLastActiveRect);
		_MouseIn(sbhit);
		m_nLastActiveRect = sbhit;
		Plugin_Redraw(this, TRUE);
	}

	if (m_fTracking)
		CalcRectByPt(ptClient, rcScollClient);
}

void duScroll::_MouseLeave(ScrollBarHit sbHit)
{
	switch(sbHit)
	{
	case UP_RECT:
		m_uUpState = DU_STATE_NORMAL;
		break;
	case DOWN_RECT:
		m_uDownState = DU_STATE_NORMAL;
		break;
	case THUMB_RECT:
		m_uThumbState = DU_STATE_NORMAL;
		break;
	}

	Plugin_KillTimer(this, m_nTimerid);
}

void duScroll::_MouseDown(POINT pt)
{
	if (m_nPage >= m_nMaxPos)
		return;

	duRect rcScroll;
	rcScroll.SetRectEmpty();
	GetRect(&rcScroll);
	
	duRect rcScrollClient;
	rcScrollClient = rcScroll;
	rcScrollClient.OffsetRect(-rcScrollClient.left, -rcScrollClient.top);

	POINT ptClient(pt);
	ptClient.x -= rcScroll.left;
	ptClient.y -= rcScroll.top;

	ScrollBarAction sbAction = SCROLL_NO_WHERE;
	
	if (rcScroll.PtInRect(pt) && !m_fTracking)
	{
		switch(m_nLastActiveRect)
		{
		case UP_RECT:
			{
				m_nOldPos = m_nPos;
				m_nPos = max(0, m_nPos-m_nStep);
				CalcRect(rcScrollClient);
				Plugin_SetTimer(this, m_nTimerid, 100);
				m_uUpState = DU_STATE_PRESS;
				sbAction = SCROLL_UP;
			}
			break;
		case DOWN_RECT:
			{
				m_nOldPos = m_nPos;
				m_nPos = min(m_nMaxPos-m_nPage, m_nPos+m_nStep);
				CalcRect(rcScrollClient);
				Plugin_SetTimer(this, m_nTimerid, 100);
				m_uDownState = DU_STATE_PRESS;
				sbAction = SCROLL_DOWN;
			}
			break;
		case THUMB_RECT:
			{
				m_uThumbState = DU_STATE_PRESS;
				m_fTracking = TRUE;
				m_ptTrack = ptClient;
				m_nTrackPos = m_nPos;
				Plugin_Redraw(this, TRUE);
				Plugin_SetCapture(this);
				CalcRect(rcScrollClient);
				//sbAction = SCROLL_TRACK;
			}
			break;
		case PAGE1_RECT:
			{
				m_nOldPos = m_nPos;
				m_nPos = max(m_nLimitPage * m_nPage, m_nPos-m_nPage);
				CalcRect(rcScrollClient);
				Plugin_SetTimer(this, m_nTimerid, 100);
				sbAction = SCROLL_PAGEUP;
			}
			break;
		case PAGE2_RECT:
			{
				m_nOldPos = m_nPos;
				m_nPos = min(m_nMaxPos-m_nPage, min(m_nLimitPage*m_nPage, m_nPos+m_nPage));
				CalcRect(rcScrollClient);
				Plugin_SetTimer(this, m_nTimerid, 100);
				sbAction = SCROLL_PAGEDOWN;
			}
			break;
		}

		if (sbAction == SCROLL_NO_WHERE)
			return;

		if (m_fVert)
			NotifyParent(DUMI_VSCROLL, sbAction, m_nPos);
		else
			NotifyParent(DUMI_HSCROLL, sbAction, m_nPos);
	}
}

void duScroll::_MouseUp(POINT pt)
{
	if (m_nPage >= m_nMaxPos)
		return;

	duRect rcScroll;
	rcScroll.SetRectEmpty();
	GetRect(&rcScroll);
	
	if (rcScroll.PtInRect(pt) && !m_fTracking) 
	{
		switch(m_nLastActiveRect)
		{
		case UP_RECT:
			m_uUpState = DU_STATE_OVER;
			break;
		case DOWN_RECT:
			m_uDownState = DU_STATE_OVER;
			break;
		case THUMB_RECT:		
			m_uThumbState = DU_STATE_OVER;
			break;
		}
		Plugin_Redraw(this, TRUE);
	}

	if (m_fTracking)
	{
		Plugin_ReleaseCapture(this);
		m_fTracking = FALSE;
		m_uThumbState = DU_STATE_NORMAL;
		m_nLastActiveRect = NO_WHERE;
		Plugin_Redraw(this, TRUE);
	}

	Plugin_KillTimer(this, m_nTimerid);
}

void duScroll::_MouseWheel(int nDelta)
{
	if (m_nPage >= m_nMaxPos)
		return;

	int nStep = abs(nDelta / WHEEL_DELTA);
	m_nOldPos = m_nPos;

	ScrollBarAction sbAction = SCROLL_NO_WHERE;

	if (nDelta > 0)
	{
		sbAction = SCROLL_WHEEL_UP;
		m_nPos = max(0, m_nPos-(m_nStep * nStep));
	}
	else
	{
		sbAction = SCROLL_WHEEL_DOWN;
		m_nPos = min(m_nMaxPos-m_nPage, m_nPos+(m_nStep * nStep));
	}

	RefreshScrollBar();

	if (m_fVert)
	{
		NotifyParent(DUMI_VSCROLL, sbAction, m_nPos);
		NotifyUser(DUM_VSCROLL, sbAction, m_nPos);
	}
	else
	{
		NotifyParent(DUMI_HSCROLL, sbAction, m_nPos);
		NotifyUser(DUM_HSCROLL, sbAction, m_nPos);
	}
}

void duScroll::_OnTimer()
{
	if (m_nPage >= m_nMaxPos)
		return;

	ScrollBarAction sbAction = SCROLL_NO_WHERE;

	switch(m_nLastActiveRect)
	{
	case UP_RECT:
		{
			m_nOldPos = m_nPos;
			m_nPos = max(0, m_nPos-m_nStep);
			sbAction = SCROLL_UP;
		}
		break;

	case DOWN_RECT:
		{
			m_nOldPos = m_nPos;
			m_nPos = min(m_nMaxPos-m_nPage, m_nPos+m_nStep);
			sbAction = SCROLL_DOWN;
		}
		break;

	case PAGE1_RECT:
		{
			m_nOldPos = m_nPos;
			m_nPos = max(m_nLimitPage * m_nPage, m_nPos-m_nPage);
			sbAction = SCROLL_PAGEUP;
		}
		break;

	case PAGE2_RECT:
		{
			m_nOldPos = m_nPos;
			m_nPos = min(m_nMaxPos-m_nPage, min(m_nLimitPage *m_nPage, m_nPos+m_nPage));
			sbAction = SCROLL_PAGEDOWN;
		}
		break;
		
	default:
		break;
	}
	
	RefreshScrollBar();

	if (m_fVert)
		NotifyParent(DUMI_VSCROLL, sbAction, m_nPos);
	else
		NotifyParent(DUMI_HSCROLL, sbAction, m_nPos);
}

void duScroll::NotifyParent(UINT uMsg, ScrollBarAction sbAction, int nPos)
{
	duPlugin *pParent = GetParent();
	if (!Plugin_IsValid(pParent))
		return;
	
	if (uMsg == DUMI_VSCROLL)
		pParent->OnVScroll(sbAction, nPos);
	else
		pParent->OnHScroll(sbAction, nPos);
}

void duScroll::SetMaxPos(int nMaxPos)
{
	if (nMaxPos < 0)
		return;

	m_nMaxPos = nMaxPos;
	RefreshScrollBar();
}

void duScroll::SetPos(int nPos)
{
	if (nPos < 0)
		return;

	m_nPos = nPos;
	RefreshScrollBar();
}

void duScroll::SetPage(int nPage)
{
	if (nPage < 0)
		return;

	m_nPage = nPage;
	RefreshScrollBar();
}

void duScroll::SetArrowHeight(int nArrowHeight)
{
	if (nArrowHeight < 0)
		return;

	m_nArrowHeight = nArrowHeight;
	RefreshScrollBar();
}

void duScroll::SetVert(BOOL bVert)
{
	m_fVert = bVert;
	RefreshScrollBar();
}

void duScroll::SetStep(int nStep)
{
	if (nStep < 0)
		return;

	m_nStep = nStep;
}

void duScroll::SetUpStyle(LPCTSTR lpszStyle)
{
	if (lpszStyle)
		_tcsncpy(m_szUpStyle, lpszStyle, MAX_NAME);
	else
		ZeroMemory(m_szUpStyle, MAX_NAME * sizeof(TCHAR));
}

void duScroll::SetDownStyle(LPCTSTR lpszStyle)
{
	if (lpszStyle)
		_tcsncpy(m_szDownStyle, lpszStyle, MAX_NAME);
	else
		ZeroMemory(m_szDownStyle, MAX_NAME * sizeof(TCHAR));
}

void duScroll::SetThumbStyle(LPCTSTR lpszStyle)
{
	if (lpszStyle)
		_tcsncpy(m_szThumbStyle, lpszStyle, MAX_NAME);
	else
		ZeroMemory(m_szThumbStyle, MAX_NAME * sizeof(TCHAR));
}
