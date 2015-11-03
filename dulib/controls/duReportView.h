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
	virtual void RegisterControlProperty();
	virtual void DrawObject(HDC hDC);
	virtual void OnCreate();
	virtual void DrawView(HDC hDC);
	virtual void GetViewSize(LPSIZE lpSize);
	virtual void OnMouseLeave(POINT pt);
	
	virtual void OnMouseLDown(POINT pt);
	virtual void OnMouseLUp(POINT pt);
	virtual void OnMouseRDown(POINT pt);
	virtual void OnMouseRUp(POINT pt);
	virtual void OnMouseHover(POINT pt);
	virtual void OnMouseMove(POINT pt);
	virtual void OnMouseDbClick(POINT pt);
	
	virtual void OnViewMouseLDown(POINT pt);
	virtual void OnViewMouseLUp(POINT pt);
	virtual void OnViewMouseRDown(POINT pt);
	virtual void OnViewMouseRUp(POINT pt);
	virtual void OnViewMouseHover(POINT pt);
	virtual void OnViewMouseMove(POINT pt);
	virtual void OnViewMouseDbClick(POINT pt);
	virtual BOOL OnViewSetCursor(POINT pt);
	
	virtual BOOL InsertColumn(int nCol, int nWidth, LPCTSTR lpszText);
	virtual int GetLineCount() { return m_nLineCount; }
	virtual int InsertLine(int nLine, int nHeight);
	virtual int GetColumnCount() { return (int)m_vtColumn.size(); }
	virtual int GetColumnWidth(int nCol);
	virtual void GetHeaderRect(LPRECT lpRect);
	virtual BOOL DeleteColumn(int nCol);
	virtual BOOL DeleteLine(int nLine);
	
	virtual LPCTSTR GetCellText(int nLine, int nCol);
	virtual void SetCellText(int nLine, int nCol, LPCTSTR lpszText);
	
	virtual LPCTSTR GetCellStyle() { return m_szCellStyle; }
	virtual void SetCellStyle(LPCTSTR lpszStyle);
	
	virtual int GetHeaderHeight() { return m_nHeaderHeight; }
	
	virtual LPCTSTR GetLineStyle() { return m_szLineStyle; }
	virtual void SetLineStyle(LPCTSTR lpszStyle);
	
	virtual BOOL IsMultiSel() { return m_fMultiSel; }
	virtual void SetMultiSel(BOOL fMultiSel) { m_fMultiSel = fMultiSel; }
	
	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duReportView"); }	

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
