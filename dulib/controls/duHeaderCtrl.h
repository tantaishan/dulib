//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duHeaderCtrl.h
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once

#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"
#include "duListCtrl.h"

//
//
//
typedef struct _tagHeaderCtrlItem
{
	BOOL bPercent;
	int nPercent;
	int nWidth;
	BOOL fShowSort;
	BOOL fSortUp;
	UINT uState;
	dustring strText;
	dustring strIcon;
}HeaderCtrlItem;

#define HEADER_CHECKBOX_NORMAL			1
#define HEADER_CHECKBOX_CHECKED			2
#define HEADER_CHECKBOX_CHECKEDPART		3

// duHeaderCtrl
//
//
class duHeaderCtrl : public duPlugin
{
public:
	friend class duListCtrl;

public:
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);
	
	virtual void OnMouseMove(POINT pt);
	virtual void OnMouseLeave(POINT pt);
	virtual BOOL OnSetCursor();
	virtual void OnMouseLDown(POINT pt);
	virtual void OnMouseLUp(POINT pt);
	
	virtual BOOL InsertItem(int nIndex, LPCTSTR lpszText, BOOL bPercent, int nWidthOrPercent, LPCTSTR lpszIcon);
	virtual int GetItemCount();
	virtual int GetTotalWidth();
	virtual int GetOffsetX() { return m_nOffsetX; }
	virtual void SetOffsetX(int nOffsetX);
	virtual UINT GetCheckBoxState() { return m_uCheckBoxState; }
	virtual void SetCheckBoxState(UINT uState) { m_uCheckBoxState = uState; }
	virtual int GetItemWidth(int nIndex);
	virtual void SetItemWidth(int nIndex, int nWidth);
	virtual LPCTSTR GetItemText(int nIndex);
	virtual void SetItemText(int nIndex, LPCTSTR lpszText);
	virtual LPCTSTR GetItemIcon(int nIndex);
	virtual void SetItemIcon(int nIndex, LPCTSTR lpszIcon);

	// other
	virtual int GetSepWidth() { return m_nSepWidth; }
	virtual void SetSepWidth(int nSepWidth) { m_nSepWidth = nSepWidth; }

	virtual LPCTSTR GetSepImage() { return m_szSepImage; }
	virtual void SetSepImage(LPCTSTR lpszImage);
	
	virtual LPCTSTR GetListCtrl() { return m_szListCtrl; }
	virtual void SetListCtrl(LPCTSTR lpszCtrl);

	virtual int GetBorderWidth() { return m_nBorderWidth; }
	virtual void SetBorderWidth(int nWidth) { m_nBorderWidth = nWidth; }

	virtual LPCTSTR GetItemStyle() { return m_szItemStyle; }
	virtual void SetItemStyle(LPCTSTR lpszStyle);

	// icon
	virtual int GetIconX() { return m_nIconX; }
	virtual void SetIconX(int nIconX) { m_nIconX = nIconX; }

	virtual int GetIconY() { return m_nIconY; }
	virtual void SetIconY(int nIconY) { m_nIconY = nIconY; }

	virtual int GetIconWidth() { return m_nIconWidth; }
	virtual void SetIconWidth(int nIconWidth) { m_nIconWidth = nIconWidth; }

	virtual int GetIconHeight() { return m_nIconHeight; }
	virtual void SetIconHeight(int nIconHeight) { m_nIconHeight = nIconHeight; }

	// sort
	virtual LPCTSTR GetSortStyle() { return m_szSortStyle; }
	virtual void SetSortStyle(LPCTSTR lpszStyle);

	virtual int GetSortX() { return m_nSortX; }
	virtual void SetSortX(int nSortX) { m_nSortX = nSortX; }

	virtual int GetSortY() { return m_nSortY; }
	virtual void SetSortY(int nSortY) { m_nSortY = nSortY; }

	virtual int GetSortWidth() { return m_nSortWidth; }
	virtual void SetSortWidth(int nSortWidth) { m_nSortWidth = nSortWidth; }

	virtual int GetSortHeight() { return m_nSortHeight; }
	virtual void SetSortHeight(int nSortHeight) { m_nSortHeight = nSortHeight; }

	// checkbox
	virtual LPCTSTR GetCheckBoxImage() { return m_szCheckBox; }
	virtual void SetCheckBoxImage(LPCTSTR lpszImage);

	virtual BOOL IsShowCheckBox() { return m_fShowCheckBox; }
	virtual void SetShowCheckBox(BOOL fShow) { m_fShowCheckBox = fShow; }

	virtual int GetCheckBoxX() { return m_nCheckBoxX; }
	virtual void SetCheckBoxX(int nX) { m_nCheckBoxX = nX; }

	virtual int GetCheckBoxY() { return m_nCheckBoxY; }
	virtual void SetCheckBoxY(int nY) { m_nCheckBoxY = nY; }

	virtual int GetCheckBoxWidth() { return m_nCheckBoxWidth; }
	virtual void SetCheckBoxWidth(int nWidth) { m_nCheckBoxWidth = nWidth; }

	virtual int GetCheckBoxHeight() { return m_nCheckBoxHeight; }
	virtual void SetCheckBoxHeight(int nHeight) { m_nCheckBoxHeight = nHeight; }


	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duHeaderCtrl"); }	
	
public:
	duHeaderCtrl();
	~duHeaderCtrl();
	
protected:
	HeaderCtrlItem *GetItem(int nItem);
	int GetItemIndex(HeaderCtrlItem *pHeaderItem);

protected:
	vector<HeaderCtrlItem *> m_vtItems;
	HeaderCtrlItem *m_pHot;
	BOOL m_fTracking;
	POINT m_ptTrack;
	int m_nHotBorder;
	int m_nTrackWidth;
	int m_nOffsetX;
	UINT m_uCheckBoxState;
	
protected:
	int m_nBorderWidth; 
	TCHAR m_szItemStyle[MAX_NAME]; //style
	int m_nIconX;
	int m_nIconY;
	int m_nIconWidth;
	int m_nIconHeight;
	TCHAR m_szSortStyle[MAX_NAME]; //style
	int m_nSortX;
	int m_nSortY;
	int m_nSortWidth;
	int m_nSortHeight;
	
	BOOL m_fFixed;
	TCHAR m_szSepImage[MAX_NAME]; //image
	int m_nSepWidth;
	
	BOOL m_fShowCheckBox;
	int m_nCheckBoxX;
	int m_nCheckBoxY;
	int m_nCheckBoxWidth;
	int m_nCheckBoxHeight;
	TCHAR m_szCheckBox[MAX_NAME]; //image
	
	TCHAR m_szHorzScroll[MAX_NAME]; 
	TCHAR m_szListCtrl[MAX_NAME];
};
