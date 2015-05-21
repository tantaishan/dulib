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
#include "stdafx.h"
#include "duHwndObj.h"

duHwndObj::duHwndObj()
{
	m_hWndObject = NULL;
}

duHwndObj::~duHwndObj()
{
}

BOOL WINAPI duHwndObj::Attach(HWND hWnd)
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

HWND WINAPI duHwndObj::GetAttach()
{
	return m_hWndObject;
}

HWND WINAPI duHwndObj::Detach()
{
	HWND ret = m_hWndObject;
	m_hWndObject = NULL;
	return ret;
}

void WINAPI duHwndObj::Resize(LPRECT lpRect)
{
	duPlugin::Resize(lpRect);

	duRect rcHwnd;
	Plugin_GetRect(this, &rcHwnd);

	if (::IsWindow(m_hWndObject))
		::SetWindowPos(m_hWndObject, NULL, rcHwnd.left, rcHwnd.top, rcHwnd.Width(), rcHwnd.Height(), SWP_NOREDRAW|SWP_NOZORDER);
}

BOOL WINAPI duHwndObj::IsDisable()
{
	if (::IsWindow(m_hWndObject) && ::GetWindowLong(m_hWndObject, GWL_STYLE) & WS_DISABLED)
		return TRUE;
	
	return duPlugin::IsDisable();
}

void WINAPI duHwndObj::SetDisable(BOOL fDisable)
{
	if (::IsWindow(m_hWndObject))
		::EnableWindow(m_hWndObject, !fDisable);
	
	return duPlugin::SetDisable(fDisable);
}

BOOL WINAPI duHwndObj::IsFocus()
{
	if (::IsWindow(m_hWndObject))
		return (::GetFocus() == m_hWndObject) ? TRUE : FALSE;
	
	return duPlugin::IsFocus();
}

void WINAPI duHwndObj::SetFocus(BOOL fFocus)
{
	if (::IsWindow(m_hWndObject))
		::SetFocus(fFocus ? m_hWndObject : m_hWnd);
	
	return duPlugin::SetFocus(fFocus);
}


