//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duSpin.cpp
//
//  History:    Nov-17-2009   Steven Li  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duSpin.h"

duSpin::duSpin() :
	 m_nVal(0)
	,n_nSpinWidth(0)
	,m_nSpinLeft(0)
	,m_nSpinTop(0)
	,m_nSpinBottom(0)
	,m_uUpState(DU_STATE_NORMAL)
	,m_uDownState(DU_STATE_NORMAL)
{
	ZeroMemory(m_szUpStyle, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szDownStyle, MAX_NAME * sizeof(TCHAR));
}

duSpin::~duSpin()
{	
}

void WINAPI duSpin::RegisterControlProperty()
{
	duEdit::RegisterControlProperty();
	
	RegisterProperty(_T("value"),   DU_PROPERTY_LONG, &m_nVal);
	RegisterProperty(_T("spinleft"),   DU_PROPERTY_LONG, &m_nSpinLeft);
	RegisterProperty(_T("spinwidth"),   DU_PROPERTY_LONG, &n_nSpinWidth);
	RegisterProperty(_T("spintop"),   DU_PROPERTY_LONG, &m_nSpinTop);
	RegisterProperty(_T("spinbottom"),   DU_PROPERTY_LONG, &m_nSpinBottom);
	
	RegisterProperty(_T("upstyle"),   DU_PROPERTY_STYLEGROUP, m_szUpStyle);
	RegisterProperty(_T("downstyle"),   DU_PROPERTY_STYLEGROUP, m_szDownStyle);
}

void WINAPI duSpin::OnCreate()
{
	m_fNumber = TRUE;

	TCHAR m_szValue[256];
	_stprintf(m_szValue, _T("%d"), m_nVal);
	SetText(m_szValue);

	duEdit::OnCreate();
}

void WINAPI duSpin::DrawObject(HDC hDC)
{
	duRect rcEdit;
	rcEdit.SetRectEmpty();
	GetRect(&rcEdit);
	rcEdit.OffsetRect(-rcEdit.left, -rcEdit.top);

	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup)
		pStyleGroup->Draw(hDC, &rcEdit, GetState(), NULL, GetAlpha());
	
	duRect rcSpinUpButton, rcSpinDownButton;
	rcSpinUpButton.left = rcEdit.right - m_nSpinLeft;
	rcSpinUpButton.top = rcEdit.top + m_nSpinTop;
	rcSpinUpButton.right =  rcSpinUpButton.left + n_nSpinWidth;
	rcSpinUpButton.bottom = rcSpinUpButton.top + (rcEdit.Height() - m_nSpinTop - m_nSpinBottom) / 2;
		
	rcSpinDownButton = rcSpinUpButton;
	rcSpinDownButton.top = rcSpinUpButton.bottom;
	rcSpinDownButton.bottom = rcEdit.bottom - m_nSpinBottom;
	
	duStyleGroup *pUpStyle = (duStyleGroup *)GetResObj(m_szUpStyle, DU_RES_STYLEGROUP);
	if (pUpStyle)
		pUpStyle->Draw(hDC, &rcSpinUpButton, m_uUpState, NULL, GetAlpha());
	
	duStyleGroup *pDownStyle = (duStyleGroup *)GetResObj(m_szDownStyle, DU_RES_STYLEGROUP);
	if (pDownStyle)
		pDownStyle->Draw(hDC, &rcSpinDownButton, m_uDownState, NULL, GetAlpha());
	
	HRGN hRgnClip = ::CreateRectRgn(m_rcFormat.left, m_rcFormat.top, m_rcFormat.right, m_rcFormat.bottom);
	::SelectClipRgn(hDC, hRgnClip);
	
	int s = m_nSelectionStart;
	int e = m_nSelectionEnd;
	int ll;
	int x;
	int y;
	LRESULT pos;

	pos = EMPosFromChar(0);
	x = (short)LOWORD(pos);
	y = (short)HIWORD(pos);
	ll = GetEditTextLength();
	s = min(m_nSelectionStart, m_nSelectionEnd);
	e = max(m_nSelectionStart, m_nSelectionEnd);
	s = min(ll, s);
	e = min(ll, e);
	if ((s != e) && IsFocus())
	{
		x += PaintText(hDC, x, y, 0, 0, s, FALSE);
		x += PaintText(hDC, x, y, 0, s, e - s, TRUE);
		x += PaintText(hDC, x, y, 0, e, ll - e, FALSE);
	}
	else
	{
		x += PaintText(hDC, x, y, 0, 0, ll, FALSE);
	}

	::SelectClipRgn(hDC, NULL);
	::DeleteObject(hRgnClip);
}

