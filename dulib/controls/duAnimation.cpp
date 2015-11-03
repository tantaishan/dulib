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
#include "duAnimation.h"
#include "duCtrlManager.h"
#include "duDrawHelper.h"

#define WM_ANIMATION_TIMER			WM_USER + 0x189

duAnimation::duAnimation() :
	 m_nTimerId(0)
	,m_nCurFrame(0)
	,m_nFrameCount(0)
	,m_nTime(1000)
	,m_hThread(NULL)
	,m_hTimerEvent(NULL)
	,m_hExitEvent(NULL)
{
	ZeroMemory(m_szAnimation, sizeof(TCHAR) * MAX_NAME);
}

duAnimation::~duAnimation()
{
	SetEvent(m_hExitEvent);
	::WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hTimerEvent);
	CloseHandle(m_hExitEvent);
	CloseHandle(m_hThread);

	Plugin_UnHookWindowMessage(this);
}

void duAnimation::RegisterControlProperty()
{
	//RegisterProperty(_T("acceleration"), DU_PROPERTY_LONG,&m_nAcceleration);
	RegisterProperty(_T("frame"), DU_PROPERTY_LONG, &m_nFrameCount);
	RegisterProperty(_T("animation"), DU_PROPERTY_STRING, &m_szAnimation);
	RegisterProperty(_T("time"), DU_PROPERTY_LONG, &m_nTime);
}

DWORD WINAPI AnimationThread(LPVOID pvParam)
{
	duAnimation *p = (duAnimation *)pvParam;
	while (1)
	{
		while (WAIT_TIMEOUT == WaitForSingleObject(p->m_hTimerEvent, p->m_nTime / p->m_nFrameCount))
		{
			if (WaitForSingleObject(p->m_hExitEvent, 0) == WAIT_OBJECT_0)
			{
				return 0;
			}

			p->m_nCurFrame = (p->m_nCurFrame + 1) % p->m_nFrameCount;
			::PostMessage(p->GetHwnd(), WM_ANIMATION_TIMER, (WPARAM)p, 0);
			ResetEvent(p->m_hTimerEvent);
		}
	}
}

void duAnimation::OnCreate()
{	
	m_hTimerEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	Plugin_HookWindowMessage(this);
}

void duAnimation::DrawObject(HDC hDC)
{
	duImage *pImage = (duImage *)GetResObj(m_szAnimation, DU_RES_IMAGE);
	if (pImage == NULL)
		return;

	int left = m_nCurFrame * (pImage->GetWidth() / m_nFrameCount);
	
	DrawNormal(hDC, 0, 0, pImage->GetWidth() / m_nFrameCount, pImage->GetHeight(), pImage, left, 0, GetAlpha());
}

LRESULT duAnimation::OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!Plugin_IsVisible(this))
		return 0;

	if (uMsg == WM_ANIMATION_TIMER)
	{
		if ((duAnimation *)wParam == this)
		{
			Plugin_Redraw(this, TRUE);
		}
	}

	return 0;
}

void duAnimation::Play()
 {
	duImage *pImage = (duImage *)GetResObj(m_szAnimation, DU_RES_IMAGE);
	if (pImage == NULL)
		return;

	StopPlay();

	DWORD dwThreadId = 0;

	m_hTimerEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hThread = CreateThread(NULL, 0, AnimationThread, this, 0, &dwThreadId);
}

void duAnimation::SetAnimation(LPCTSTR lpszName)
{
	if (lpszName)
		_tcscpy(m_szAnimation, lpszName);
	else
		ZeroMemory(m_szAnimation, sizeof(TCHAR) * MAX_NAME);
}

void duAnimation::StopPlay()
{
	if (m_hThread)
	{
		SetEvent(m_hExitEvent);
		::WaitForSingleObject(m_hThread, INFINITE);		

		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	if (m_hTimerEvent)
	{
		CloseHandle(m_hTimerEvent);
		m_hTimerEvent = NULL;
	}

	if (m_hExitEvent)
	{
		CloseHandle(m_hExitEvent);
		m_hExitEvent = NULL;
	}

	m_nCurFrame = 0;
	::InvalidateRect(m_hWnd, NULL, TRUE);
}

