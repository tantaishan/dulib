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
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);
	virtual void WINAPI OnVScroll(ScrollBarAction sbAction, int nPos);
	virtual void WINAPI OnMouseWheel(UINT fwKeys, int zDelta, POINT pt);
	virtual void WINAPI OnMouseLeave(POINT pt);
	virtual void WINAPI OnMouseLDown(POINT pt);
	virtual void WINAPI OnMouseLUp(POINT pt);
	virtual void WINAPI OnMouseRDown(POINT pt);
	virtual void WINAPI OnMouseRUp(POINT pt);
	virtual void WINAPI OnMouseHover(POINT pt);
	virtual void WINAPI OnMouseMove(POINT pt);
	virtual void WINAPI OnMouseDbClick(POINT pt);
	virtual void WINAPI Resize(LPRECT lpRect/*=NULL*/);
	virtual void WINAPI OnTimer(UINT nEventId);
	virtual void WINAPI OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual BOOL WINAPI UpdateScroll();
	
	virtual int WINAPI GetSel();
	virtual void WINAPI SetSel(int nIndex);
	virtual int WINAPI GetItemCount();
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
	virtual BOOL WINAPI IsItemDisable(int nIndex);
	virtual BOOL WINAPI SetItemDisable(int nIndex, BOOL fDisable);

	virtual duScroll *WINAPI GetVertScrollBar();
	virtual void WINAPI SetVertScrollBar(LPCTSTR lpszScroll);
	
	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duListBox"); }	

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
