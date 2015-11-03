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
	virtual void RegisterControlProperty() {}
	virtual void OnCreate() {}
	virtual void DrawObject(HDC hDC);
	virtual void Resize(LPRECT lpRect);
	
	virtual void SetVisible(BOOL fVisible);

	virtual BOOL IsDisable();
	virtual void SetDisable(BOOL fDisable);
	
	virtual BOOL IsFocus();
	virtual void SetFocus(BOOL fFocus);
	
	virtual BOOL Attach(HWND hWnd);
	virtual HWND GetAttach();
	virtual HWND Detach();
	
	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duHwndObj"); }	

public:
	duHwndObj();
	~duHwndObj();
	
protected:
	HWND m_hWndObject;
};
