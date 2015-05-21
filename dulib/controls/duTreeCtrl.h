//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duTreeCtrl.h
//
//  History:    Feb-21-2011   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duScrollView.h"

typedef struct tagTreeCtrlItem
{
	dustring strText;
	UINT nTextWidth;
	dustring strImage;
	UINT nLevel;
	BOOL fExpand;
	struct tagTreeCtrlItem *pParent;
	struct tagTreeCtrlItem *pNextSibling;
	struct tagTreeCtrlItem *pFirstChild;
	INT_PTR lParam;
}TreeCtrlItem;

enum TCHitTestCode
{
	tc_hit_empty = 1,
	tc_hit_indentspace = 2,
	tc_hit_indent = 3,
	tc_hit_indenticonspace = 4,
	tc_hit_icon = 5,
	tc_hit_icontextspace = 6,
	tc_hit_text = 7,
	tc_hit_textspace = 8
};

#define TCVI_ROOT 			((TreeCtrlItem *)(ULONG_PTR)-0x10000)
#define TCVI_FIRST 			((TreeCtrlItem *)(ULONG_PTR)-0x0FFFF)
#define TCVI_LAST 			((TreeCtrlItem *)(ULONG_PTR)-0x0FFFE)




// duTreeCtrl
//
//
class duTreeCtrl : public duScrollView
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);

	virtual void WINAPI OnMouseMove(POINT pt);
	virtual void WINAPI OnMouseLeave(POINT pt);
	virtual void WINAPI OnMouseHover(POINT pt);
	virtual void WINAPI OnMouseLDown(POINT pt);
	virtual void WINAPI OnMouseLUp(POINT pt);
	virtual void WINAPI OnMouseRDown(POINT pt);
	virtual void WINAPI OnMouseRUp(POINT pt);
	virtual void WINAPI OnMouseDbClick(POINT pt);
	virtual void WINAPI OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void WINAPI OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void WINAPI GetViewSize(LPSIZE lpSize);
	
	virtual TreeCtrlItem *WINAPI GetRootItem() { return m_pRoot; }	
	virtual TreeCtrlItem *WINAPI InsertItem(LPCTSTR lpszText, LPCTSTR lpszImage, TreeCtrlItem *pParent, TreeCtrlItem *pInsertAfter);
	virtual BOOL WINAPI DeleteItem(TreeCtrlItem *pItem);
	virtual BOOL WINAPI ItemHasChildren(TreeCtrlItem *pItem);
	virtual LPCTSTR WINAPI GetItemStyle() { return m_szItemStyle; }
	virtual void WINAPI SetItemStyle(LPCTSTR lpszStyle);
	virtual LPCTSTR WINAPI GetIndentStyle() { return m_szIndentStyle; }
	virtual void WINAPI SetIndentStyle(LPCTSTR lpszStyle);
	virtual LPCTSTR WINAPI GetFont() { return m_szFont; }
	virtual void WINAPI SetFont(LPCTSTR lpszFont);
	virtual int WINAPI GetIndentWidth() { return m_nIndentWidth; }
	virtual void WINAPI SetIndentWidth(int nIndentWidth) { m_nIndentWidth = nIndentWidth; }
	virtual int WINAPI GetIndentHeight() { return m_nIndentHeight; }
	virtual void WINAPI SetIndentHeight(int nIndentHeight) { m_nIndentHeight = nIndentHeight; }
	virtual int WINAPI GetItemHeight() { return m_nItemHeight; }
	virtual void WINAPI SetItemHeight(int nItemHeight);
	virtual int WINAPI GetIconWidth() { return m_nIconWidth; }
	virtual void WINAPI SetIconWidth(int nIconWidth) { m_nIconWidth = nIconWidth; }
	virtual int WINAPI GetIconHeight() { return m_nIconHeight; }
	virtual void WINAPI SetIconHeight(int nIconHeight) { m_nIconHeight = nIconHeight; }
	virtual int WINAPI GetIndentIconSpace() { return m_nIndentIconSpace; }
	virtual void WINAPI SetIndentIconSpace(int nIndentIconSpace) { m_nIndentIconSpace = nIndentIconSpace; }
	virtual int WINAPI GetIconTextSpace() { return m_nIconTextSpace; }
	virtual void WINAPI SetIconTextSpace(int nIconTextSpace) { m_nIconTextSpace = nIconTextSpace; }
	virtual COLORREF WINAPI GetTextColor() { return m_clrText; }
	virtual void WINAPI SetTextColor(COLORREF clrText) { m_clrText = clrText; }
	virtual int WINAPI GetLastHitCode() { return (int)m_hitCode; }
	
	virtual TreeCtrlItem *WINAPI GetNextItem(TreeCtrlItem *pItem);
	virtual LPCTSTR WINAPI GetItemText(TreeCtrlItem *pItem);
	virtual void WINAPI SetItemText(TreeCtrlItem *pItem, LPCTSTR lpszText);
	virtual LPCTSTR WINAPI GetItemIcon(TreeCtrlItem *pItem);
	virtual void WINAPI SetItemIcon(TreeCtrlItem *pItem, LPCTSTR lpszIcon);
	virtual int WINAPI GetItemLevel(TreeCtrlItem *pItem);
	virtual BOOL WINAPI IsExpand(TreeCtrlItem *pItem);
	virtual void WINAPI Expand(TreeCtrlItem *pItem, BOOL fExpand);
	virtual TreeCtrlItem *WINAPI GetParent(TreeCtrlItem *pItem);
	virtual TreeCtrlItem *WINAPI GetNextSibling(TreeCtrlItem *pItem);
	virtual TreeCtrlItem *WINAPI GetFirstChild(TreeCtrlItem *pItem);
	virtual TreeCtrlItem *WINAPI GetCurSelItem() { return m_pSelect; }
	virtual void WINAPI DeleteAllItems();
	virtual INT_PTR WINAPI GetItemData(TreeCtrlItem *pItem);
	virtual BOOL WINAPI SetItemData(TreeCtrlItem *pItem, INT_PTR pData);
	virtual BOOL WINAPI IsAutoShowTooltip(){return m_fAutoShowToolTip;}
	virtual void WINAPI SetAutoShowTooltip(BOOL fAutoShowTooltip);
	virtual TreeCtrlItem * WINAPI HitTest(LPPOINT lppt);
	

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duTreeCtrl"); }	
	
