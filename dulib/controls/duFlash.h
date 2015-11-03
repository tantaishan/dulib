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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);
	virtual void Resize(LPRECT lpRect);
	
	virtual LPCTSTR GetURL();
	virtual void SetURL(LPCTSTR lpszURL);

	virtual IUnknown *GetControl() { return (IUnknown *)m_pFlash; }
	virtual IUnknown *GetContainer() { return m_pContainer; }
	virtual BOOL Play(LPCTSTR lpszMovie);
	virtual BOOL Stop();
	virtual UINT GetPlayState();
	virtual void Pause();
	virtual LPCTSTR GetMovie() { return m_szMovie; }

	virtual BOOL IsAllowFullScreen();
	virtual void SetAllowFullScreen(BOOL fAllowFullScreen);
	
	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duFlash"); }
	
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
