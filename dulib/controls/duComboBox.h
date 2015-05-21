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
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI OnMouseLDown(POINT pt);
	virtual BOOL WINAPI OnSetFocus(duPlugin *pOldFocus);
	virtual BOOL WINAPI OnSetCursor();
	virtual void WINAPI OnMouseDbClick(POINT pt);
	virtual void WINAPI OnKeyDown(UINT key, UINT nRepCnt, UINT nFlags);

	virtual BOOL WINAPI IsEnd() { return m_fEnd; };
	virtual void WINAPI SetEnd(BOOL fEnd) { m_fEnd = fEnd; }
	
	virtual int WINAPI GetItemCount() { return (int)m_vtItem.size(); }
	virtual BOOL WINAPI InsertItem(int nIndex, LPCTSTR lpszText, LPCTSTR lpszIcon);
	virtual BOOL WINAPI DeleteItem(int nIndex);
	virtual void WINAPI DeleteAll();
	virtual LPCTSTR WINAPI GetItemText(int nIndex);
	virtual BOOL WINAPI SetItemText(int nIndex, LPCTSTR lpszText);
	virtual LPCTSTR WINAPI GetItemIcon(int nIndex);
	virtual BOOL WINAPI SetItemIcon(int nIndex, LPCTSTR lpszIcon);
	virtual UINT WINAPI GetItemIconX(int nIndex);
	virtual BOOL WINAPI SetItemIconX(int nIndex, UINT uIconX);
	virtual UINT WINAPI GetItemIconY(int nIndex);
	virtual BOOL WINAPI SetItemIconY(int nIndex, UINT uIconY);
	virtual UINT WINAPI GetItemIconWidth(int nIndex);
	virtual BOOL WINAPI SetItemIconWidth(int nIndex, UINT uIconWidth);
	virtual UINT WINAPI GetItemIconHeight(int nIndex);
	virtual BOOL WINAPI SetItemIconHeight(int nIndex, UINT uIconHeight);
	virtual int WINAPI GetCurSel();
	virtual void WINAPI SetCurSel(int nIndex);
	virtual INT_PTR WINAPI GetItemData(int nIndex);
	virtual void WINAPI SetItemData(int nIndex, INT_PTR pData);
	virtual int WINAPI AddItem(LPCTSTR lpszText, LPCTSTR lpszIcon);
	virtual int WINAPI FindItem(LPCTSTR lpszText);
	virtual int WINAPI SelectItem(LPCTSTR lpszText);
	virtual BOOL WINAPI IsItemDisable(int nIndex);
	virtual BOOL WINAPI SetItemDisable(int nIndex, BOOL fDisable);

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duComboBox"); }

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

static LRESULT WINAPI ComboListBox_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
