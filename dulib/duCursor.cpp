//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duCursor.cpp
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duCursor.h"
#include "duWindowManager.h"

duCursor::duCursor()
{
	m_hCursor = NULL;
	ZeroMemory(m_strCursor, MAX_NAME);
}

duCursor::~duCursor()
{
	Destroy();
}

BOOL WINAPI duCursor::OnCreate(TiXmlElement *pElement)
{
	if (pElement == NULL)
		return FALSE;

	ReadXmlAttributeText(pElement, _T("value"), m_strCursor, MAX_NAME);
	
	duWindowManager *pWinManager = GetResManager()->GetWindowManager();
	if (pWinManager == NULL)
		return duResBase::OnCreate(pElement);
	
	IStream *pStream = NULL;
	HANDLE hMem = NULL;
	PBYTE pData = pWinManager->GetResource(m_strCursor, hMem, pStream);
	if (hMem == NULL && pStream == NULL)
		return duResBase::OnCreate(pElement);
		
	int nSize = ::GlobalSize(hMem);
	LoadFromMemory(pData, nSize);
	
	pWinManager->ReleaseResource(hMem, pStream);
	return duResBase::OnCreate(pElement);
}

void WINAPI duCursor::SetValue(LPCTSTR lpszCursor)
{
	if (lpszCursor)
		_tcsncpy(m_strCursor, lpszCursor, MAX_NAME);
	else
		ZeroMemory(m_strCursor, MAX_NAME * sizeof(TCHAR));
}

void WINAPI duCursor::SetHCursor(HCURSOR hCursor)
{
	Destroy();
	m_hCursor = hCursor;
}

HCURSOR WINAPI duCursor::LoadFromFile(LPCTSTR lpszFile)
{
	if (lpszFile == NULL)
		return NULL;

	Destroy();

	m_hCursor = (HCURSOR)::LoadImage(NULL, lpszFile, IMAGE_CURSOR, 0, 0, 
			LR_DEFAULTSIZE|LR_LOADFROMFILE);

	return m_hCursor;
}

HCURSOR WINAPI duCursor::LoadFromMemory(PBYTE pData, int nSize)
{
	if (pData == NULL || nSize <= 0)
		return FALSE;

	Destroy();
	TCHAR szPathBuf[MAX_PATH];
	HANDLE hFile = CreateTempFile(szPathBuf);
	if (hFile == NULL)
		return NULL;
	
	DWORD dwNumWrites = 0;
	if (::WriteFile(hFile, pData, nSize, &dwNumWrites, NULL))
	{
		::CloseHandle(hFile);
		m_hCursor = (HCURSOR)::LoadImage(NULL, szPathBuf, IMAGE_CURSOR, 0, 0, 
			LR_DEFAULTSIZE|LR_LOADFROMFILE);
			
		::DeleteFile(szPathBuf);
		return m_hCursor;
	}
	
	
	::CloseHandle(hFile);
	::DeleteFile(szPathBuf);
	return NULL;
}

void duCursor::Destroy()
{
	if (m_hCursor)
	{
		::DestroyCursor(m_hCursor);
		m_hCursor = NULL;
	}
}

HANDLE duCursor::CreateTempFile(LPTSTR lpszFilePathBufOut)
{
    TCHAR szPathBuf[MAX_PATH];
    DWORD ret = ::GetTempPath(MAX_PATH, szPathBuf);
    if (0 == ret || ret > MAX_PATH)
        return NULL;

    UINT uret = ::GetTempFileName(szPathBuf, _T("CUR"), 0, lpszFilePathBufOut);
    if (0 == uret)
        return NULL;

	HANDLE hFile = ::CreateFile(lpszFilePathBufOut, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
        return NULL;

    return hFile;
}
