//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duFlash.h
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duTypes.h"
#include "duHwndObj.h"
#include "Flash.h"

#define MAX_URL_LENGTH 	1024
#define FLASH_PLAYER_ERROR		1
#define FLASH_PLAYER_IDLE		2
#define FLASH_PLAYER_BUFFING	3
#define FLASH_PLAYER_PLAYING	4
#define FLASH_PLAYER_PAUSED		5

// duFlash
// 
// 
class duFlash : public duHwndObj
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);
	virtual void WINAPI Resize(LPRECT lpRect);
	
	virtual LPCTSTR WINAPI GetURL();
	virtual void WINAPI SetURL(LPCTSTR lpszURL);

	virtual IUnknown *WINAPI GetControl() { return (IUnknown *)m_pFlash; }
	virtual IUnknown *WINAPI GetContainer() { return m_pContainer; }
	virtual BOOL WINAPI Play(LPCTSTR lpszMovie);
	virtual BOOL WINAPI Stop();
	virtual UINT WINAPI GetPlayState();
	virtual void WINAPI Pause();
	virtual LPCTSTR WINAPI GetMovie() { return m_szMovie; }

	virtual BOOL WINAPI IsAllowFullScreen();
	virtual void WINAPI SetAllowFullScreen(BOOL fAllowFullScreen);
	
	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duFlash"); }
	
public:
	duFlash();
	~duFlash();

public:
	void OnPlayerReady();
	void CallPlay();
	void CallContinue();
	void CallStop();
	void CallPause();
	
protected:
	TCHAR m_szURL[MAX_URL_LENGTH];
	TCHAR m_szMovie[MAX_URL_LENGTH];
	IShockwaveFlash *m_pFlash;
	IUnknown *m_pContainer;
	BOOL m_bPlayerReady;
};
