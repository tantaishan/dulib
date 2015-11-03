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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);

	virtual void OnMouseMove(POINT pt);
	virtual void OnMouseLeave(POINT pt);
	virtual void OnMouseHover(POINT pt);
	virtual void OnMouseLDown(POINT pt);
	virtual void OnMouseLUp(POINT pt);
	virtual void OnMouseRDown(POINT pt);
	virtual void OnMouseRUp(POINT pt);
	virtual void OnMouseDbClick(POINT pt);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void GetViewSize(LPSIZE lpSize);
	
	virtual TreeCtrlItem *GetRootItem() { return m_pRoot; }	
	virtual TreeCtrlItem *InsertItem(LPCTSTR lpszText, LPCTSTR lpszImage, TreeCtrlItem *pParent, TreeCtrlItem *pInsertAfter);
	virtual BOOL DeleteItem(TreeCtrlItem *pItem);
	virtual BOOL ItemHasChildren(TreeCtrlItem *pItem);
	virtual LPCTSTR GetItemStyle() { return m_szItemStyle; }
	virtual void SetItemStyle(LPCTSTR lpszStyle);
	virtual LPCTSTR GetIndentStyle() { return m_szIndentStyle; }
	virtual void SetIndentStyle(LPCTSTR lpszStyle);
	virtual LPCTSTR GetFont() { return m_szFont; }
	virtual void SetFont(LPCTSTR lpszFont);
	virtual int GetIndentWidth() { return m_nIndentWidth; }
	virtual void SetIndentWidth(int nIndentWidth) { m_nIndentWidth = nIndentWidth; }
	virtual int GetIndentHeight() { return m_nIndentHeight; }
	virtual void SetIndentHeight(int nIndentHeight) { m_nIndentHeight = nIndentHeight; }
	virtual int GetItemHeight() { return m_nItemHeight; }
	virtual void SetItemHeight(int nItemHeight);
	virtual int GetIconWidth() { return m_nIconWidth; }
	virtual void SetIconWidth(int nIconWidth) { m_nIconWidth = nIconWidth; }
	virtual int GetIconHeight() { return m_nIconHeight; }
	virtual void SetIconHeight(int nIconHeight) { m_nIconHeight = nIconHeight; }
	virtual int GetIndentIconSpace() { return m_nIndentIconSpace; }
	virtual void SetIndentIconSpace(int nIndentIconSpace) { m_nIndentIconSpace = nIndentIconSpace; }
	virtual int GetIconTextSpace() { return m_nIconTextSpace; }
	virtual void SetIconTextSpace(int nIconTextSpace) { m_nIconTextSpace = nIconTextSpace; }
	virtual COLORREF GetTextColor() { return m_clrText; }
	virtual void SetTextColor(COLORREF clrText) { m_clrText = clrText; }
	virtual int GetLastHitCode() { return (int)m_hitCode; }
	
	virtual TreeCtrlItem *GetNextItem(TreeCtrlItem *pItem);
	virtual LPCTSTR GetItemText(TreeCtrlItem *pItem);
	virtual void SetItemText(TreeCtrlItem *pItem, LPCTSTR lpszText);
	virtual LPCTSTR GetItemIcon(TreeCtrlItem *pItem);
	virtual void SetItemIcon(TreeCtrlItem *pItem, LPCTSTR lpszIcon);
	virtual int GetItemLevel(TreeCtrlItem *pItem);
	virtual BOOL IsExpand(TreeCtrlItem *pItem);
	virtual void Expand(TreeCtrlItem *pItem, BOOL fExpand);
	virtual TreeCtrlItem *GetParent(TreeCtrlItem *pItem);
	virtual TreeCtrlItem *GetNextSibling(TreeCtrlItem *pItem);
	virtual TreeCtrlItem *GetFirstChild(TreeCtrlItem *pItem);
	virtual TreeCtrlItem *GetCurSelItem() { return m_pSelect; }
	virtual void DeleteAllItems();
	virtual INT_PTR GetItemData(TreeCtrlItem *pItem);
	virtual BOOL SetItemData(TreeCtrlItem *pItem, INT_PTR pData);
	virtual BOOL IsAutoShowTooltip(){return m_fAutoShowToolTip;}
	virtual void SetAutoShowTooltip(BOOL fAutoShowTooltip);
	virtual TreeCtrlItem * HitTest(LPPOINT lppt);
	

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duTreeCtrl"); }	
	
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
