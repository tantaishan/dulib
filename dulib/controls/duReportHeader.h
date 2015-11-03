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
	void DrawHeader(HDC hDC);
	int GetItemCount();
	void SetReportView(duReportView *pReportView) { m_pReportView = pReportView; }
	LPTSTR GetStyle() { return m_szItemStyle; }
	int GetTotalWidth();
	void OnMouseLDown(POINT pt);
	void OnMouseLUp(POINT pt);
	void OnMouseRDown(POINT pt);
	void OnMouseRUp(POINT pt);
	void OnMouseHover(POINT pt);
	void OnMouseMove(POINT pt);
	void OnMouseDbClick(POINT pt);
	void OnMouseLeave();
	BOOL OnSetCursor(POINT pt);
	BOOL InsertItem(int nCol, int nWidth, LPCTSTR lpszText);
	int GetColumnWidth(int nCol);
	BOOL DeleteColumn(int nCol);
	
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
