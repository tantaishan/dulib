//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duReportCell.cpp
//
//  History:    Mar-08-2011   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duReportCell.h"
#include "duReportView.h"

duReportCell::duReportCell() :
	 m_pReportView(NULL)
	,m_strText(_T(""))
{
}

duReportCell::~duReportCell()
{
}

void duReportCell::DrawCell(HDC hDC, LPRECT lprcCell, int nLine, int nCol)
{
	duStyleGroup *pStyleGroup = (duStyleGroup *)m_pReportView->GetResObj(m_pReportView->GetCellStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
		pStyleGroup->Draw(hDC, lprcCell, DU_STATE_NORMAL, m_strText.c_str(), m_pReportView->GetAlpha());
}