void WINAPI duSpin::OnMouseLDown(POINT pt)
{
	duPoint ptSpin(pt);
	
	duRect rcSpin;
	Plugin_GetRect(this, &rcSpin);
	ptSpin.Offset(-rcSpin.left, -rcSpin.top);
	
	if (!m_fReadOnly && ::PtInRect(&m_rcFormat, ptSpin))
		duEdit::OnMouseLDown(pt);
	else
	{
		duRect rcSpinUpButton, rcSpinDownButton;
		rcSpinUpButton.left = rcSpin.right - m_nSpinLeft;
		rcSpinUpButton.top = rcSpin.top + m_nSpinTop;
		rcSpinUpButton.right =  rcSpinUpButton.left + n_nSpinWidth;
		rcSpinUpButton.bottom = rcSpinUpButton.top + (rcSpin.Height() - m_nSpinTop - m_nSpinBottom) / 2;
			
		rcSpinDownButton = rcSpinUpButton;
		rcSpinDownButton.top = rcSpinUpButton.bottom;
		rcSpinDownButton.bottom = rcSpin.bottom - m_nSpinBottom;
		
		if (rcSpinUpButton.PtInRect(pt))
		{
			m_uUpState = DU_STATE_PRESS;
			Plugin_Redraw(this, TRUE);
		}
		else if (rcSpinDownButton.PtInRect(pt))
		{
			m_uDownState = DU_STATE_PRESS;
			Plugin_Redraw(this, TRUE);
		}
	}
}

void WINAPI duSpin::OnMouseLUp(POINT pt)
{	
	duRect rcSpin;
	Plugin_GetRect(this, &rcSpin);

	duRect rcSpinUpButton, rcSpinDownButton;
	rcSpinUpButton.left = rcSpin.right - m_nSpinLeft;
	rcSpinUpButton.top = rcSpin.top + m_nSpinTop;
	rcSpinUpButton.right =  rcSpinUpButton.left + n_nSpinWidth;
	rcSpinUpButton.bottom = rcSpinUpButton.top + (rcSpin.Height() - m_nSpinTop - m_nSpinBottom) / 2;
			
	rcSpinDownButton = rcSpinUpButton;
	rcSpinDownButton.top = rcSpinUpButton.bottom;
	rcSpinDownButton.bottom = rcSpin.bottom - m_nSpinBottom;
		
	if (rcSpinUpButton.PtInRect(pt))
	{
		if (m_uUpState == DU_STATE_PRESS)
		{
			m_uUpState = DU_STATE_NORMAL;
			SetValue(GetValue() + 1);
		}
	}
	else if (rcSpinDownButton.PtInRect(pt))
	{
		if (m_uDownState == DU_STATE_PRESS)
		{
			m_uDownState = DU_STATE_NORMAL;
			SetValue(GetValue() - 1);
		}
	}
	
	duEdit::OnMouseLUp(pt);
}

void WINAPI duSpin::OnMouseDbClick(POINT pt)
{
	duPoint ptSpin(pt);
	
	duRect rcSpin;
	Plugin_GetRect(this, &rcSpin);
	ptSpin.Offset(-rcSpin.left, -rcSpin.top);
	
	if (!m_fReadOnly && ::PtInRect(&m_rcFormat, ptSpin))
		duEdit::OnMouseLDown(pt);
}

void WINAPI duSpin::OnMouseLeave(POINT pt)
{
	BOOL fRedraw = FALSE;
	if (m_uUpState != DU_STATE_NORMAL)
	{
		m_uUpState = DU_STATE_NORMAL;
		fRedraw = TRUE;
	}

	if (m_uDownState != DU_STATE_NORMAL)
	{
		m_uDownState = DU_STATE_NORMAL;
		fRedraw = TRUE;
	}

	if (fRedraw)
		Plugin_Redraw(this, TRUE);
}

int WINAPI duSpin::GetValue()
{
	return m_nVal;
}

void WINAPI duSpin::SetValue(int nVal)
{
	m_nVal = nVal;
	TCHAR m_szValue[256];
	_stprintf(m_szValue, _T("%d"), m_nVal);
	SetText(m_szValue);

	m_nSelectionStart = 0;
	m_nSelectionEnd = GetEditTextLength();

	OnEmReplaceSel(FALSE, m_szValue, FALSE, FALSE);
	m_nSelectionStart = m_nSelectionEnd = 0;
	
	NotifyUser(DUM_SPINCHANGE, m_nVal, NULL);
}

