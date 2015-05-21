//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duFlash.cpp
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duFlash.h"
#include "AxControl.h"

extern HRESULT AxAdviseAll(duFlash *pFlash, IUnknown *pUnk);

duFlash::duFlash()
{
	m_pFlash = NULL;
	m_pContainer = NULL;
	m_bPlayerReady = FALSE;
	ZeroMemory(m_szURL, MAX_URL_LENGTH * sizeof(TCHAR));
	ZeroMemory(m_szMovie, MAX_URL_LENGTH * sizeof(TCHAR));
}

duFlash::~duFlash()
{
	if (m_pFlash)
	{
		IOleObject * pObject = NULL;
		if (SUCCEEDED(m_pFlash->QueryInterface(IID_IOleObject,(void**)&pObject)))
		{
			pObject->Close(0);
			pObject->Release();
		}
		m_pFlash->Release();
		m_pFlash = NULL;
	}

	if (m_pContainer)
	{
		m_pContainer->Release();
		m_pContainer = NULL;
	}
}

void WINAPI duFlash::RegisterControlProperty()
{
	RegisterProperty(_T("url"), DU_PROPERTY_STRING, &m_szURL);
}

void WINAPI duFlash::OnCreate()
{
	IUnknown *pControl = NULL;
	IUnknown *pContainer = NULL;
	HRESULT hr = CreateAxControl(this->GetHwnd(), L"ShockwaveFlash.ShockwaveFlash", &pControl, &pContainer);
	if (FAILED(hr))
		return;
	
	AxAdviseAll(this, pControl);
	
	hr = pControl->QueryInterface(__uuidof(IShockwaveFlash), (void **)&m_pFlash);
	hr = m_pFlash->put_Movie(m_szURL);
	hr = m_pFlash->put_AllowFullScreen(L"true");
	
	IOleWindow *pOleWindow = NULL;
	hr = pControl->QueryInterface(IID_IOleWindow, (void **)&pOleWindow);
				
	HWND hWndObject = NULL;
	pOleWindow->GetWindow(&hWndObject);
	pOleWindow->Release();
	
	Attach(hWndObject);

	m_pContainer = pContainer;
	m_pContainer->AddRef();

	pControl->Release();
	pContainer->Release();
}

void WINAPI duFlash::DrawObject(HDC hDC)
{
}

void WINAPI duFlash::Resize(LPRECT lpRect)
{
	duHwndObj::Resize(lpRect);
	
	duRect rcFlash;
	GetRect(&rcFlash);
	
	IOleInPlaceObject *pInPlaceObject = NULL;
	if (m_pFlash && SUCCEEDED(m_pFlash->QueryInterface(IID_IOleInPlaceObject,(void**)&pInPlaceObject)))
	{
		pInPlaceObject->SetObjectRects(&rcFlash, &rcFlash);
		pInPlaceObject->Release();
	}
}

LPCTSTR WINAPI duFlash::GetURL()
{
	return m_szURL;
}

void WINAPI duFlash::SetURL(LPCTSTR lpszURL)
{
	if(lpszURL)
		_tcsncpy(m_szURL, lpszURL, MAX_URL_LENGTH);
	else
		ZeroMemory(m_szURL, MAX_URL_LENGTH * sizeof(TCHAR));

	if (m_pFlash)
		m_pFlash->put_Movie(m_szURL);
}

BOOL WINAPI duFlash::IsAllowFullScreen()
{
	if (m_pFlash)
	{
		BSTR bstrFullScreen = NULL;
		HRESULT hr = m_pFlash->get_AllowFullScreen(&bstrFullScreen);
		if (FAILED(hr))
			return FALSE;

		if (!bstrFullScreen)
			return FALSE;

		if (lstrcmpi(bstrFullScreen, L"true") == 0)
			return TRUE;
		else if (lstrcmpi(bstrFullScreen, L"false") == 0)
			return FALSE;
	}
	
	return FALSE;
}

void WINAPI duFlash::SetAllowFullScreen(BOOL fAllowFullScreen)
{
	if (m_pFlash)
	{
		if (fAllowFullScreen)
			m_pFlash->put_AllowFullScreen(L"true");
		else
			m_pFlash->put_AllowFullScreen(L"false");
	}
}

