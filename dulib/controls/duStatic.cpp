//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duEdit.h
//
//  History:    Jan-05-2010   Denny Chen  Created
//
//--------------------------------------------------------------------------
#include "duStatic.h"
#include "duCtrlManager.h"

duStatic::duStatic()
{
	m_fShowToolTip = FALSE;
}

duStatic::~duStatic()
{
}

void duStatic::RegisterControlProperty()
{
	RegisterProperty(_T("showtooltip"), DU_PROPERTY_BOOL, &m_fShowToolTip);
}

void duStatic::OnCreate()
{
}

void duStatic::DrawObject(HDC hDC)
{
	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup == NULL)
		return;

	duRect rcCtrl;
	rcCtrl.SetRectEmpty();
	GetRect(&rcCtrl);
	rcCtrl.OffsetRect(-rcCtrl.left, -rcCtrl.top);
	
	pStyleGroup->Draw(hDC, &rcCtrl, GetState(), GetText(), GetAlpha());
}

void duStatic::OnMouseIn(POINT pt)
{
	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return;

	if (m_fShowToolTip)
	{
		pCtrlManager->AddToolTip((LPTSTR)GetText());
	}
	else
	{
		LPCTSTR lpszText = GetText();
		if (lpszText && *lpszText)
		{
			duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
			if (pStyleGroup == NULL)
				return;

			int i;
			int nStyleCount = pStyleGroup->GetStyleCount();
			for (i = 0;i < nStyleCount; i++)
			{
				duStyleBase *pStyleBase = pStyleGroup->GetStyle(i);
				if (pStyleBase->GetType() == STYLE_TEXT)
				{
					duTextStyle *pTextStyle = (duTextStyle *)pStyleBase;

					duSize size(0, 0);
					pTextStyle->CalcTextRect(lpszText, &size);

					duRect rcStyle;
					rcStyle.SetRectEmpty();

					duRect rcStatic;
					Plugin_GetRect(this, &rcStatic);
					rcStatic.OffsetRect(-rcStatic.left, -rcStatic.top);

					MakeRect(pTextStyle->GetLeft(), pTextStyle->GetTop(), pTextStyle->GetRight(), pTextStyle->GetBottom(), 
						pTextStyle->GetWidth(), pTextStyle->GetHeight(), &rcStatic, &rcStyle);

					POINT point;
					point.x = size.cx;
					point.y = size.cy;
					if (!rcStyle.PtInRect(point))
					{
						pCtrlManager->AddToolTip((LPTSTR)lpszText);
					}

					break;
				}
			}
		}
	}
}

void duStatic::OnMouseLeave(POINT pt)
{
	duCtrlManager *pCtrlManager = GetCtrlManager(m_hWnd);
	if (pCtrlManager == NULL)
		return;

	pCtrlManager->DelToolTip();
}
