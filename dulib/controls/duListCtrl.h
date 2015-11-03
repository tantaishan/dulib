//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duListCtrl.h
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once

#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"
#include "duScrollView.h"

// duListCtrl
// 
// 
class duListCtrl : public duScrollView
{
public:
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);
	virtual void GetViewSize(LPSIZE lpSize);
	virtual void OnVScroll(ScrollBarAction sbAction, int nPos);
	virtual void OnHScroll(ScrollBarAction sbAction, int nPos);
	virtual LRESULT OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void Resize(LPRECT lpRect/*=NULL*/);
	
	virtual duPlugin *InsertLine(int nIndex, LPCTSTR lpszTemplate);
	virtual duPlugin *GetLine(int nIndex);
	virtual int GetLineCount();
	virtual int GetTotalHeight();
	virtual duPlugin *GetFirstVisibleLine();
	virtual BOOL MoveLine(int nIndex, int nNewIndex);
	virtual BOOL DeleteLine(int nIndex, BOOL fAdjust);
	virtual BOOL DeleteLine(duPlugin* pItem, BOOL fAjust);
	virtual void DeleteAll();
	virtual void ResetLine();

	virtual void VertScroll(int nPos);
	virtual void HorzScroll(int nPos);
	virtual void AdjustColumn();
	virtual void AdjustColumnWidth(int nCol, int nWidth);
	virtual int GetSel();
	virtual void SetSel(int nSel);

	virtual LPCTSTR GetHeaderCtrl() { return m_szHeaderCtrl; }
	virtual void SetHeaderCtrl(LPCTSTR lpszHeader);
	
	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duListCtrl"); }

public:
	duListCtrl();
	~duListCtrl();
	
protected:
	void AdjustVisibleLine(int nTop);
	void CompleteAdjustLine();
	int GetLinePluginIndex(duPlugin *pPlugin);

protected:
	int m_nCloneIndex;
	duPlugin *m_pSel;
	duPlugin *m_pHot;

protected:
	int m_nHotLine;					// 鼠标高亮行
	int m_nFirstLine;				// 显示区域第一行
	int m_nFLTotalHeight;			// 显示区域前面所有行高的总和
	int m_nViewCy;					// 视图高度
	TCHAR m_szHeaderCtrl[MAX_NAME]; 
	vector<duPlugin *> m_vtLines;
	BOOL m_fDispMouseWheel;
};
