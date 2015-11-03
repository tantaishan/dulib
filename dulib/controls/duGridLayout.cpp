//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duGridLayout.cpp
//
//  History:    JAN-09-2010   Denny Chen  Created
//
//--------------------------------------------------------------------------
#include "duGridLayout.h"


duGridLayout::duGridLayout() :
	 m_fGrid(FALSE)
	,m_nStartX(0)
	,m_nStartY(0)
	,m_nXSpace(0)
	,m_nYSpace(0)
	,m_nMaxCount(0)
	,m_nItemWidth(0)
	,m_nItemHeight(0)
{
	
}

duGridLayout::~duGridLayout()
{
	
}

void duGridLayout::RegisterControlProperty()
{
	RegisterProperty(_T("grid"), DU_PROPERTY_BOOL, &m_fGrid);
	RegisterProperty(_T("startx"), DU_PROPERTY_LONG, &m_nStartX);
	RegisterProperty(_T("starty"), DU_PROPERTY_LONG, &m_nStartY);
	RegisterProperty(_T("xspace"), DU_PROPERTY_LONG, &m_nXSpace);
	RegisterProperty(_T("yspace"), DU_PROPERTY_LONG, &m_nYSpace);
	RegisterProperty(_T("maxcount"), DU_PROPERTY_LONG, &m_nMaxCount);
	RegisterProperty(_T("itemwidth"), DU_PROPERTY_LONG, &m_nItemWidth);
	RegisterProperty(_T("itemheight"), DU_PROPERTY_LONG, &m_nItemHeight);
}

void duGridLayout::OnCreate()
{
	if (m_fGrid)
		AdjustGrid();
}

void duGridLayout::DrawObject(HDC hDC)
{
	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup == NULL)
		return;

	duRect rcCtrl;
	GetRect(&rcCtrl);
	rcCtrl.OffsetRect(-rcCtrl.left, -rcCtrl.top);
	
	pStyleGroup->Draw(hDC, &rcCtrl, GetState(), GetText(), GetAlpha());
}

void duGridLayout::AdjustGrid()
{
	duPlugin *pChild = GetFirstChild();
	int nCount = 0;
	while (pChild)
	{
		nCount++;
		pChild = pChild->GetNextSibling();
	}
	
	if (nCount > m_nMaxCount)
		return;
	
	int nAdjustCount = m_nMaxCount - nCount;
	int nTop = m_nStartY;
	int nLeft = m_nStartX;
	
	duRect rcGridLayout;
	GetRect(&rcGridLayout);
		
	int nIndex = 0;
	pChild = GetFirstChild();
	while (pChild)
	{
		if (nIndex < nAdjustCount)
		{
			pChild->SetPosLeft(nLeft);
			pChild->SetPosTop(nTop);
			pChild->SetPosWidth(m_nItemWidth * 2 + m_nXSpace);
			
			nLeft = m_nStartX;
			nTop += m_nItemHeight + m_nYSpace;
		}
		else
		{
			pChild->SetPosTop(nTop);
			pChild->SetPosLeft(nLeft);

			nLeft += (m_nItemWidth + m_nXSpace);
			if (nLeft + m_nItemWidth > rcGridLayout.Width())
			{
				nTop += m_nItemHeight + m_nYSpace;
				nLeft = m_nStartX;
			}
		}
		
		nIndex++;
		pChild = pChild->GetNextSibling();
	}
	
	Resize(NULL);
}