public:
	duTreeCtrl();
	~duTreeCtrl();
	
protected:
	TreeCtrlItem *GetNextVisibleItem(TreeCtrlItem *pItem);
	int CalcItemWidth(TreeCtrlItem *pItem);
	BOOL DeleteItemT(TreeCtrlItem *pItem);
	TreeCtrlItem *GetFirstVisibleItem(int &nOffset);
	duRect CalcVCenterRect(duRect &rcItemRect, int nLeft, int nWidth, int nHeight);
	TreeCtrlItem *yHitTest(const duRect &rcTreeCtrl, const POINT pt, duRect &rcItem);
	
protected:
	TreeCtrlItem *m_pRoot;
	TreeCtrlItem *m_pHot;
	TreeCtrlItem *m_pSelect;
	UINT m_nItems;

protected:
	TCHAR m_szItemStyle[MAX_NAME];
	TCHAR m_szIndentStyle[MAX_NAME];
	TCHAR m_szFont[MAX_NAME];
	int m_nItemHeight;
	int m_nIndentWidth;
	int m_nIndentHeight;
	int m_nIconWidth;
	int m_nIconHeight;
	int m_nIndentIconSpace;
	int m_nIconTextSpace;
	COLORREF m_clrText;

	BOOL m_fShowTooltip;
	BOOL m_fAutoShowToolTip;
	
protected:
	TCHitTestCode m_hitCode;
};
