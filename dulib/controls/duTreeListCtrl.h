//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duTreeListCtrl.h
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"
#include "duScrollView.h"
#include "duListCtrl.h"

typedef struct tagTreeListItem
{
	duPlugin *pPlugin;
	UINT nLevel;
	BOOL fExpand;
	struct tagTreeListItem *pParent;
	struct tagTreeListItem *pNextSibling;
	struct tagTreeListItem *pFirstChild;
}TreeListItem;

#define TCVIL_ROOT 				((TreeListItem *)(ULONG_PTR)-0x10000)
#define TCVIL_FIRST 			((TreeListItem *)(ULONG_PTR)-0x0FFFF)
#define TCVIL_LAST 				((TreeListItem *)(ULONG_PTR)-0x0FFFE)

// duTreeListCtrl
//
//
class duTreeListCtrl : public duListCtrl
{
public:
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);
	
	virtual TreeListItem *GetRoot() { return &m_RootItem; }
	virtual TreeListItem *InsertTreeItem(TreeListItem *pParent, TreeListItem *pInsertAfter, LPCTSTR lpszTemplate);
	virtual BOOL DeleteItem(TreeListItem *pItem);
	virtual int GetTreeItemIndex(TreeListItem *pItem);
	virtual TreeListItem *GetNextItem(TreeListItem *pItem);
	virtual void ExpandItem(TreeListItem *pItem, BOOL fExpand);
	virtual TreeListItem *GetNextVisibleItem(TreeListItem *pItem);
	virtual TreeListItem *GetSelTreeItem();
	virtual BOOL SetSelTreeItem(TreeListItem *pItem);

	virtual duPlugin *GetTreeItemLine(TreeListItem *pItem);
	virtual BOOL IsTreeItemExpand(TreeListItem *pItem);
	virtual TreeListItem *GetTreeItemParent(TreeListItem *pItem);
	virtual TreeListItem *GetTreeItemNextSibling(TreeListItem *pItem);
	virtual TreeListItem *GetTreeItemFirstChild(TreeListItem *pItem);
	virtual void OnItemCheckBoxClick(TreeListItem *pItem, BOOL fChecked, BOOL fRedraw);

public:
	duTreeListCtrl();
	~duTreeListCtrl();
	
public:
	int GetInsertItem(TreeListItem *pItem, vector<duPlugin *> &vtInsert);
	void CheckTreeListItem(TreeListItem *pItem, BOOL fChecked);
	UINT GetTreeListItemCheckState(TreeListItem *pItem);
	void SetTreeListItemCheckState(TreeListItem *pItem, UINT uState);
	BOOL IsTreeListItemChecked(TreeListItem *pItem);
	BOOL DeleteItemT(TreeListItem *pItem);
	void SetHeaderCtrlCheckBoxState(UINT uState);
	
protected:
	TreeListItem m_RootItem;
};
