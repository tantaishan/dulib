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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);
	virtual void OnMouseLDown(POINT pt);
	
	virtual int GetLevel();
	virtual void SetLevel(int nLevel);
	
	virtual BOOL IsExpand();
	virtual void SetExpand(BOOL fExpand);
	
	virtual BOOL IsCheckBoxShow();
	virtual void ShowCheckBox(BOOL fShow);

	virtual UINT GetCheckBoxState() { return m_uCheckBoxState; }
	virtual void SetCheckBoxState(UINT uState) { m_uCheckBoxState = uState; }
	
	virtual TreeListItem *GetTreeListItem() { return m_pTreeListItem; }
	virtual void SetTreeListItem(TreeListItem *pTreeListItem) { m_pTreeListItem = pTreeListItem; }
	
	virtual void Adjust();
	
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
