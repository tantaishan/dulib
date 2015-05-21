//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duTreeListCtrl.cpp
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duTreeListCtrl.h"
#include "duButton.h"
#include "duTreeListItem.h"
#include "duHeaderCtrl.h"

duTreeListCtrl::duTreeListCtrl()
{

}

duTreeListCtrl::~duTreeListCtrl()
{
	
}

void WINAPI duTreeListCtrl::RegisterControlProperty()
{
	duListCtrl::RegisterControlProperty();
	

}

void WINAPI duTreeListCtrl::OnCreate()
{
	duListCtrl::OnCreate();
	
	m_RootItem.pPlugin = NULL;
	m_RootItem.nLevel = 0;
	m_RootItem.fExpand = TRUE;
	m_RootItem.pParent = NULL;
	m_RootItem.pNextSibling = NULL;
	m_RootItem.pFirstChild = NULL;
}

void WINAPI duTreeListCtrl::DrawObject(HDC hDC)
{
	
}

TreeListItem *WINAPI duTreeListCtrl::InsertTreeItem(TreeListItem *pParent, TreeListItem *pInsertAfter, LPCTSTR lpszTemplate)
{
	if (lpszTemplate == NULL)
		return NULL;

	if ((pParent == NULL) || (pParent == TCVIL_ROOT))
		pParent = &m_RootItem;
	
	TreeListItem *pNewItem = new TreeListItem;
	pNewItem->pPlugin = NULL;
	pNewItem->nLevel = 0;
	pNewItem->fExpand = TRUE;
	pNewItem->pParent = NULL;
	pNewItem->pNextSibling = NULL;
	pNewItem->pFirstChild = NULL;
	
	if ((pInsertAfter == TCVIL_FIRST) || !pParent->pFirstChild)
	{
		pNewItem->pNextSibling = pParent->pFirstChild;
		pParent->pFirstChild = pNewItem;
	}
	else
	{
		TreeListItem *pItem = NULL;
		if (pInsertAfter == TCVIL_LAST)
		{
			for (pItem = pParent->pFirstChild; pItem->pNextSibling; pItem = pItem->pNextSibling)
				;
		}
		else
		{
			for (pItem = pParent->pFirstChild; pItem->pNextSibling; pItem = pItem->pNextSibling)
				if (pItem == pInsertAfter)
					break;
		}

		pNewItem->pNextSibling = pItem->pNextSibling;
		pItem->pNextSibling = pNewItem;
	}

	pNewItem->pParent = pParent;
	pNewItem->nLevel = pParent->nLevel + 1;

	int nIndex = GetTreeItemIndex(pNewItem);
	pNewItem->pPlugin = duListCtrl::InsertLine(nIndex, lpszTemplate);
	if (pNewItem->pPlugin == NULL)
	{
		delete pNewItem;
		return NULL;
	}
	
	duPlugin *pFirstCol = pNewItem->pPlugin->GetFirstChild();
	if (pFirstCol && lstrcmpi(pFirstCol->GetType(), _T("treelistitem")) == 0)
	{
		duTreeListItem *pTreeListItem = (duTreeListItem *)pFirstCol;
		pTreeListItem->SetLevel(pNewItem->nLevel);
		pTreeListItem->SetExpand(pNewItem->fExpand);
		pTreeListItem->ShowCheckBox(TRUE);
		pTreeListItem->SetTreeListItem(pNewItem);
		pTreeListItem->Adjust();

		//TCHAR szNameStatic[256];
		//_stprintf(szNameStatic, _T("Name %d"), m_nCloneIndex);

		//pFirstCol->GetFirstChild()->GetFirstChild()->GetNextSibling()->SetText(szNameStatic);

		//pNewItem->pPlugin->SetText(szNameStatic);
	}
	
	return pNewItem;
}

TreeListItem *WINAPI duTreeListCtrl::GetNextItem(TreeListItem *pItem)
{
	if (pItem->pFirstChild)
		return pItem->pFirstChild;

checkNext:
	if (pItem->pNextSibling)
		return pItem->pNextSibling;

	pItem = pItem->pParent;
	if (pItem)
		goto checkNext;

	return NULL;
}

TreeListItem *WINAPI duTreeListCtrl::GetNextVisibleItem(TreeListItem *pItem)
{
	if (pItem->pFirstChild && pItem->fExpand)
		return pItem->pFirstChild;

checkNext:
	if (pItem->pNextSibling)
		return pItem->pNextSibling;

	pItem = pItem->pParent;
	if (pItem)
		goto checkNext;

	return NULL;
}

