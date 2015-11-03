//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duHwndObj.cpp
//
//  History:    Dec-09-2009   Jimmy Wu  Created
//
//--------------------------------------------------------------------------
#include "duHwndObj.h"

duHwndObj::duHwndObj()
{
	m_hWndObject = NULL;
}

duHwndObj::~duHwndObj()
{
}

BOOL duHwndObj::Attach(HWND hWnd)
{
	if (!::IsWindow(hWnd))
		return FALSE;
		
	m_hWndObject = hWnd;
	duRect rcHwnd;
	Plugin_GetRect(this, &rcHwnd);
		
	::SetWindowPos(m_hWndObject, NULL, rcHwnd.left, rcHwnd.top, rcHwnd.Width(), rcHwnd.Height(), SWP_NOZORDER);
	if (Plugin_IsVisible(this))
		::ShowWindow(m_hWndObject, SW_SHOWNOACTIVATE);
	else
		::ShowWindow(m_hWndObject, SW_HIDE);
	
	return TRUE;
}

HWND duHwndObj::GetAttach()
{
	return m_hWndObject;
}

HWND duHwndObj::Detach()
{
	HWND ret = m_hWndObject;
	m_hWndObject = NULL;
	return ret;
}

void duHwndObj::DrawObject(HDC hDC)
{
//	InvalidateRect(m_hWndObject, NULL, TRUE);

//	duRect rcHwnd;
// 	Plugin_GetRect(this, &rcHwnd);
// 	int cx = rcHwnd.right - rcHwnd.left;
// 	int cy = rcHwnd.bottom - rcHwnd.top;
// 	POINT pt = { rcHwnd.left, rcHwnd.top };
// 	::ClientToScreen(m_hWnd, &pt);
// 
// 	if (::IsWindow(m_hWndObject))
// 		::SetWindowPos(m_hWndObject, NULL, pt.x, pt.y, cx, cy, m_fVisible ? SWP_SHOWWINDOW : SWP_HIDEWINDOW);
}

void duHwndObj::Resize(LPRECT lpRect)
{
	duPlugin::Resize(lpRect);

	duRect rcHwnd;
	Plugin_GetRect(this, &rcHwnd);

	if (::IsWindow(m_hWndObject))
		::SetWindowPos(m_hWndObject, NULL, rcHwnd.left, rcHwnd.top, rcHwnd.Width(), rcHwnd.Height(), SWP_NOREDRAW|SWP_NOZORDER);
}

void duHwndObj::SetVisible(BOOL fVisible)
{
	m_fVisible = fVisible;
	if (::IsWindow(m_hWndObject))
		::ShowWindow(m_hWndObject, fVisible ? SW_SHOW : SW_HIDE);
}


BOOL duHwndObj::IsDisable()
{
	if (::IsWindow(m_hWndObject) && ::GetWindowLong(m_hWndObject, GWL_STYLE) & WS_DISABLED)
		return TRUE;
	
	return duPlugin::IsDisable();
}

void duHwndObj::SetDisable(BOOL fDisable)
{
	if (::IsWindow(m_hWndObject))
		::EnableWindow(m_hWndObject, !fDisable);
	
	return duPlugin::SetDisable(fDisable);
}

BOOL duHwndObj::IsFocus()
{
	if (::IsWindow(m_hWndObject))
		return (::GetFocus() == m_hWndObject) ? TRUE : FALSE;
	
	return duPlugin::IsFocus();
}

void duHwndObj::SetFocus(BOOL fFocus)
{
	if (::IsWindow(m_hWndObject))
		::SetFocus(fFocus ? m_hWndObject : m_hWnd);
	
	return duPlugin::SetFocus(fFocus);
}


