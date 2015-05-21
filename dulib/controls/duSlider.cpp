//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duSlider.cpp
//
//  History:    Dec-29-2009   Steven Li  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duSlider.h"


duSlider::duSlider() :
	 m_nMaxPos(0)
	,m_nPos(0)
	,m_fVert(FALSE)
	,m_nThumbWidth(0)
	,m_nThumbHeight(0)
{
	ZeroMemory(m_szForeStyle, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szThumbStyle, MAX_NAME * sizeof(TCHAR));

	m_fMaxPos = 0.0;
	m_fPos = 0.0;
	m_uThumbState = DU_STATE_NORMAL;
	m_fTracking = FALSE;
	m_rcFore.SetRectEmpty();
	m_rcThumb.SetRectEmpty();
}

duSlider::~duSlider()
{
}

void WINAPI duSlider::RegisterControlProperty()
{
	RegisterProperty(_T("maxpos"),        DU_PROPERTY_LONG,       &m_nMaxPos);
	RegisterProperty(_T("pos"),           DU_PROPERTY_LONG,       &m_nPos);
	RegisterProperty(_T("thumbwidth"),    DU_PROPERTY_LONG,       &m_nThumbWidth);
	RegisterProperty(_T("thumbheight"),   DU_PROPERTY_LONG,       &m_nThumbHeight);
	RegisterProperty(_T("thumbstyle"),    DU_PROPERTY_STYLEGROUP, &m_szThumbStyle);
	RegisterProperty(_T("forestyle"),     DU_PROPERTY_STYLEGROUP, &m_szForeStyle);
	RegisterProperty(_T("vert"),          DU_PROPERTY_BOOL,       &m_fVert);
}

void WINAPI duSlider::OnCreate()
{
	duRect rectSlider;
	Plugin_GetRect(this, &rectSlider);
	rectSlider.OffsetRect(-rectSlider.left, -rectSlider.top);

	m_fMaxPos = m_fVert ? (float)rectSlider.Height() : (float)rectSlider.Width();
	if (m_nMaxPos != 0) 
		m_fPos = m_fMaxPos * (m_nPos/(float)m_nMaxPos);

	calcRect(rectSlider);
}

void WINAPI duSlider::DrawObject(HDC hDC)
{
	duRect rectSlider;
	Plugin_GetRect(this, &rectSlider);
	rectSlider.OffsetRect(-rectSlider.left, -rectSlider.top);
	DrawByStyle(this, GetStyle(), hDC, &rectSlider, GetState(), NULL, GetAlpha());
	
	DrawByStyle(this, m_szForeStyle, hDC, &m_rcFore, GetState(), NULL, GetAlpha());
	DrawByStyle(this, m_szThumbStyle, hDC, &m_rcThumb, m_uThumbState, NULL, GetAlpha());
}

void WINAPI duSlider::OnMouseLeave(POINT pt)
{
	m_uThumbState = DU_STATE_NORMAL;
	Plugin_Redraw(this, TRUE);
}

void WINAPI duSlider::OnMouseLDown(POINT pt)
{
	duRect rectSlider;
	Plugin_GetRect(this, &rectSlider);

	if (rectSlider.PtInRect(pt) && !m_fTracking)
	{
		POINT pointClient(pt);
		pointClient.x += -rectSlider.left;
		pointClient.y += -rectSlider.top;

		if (m_rcThumb.PtInRect(pointClient))
		{
			m_uThumbState = DU_STATE_PRESS;
			m_fTracking = TRUE;

			Plugin_SetCapture(this);
		}

		calcRectByPt(pt, rectSlider);
		Plugin_Redraw(this, TRUE);
	}
}

void WINAPI duSlider::OnMouseLUp(POINT pt)
{
	if (m_fTracking)
	{
		Plugin_ReleaseCapture(this);
		m_fTracking = FALSE;
		m_uThumbState = DU_STATE_NORMAL;
		Plugin_Redraw(this, TRUE);
	}
}

void WINAPI duSlider::OnMouseMove(POINT pt)
{
	duRect rectSlider;
	Plugin_GetRect(this, &rectSlider);

	POINT pointClient;
	pointClient.x = pt.x;
	pointClient.y = pt.y;

	if (rectSlider.PtInRect(pt) && !m_fTracking) 
	{
		pointClient.x -= rectSlider.left;
		pointClient.y -= rectSlider.top;

		if (m_rcThumb.PtInRect(pointClient))
		{
			if (m_uThumbState == DU_STATE_NORMAL)
			{
				m_uThumbState = DU_STATE_OVER;
				Plugin_Redraw(this, TRUE);
			}
		}
		else
		{
			if (m_uThumbState != DU_STATE_NORMAL)
			{
				m_uThumbState = DU_STATE_NORMAL;
				Plugin_Redraw(this, TRUE);
			}
		}
	}

	if (m_fTracking)
	{
		calcRectByPt(pt, rectSlider);
		Plugin_Redraw(this, TRUE);
	}
}

void WINAPI duSlider::Resize(LPRECT lpRect)
{
	duPlugin::Resize(lpRect);

	duRect rectSlider;
	Plugin_GetRect(this, &rectSlider);
	rectSlider.OffsetRect(-rectSlider.left, -rectSlider.top);

	//计算最大像素值
	m_fMaxPos = m_fVert ? (float)rectSlider.Height() : (float)rectSlider.Width();
    //计算当前像素值
	m_fPos = m_fMaxPos * m_nPos / m_nMaxPos;

	SetPos(GetPos());
	calcRect(rectSlider);
}

