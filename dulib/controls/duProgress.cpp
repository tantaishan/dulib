//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duProgress.cpp
//
//  History:    MAR-16-2010   Denny Chen  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duProgress.h"

duProgress::duProgress() :
	 m_flPos(0.000000)
	,m_nLeftBorder(0)
	,m_nRightBorder(0)
	,m_nTopBorder(0)
	,m_nBottomBorder(0)
{
	ZeroMemory(m_szProgressStyle, MAX_NAME * sizeof(TCHAR));
}

duProgress::~duProgress()
{
}

void WINAPI duProgress::RegisterControlProperty()
{
    RegisterProperty(_T("progresspos"), DU_PROPERTY_FLOAT, &m_flPos);
	RegisterProperty(_T("leftborder"), DU_PROPERTY_LONG, &m_nLeftBorder);
	RegisterProperty(_T("rightborder"), DU_PROPERTY_LONG, &m_nRightBorder);
    RegisterProperty(_T("topborder"), DU_PROPERTY_LONG, &m_nTopBorder);
	RegisterProperty(_T("bottomborder"), DU_PROPERTY_LONG, &m_nBottomBorder);
	RegisterProperty(_T("progressstyle"), DU_PROPERTY_STYLEGROUP, m_szProgressStyle);
}

void WINAPI duProgress::OnCreate()
{
}

void WINAPI duProgress::DrawObject(HDC hDC)
{
	duRect rcProgress;
	Plugin_GetRect(this, &rcProgress);
	rcProgress.OffsetRect(-rcProgress.left, -rcProgress.top);
	
	DrawByStyle(this, GetStyle(), hDC, &rcProgress, DU_STATE_NORMAL, NULL, GetAlpha());
	
	duRect rcInner = GetInnerRect(rcProgress, (float)m_flPos);
	DrawByStyle(this, m_szProgressStyle, hDC, &rcInner, DU_STATE_NORMAL, NULL, GetAlpha());
}

duRect duProgress::GetInnerRect(duRect &rect, float pos)
{
	int innerLeft = 0;
	int innerRight = 0;
	int innerTop = 0;
	int innerBottom = 0;

	if(pos > 100.0)
	{
		pos = 100.0;
		innerLeft = rect.left + m_nLeftBorder;
		innerRight = rect.right - m_nRightBorder;
		innerTop = rect.top + m_nTopBorder;
		innerBottom = rect.bottom - m_nBottomBorder;
	}
	else if(pos < 0.0)
	{
		pos = 0.0;
	}
	else
	{
		innerLeft = rect.left + m_nLeftBorder;
		innerRight = innerLeft + (int)(rect.Width() - m_nLeftBorder - m_nRightBorder) * (pos / 100.0);
	        
		innerTop = rect.top + m_nTopBorder;
		innerBottom = rect.bottom - m_nBottomBorder;

		if (pos != 0.0 && innerLeft == innerRight)
			innerRight = innerLeft + 1;
	}

	duRect innerRect = duRect(innerLeft,innerTop,innerRight,innerBottom);
	return innerRect;
}

void WINAPI duProgress::SetProgressStyle(LPCTSTR lpszStyle)
{
	if (lpszStyle)
		_tcsncpy(m_szProgressStyle, lpszStyle, MAX_NAME);
	else
		ZeroMemory(m_szProgressStyle, MAX_NAME * sizeof(TCHAR));
}
