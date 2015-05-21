//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duReportView.h
//
//  History:    Mar-08-2011   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duScrollView.h"
#include "duReportHeader.h"
#include "duReportColumn.h"
#include "duReportLine.h"

// duReportView
// 
// 
class duReportView : public duScrollView
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI DrawObject(HDC hDC);
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawView(HDC hDC);
	virtual void WINAPI GetViewSize(LPSIZE lpSize);
	virtual void WINAPI OnMouseLeave(POINT pt);
	
	virtual void WINAPI OnMouseLDown(POINT pt);
	virtual void WINAPI OnMouseLUp(POINT pt);
	virtual void WINAPI OnMouseRDown(POINT pt);
	virtual void WINAPI OnMouseRUp(POINT pt);
	virtual void WINAPI OnMouseHover(POINT pt);
	virtual void WINAPI OnMouseMove(POINT pt);
	virtual void WINAPI OnMouseDbClick(POINT pt);
	
	virtual void WINAPI OnViewMouseLDown(POINT pt);
	virtual void WINAPI OnViewMouseLUp(POINT pt);
	virtual void WINAPI OnViewMouseRDown(POINT pt);
	virtual void WINAPI OnViewMouseRUp(POINT pt);
	virtual void WINAPI OnViewMouseHover(POINT pt);
	virtual void WINAPI OnViewMouseMove(POINT pt);
	virtual void WINAPI OnViewMouseDbClick(POINT pt);
	virtual BOOL WINAPI OnViewSetCursor(POINT pt);
	
	virtual BOOL WINAPI InsertColumn(int nCol, int nWidth, LPCTSTR lpszText);
	virtual int WINAPI GetLineCount() { return m_nLineCount; }
	virtual int WINAPI InsertLine(int nLine, int nHeight);
	virtual int WINAPI GetColumnCount() { return (int)m_vtColumn.size(); }
	virtual int WINAPI GetColumnWidth(int nCol);
	virtual void WINAPI GetHeaderRect(LPRECT lpRect);
	virtual BOOL WINAPI DeleteColumn(int nCol);
	virtual BOOL WINAPI DeleteLine(int nLine);
	
	virtual LPCTSTR WINAPI GetCellText(int nLine, int nCol);
	virtual void WINAPI SetCellText(int nLine, int nCol, LPCTSTR lpszText);
	
	virtual LPCTSTR WINAPI GetCellStyle() { return m_szCellStyle; }
	virtual void WINAPI SetCellStyle(LPCTSTR lpszStyle);
	
	virtual int WINAPI GetHeaderHeight() { return m_nHeaderHeight; }
	
	virtual LPCTSTR WINAPI GetLineStyle() { return m_szLineStyle; }
	virtual void WINAPI SetLineStyle(LPCTSTR lpszStyle);
	
	virtual BOOL WINAPI IsMultiSel() { return m_fMultiSel; }
	virtual void WINAPI SetMultiSel(BOOL fMultiSel) { m_fMultiSel = fMultiSel; }
	
	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duReportView"); }	

public:
	duReportView();
	~duReportView();
	
public:
	int GetTotalHeight();
	duReportCell *GetCell(int nLine, int nCol);
	duReportColumn *GetColumn(int nCol);
	duReportLine *GetLine(int nLine);
	int GetFirstVisibleLine(int &nFirstHeight);
	void SelectLine(int nLine);
	void SelectRange(int nStart, int nEnd);
	int GetPreSelLine(int nLine);
	
protected:
	duReportHeader *m_pHeaderCtrl;
	int m_nHeaderHeight;
	BOOL m_fGridLine;
	TCHAR m_szLineStyle[MAX_NAME];
	TCHAR m_szCellStyle[MAX_NAME];
	BOOL m_fMultiSel;
	
protected:
	int m_nHotLine;
	int m_nLineCount;
	vector<duReportColumn *> m_vtColumn;
	vector<duReportLine *> m_vtLine;
};
