//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       ScriptObject.h
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#pragma once

#include "duUtility.h"
#include <ActivScp.h>
#include <DispEx.h>

#define MAX_SCRIPT_NAME 1024

// duJScriptSite
//
// IActiveScriptSite接口实现类
class duJScriptSite : public IActiveScriptSite
{
public:
	duJScriptSite();
	~duJScriptSite();

public:
	// IUnknown接口
	virtual HRESULT WINAPI QueryInterface(REFIID riid, void **ppvObject);
	virtual ULONG   WINAPI AddRef();
	virtual ULONG   WINAPI Release();

	// IActiveScriptSite接口
	virtual HRESULT WINAPI GetLCID(LCID *plcid);
	virtual HRESULT WINAPI GetItemInfo(LPCOLESTR pstrName, DWORD dwReturnMask,
		IUnknown **ppiunkItem, ITypeInfo **ppti);
	virtual HRESULT WINAPI GetDocVersionString(BSTR *pbstrVersion);
	virtual HRESULT WINAPI OnScriptTerminate(const VARIANT *pvarResult, 
		const EXCEPINFO *pexcepinfo);
	virtual HRESULT WINAPI OnStateChange(SCRIPTSTATE ssScriptState);
	virtual HRESULT WINAPI OnScriptError(IActiveScriptError *pscripterror);
	virtual HRESULT WINAPI OnEnterScript();
	virtual HRESULT WINAPI OnLeaveScript();

public:
	IUnknown *GetNameItemDispatch(LPTSTR pObjectName);
	BOOL AddNameItem(LPTSTR pObjectName, IDispatch *pObject);

protected:
	LONG m_lRef;
	map<dustring, IUnknown *> m_mapNameItem;
};


// duJavaScript
//
// JavaScript脚本引擎实例类
class duJavaScript
{
public:
	duJavaScript();
	~duJavaScript();

public:
	BOOL InitJavaScript(LPTSTR pstrScriptCode);
	BOOL AddObject(LPTSTR pObjectName, IDispatch *pObject);
	BOOL ExecScript(LPCTSTR pFuncName, int nParamCount, VARIANT *pParams);

protected:
	HRESULT m_hr;
	DISPID m_nDispGC;
	IActiveScript * m_pJScript;
	IActiveScriptParse *m_pJParser;
	IDispatch * m_pJDispatch;
	IDispatchEx * m_pJDispatchEx;
	duJScriptSite m_JScriptSite;
};
