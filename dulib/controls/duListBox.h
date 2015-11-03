//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duListBox.h
//
//  History:    MAR-03-2010   Denny Chen  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"
#include "duScroll.h"
#include "duDrawHelper.h"

typedef struct _tagListBoxItem
{
	UINT uIconX;
	UINT uIconY;
	UINT uIconWidth;
	UINT uIconHeight;
	dustring szIcon;
	dustring szText;
	BOOL fDisable;
	INT_PTR lParam;
}ListBoxItem;

// duListBox
// 
// 
class duListBox : public duPlugin
{
public:
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);
	virtual void OnVScroll(ScrollBarAction sbAction, int nPos);
	virtual void OnMouseWheel(UINT fwKeys, int zDelta, POINT pt);
	virtual void OnMouseLeave(POINT pt);
	virtual void OnMouseLDown(POINT pt);
	virtual void OnMouseLUp(POINT pt);
	virtual void OnMouseRDown(POINT pt);
	virtual void OnMouseRUp(POINT pt);
	virtual void OnMouseHover(POINT pt);
	virtual void OnMouseMove(POINT pt);
	virtual void OnMouseDbClick(POINT pt);
	virtual void Resize(LPRECT lpRect/*=NULL*/);
	virtual void OnTimer(UINT nEventId);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual BOOL UpdateScroll();
	
	virtual int GetSel();
	virtual void SetSel(int nIndex);
	virtual int GetItemCount();
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
	virtual BOOL IsItemDisable(int nIndex);
	virtual BOOL SetItemDisable(int nIndex, BOOL fDisable);

	virtual duScroll *GetVertScrollBar();
	virtual void SetVertScrollBar(LPCTSTR lpszScroll);
	
	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duListBox"); }	

public:
	duListBox();
	~duListBox();

public:
	vector<ListBoxItem *> &GetItemVector() { return m_vtItem; }
	void SetComboBoxListBox(BOOL fComboBox) { m_fCombobox = fComboBox; }
	BOOL IsComboBoxListBox() { return m_fCombobox; }
	
protected:
	void Destroy();
	ListBoxItem *GetItem(int nIndex);
	void FadeRedraw();
	void DrawItem(ListBoxItem *pItem, HDC hDC, duStyleGroup *pStyleGroup, duRect &rcItem);
	int GetItemIndex(ListBoxItem *pItem);

public:
	TCHAR m_szVertScroll[MAX_NAME];
	int  m_nItemHeight;
	TCHAR m_szItemStyle[MAX_NAME];	
	int  m_nItemIconX;
	int  m_nItemIconY;
	int  m_nItemIconWidth;
	int  m_nItemIconHeight;
	BOOL m_fFade;
	
protected:
	int m_nYOffset;
	BOOL m_fVert;
	ListBoxItem *m_pHotItem;
	ListBoxItem *m_pSelItem;
	vector<ListBoxItem *> m_vtItem;
	BOOL m_fCombobox;
	
protected:
	BOOL m_fFadeIn;
	int m_nAlphaFadeIn;
	UINT m_nTimerFadeIn;
};
