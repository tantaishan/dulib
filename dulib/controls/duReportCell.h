//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duReportCell.h
//
//  History:    Mar-08-2011   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once

#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"
#include "duReportCell.h"

class duReportView;

// duReportCell
// 
// 
class duReportCell
{
public:
	void SetReportView(duReportView *pReportView) { m_pReportView = pReportView; }
	void DrawCell(HDC hDC, LPRECT lprcCell, int nLine, int nCol);
	
	LPCTSTR GetText() { return m_strText.c_str(); }
	void SetText(LPCTSTR lpszText) { m_strText = lpszText; }
	
public:
	duReportCell();
	~duReportCell();
	
protected:
	duReportView *m_pReportView;
	dustring m_strText;
};
