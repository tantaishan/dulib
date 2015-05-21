//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duOutlookBar.h
//
//  History:    Jun-20-11   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once

#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

typedef struct tagOutlookBarItem
{
	dustring strText;
	int iState;
	duPlugin *pChild;
	BOOL fSelected;
}OutlookBarItem;


// duOutlookBar
//
//
class duOutlookBar : public duPlugin
{	
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);

	virtual void WINAPI OnMouseLeave(POINT pt);
	virtual void WINAPI OnMouseLDown(POINT pt);
	virtual void WINAPI OnMouseLUp(POINT pt);
	virtual void WINAPI OnMouseMove(POINT pt);

	virtual int WINAPI GetItemCount();
	virtual BOOL WINAPI InsertItem(int nIndex, LPCTSTR lpszText, duPlugin *pChild);
	virtual LPCTSTR WINAPI GetItemText(int nIndex);
	virtual void WINAPI SetItemText(int nIndex, LPCTSTR lpszText);
	virtual duPlugin * WINAPI GetItem(int nIndex);
	virtual void WINAPI SetItem(int nIndex, duPlugin *pChild);
	virtual BOOL WINAPI DeleteItem(int nIndex);

	virtual int WINAPI GetSelectedItem();
	virtual void WINAPI SetSelectedItem(int nIndex);

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duOutlookBar"); }	
public:
	duOutlookBar();
	~duOutlookBar();

protected:
	TCHAR m_szItemStyle[MAX_NAME];
	int m_nItemHeight;

private:
	vector<OutlookBarItem *> m_vtItems;
};
