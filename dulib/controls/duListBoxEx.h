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
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);

	virtual void WINAPI Resize(LPRECT lpRect);
	virtual void WINAPI OnVScroll(ScrollBarAction sbAction, int nPos);
	virtual void WINAPI OnChildShow(duPlugin *pChild, BOOL fVisible);
	virtual LRESULT WINAPI OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void WINAPI OnChildDelete(duPlugin *pChild);

	virtual LPCTSTR WINAPI GetItemStyle();
	virtual void WINAPI SetItemStyle(LPCTSTR pStyle);

	virtual duScroll *WINAPI GetVertScrollBar();
	virtual duPlugin *WINAPI GetHotItem() { return m_pHotItem; }
	virtual duPlugin *WINAPI GetSelItem() { return m_pSelItem; }
	virtual duPlugin *WINAPI GetPressItem() { return m_pPressItem; }

	virtual void WINAPI SetHotItem(duPlugin *pPlugin) { m_pHotItem = pPlugin; }
	virtual void WINAPI SetSelItem(duPlugin *pPlugin) { m_pSelItem = pPlugin; }
	virtual void WINAPI SetPressItem(duPlugin *pPlugin) { m_pPressItem = pPlugin; }

	virtual void WINAPI RefreshPane();
	virtual void WINAPI SetVertScrollBar(LPCTSTR lpszScroll);
	
	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duListBoxEx"); }	

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
