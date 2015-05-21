//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duReportLine.h
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

// duReportLine
// 
// 
class duReportLine
{
public:
	void WINAPI DrawLine(HDC hDC, LPRECT lprcLine, int nLine);
	void WINAPI SetReportView(duReportView *pReportView) { m_pReportView = pReportView; }
	
	int GetHeight() { return m_nHeight; }
	void SetHeight(int nHeight) { m_nHeight = nHeight; }
	
	UINT GetState();
	void SetState(UINT uState);
	
	LPCTSTR GetStyle() { return m_szStyle; }
	void SetStyle(LPCTSTR lpszStyle);
	
	BOOL IsSelected() { return m_fSelected; }
	void SetSelected(BOOL fSelected) { m_fSelected = fSelected; }
	
public:
	duReportLine();
	~duReportLine();
	
protected:
	duReportView *m_pReportView;
	int m_nHeight;
	UINT m_uState;
	BOOL m_fSelected;
	TCHAR m_szStyle[MAX_NAME];
};
