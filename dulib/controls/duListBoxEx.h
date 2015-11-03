//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duListBoxEx.h
//
//  History:    MAR-16-2010   Denny Chen  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"
#include "duScroll.h"

// duListBoxEx
//
//
class duListBoxEx : public duPlugin
{
public:
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);

	virtual void Resize(LPRECT lpRect);
	virtual void OnVScroll(ScrollBarAction sbAction, int nPos);
	virtual void OnChildShow(duPlugin *pChild, BOOL fVisible);
	virtual LRESULT OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnChildDelete(duPlugin *pChild);

	virtual LPCTSTR GetItemStyle();
	virtual void SetItemStyle(LPCTSTR pStyle);

	virtual duScroll *GetVertScrollBar();
	virtual duPlugin *GetHotItem() { return m_pHotItem; }
	virtual duPlugin *GetSelItem() { return m_pSelItem; }
	virtual duPlugin *GetPressItem() { return m_pPressItem; }

	virtual void SetHotItem(duPlugin *pPlugin) { m_pHotItem = pPlugin; }
	virtual void SetSelItem(duPlugin *pPlugin) { m_pSelItem = pPlugin; }
	virtual void SetPressItem(duPlugin *pPlugin) { m_pPressItem = pPlugin; }

	virtual void RefreshPane();
	virtual void SetVertScrollBar(LPCTSTR lpszScroll);
	
	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duListBoxEx"); }	

public:
	duListBoxEx();
	~duListBoxEx();
	void ResetScrollPos();

protected:
	TCHAR m_szItemStyle[MAX_NAME];
	TCHAR m_szVertScrollBar[MAX_NAME];

protected:
	int m_nYOffset;
	duPlugin *m_pHotItem;
	duPlugin *m_pPressItem;
	duPlugin *m_pSelItem;
	BOOL m_fContentMenu;
	BOOL m_fDispMouseWheel;
};
