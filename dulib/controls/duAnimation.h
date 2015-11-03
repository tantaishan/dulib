//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duAnimation.h
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once

#include "duUtility.h"

#include "duPlugin.h"
#include "duTypes.h"

// duAnimation
//
// 
class duAnimation : public duPlugin
{
public:
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);
	virtual LRESULT OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	//virtual void OnTimer(UINT nEventId);
	
	virtual void Play();
	virtual void StopPlay();

	//
	//获取动画图片和设置图片
	//
	virtual LPCTSTR GetAnimation() { return m_szAnimation; }
	virtual void SetAnimation(LPCTSTR lpszName);
	
	//
	//获取加速度和设置加速度
	//
	virtual int GetAcceleration() { return m_nAcceleration; }
	virtual void SetAcceleration(int nAcceleration) { m_nAcceleration=nAcceleration; }
	
	//
	//帧数的设置和获取
	//
	virtual int GetFrameCount() { return m_nFrameCount; }
	virtual void SetFrameCount(int nFrameCount) { m_nFrameCount=nFrameCount; }

	//
	//一个周期的时间 没有包含加速度的时间
	//
	virtual int GetTime() { return m_nTime; }
	virtual void SetTime(int nTime) { m_nTime = nTime; }

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duAnimation"); }	
	
public:
	duAnimation();
	~duAnimation();
	
public:
	TCHAR m_szAnimation[MAX_NAME];
	int m_nAcceleration;
	int m_nFrameCount;
	int m_nTime;
	
	HANDLE m_hThread;
	HANDLE m_hTimerEvent;
	HANDLE m_hExitEvent;
	int m_nTimerId;
	int m_nCurFrame;
};
