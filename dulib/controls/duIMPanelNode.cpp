//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duIMPanelNode.cpp
//
//  History:    Dec-21-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duIMPanelNode.h"
#include "duListBoxEx.h"

duIMPanelNode::duIMPanelNode() :
	 m_fExpand(FALSE)
	,m_nLevel(0)
{
}

duIMPanelNode::~duIMPanelNode()
{
}

void WINAPI duIMPanelNode::RegisterControlProperty()
{
	RegisterProperty(_T("level"), DU_PROPERTY_LONG, &m_nLevel);
	RegisterProperty(_T("expand"), DU_PROPERTY_BOOL, &m_fExpand);
}

void WINAPI duIMPanelNode::OnCreate()
{
}

void WINAPI duIMPanelNode::DrawObject(HDC hDC)
{
	duRect rcIMPanelNode;
	Plugin_GetRect(this, &rcIMPanelNode);
	rcIMPanelNode.OffsetRect(-rcIMPanelNode.left, -rcIMPanelNode.top);

	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
		pStyleGroup->Draw(hDC, &rcIMPanelNode, GetState(), GetText(), GetAlpha());

}

void WINAPI duIMPanelNode::OnMouseLDown(POINT pt)
{
	int nState = GetState();
	if (IS_STATE_CHECKED(nState))
		SetState(DU_STATE_CHECKEDPRESS);
	else
		SetState(DU_STATE_PRESS);
}

void WINAPI duIMPanelNode::OnMouseLUp(POINT pt)
{
	int nState = GetState();

	if (IS_STATE_CHECKED(nState))
	{
		if (nState == DU_STATE_CHECKEDPRESS)
		{
			SetState(DU_STATE_NORMAL);
			Expand(TRUE);
		}
		else
			SetState(DU_STATE_CHECKED);
	}
	else
	{
		if (nState == DU_STATE_PRESS)
		{
			SetState(DU_STATE_CHECKED);
			Expand(FALSE);
		}
		else
			SetState(DU_STATE_NORMAL);
	}

	Plugin_Redraw(this, TRUE);
}

void WINAPI duIMPanelNode::OnMouseDbClick(POINT pt)
{
	int nState = GetState();
	if (IS_STATE_CHECKED(nState))
	{
		SetState(DU_STATE_NORMAL);
		Expand(TRUE);
	}
	else
	{
		SetState(DU_STATE_CHECKED);
		Expand(FALSE);
	}

	Plugin_Redraw(this, TRUE);
}

void WINAPI duIMPanelNode::Expand(BOOL fExpand)
{
	if (fExpand)
	{
		duPlugin *pNextSibling = GetNextSibling();
		while (pNextSibling)
		{
			LPCTSTR lpszType = pNextSibling->GetType();
			if (lstrcmpi(lpszType, _T("scrollbar")) == 0)
			{
				pNextSibling = pNextSibling->GetNextSibling();
				continue;
			}

			if (lstrcmpi(lpszType, _T("impanelnode")) == 0)
			{
				duIMPanelNode *pIMPanelNode = (duIMPanelNode *)pNextSibling;
				if (pIMPanelNode->GetLevel() == GetLevel())
					break;
			}

			pNextSibling->SetVisible(TRUE);
			pNextSibling = pNextSibling->GetNextSibling();
		}
	}
	else
	{
		duPlugin *pNextSibling = GetNextSibling();
		while (pNextSibling)
		{
			LPCTSTR lpszType = pNextSibling->GetType();
			if (lstrcmpi(lpszType, _T("scrollbar")) == 0)
			{
				pNextSibling = pNextSibling->GetNextSibling();
				continue;
			}

			if (lstrcmpi(lpszType, _T("impanelnode")) == 0)
			{
				duIMPanelNode *pIMPanelNode = (duIMPanelNode *)pNextSibling;
				if (pIMPanelNode->GetLevel() == GetLevel())
					break;
			}

			pNextSibling->SetVisible(FALSE);
			pNextSibling = pNextSibling->GetNextSibling();
		}

		SetState(DU_STATE_CHECKED);
	}

	duPlugin *pParent = GetParent();
	if (lstrcmpi(pParent->GetType(), _T("listboxex")) == 0)
	{
		duListBoxEx *pListBoxEx = (duListBoxEx *)pParent;
		pListBoxEx->RefreshPane();
		Plugin_Redraw(pListBoxEx, TRUE);
	}

	NotifyUser(DUM_IMPEXPAND, (WPARAM)fExpand, NULL);
}

BOOL WINAPI duIMPanelNode::IsExpand()
{
	return m_fExpand;
}

int WINAPI duIMPanelNode::GetLevel()
{
	return m_nLevel;
}
