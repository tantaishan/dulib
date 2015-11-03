//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duZorderList.cpp
//
//  History:    Apl-12-11   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "duZorderList.h"
#include "duCtrlManager.h"

duZorderList::duZorderList()
{
	m_pRoot = NULL;
	m_dqZorderList.clear();
}

duZorderList::duZorderList(duPlugin *pRoot) :
	m_pRoot(pRoot)
{
	m_dqZorderList.clear();
	Init();
}

duZorderList::~duZorderList()
{
	Destroy();
}

void duZorderList::Init()
{
	if (m_pRoot == NULL)
		return;

	LPCTSTR lpszType = m_pRoot->GetType();
	if (lstrcmpi(lpszType, _T("duwindow")) != 0)
	{
		m_pRoot = NULL;
		return;
	}
	
	deque<duPlugin *> dqPlugin;
	dqPlugin.clear();
	dqPlugin.push_back(m_pRoot);
	
	duPlugin *pPlugin = NULL;
	while (!dqPlugin.empty())
	{
		pPlugin = dqPlugin.at(0); 
		dqPlugin.pop_front();
		
		if (!Add(pPlugin))
		{
			dqPlugin.clear();
			Destroy();
			return;
		}
		
		pPlugin = pPlugin->GetFirstChild();
		while (pPlugin)
		{
			dqPlugin.push_back(pPlugin);
			pPlugin = pPlugin->GetNextSibling();
		}
	}
	dqPlugin.clear();
}

void duZorderList::Destroy()
{
	int i;
	int nLayerCount = GetLayerCount();
	ZorderItem *pZorderItem = NULL;
	for (i = 0;i < nLayerCount; i++)
	{
		pZorderItem = m_dqZorderList[i];
		if (pZorderItem)
		{
			pZorderItem->vtPlugin.clear();
			delete pZorderItem;
			pZorderItem = NULL;
		}
	}
	m_dqZorderList.clear();
	m_pRoot = NULL;
}

