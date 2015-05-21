//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duReportColumn.h
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

// duReportColumn
// 
// 
class duReportColumn
{	
public:
	void SetReportView(duReportView *pReportView) { m_pReportView = pReportView; }
	void Init();
	BOOL InsertLine(int nLine);
	BOOL DeleteLine(int nLine);
	duReportCell *GetCell(int nLine);
	
public:
	duReportColumn();
	~duReportColumn();
	
public:
	duReportView *m_pReportView;
	vector<duReportCell *> m_vtCell;
};
