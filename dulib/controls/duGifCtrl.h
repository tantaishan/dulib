//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duGifCtrl.h
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once

#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

// class duGifCtrl
//
// 
class duGifCtrl : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);
	virtual void WINAPI OnTimer(UINT nEventId);
	virtual void WINAPI OnMouseLDown(POINT pt);
	virtual void WINAPI OnMouseLUp(POINT pt);
	
	virtual void WINAPI Play();
	virtual void WINAPI StopPlay();
	
	virtual LPCTSTR WINAPI GetGif();
	virtual void WINAPI SetGif(LPCTSTR lpszName);
	
	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duGifCtrl"); }	
	
public:
	duGifCtrl();
	~duGifCtrl();
	
protected:
	TCHAR m_szGifName[MAX_NAME];
	
protected:
	int m_nTimerId;
	int m_nCurFrame;
};