BOOL duZorderList::DeleteZorderItem(ZorderItem *pZorderItem)
{
	if (pZorderItem == NULL)
		return FALSE;
		
	deque<ZorderItem *>::iterator iterDelete = m_dqZorderList.begin();
	for (; iterDelete != m_dqZorderList.end(); iterDelete++)
	{
		if (pZorderItem == *iterDelete)
		{
			delete pZorderItem;
			m_dqZorderList.erase(iterDelete);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL duZorderList::Add(duPlugin *pPlugin)
{
	if (pPlugin == NULL)
		return FALSE;

	int nLayerCount = GetLayerCount();
	ZorderItem *pZorderItem = NULL;
	int i;
	UINT uZorder = pPlugin->GetZorder();
	for (i = 0;i < nLayerCount; i++)
	{
		pZorderItem = m_dqZorderList[i];
		if (pZorderItem == NULL)
			return FALSE;
		
		if (pZorderItem->uZorder == uZorder)
		{
			pZorderItem->vtPlugin.push_back(pPlugin);
			return TRUE;
		}
	}
	
	ZorderItem *pNewZorderItem = new ZorderItem;
	pNewZorderItem->uZorder = uZorder;
	pNewZorderItem->vtPlugin.clear();
	pNewZorderItem->vtPlugin.push_back(pPlugin);

	for (i = 0;i < nLayerCount; i++)
	{
		pZorderItem = m_dqZorderList[i];
		if (pZorderItem == NULL)
			return FALSE;
		
		if (pZorderItem->uZorder < uZorder)
			break;
	}
	
	deque<ZorderItem *>::iterator iterInsert = m_dqZorderList.begin() + i;
	m_dqZorderList.insert(iterInsert, pNewZorderItem);
	return TRUE;
}

BOOL duZorderList::Modify(duPlugin *pPlugin)
{	
	if (Delete(pPlugin))
		return Add(pPlugin);

	return FALSE;
}

BOOL duZorderList::Delete(duPlugin *pPlugin)
{
	if (pPlugin == NULL)
		return FALSE;

	int nLayerCount = GetLayerCount();
	int i;
	UINT uZorder = pPlugin->GetZorder();
	duPlugin *pTempPlugin = NULL;
	for (i = 0;i < nLayerCount; i++)
	{
		ZorderItem *pZorderItem = m_dqZorderList[i];
		if (pZorderItem == NULL)
			return FALSE;

		if (pZorderItem->uZorder != uZorder)
			continue;

		vector<duPlugin *>::iterator iterPlugin = pZorderItem->vtPlugin.begin();
		for ( ;iterPlugin != pZorderItem->vtPlugin.end(); iterPlugin++)
		{
			pTempPlugin = *iterPlugin;
			if (pTempPlugin != pPlugin)
				continue;
			
			pZorderItem->vtPlugin.erase(iterPlugin);
			if (pZorderItem->vtPlugin.empty())
				DeleteZorderItem(pZorderItem);
			return TRUE;
		}
	}
	
	return FALSE;
}

void duZorderList::DispatchOnCreate()
{
	duCtrlManager *pCtrlManager = GetCtrlManager(m_pRoot->GetHwnd());
	if (pCtrlManager == NULL)
		return;

	duPlugin *pPlugin = NULL;
	int i, j;
	int nLevelCount = GetLayerCount();
	for (i = 0;i < nLevelCount; i++)
	{
		int nCtrlCount = GetLayerChildCount(i);
		for (j = nCtrlCount - 1;j >= 0; j--)
		{
			pPlugin = GetPlugin(i, j);
			if (pPlugin == NULL || !Plugin_IsValid(pPlugin))
				continue;

			pCtrlManager->AddName(pPlugin);
		}
	}

	for (i = 0;i < nLevelCount; i++)
	{
		int nCtrlCount = GetLayerChildCount(i);
		for (j = nCtrlCount - 1;j >= 0; j--)
		{
			pPlugin = GetPlugin(i, j);
			if (pPlugin == NULL || !Plugin_IsValid(pPlugin))
				continue;

			pPlugin->OnCreate();
		}
	}
}

int duZorderList::GetLayerCount()
{
	return (int)m_dqZorderList.size();
}

int duZorderList::GetLayerChildCount(int nLayer)
{
	int nLayerCount = GetLayerCount();
	if (nLayer < 0 || nLayer >= nLayerCount)
		return 0;
	
	ZorderItem *pZorderItem = m_dqZorderList[nLayer];
	if (pZorderItem == NULL)
		return 0;

	return (int)pZorderItem->vtPlugin.size();
}

duPlugin *duZorderList::GetPlugin(int nLayer, int nIndex)
{
	int nLayerCount = GetLayerCount();
	if (nLayer < 0 || nLayer >= nLayerCount)
		return NULL;
	
	ZorderItem *pZorderItem = m_dqZorderList[nLayer];
	if (pZorderItem == NULL)
		return NULL;
		
	if (nIndex < 0 || nIndex >= (int)pZorderItem->vtPlugin.size())
		return NULL;
	
	return pZorderItem->vtPlugin[nIndex];
}

int duZorderList::GetZorderLayer(UINT uZorder)
{
	int nLayerCount = GetLayerCount();
	ZorderItem *pZorderItem = NULL;
	int i;
	for (i = 0;i < nLayerCount; i++)
	{
		pZorderItem = m_dqZorderList[i];
		if (pZorderItem == NULL)
			return FALSE;
		
		if (pZorderItem->uZorder == uZorder)		
			return i;
	}
	
	return -1;
}

int duZorderList::GetNextLayer(UINT uZorder)
{
	int nLayerCount = GetLayerCount();
	ZorderItem *pZorderItem = NULL;
	int i;
	for (i = 0;i < nLayerCount; i++)
	{
		pZorderItem = m_dqZorderList[i];
		if (pZorderItem == NULL)
			return FALSE;
		
		if (pZorderItem->uZorder > uZorder)		
			return i;
	}
	
	return -1;
}

UINT duZorderList::GetLayerZorder(int nLayer)
{
	int nLayerCount = GetLayerCount();
	if (nLayer < 0 || nLayer >= nLayerCount)
		return 0;
	
	ZorderItem *pZorderItem = m_dqZorderList[nLayer];
	if (pZorderItem == NULL)
		return 0;

	return pZorderItem->uZorder;
}