int WINAPI duTreeListCtrl::GetTreeItemIndex(TreeListItem *pItem)
{
	int nIndex = 0;
	TreeListItem *pNext = GetNextVisibleItem(&m_RootItem);
	while (pNext)
	{
		if (pNext == pItem)
			return nIndex;

		
		nIndex++;
		pNext = GetNextVisibleItem(pNext);
	}

	return -1;
}

void WINAPI duTreeListCtrl::ExpandItem(TreeListItem *pItem, BOOL fExpand)
{
	if (pItem == NULL || pItem == &m_RootItem)
		return;

	if (pItem->fExpand == fExpand || pItem->pFirstChild == NULL)
		return;
		
	duPlugin *pNextSibling = NULL;
	if (pItem->pNextSibling)
		pNextSibling = pItem->pNextSibling->pPlugin;
	
	if (!fExpand)
	{
		pItem->fExpand = FALSE;
		
		vector<int> vtRemoveIndex;
		vtRemoveIndex.clear();

		//////////////////////////////////////////////////////////////////////////////////////
		// TreeListItem
		// bug: nIndex return -1
		// 
		// 
		// 
		// 
		//////////////////////////////////////////////////////////////////////////////////////
		
		int i;
		int nIndex = GetTreeItemIndex(pItem);
		for (i = nIndex + 1;i < GetLineCount(); i++)
		{
			duPlugin *pTemp = m_vtLines[i];
			if (pTemp == pNextSibling)
				break;
		
			duRect rcTemp;
			pTemp->GetRect(&rcTemp);
			pTemp->SetVisible(FALSE);

			m_nViewCy -= rcTemp.Height();
			vtRemoveIndex.push_back(i);
		}

		m_vtLines.erase(m_vtLines.begin() + nIndex + 1, m_vtLines.begin() + i);
		UpdateScroll();
		CompleteAdjustLine();
	}
	else
	{
		pItem->fExpand = TRUE;
		
		vector<duPlugin *> vtInsert;
		vtInsert.clear();
		int nIndex = GetInsertItem(pItem, vtInsert);
		
		int i;
		int nInsertCount = vtInsert.size();
		for (i = 0;i < nInsertCount; i++)
		{
			duPlugin *pTemp = vtInsert[i];
			
			duRect rcTemp;
			pTemp->GetRect(&rcTemp);
			
			m_nViewCy += rcTemp.Height();
			m_vtLines.insert(m_vtLines.begin() + nIndex + i + 1, pTemp);
		}
		
		UpdateScroll();
		CompleteAdjustLine();
	}
	
	duPlugin *pFirstCol = pItem->pPlugin->GetFirstChild();
	if (pFirstCol && lstrcmpi(pFirstCol->GetType(), _T("treelistitem")) == 0)
	{
		duTreeListItem *pTreeListItem = (duTreeListItem *)pFirstCol;
		pTreeListItem->SetExpand(fExpand);
	}

	Plugin_Redraw(this, TRUE);
}

int duTreeListCtrl::GetInsertItem(TreeListItem *pItem, vector<duPlugin *> &vtInsert)
{
	int nIndex = GetTreeItemIndex(pItem);
	TreeListItem *pNext = GetNextVisibleItem(pItem);
	while (pNext)
	{
		if (pNext == pItem->pNextSibling)
			break;
		
		vtInsert.push_back(pNext->pPlugin);
		pNext = GetNextVisibleItem(pNext);
	}
	
	return nIndex;
}

