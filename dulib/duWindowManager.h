//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duWindowManager.h
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------

#pragma once
#include "duUtility.h"
#include "duResBase.h"
#include "controls/duWindow.h"
#include "duCtrlManager.h"
#include "unzip.h"
#include "duImage.h"

// duWindowManager
//
// 
class duWindowManager
{
public:
	duWindowManager();
	~duWindowManager();
	
public:
	// Loading Skin From xml/zip File
	BOOL OpenSkin(LPCTSTR lpszXmlFile);
	BOOL OpenSkinZip(LPCTSTR lpszZipFile, LPCSTR lpszZipPassword);
	BOOL OpenSkinMemory(BYTE *pData, int nSize);
	TiXmlElement *GetWindowXmlElement(LPCTSTR lpszWindowName);
	BOOL RemoveHwnd(HWND hWnd);
	PBYTE GetZipResource(LPCTSTR lpszResName, HANDLE &hMem, IStream *&pStream);
	duImage *GetTrialBitmap() { return m_pTrialImage; }
	void ResetEditHFont();
	
	
protected:
	void InitTrialBitmap();
	
public:
	virtual BOOL Attach(HWND hWnd, LPCTSTR lpWindowName);
	virtual LPCTSTR GetSkinPath();
	virtual duResManager *GetResManager() { return m_pResManager; }
	virtual duPlugin *CreateControl(LPCTSTR lpszType, LPCTSTR lpszName, duPlugin *pParent);
	virtual BOOL ExecScript(LPCTSTR lpszFuncName, int nParamCount, VARIANT *pParams);
	virtual int GetWindowCount();
	virtual HWND GetWindowAt(int nIndex);
	virtual PBYTE GetResource(LPCTSTR lpszResName, HANDLE &hMem, IStream *&pStream);
	virtual void ReleaseResource(HANDLE hMem, IStream *pStream);
	
	virtual void OutputDebugString(LPCTSTR lpszOutput);
	virtual BOOL ShowWindow(HWND hWnd, int nCmdShow);
	virtual BOOL DestroyWindow(HWND hWnd);
	virtual void PostQuitMessage(int nExitCode);
	virtual BOOL SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
	virtual int GetRed(int color);
	virtual int GetGreen(int color);
	virtual int GetBlue(int color);
	virtual BOOL ChangeSkin(LPCTSTR lpszFile);
	virtual void Redraw(duPlugin *pPlugin);

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duWindowManager"); }

protected:
	TCHAR m_szSkinPath[MAX_PATH];
	TiXmlDocument *m_pDocXml;
	duResManager *m_pResManager;
	BOOL m_fZip;
	HZIP m_hZip;
	duImage *m_pTrialImage;
	
	map<HWND, duCtrlManager *> m_mapCtrlManager;
};
