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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);

	virtual void OnMouseLeave(POINT pt);
	virtual void OnMouseLDown(POINT pt);
	virtual void OnMouseLUp(POINT pt);
	virtual void OnMouseMove(POINT pt);

	virtual int GetItemCount();
	virtual BOOL InsertItem(int nIndex, LPCTSTR lpszText, duPlugin *pChild);
	virtual LPCTSTR GetItemText(int nIndex);
	virtual void SetItemText(int nIndex, LPCTSTR lpszText);
	virtual duPlugin * GetItem(int nIndex);
	virtual void SetItem(int nIndex, duPlugin *pChild);
	virtual BOOL DeleteItem(int nIndex);

	virtual int GetSelectedItem();
	virtual void SetSelectedItem(int nIndex);

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duOutlookBar"); }	
public:
	duOutlookBar();
	~duOutlookBar();

protected:
	TCHAR m_szItemStyle[MAX_NAME];
	int m_nItemHeight;

private:
	vector<OutlookBarItem *> m_vtItems;
};
