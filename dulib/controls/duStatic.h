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
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

class duStatic : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);
	virtual void WINAPI OnMouseIn(POINT pt);
	virtual void WINAPI OnMouseLeave(POINT pt);
	
	virtual BOOL WINAPI IsShowToolTip() { return m_fShowToolTip; }
	virtual void WINAPI SetShowToolTip(BOOL fShow) { m_fShowToolTip = fShow; }
	
	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duStatic"); }

protected:
	BOOL m_fShowToolTip;

public:
	duStatic();
	~duStatic();	
};
