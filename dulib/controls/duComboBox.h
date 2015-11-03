//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duComboBox.h
//
//  History:    Nov-17-2009   Steven Li  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duEdit.h"
#include "duListBox.h"

// duComboBox
// 
// 
class duComboBox : public duEdit
{
public:
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void OnMouseLDown(POINT pt);
	virtual BOOL OnSetFocus(duPlugin *pOldFocus);
	virtual BOOL OnSetCursor();
	virtual void OnMouseDbClick(POINT pt);
	virtual void OnKeyDown(UINT key, UINT nRepCnt, UINT nFlags);

	virtual BOOL IsEnd() { return m_fEnd; };
	virtual void SetEnd(BOOL fEnd) { m_fEnd = fEnd; }
	
	virtual int GetItemCount() { return (int)m_vtItem.size(); }
	virtual BOOL InsertItem(int nIndex, LPCTSTR lpszText, LPCTSTR lpszIcon);
	virtual BOOL DeleteItem(int nIndex);
	virtual void DeleteAll();
	virtual LPCTSTR GetItemText(int nIndex);
	virtual BOOL SetItemText(int nIndex, LPCTSTR lpszText);
	virtual LPCTSTR GetItemIcon(int nIndex);
	virtual BOOL SetItemIcon(int nIndex, LPCTSTR lpszIcon);
	virtual UINT GetItemIconX(int nIndex);
	virtual BOOL SetItemIconX(int nIndex, UINT uIconX);
	virtual UINT GetItemIconY(int nIndex);
	virtual BOOL SetItemIconY(int nIndex, UINT uIconY);
	virtual UINT GetItemIconWidth(int nIndex);
	virtual BOOL SetItemIconWidth(int nIndex, UINT uIconWidth);
	virtual UINT GetItemIconHeight(int nIndex);
	virtual BOOL SetItemIconHeight(int nIndex, UINT uIconHeight);
	virtual int GetCurSel();
	virtual void SetCurSel(int nIndex);
	virtual INT_PTR GetItemData(int nIndex);
	virtual void SetItemData(int nIndex, INT_PTR pData);
	virtual int AddItem(LPCTSTR lpszText, LPCTSTR lpszIcon);
	virtual int FindItem(LPCTSTR lpszText);
	virtual int SelectItem(LPCTSTR lpszText);
	virtual BOOL IsItemDisable(int nIndex);
	virtual BOOL SetItemDisable(int nIndex, BOOL fDisable);

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duComboBox"); }

public:
	duComboBox();
	~duComboBox();
	
public:
	ListBoxItem *GetItem(int nIndex);
	int CalcListBoxHeight();

protected:
	void Destroy();
	
public:
	TCHAR m_szWindow[MAX_NAME];
	TCHAR m_szListBox[MAX_NAME];
	int m_nWindowHeight;
	int m_nCurSel;
	
protected:
	HWND m_hWndListBox;
	BOOL m_fEnd;
	vector<ListBoxItem *> m_vtItem;
};

static LRESULT ComboListBox_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
