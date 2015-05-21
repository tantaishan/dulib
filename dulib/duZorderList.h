//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duZorderList.h
//
//  History:    Apl-12-11   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"

typedef struct _tagZorderItem
{
	UINT uZorder;
	vector<duPlugin *> vtPlugin;
}ZorderItem;

// duZorderList
//
//
class duZorderList
{
public:
	duZorderList();
	duZorderList(duPlugin *pRoot);
	virtual ~duZorderList();

public:
	BOOL Add(duPlugin *pPlugin);
	BOOL Modify(duPlugin *pPlugin);
	BOOL Delete(duPlugin *pPlugin);
	void DispatchOnCreate();
	
	int GetLayerCount();
	int GetLayerChildCount(int nLayer);
	duPlugin *GetPlugin(int nLayer, int nIndex);
	int GetZorderLayer(UINT uZorder);
	int GetNextLayer(UINT uZorder);
	UINT GetLayerZorder(int nLayer);
	duPlugin *GetRoot() { return m_pRoot; }
	
protected:
	void Init();
	void Destroy();
	BOOL DeleteZorderItem(ZorderItem *pZorderItem);

protected:
	deque<ZorderItem *> m_dqZorderList;
	duPlugin *m_pRoot;
};
