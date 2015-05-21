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
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);
	virtual void WINAPI OnTimer(UINT nEventId);

	virtual LPCTSTR WINAPI GetImageFont() { return m_szImageFont; }
	virtual void WINAPI SetImageFont(LPCTSTR lpszImageFont);

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duLEDTimer"); }	
	
public:
	duLEDTimer();
	~duLEDTimer();

protected:
	TCHAR m_szImageFont[MAX_NAME];
	int m_nTimerID;
};
