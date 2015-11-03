//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duLEDTimer.h
//
//  History:    Mar-23-2011   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

// class duLEDTimer
// 
//
class duLEDTimer : public duPlugin
{
public:
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);
	virtual void OnTimer(UINT nEventId);

	virtual LPCTSTR GetImageFont() { return m_szImageFont; }
	virtual void SetImageFont(LPCTSTR lpszImageFont);

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duLEDTimer"); }	
	
public:
	duLEDTimer();
	~duLEDTimer();

protected:
	TCHAR m_szImageFont[MAX_NAME];
	int m_nTimerID;
};
