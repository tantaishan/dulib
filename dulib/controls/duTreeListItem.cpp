//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duTreeListItem.cpp
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "duTreeListItem.h"
#include "duHeaderCtrl.h"

duTreeListItem::duTreeListItem() :
	 m_nIndentStart(0)
	,m_nExpandSpace(0)
	,m_nIndentWidth(0)
	,m_nIndentHeight(0)
	,m_nCheckBoxWidth(0)
	,m_nCheckBoxHeight(0)
	,m_nIndentSpace(0)
	,m_nCheckBoxSpace(0)
	,m_nLevel(0)
	,m_fExpand(FALSE)
	,m_fShowCheckBox(FALSE)
	,m_uCheckBoxState(HEADER_CHECKBOX_NORMAL)
	,m_pTreeListItem(NULL)
{
	ZeroMemory(m_szIndentStyle, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szCheckBoxImage, MAX_NAME * sizeof(TCHAR));
	
}

duTreeListItem::~duTreeListItem()
{

}

void duTreeListItem::RegisterControlProperty()
{
	RegisterProperty(_T("indentx"), DU_PROPERTY_LONG, &m_nIndentStart);

	RegisterProperty(_T("indentstyle"), DU_PROPERTY_STYLEGROUP, m_szIndentStyle);
	RegisterProperty(_T("indentwidth"), DU_PROPERTY_LONG, &m_nIndentWidth);
	RegisterProperty(_T("indentheight"), DU_PROPERTY_LONG, &m_nIndentHeight);
	
	RegisterProperty(_T("checkboximage"), DU_PROPERTY_IMAGE, m_szCheckBoxImage);
	RegisterProperty(_T("checkboxwidth"), DU_PROPERTY_LONG, &m_nCheckBoxWidth);
	RegisterProperty(_T("checkboxheight"), DU_PROPERTY_LONG, &m_nCheckBoxHeight);
	
	RegisterProperty(_T("indentspace"), DU_PROPERTY_LONG, &m_nIndentSpace);
	RegisterProperty(_T("checkboxspace"), DU_PROPERTY_LONG, &m_nCheckBoxSpace);
	
	RegisterProperty(_T("expandspace"), DU_PROPERTY_LONG, &m_nExpandSpace);	
}

void duTreeListItem::OnCreate()
{
	
}

void duTreeListItem::DrawObject(HDC hDC)
{
	duRect rcTreeListItem;
	GetRect(&rcTreeListItem);
	rcTreeListItem.OffsetRect(-rcTreeListItem.left, -rcTreeListItem.top);
	
	duRect rcIndent;
	rcIndent.left = m_nIndentStart + (m_nLevel - 1) * m_nExpandSpace;
	rcIndent.right = rcIndent.left + m_nIndentWidth;
	rcIndent.top = (rcTreeListItem.Height() - m_nIndentHeight) / 2;
	rcIndent.bottom = rcIndent.top + m_nIndentHeight;
	
	if (m_pTreeListItem && m_pTreeListItem->pFirstChild)
	{
		duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(m_szIndentStyle, DU_RES_STYLEGROUP);
		if (pStyleGroup)
			pStyleGroup->Draw(hDC, &rcIndent, m_fExpand ? DU_STATE_NORMAL : DU_STATE_CHECKED, NULL, GetAlpha());
	}

	if (m_fShowCheckBox)
	{
		duImage *pCheckBoxImage = (duImage *)GetResObj(m_szCheckBoxImage, DU_RES_IMAGE);
		if (pCheckBoxImage)
		{
			duRect rcCheckBox;
			rcCheckBox.left = rcIndent.right + m_nIndentSpace;
			rcCheckBox.right = rcCheckBox.left + m_nCheckBoxWidth;
			rcCheckBox.top = (rcTreeListItem.Height() - m_nCheckBoxHeight) / 2;
			rcCheckBox.bottom = rcCheckBox.top + m_nCheckBoxHeight;
			
			int nCheckBoxImageLeft = 0;
			if (m_uCheckBoxState == HEADER_CHECKBOX_CHECKED)
				nCheckBoxImageLeft = m_nCheckBoxWidth;
			else if (m_uCheckBoxState == HEADER_CHECKBOX_CHECKEDPART)
				nCheckBoxImageLeft = m_nCheckBoxWidth + m_nCheckBoxWidth;
						
			DrawNormal(hDC, rcCheckBox.left, rcCheckBox.top, rcCheckBox.Width(), rcCheckBox.Height(), 
							pCheckBoxImage, nCheckBoxImageLeft, 0, GetAlpha());
			
		}
	}
}

