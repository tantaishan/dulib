//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duTreeListItem.h
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"
#include "duScrollView.h"
#include "duTreeListCtrl.h"

//
//
//
class duTreeListItem : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);
	virtual void WINAPI OnMouseLDown(POINT pt);
	
	virtual int WINAPI GetLevel();
	virtual void WINAPI SetLevel(int nLevel);
	
	virtual BOOL WINAPI IsExpand();
	virtual void WINAPI SetExpand(BOOL fExpand);
	
	virtual BOOL WINAPI IsCheckBoxShow();
	virtual void WINAPI ShowCheckBox(BOOL fShow);

	virtual UINT WINAPI GetCheckBoxState() { return m_uCheckBoxState; }
	virtual void WINAPI SetCheckBoxState(UINT uState) { m_uCheckBoxState = uState; }
	
	virtual TreeListItem *WINAPI GetTreeListItem() { return m_pTreeListItem; }
	virtual void WINAPI SetTreeListItem(TreeListItem *pTreeListItem) { m_pTreeListItem = pTreeListItem; }
	
	virtual void WINAPI Adjust();
	
public:
	duTreeListItem();
	~duTreeListItem();
	
public:
	duTreeListCtrl *GetTreeListCtrl();
	void CheckBoxClick();
	
protected:
	int m_nLevel;
	BOOL m_fExpand;
	BOOL m_fShowCheckBox;
	UINT m_uCheckBoxState;
	TreeListItem *m_pTreeListItem;
	
protected:
	int m_nIndentStart;
	TCHAR m_szIndentStyle[MAX_NAME];
	int m_nExpandSpace;
	int m_nIndentWidth;
	int m_nIndentHeight;
	TCHAR m_szCheckBoxImage[MAX_NAME];
	int m_nCheckBoxWidth;
	int m_nCheckBoxHeight;
	int m_nIndentSpace;
	int m_nCheckBoxSpace;
};
