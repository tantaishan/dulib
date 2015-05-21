//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duReportHeader.h
//
//  History:    Mar-08-2011   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

class duReportView;

#define HEADER_BORDER_WIDTH			2
#define HEADER_ITEM_MAXWIDTH		0xFFFF
#define REPORTVIEW_DEFAULT_HEIGHT	20


typedef struct _tagReportHeaderItem
{
	dustring strText;
	int nWidth;
	UINT uState;
	int nMinWidth;
	int nMaxWidth;
}ReportHeaderItem;

// duReportHeader
// 
// 
class duReportHeader
{
public:
	void WINAPI DrawHeader(HDC hDC);
	int WINAPI GetItemCount();
	void WINAPI SetReportView(duReportView *pReportView) { m_pReportView = pReportView; }
	LPTSTR WINAPI GetStyle() { return m_szItemStyle; }
	int WINAPI GetTotalWidth();
	void WINAPI OnMouseLDown(POINT pt);
	void WINAPI OnMouseLUp(POINT pt);
	void WINAPI OnMouseRDown(POINT pt);
	void WINAPI OnMouseRUp(POINT pt);
	void WINAPI OnMouseHover(POINT pt);
	void WINAPI OnMouseMove(POINT pt);
	void WINAPI OnMouseDbClick(POINT pt);
	void WINAPI OnMouseLeave();
	BOOL WINAPI OnSetCursor(POINT pt);
	BOOL WINAPI InsertItem(int nCol, int nWidth, LPCTSTR lpszText);
	int WINAPI GetColumnWidth(int nCol);
	BOOL WINAPI DeleteColumn(int nCol);
	
public:
	duReportHeader();
	~duReportHeader();
	
protected:
	void Destroy();
	ReportHeaderItem *GetItem(int nIndex);
	
protected:
	duReportView *m_pReportView;
	int m_nRef;
	TCHAR m_szItemStyle[MAX_NAME];
	vector<ReportHeaderItem *> m_vtHeaderItem;
	int m_nHot;
	BOOL m_fFixed;
	BOOL m_fTracking;
	duPoint m_ptTrack;
	int m_nHotBorder;
	int m_nTrackWidth;
};