void duSlider::calcRect(duRect &rectSlider)
{
	m_rcFore = rectSlider;
	if (m_fVert)
		m_rcFore.bottom = (long)(m_rcFore.top + rectSlider.Height() * (float)m_fPos / m_fMaxPos);
	else
		m_rcFore.right = (long)(m_rcFore.left + rectSlider.Width() * (float)m_fPos / m_fMaxPos);

	calcThumbRect(rectSlider);
}

void duSlider::calcThumbRect(duRect &rectSlider)
{
	if (m_nThumbWidth <= 0 || m_nThumbHeight <= 0)
		return;

	POINT ptCenter = m_rcFore.CenterPoint();
	m_rcThumb = m_rcFore;
	if (m_fVert)
	{
		m_rcThumb.top = max(m_rcFore.bottom - m_nThumbHeight/2, rectSlider.top);
		m_rcThumb.bottom = m_rcThumb.top + m_nThumbHeight;
		if (m_rcThumb.bottom > rectSlider.bottom)
		{
			m_rcThumb.bottom = rectSlider.bottom;
			m_rcThumb.top = m_rcThumb.bottom - m_nThumbHeight;
		}

		m_rcThumb.left = ptCenter.x - m_nThumbWidth/2;
		m_rcThumb.right = m_rcThumb.left + m_nThumbWidth;
	}
	else
	{
		m_rcThumb.left = max(m_rcFore.right - m_nThumbWidth/2, rectSlider.left);
		m_rcThumb.right = m_rcThumb.left + m_nThumbWidth;
		if (m_rcThumb.right > rectSlider.right)
		{
			m_rcThumb.right = rectSlider.right;
			m_rcThumb.left = m_rcThumb.right - m_nThumbWidth;
		}

		m_rcThumb.top = ptCenter.y - m_nThumbHeight/2;
		m_rcThumb.bottom = m_rcThumb.top + m_nThumbHeight;
	}
}

void duSlider::calcRectByPt(POINT &pt, duRect &rectSlider)
{
	if (m_fVert)
	{
		m_rcFore.bottom = max(rectSlider.top, min(pt.y, rectSlider.bottom)) - rectSlider.top;
		m_fPos = max(0, min(pt.y - rectSlider.top, m_fMaxPos));
	}
	else
	{
		m_rcFore.right = max(rectSlider.left, min(pt.x, rectSlider.right)) - rectSlider.left;
		m_fPos = max(0, min(pt.x - rectSlider.left, m_fMaxPos));
	}
	calcThumbRect(rectSlider);

	m_nPos = GetPos();
	SetPos(m_nPos);
	NotifyUser(DUM_SLIDERCHANGED, m_nPos, m_nMaxPos);
}

int WINAPI duSlider::GetPos()
{
	if (m_fMaxPos == 0)
		return 0;

	return (int)((m_fPos / m_fMaxPos * m_nMaxPos) + 0.5);
}

void WINAPI duSlider::SetPos(int nPos)
{
	if (nPos > m_nMaxPos || m_nMaxPos == 0)
		return;

	m_fPos = nPos/ (float)m_nMaxPos * m_fMaxPos;

	duRect rectSlider;
	Plugin_GetRect(this, &rectSlider);
	rectSlider.OffsetRect(-rectSlider.left, -rectSlider.top);
	calcRect(rectSlider);
}

int WINAPI duSlider::GetMaxPos()
{
	return m_nMaxPos;
}

void WINAPI duSlider::SetMaxPos(int nMaxPos)
{
	m_nMaxPos = nMaxPos;

	duRect rectSlider;
	Plugin_GetRect(this, &rectSlider);
	rectSlider.OffsetRect(-rectSlider.left, -rectSlider.top);
	calcRect(rectSlider);
}

int WINAPI duSlider::GetThumbWidth()
{
	return m_nThumbWidth;
}

void WINAPI duSlider::SetThumbWidth(int nWidth)
{
	m_nThumbWidth = nWidth;

	duRect rectSlider;
	Plugin_GetRect(this, &rectSlider);
	rectSlider.OffsetRect(-rectSlider.left, -rectSlider.top);
	calcRect(rectSlider);
}

int WINAPI duSlider::GetThumbHeight()
{
	return m_nThumbHeight;
}

void WINAPI duSlider::SetThumbHeight(int nHeight)
{
	m_nThumbHeight = nHeight;

	duRect rectSlider;
	Plugin_GetRect(this, &rectSlider);
	rectSlider.OffsetRect(-rectSlider.left, -rectSlider.top);
	calcRect(rectSlider);
}

BOOL WINAPI duSlider::IsVert()
{
	return m_fVert;
}

void  WINAPI duSlider::SetVert(BOOL fVert)
{
	m_fVert = fVert;

	duRect rectSlider;
	Plugin_GetRect(this, &rectSlider);
	rectSlider.OffsetRect(-rectSlider.left, -rectSlider.top);
	calcRect(rectSlider);
}

LPCTSTR WINAPI duSlider::GetForeStyle()
{
	return m_szForeStyle;
}

void WINAPI duSlider::SetForeStyle(LPCTSTR lpszStyle)
{
	if (lpszStyle)
		_tcsncpy(m_szForeStyle, lpszStyle, MAX_NAME);
	else
		ZeroMemory(m_szForeStyle, MAX_NAME * sizeof(TCHAR));
}

LPCTSTR WINAPI duSlider::GetThumbStyle()
{
	return m_szThumbStyle;
}

void WINAPI duSlider::SetThumbStyle(LPCTSTR lpszStyle)
{
	if (lpszStyle)
		_tcsncpy(m_szThumbStyle, lpszStyle, MAX_NAME);
	else
		ZeroMemory(m_szThumbStyle, MAX_NAME * sizeof(TCHAR));
}
