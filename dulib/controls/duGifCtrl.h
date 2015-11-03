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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);
	virtual void OnTimer(UINT nEventId);
	virtual void OnMouseLDown(POINT pt);
	virtual void OnMouseLUp(POINT pt);
	
	virtual void Play();
	virtual void StopPlay();
	
	virtual LPCTSTR GetGif();
	virtual void SetGif(LPCTSTR lpszName);
	
	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duGifCtrl"); }	
	
public:
	duGifCtrl();
	~duGifCtrl();
	
protected:
	TCHAR m_szGifName[MAX_NAME];
	
protected:
	int m_nTimerId;
	int m_nCurFrame;
};