void WINAPI duTreeListCtrl::OnItemCheckBoxClick(TreeListItem *pItem, BOOL fChecked, BOOL fRedraw)
{
	if (pItem == NULL || pItem == &m_RootItem)
		return;

	//
	// check parent item
	//
	TreeListItem *pParent = pItem->pParent;
	if (pParent != NULL && pParent != &m_RootItem)
	{
		while (pParent != &m_RootItem)
		{
			//
			// loop check pNextSibling state
			//
			TreeListItem *pChild = pParent->pFirstChild;
			while (pChild)
			{
				if (IsTreeListItemChecked(pChild) != fChecked)
				{
					SetTreeListItemCheckState(pParent, HEADER_CHECKBOX_CHECKEDPART);
					goto Parent_loopNext;
				}

				pChild = pChild->pNextSibling;
			}
			
			SetTreeListItemCheckState(pParent, fChecked ? HEADER_CHECKBOX_CHECKED : HEADER_CHECKBOX_NORMAL);

Parent_loopNext:
			pParent = pParent->pParent;
		}
	}

	//
	// using deque travel child item tree
	//
	deque<TreeListItem *> dqChild;
	dqChild.clear();
	dqChild.push_back(pItem);

	while (!dqChild.empty())
	{
		TreeListItem *pChild = dqChild.at(0); 
		dqChild.pop_front();

		pChild = pChild->pFirstChild;
		while (pChild)
		{
			CheckTreeListItem(pChild, fChecked);

			dqChild.push_back(pChild);
			pChild = pChild->pNextSibling;
		}
	}
	dqChild.clear();
	
	CheckTreeListItem(pItem, fChecked);


	//
	// check header ctrl
	//
	TreeListItem *pChild = m_RootItem.pFirstChild;
	while (pChild)
	{
		if (IsTreeListItemChecked(pChild) != fChecked)
		{
			SetHeaderCtrlCheckBoxState(HEADER_CHECKBOX_CHECKEDPART);
			goto OnItemCheckBoxClick_end;
		}
		
		pChild = pChild->pNextSibling;
	}
	
	SetHeaderCtrlCheckBoxState(fChecked ? HEADER_CHECKBOX_CHECKED : HEADER_CHECKBOX_NORMAL);
	
OnItemCheckBoxClick_end:


	if (fRedraw)
		Plugin_Redraw(this, TRUE);
}

void duTreeListCtrl::SetHeaderCtrlCheckBoxState(UINT uState)
{
	duHeaderCtrl *pHeaderCtrl = (duHeaderCtrl *)GetPluginByName(m_szHeaderCtrl);
	if (pHeaderCtrl == NULL)
		return;
		
	pHeaderCtrl->SetCheckBoxState(uState);
	Plugin_Redraw(pHeaderCtrl, TRUE);
}

void duTreeListCtrl::CheckTreeListItem(TreeListItem *pItem, BOOL fChecked)
{
	if (pItem == NULL || pItem == &m_RootItem)
		return;

	duPlugin *pPlugin = pItem->pPlugin;
	if (pPlugin)
	{
		duPlugin *pFirstPlugin = pPlugin->GetFirstChild();
		if (pFirstPlugin && lstrcmpi(pFirstPlugin->GetType(), _T("treelistitem")) == 0)
		{
			duTreeListItem *pTreeListItem = (duTreeListItem *)pFirstPlugin;
			pTreeListItem->SetCheckBoxState(fChecked ? HEADER_CHECKBOX_CHECKED : HEADER_CHECKBOX_NORMAL);
		}
	}
}

UINT duTreeListCtrl::GetTreeListItemCheckState(TreeListItem *pItem)
{
	if (pItem == NULL || pItem == &m_RootItem)
		return HEADER_CHECKBOX_NORMAL;

	duPlugin *pPlugin = pItem->pPlugin;
	if (pPlugin)
	{
		duPlugin *pFirstPlugin = pPlugin->GetFirstChild();
		if (pFirstPlugin && lstrcmpi(pFirstPlugin->GetType(), _T("treelistitem")) == 0)
		{
			duTreeListItem *pTreeListItem = (duTreeListItem *)pFirstPlugin;
			return pTreeListItem->GetCheckBoxState();
		}
	}

	return HEADER_CHECKBOX_NORMAL;
}

void duTreeListCtrl::SetTreeListItemCheckState(TreeListItem *pItem, UINT uState)
{
	if (pItem == NULL || pItem == &m_RootItem)
		return;

	duPlugin *pPlugin = pItem->pPlugin;
	if (pPlugin)
	{
		duPlugin *pFirstPlugin = pPlugin->GetFirstChild();
		if (pFirstPlugin && lstrcmpi(pFirstPlugin->GetType(), _T("treelistitem")) == 0)
		{
			duTreeListItem *pTreeListItem = (duTreeListItem *)pFirstPlugin;
			pTreeListItem->SetCheckBoxState(uState);
		}
	}
}

