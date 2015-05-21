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
#include "duObject.h"
#include "controls/duWindow.h"
#include "duCtrlManager.h"
#include "ScriptObject.h"
#include "duTypeLib.h"
#include "unzip.h"
#include "duImage.h"

// duWindowManager
//
// 
class duWindowManager : public duObject
{
public:
	duWindowManager();
	~duWindowManager();
	
public:
	// Loading Skin From xml/zip File
	BOOL WINAPI OpenSkin(LPCTSTR lpszXmlFile);
	BOOL WINAPI OpenSkinZip(LPCTSTR lpszZipFile, LPCSTR lpszZipPassword);
	BOOL WINAPI OpenSkinMemory(BYTE *pData, int nSize);
	TiXmlElement *GetWindowXmlElement(LPCTSTR lpszWindowName);
	ITypeInfo *GetTypeInfoByName(LPCTSTR lpszTypeName);
	BOOL RemoveHwnd(HWND hWnd);
	PBYTE WINAPI GetZipResource(LPCTSTR lpszResName, HANDLE &hMem, IStream *&pStream);
	duImage *GetTrialBitmap() { return m_pTrialImage; }
	void ResetEditHFont();
	
	
protected:
	void InitTrialBitmap();
	
public:
	virtual BOOL WINAPI Attach(HWND hWnd, LPCTSTR lpWindowName);
	virtual LPCTSTR WINAPI GetSkinPath();
	virtual duResManager *WINAPI GetResManager() { return m_pResManager; }
	virtual duPlugin *WINAPI CreateControl(LPCTSTR lpszType, LPCTSTR lpszName, duPlugin *pParent);
	virtual BOOL WINAPI ExecScript(LPCTSTR lpszFuncName, int nParamCount, VARIANT *pParams);
	virtual int WINAPI GetWindowCount();
	virtual HWND WINAPI GetWindowAt(int nIndex);
	virtual PBYTE WINAPI GetResource(LPCTSTR lpszResName, HANDLE &hMem, IStream *&pStream);
	virtual void WINAPI ReleaseResource(HANDLE hMem, IStream *pStream);
	
	virtual void WINAPI OutputDebugString(LPCTSTR lpszOutput);
	virtual BOOL WINAPI ShowWindow(HWND hWnd, int nCmdShow);
	virtual BOOL WINAPI DestroyWindow(HWND hWnd);
	virtual void WINAPI PostQuitMessage(int nExitCode);
	virtual BOOL WINAPI SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
	virtual int WINAPI GetRed(int color);
	virtual int WINAPI GetGreen(int color);
	virtual int WINAPI GetBlue(int color);
	virtual BOOL WINAPI ChangeSkin(LPCTSTR lpszFile);
	virtual void WINAPI Redraw(duPlugin *pPlugin);

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duWindowManager"); }

protected:
	TCHAR m_szSkinPath[MAX_PATH];
	TiXmlDocument *m_pDocXml;
	duResManager *m_pResManager;
	BOOL m_fZip;
	HZIP m_hZip;
	duJavaScript *m_pJavaScript;
	duImage *m_pTrialImage;
	
	map<HWND, duCtrlManager *> m_mapCtrlManager;
};
