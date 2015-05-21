//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duReportLine.cpp
//
//  History:    Mar-08-2011   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duReportLine.h"
#include "duReportView.h"

duReportLine::duReportLine() :
	 m_nHeight(0)
	,m_uState(DU_STATE_NORMAL)
	,m_pReportView(NULL)
	,m_fSelected(FALSE)
{
	ZeroMemory(m_szStyle, MAX_NAME * sizeof(TCHAR));
}

duReportLine::~duReportLine()
{

}

void WINAPI duReportLine::DrawLine(HDC hDC, LPRECT lprcLine, int nLine)
{
	duStyleGroup *pStyleGroup = (duStyleGroup *)m_pReportView->GetResObj(m_szStyle, DU_RES_STYLEGROUP);
	if (pStyleGroup)
		pStyleGroup->Draw(hDC, lprcLine, GetState(), NULL, m_pReportView->GetAlpha());
}

void duReportLine::SetStyle(LPCTSTR lpszStyle)
{	
	if (lpszStyle)
		_tcsncpy(m_szStyle, lpszStyle, MAX_NAME);
	else
		ZeroMemory(m_szStyle, MAX_NAME * sizeof(TCHAR));
}

UINT duReportLine::GetState()
{
	if (m_uState == DU_STATE_NORMAL && IsSelected())
		return DU_STATE_PRESS;
	else
		return m_uState;
}

void duReportLine::SetState(UINT uState)
{
	if (uState == DU_STATE_NORMAL && IsSelected())
		m_uState = DU_STATE_PRESS;
	else
		m_uState = uState;
}