BOOL duTreeListCtrl::IsTreeListItemChecked(TreeListItem *pItem)
{
	if (pItem == NULL || pItem == &m_RootItem)
		return FALSE;

	duPlugin *pPlugin = pItem->pPlugin;
	if (pPlugin)
	{
		duPlugin *pFirstPlugin = pPlugin->GetFirstChild();
		if (pFirstPlugin && lstrcmpi(pFirstPlugin->GetType(), _T("treelistitem")) == 0)
		{
			duTreeListItem *pTreeListItem = (duTreeListItem *)pFirstPlugin;
			return (pTreeListItem->GetCheckBoxState() == HEADER_CHECKBOX_CHECKED ? TRUE : FALSE);
		}
	}

	return FALSE;
}

BOOL WINAPI duTreeListCtrl::DeleteItem(TreeListItem *pItem)
{
	if (pItem == TCVIL_ROOT || pItem == NULL)
		pItem = &m_RootItem;

	if (pItem != &m_RootItem)
	{
		TreeListItem *pPreItem = NULL;
		TreeListItem *pParent = pItem->pParent;
		pPreItem = pParent->pFirstChild;
		for (; pPreItem->pNextSibling; pPreItem = pPreItem->pNextSibling)
			if (pPreItem->pNextSibling == pItem)
				break;

		if (pParent->pFirstChild == pItem)
			pParent->pFirstChild = pItem->pNextSibling;
		else
			pPreItem->pNextSibling = pItem->pNextSibling;

		DeleteItemT(pItem);
	}
	else
	{
		DeleteItemT(pItem);
		m_RootItem.pFirstChild = NULL;
	}

	UpdateScroll();
	CompleteAdjustLine();
	return TRUE;
}

BOOL duTreeListCtrl::DeleteItemT(TreeListItem *pItem)
{
	if (pItem == NULL)
		return FALSE;

	TreeListItem *pChildItem = pItem->pFirstChild;
	while (pChildItem)
	{
		TreeListItem *pDelItem = pChildItem;
		pChildItem = pChildItem->pNextSibling;
		DeleteItemT(pDelItem);
	}

	if (pItem != &m_RootItem)
	{
		if (m_pSel == pItem->pPlugin)
			m_pSel = NULL;
	
		int nIndex = GetLinePluginIndex(pItem->pPlugin);
		if (nIndex == -1)
		{
			Plugin_Delete(pItem->pPlugin);
		}

		DeleteLine(nIndex, FALSE);
		delete pItem;
	}

	return TRUE;
}

TreeListItem *WINAPI duTreeListCtrl::GetSelTreeItem()
{
	if (m_pSel == NULL)
		return NULL;
	
	TreeListItem *pItem = m_RootItem.pFirstChild;
	while (pItem)
	{
		if (m_pSel == pItem->pPlugin)
			return pItem;
		
		pItem = GetNextItem(pItem);
	}
	
	return NULL;
}

BOOL WINAPI duTreeListCtrl::SetSelTreeItem(TreeListItem *pItem)
{
	if (pItem == &m_RootItem)
		return FALSE;
		
	if (pItem)
	{
		if (pItem->pPlugin == m_pSel)
			return FALSE;
		
		if (pItem->pPlugin)
		{
			pItem->pPlugin->SetState(DU_STATE_OVER);
			Plugin_Redraw(pItem->pPlugin, TRUE);
		}
	}

	if (m_pSel)
	{
		m_pSel->SetState(DU_STATE_NORMAL);
		if (m_pSel->IsVisible())
			Plugin_Redraw(m_pSel, TRUE);
	}

	if (pItem)
		m_pSel = pItem->pPlugin;
	else
		m_pSel = NULL;
	
	return TRUE;
}

duPlugin *WINAPI duTreeListCtrl::GetTreeItemLine(TreeListItem *pItem)
{
	if (pItem)
		return pItem->pPlugin;

	return NULL;
}

BOOL WINAPI duTreeListCtrl::IsTreeItemExpand(TreeListItem *pItem)
{
	if (pItem)
		return pItem->fExpand;
	
	return FALSE;
}

TreeListItem *WINAPI duTreeListCtrl::GetTreeItemParent(TreeListItem *pItem)
{
	if (pItem)
		return pItem->pParent;
	
	return NULL;
}

TreeListItem *WINAPI duTreeListCtrl::GetTreeItemNextSibling(TreeListItem *pItem)
{
	if (pItem)
		return pItem->pNextSibling;
	return NULL;
}

TreeListItem *WINAPI duTreeListCtrl::GetTreeItemFirstChild(TreeListItem *pItem)
{
	if (pItem)
		return pItem->pFirstChild;
	return NULL;
}