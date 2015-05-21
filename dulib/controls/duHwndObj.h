//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duHwndObj.h
//
//  History:    Dec-09-2009   Jimmy Wu  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

// duHwndObj
//
//
class duHwndObj : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty() {}
	virtual void WINAPI OnCreate() {}
	virtual void WINAPI DrawObject(HDC hDC) {}
	virtual void WINAPI Resize(LPRECT lpRect);
	
	virtual BOOL WINAPI IsDisable();
	virtual void WINAPI SetDisable(BOOL fDisable);
	
	virtual BOOL WINAPI IsFocus();
	virtual void WINAPI SetFocus(BOOL fFocus);
	
	virtual BOOL WINAPI Attach(HWND hWnd);
	virtual HWND WINAPI GetAttach();
	virtual HWND WINAPI Detach();
	
	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duHwndObj"); }	

public:
	duHwndObj();
	~duHwndObj();
	
protected:
	HWND m_hWndObject;
};