void duTreeListItem::OnMouseLDown(POINT pt)
{
	if (m_pTreeListItem == NULL)
		return;
			
	duRect rcTreeListItem;
	GetRect(&rcTreeListItem);

	duRect rcIndent;
	rcIndent.left = rcTreeListItem.left + m_nIndentStart + (m_nLevel - 1) * m_nExpandSpace;
	rcIndent.right = rcIndent.left + m_nIndentWidth;
	rcIndent.top = rcTreeListItem.top + (rcTreeListItem.Height() - m_nIndentHeight) / 2;
	rcIndent.bottom = rcIndent.top + m_nIndentHeight;
	
	if (rcIndent.PtInRect(pt))
	{
		duTreeListCtrl *pTreeListCtrl = GetTreeListCtrl();
		if (pTreeListCtrl == NULL)
			return;
		
		m_fExpand = !m_fExpand;
		pTreeListCtrl->ExpandItem(m_pTreeListItem, m_fExpand);
		return;
	}
	
	if (m_fShowCheckBox)
	{
		duRect rcCheckBox;
		rcCheckBox.left = rcIndent.right + m_nIndentSpace;
		rcCheckBox.right = rcCheckBox.left + m_nCheckBoxWidth;
		rcCheckBox.top = rcTreeListItem.top + (rcTreeListItem.Height() - m_nCheckBoxHeight) / 2;
		rcCheckBox.bottom = rcCheckBox.top + m_nCheckBoxHeight;
		
		if (rcCheckBox.PtInRect(pt))
		{
			//_TRACE(_T("Name: %s Point hit CheckBox\n"), GetName());
			duTreeListCtrl *pTreeListCtrl = GetTreeListCtrl();
			if (pTreeListCtrl == NULL)
				return;
			
			if (m_uCheckBoxState == HEADER_CHECKBOX_NORMAL)
				m_uCheckBoxState = HEADER_CHECKBOX_CHECKED;
			else
				m_uCheckBoxState = HEADER_CHECKBOX_NORMAL;

			pTreeListCtrl->OnItemCheckBoxClick(m_pTreeListItem, m_uCheckBoxState == HEADER_CHECKBOX_CHECKED ? TRUE : FALSE, TRUE);
			//NotifyUser(DUM_TREELISTITEMCHECKED, (WPARAM)m_pTreeListItem, (LPARAM)m_uCheckBoxState);
		}
	}
}

int duTreeListItem::GetLevel()
{
	return m_nLevel;
}

void duTreeListItem::SetLevel(int nLevel)
{
	m_nLevel = nLevel;
}

BOOL duTreeListItem::IsExpand()
{
	return m_fExpand;
}

void duTreeListItem::SetExpand(BOOL fExpand)
{
	m_fExpand = fExpand;
}
	
BOOL duTreeListItem::IsCheckBoxShow()
{
	return m_fShowCheckBox;
}

void duTreeListItem::ShowCheckBox(BOOL fShow)
{
	m_fShowCheckBox = fShow;
}

void duTreeListItem::Adjust()
{
	duPlugin *pPlugin = GetFirstChild();
	if (pPlugin == NULL)
		return;

	int nLeft = m_nIndentStart + (m_nLevel - 1) * m_nExpandSpace + m_nIndentWidth + m_nIndentSpace;
	if (m_fShowCheckBox)
		nLeft += (m_nCheckBoxWidth + m_nCheckBoxSpace);
	
	pPlugin->SetPosLeft(nLeft);
	pPlugin->Resize(NULL);
}

duTreeListCtrl *duTreeListItem::GetTreeListCtrl()
{
	duPlugin *pParent = GetParent();
	if (pParent == NULL)
		return NULL;

	pParent = pParent->GetParent();
	if (pParent == NULL)
		return NULL;

	if (lstrcmpi(pParent->GetType(), _T("treelistctrl")) == 0)
		return (duTreeListCtrl *)pParent;
	
	return NULL;
}