BOOL WINAPI duFlash::Play(LPCTSTR lpszMovie)
{
	if (m_pFlash == NULL)
		return FALSE;

	if (lpszMovie)
	{
		_tcscpy(m_szMovie, lpszMovie);

		if (m_bPlayerReady)
		{
			CallPlay();
		}
	}
	else
	{
		if (m_bPlayerReady)
		{
			if (GetPlayState() != FLASH_PLAYER_PLAYING)
				CallContinue();
		}
	}

	return TRUE;
}

BOOL WINAPI duFlash::Stop()
{
	if (m_pFlash == NULL)
		return FALSE;

	CallStop();
	return TRUE;
}

UINT WINAPI duFlash::GetPlayState()
{
	if (m_pFlash == NULL)
		return FLASH_PLAYER_ERROR;
	
	HRESULT hr;
	BSTR bstrRet = NULL;
	hr = m_pFlash->CallFunction(L"<invoke name=\"jwGetState\" returntype=\"xml\"></invoke>", &bstrRet);
	if (bstrRet)
	{
		//<string>PLAYING</string>
		//<string>PAUSED</string>
		//<string>IDLE</string>
		//<string>BUFFERING</string>
		if (lstrcmpi(bstrRet, _T("<string>PAUSED</string>")) == 0)
			return FLASH_PLAYER_PAUSED;

		if (lstrcmpi(bstrRet, _T("<string>PLAYING</string>")) == 0)
			return FLASH_PLAYER_PLAYING;

		if (lstrcmpi(bstrRet, _T("<string>IDLE</string>")) == 0)
			return FLASH_PLAYER_IDLE;

		if (lstrcmpi(bstrRet, _T("<string>BUFFERING</string>")) == 0)
			return FLASH_PLAYER_BUFFING;
	}

	return FLASH_PLAYER_ERROR;
}

void WINAPI duFlash::Pause()
{
	if (m_pFlash == NULL)
		return;

	CallPause();
}

void duFlash::OnPlayerReady()
{
	m_bPlayerReady = TRUE;
	CallPlay();
}

void duFlash::CallPlay()
{
	if (m_pFlash == NULL)
		return;

	int len = lstrlen(m_szMovie);
	if (len == 0)
		return;

	BSTR bstrRet = NULL;
	TCHAR szCallXml[2048];
	_stprintf(szCallXml, _T("<invoke name=\"jwLoad\" returntype=\"xml\"><arguments><string>%s</string></arguments></invoke>"), 
		m_szMovie);
	
	HRESULT hr = m_pFlash->CallFunction(szCallXml, &bstrRet);
}

void duFlash::CallContinue()
{
	if (m_pFlash == NULL)
		return;

	HRESULT hr;
	BSTR bstrRet = NULL;
	hr = m_pFlash->CallFunction(L"<invoke name=\"jwGetState\" returntype=\"xml\"></invoke>", &bstrRet);
	if (bstrRet)
	{
		//<string>PLAYING</string>
		//<string>PAUSED</string>
		if (GetPlayState() != FLASH_PLAYER_PLAYING)
		{
			BSTR bstrRet2 = NULL;
			hr = m_pFlash->CallFunction(L"<invoke name=\"jwPlay\" returntype=\"xml\"><arguments><Boolean>true</Boolean></arguments></invoke>", &bstrRet2);
		}
	}
}

void duFlash::CallStop()
{
	if (m_pFlash == NULL)
		return;

	HRESULT hr;
	BSTR bstrRet = NULL;
	hr = m_pFlash->CallFunction(L"<invoke name=\"jwStop\" returntype=\"xml\"><arguments></arguments></invoke>", &bstrRet);
}

void duFlash::CallPause()
{
	if (m_pFlash == NULL)
		return;

	HRESULT hr;
	BSTR bstrRet = NULL;
	if (GetPlayState() == FLASH_PLAYER_PLAYING)
		hr = m_pFlash->CallFunction(L"<invoke name=\"jwPause\" returntype=\"xml\"><arguments></arguments></invoke>", &bstrRet);
}

void FlashEvent(void *pFlash, wstring name, wstring param_list)
{
	duFlash *pFlashControl = (duFlash *)pFlash;
	//_TRACE(_T("controlName:%s name:%s param_list:%s\n"), pFlashControl->GetName(), name.c_str(), param_list.c_str());
	if (_tcsstr(param_list.c_str(), _T("name=\"playerReady\"")))
		pFlashControl->OnPlayerReady();
}

