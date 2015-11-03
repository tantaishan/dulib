//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duReportColumn.cpp
//
//  History:    Mar-08-2011   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "duReportColumn.h"
#include "duReportView.h"

duReportColumn::duReportColumn() :
	m_pReportView(NULL)
{
	m_vtCell.clear();
}

duReportColumn::~duReportColumn()
{
	int i;
	for (i = 0;i < (int)m_vtCell.size(); i++)
	{
		duReportCell *pCell = m_vtCell[i];
		if (pCell)
			delete pCell;
	}
	m_vtCell.clear();
}

void duReportColumn::Init()
{
	int nLineCount = m_pReportView->GetLineCount();
	int i;
	duReportCell *pCell = NULL;
	for (i = 0;i < nLineCount; i++)
	{
		pCell = new duReportCell;
		pCell->SetReportView(m_pReportView);
		m_vtCell.push_back(pCell);
	}
}

BOOL duReportColumn::InsertLine(int nLine)
{
	if (nLine < 0 || nLine > (int)m_vtCell.size())
		return FALSE;
		
	duReportCell *pCell = new duReportCell;
	pCell->SetReportView(m_pReportView);
	
	vector<duReportCell *>::iterator iterInsert = m_vtCell.begin() + nLine;
	m_vtCell.insert(iterInsert, pCell);
	return TRUE;
}

BOOL duReportColumn::DeleteLine(int nLine)
{
	if(nLine < 0 || nLine > m_pReportView->GetLineCount())
		return FALSE;

	vector<duReportCell *>::iterator iter = m_vtCell.begin() + nLine;
	duReportCell *pReportCell = *iter;
	if(pReportCell)
		delete pReportCell;
	m_vtCell.erase(iter);

	return TRUE;

}

duReportCell *duReportColumn::GetCell(int nLine)
{
	if (nLine < 0 || nLine >= (int)m_vtCell.size())
		return NULL;
	
	return m_vtCell[nLine];
}
