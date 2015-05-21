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
#include "stdafx.h"
#include "duGifCtrl.h"
#include "duCtrlManager.h"

duGifCtrl::duGifCtrl() :
	 m_nTimerId(0)
	,m_nCurFrame(0)
{
	ZeroMemory(m_szGifName, sizeof(TCHAR) * MAX_NAME);
}

duGifCtrl::~duGifCtrl()
{
}

void WINAPI duGifCtrl::RegisterControlProperty()
{
	RegisterProperty(_T("gif"), DU_PROPERTY_STRING, m_szGifName);
}

void WINAPI duGifCtrl::OnCreate()
{
	m_nTimerId = GetUniTimer();
	Play();
}

void WINAPI duGifCtrl::DrawObject(HDC hDC)
{
	duImage *pImage = (duImage *)GetResObj(m_szGifName, DU_RES_IMAGE);
	if (pImage == NULL)
		return;
		
	HBITMAP hBitmap = NULL;
	if (pImage->GetFrameCount() <= 1)
		hBitmap = pImage->GetBitmap();
	else
		hBitmap = pImage->GetFrameBitmap(m_nCurFrame);

	HDC hSrcDC = ::CreateCompatibleDC(NULL);	
	HBITMAP hSrcOldBmp = (HBITMAP)::SelectObject(hSrcDC, hBitmap);

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, GetAlpha(), AC_SRC_ALPHA};
	::AlphaBlend(hDC, 0, 0, pImage->GetWidth(), pImage->GetHeight(),
		hSrcDC, 0, 0, pImage->GetWidth(), pImage->GetHeight(), bf);

	::SelectObject(hSrcDC, hSrcOldBmp);
	::DeleteDC(hSrcDC);
}

void WINAPI duGifCtrl::OnTimer(UINT nEventId)
{
	if (nEventId != m_nTimerId)
		return;

	if (!Plugin_IsVisible(this))
		return;
	
	Plugin_KillTimer(this, m_nTimerId);
	duImage *pImage = (duImage *)GetResObj(m_szGifName, DU_RES_IMAGE);
	if (pImage == NULL)
		return;
	
	int nFrameCount = pImage->GetFrameCount();
	if (nFrameCount <= 1)
		return;
	
	int nFrameDelay = pImage->GetFrameDelay(m_nCurFrame);
	Plugin_SetTimer(this, m_nTimerId, nFrameDelay);
	Plugin_Redraw(this, TRUE);

	m_nCurFrame = (m_nCurFrame + 1) % nFrameCount;
}

void WINAPI duGifCtrl::OnMouseLDown(POINT pt)
{
	Plugin_SetState(this, DU_STATE_PRESS);
}

void WINAPI duGifCtrl::OnMouseLUp(POINT pt)
{
	if (GetState() == DU_STATE_PRESS)
	{
		Plugin_SetState(this, DU_STATE_NORMAL);
		NotifyUser(DUM_GIFCLICK, NULL, NULL);
	}
}

void WINAPI duGifCtrl::Play()
{
	Plugin_KillTimer(this, m_nTimerId);
	duImage *pImage = (duImage *)GetResObj(m_szGifName, DU_RES_IMAGE);
	if (pImage == NULL)
		return;
	
	int nFrameCount = pImage->GetFrameCount();
	if (nFrameCount <= 1)
		return;

	m_nCurFrame = 0;
	int nFrameDelay = pImage->GetFrameDelay(m_nCurFrame);
	Plugin_SetTimer(this, m_nTimerId, nFrameDelay);
}

void WINAPI duGifCtrl::StopPlay()
{
	Plugin_KillTimer(this, m_nTimerId);
	m_nCurFrame = 0;
	Plugin_Redraw(this, TRUE);
}

LPCTSTR WINAPI duGifCtrl::GetGif()
{
	return m_szGifName;
}

void WINAPI duGifCtrl::SetGif(LPCTSTR lpszName)
{
	if (lpszName)
		_tcscpy(m_szGifName, lpszName);
	else
		ZeroMemory(m_szGifName, sizeof(TCHAR) * MAX_NAME);
}
