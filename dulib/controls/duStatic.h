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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);
	virtual void OnMouseIn(POINT pt);
	virtual void OnMouseLeave(POINT pt);
	
	virtual BOOL IsShowToolTip() { return m_fShowToolTip; }
	virtual void SetShowToolTip(BOOL fShow) { m_fShowToolTip = fShow; }
	
	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duStatic"); }

protected:
	BOOL m_fShowToolTip;

public:
	duStatic();
	~duStatic();	
};
