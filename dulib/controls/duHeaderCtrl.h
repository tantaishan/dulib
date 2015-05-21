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
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);
	
	virtual void WINAPI OnMouseMove(POINT pt);
	virtual void WINAPI OnMouseLeave(POINT pt);
	virtual BOOL WINAPI OnSetCursor();
	virtual void WINAPI OnMouseLDown(POINT pt);
	virtual void WINAPI OnMouseLUp(POINT pt);
	
	virtual BOOL WINAPI InsertItem(int nIndex, LPCTSTR lpszText, BOOL bPercent, int nWidthOrPercent, LPCTSTR lpszIcon);
	virtual int WINAPI GetItemCount();
	virtual int WINAPI GetTotalWidth();
	virtual int WINAPI GetOffsetX() { return m_nOffsetX; }
	virtual void WINAPI SetOffsetX(int nOffsetX);
	virtual UINT WINAPI GetCheckBoxState() { return m_uCheckBoxState; }
	virtual void WINAPI SetCheckBoxState(UINT uState) { m_uCheckBoxState = uState; }
	virtual int WINAPI GetItemWidth(int nIndex);
	virtual void WINAPI SetItemWidth(int nIndex, int nWidth);
	virtual LPCTSTR WINAPI GetItemText(int nIndex);
	virtual void WINAPI SetItemText(int nIndex, LPCTSTR lpszText);
	virtual LPCTSTR WINAPI GetItemIcon(int nIndex);
	virtual void WINAPI SetItemIcon(int nIndex, LPCTSTR lpszIcon);

	// other
	virtual int WINAPI GetSepWidth() { return m_nSepWidth; }
	virtual void WINAPI SetSepWidth(int nSepWidth) { m_nSepWidth = nSepWidth; }

	virtual LPCTSTR WINAPI GetSepImage() { return m_szSepImage; }
	virtual void WINAPI SetSepImage(LPCTSTR lpszImage);
	
	virtual LPCTSTR WINAPI GetListCtrl() { return m_szListCtrl; }
	virtual void WINAPI SetListCtrl(LPCTSTR lpszCtrl);

	virtual int WINAPI GetBorderWidth() { return m_nBorderWidth; }
	virtual void WINAPI SetBorderWidth(int nWidth) { m_nBorderWidth = nWidth; }

	virtual LPCTSTR WINAPI GetItemStyle() { return m_szItemStyle; }
	virtual void WINAPI SetItemStyle(LPCTSTR lpszStyle);

	// icon
	virtual int WINAPI GetIconX() { return m_nIconX; }
	virtual void WINAPI SetIconX(int nIconX) { m_nIconX = nIconX; }

	virtual int WINAPI GetIconY() { return m_nIconY; }
	virtual void WINAPI SetIconY(int nIconY) { m_nIconY = nIconY; }

	virtual int WINAPI GetIconWidth() { return m_nIconWidth; }
	virtual void WINAPI SetIconWidth(int nIconWidth) { m_nIconWidth = nIconWidth; }

	virtual int WINAPI GetIconHeight() { return m_nIconHeight; }
	virtual void WINAPI SetIconHeight(int nIconHeight) { m_nIconHeight = nIconHeight; }

	// sort
	virtual LPCTSTR WINAPI GetSortStyle() { return m_szSortStyle; }
	virtual void WINAPI SetSortStyle(LPCTSTR lpszStyle);

	virtual int WINAPI GetSortX() { return m_nSortX; }
	virtual void WINAPI SetSortX(int nSortX) { m_nSortX = nSortX; }

	virtual int WINAPI GetSortY() { return m_nSortY; }
	virtual void WINAPI SetSortY(int nSortY) { m_nSortY = nSortY; }

	virtual int WINAPI GetSortWidth() { return m_nSortWidth; }
	virtual void WINAPI SetSortWidth(int nSortWidth) { m_nSortWidth = nSortWidth; }

	virtual int WINAPI GetSortHeight() { return m_nSortHeight; }
	virtual void WINAPI SetSortHeight(int nSortHeight) { m_nSortHeight = nSortHeight; }

	// checkbox
	virtual LPCTSTR WINAPI GetCheckBoxImage() { return m_szCheckBox; }
	virtual void WINAPI SetCheckBoxImage(LPCTSTR lpszImage);

	virtual BOOL WINAPI IsShowCheckBox() { return m_fShowCheckBox; }
	virtual void WINAPI SetShowCheckBox(BOOL fShow) { m_fShowCheckBox = fShow; }

	virtual int WINAPI GetCheckBoxX() { return m_nCheckBoxX; }
	virtual void WINAPI SetCheckBoxX(int nX) { m_nCheckBoxX = nX; }

	virtual int WINAPI GetCheckBoxY() { return m_nCheckBoxY; }
	virtual void WINAPI SetCheckBoxY(int nY) { m_nCheckBoxY = nY; }

	virtual int WINAPI GetCheckBoxWidth() { return m_nCheckBoxWidth; }
	virtual void WINAPI SetCheckBoxWidth(int nWidth) { m_nCheckBoxWidth = nWidth; }

	virtual int WINAPI GetCheckBoxHeight() { return m_nCheckBoxHeight; }
	virtual void WINAPI SetCheckBoxHeight(int nHeight) { m_nCheckBoxHeight = nHeight; }


	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duHeaderCtrl"); }	
	
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
