//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       ScriptObject.cpp
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "ScriptObject.h"



duJScriptSite::duJScriptSite()
{
	m_lRef = 0;
	m_mapNameItem.clear();	
}

duJScriptSite::~duJScriptSite()
{
	m_mapNameItem.clear();
}

HRESULT WINAPI duJScriptSite::QueryInterface(REFIID riid, void **ppvObject)
{
	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IActiveScriptSite))
	{
		*ppvObject = this;
		return S_OK;
	}

	*ppvObject = 0;
	return E_NOINTERFACE;
}

ULONG WINAPI duJScriptSite::AddRef()
{
	m_lRef++;
	return m_lRef;
}

ULONG WINAPI duJScriptSite::Release()
{
	m_lRef--;
	return m_lRef;
}

HRESULT WINAPI duJScriptSite::GetLCID(LCID *plcid)
{
	return E_NOTIMPL;
}

HRESULT WINAPI duJScriptSite::GetItemInfo(LPCOLESTR pstrName, DWORD dwReturnMask,
	IUnknown **ppiunkItem, ITypeInfo **ppti)
{
	if (dwReturnMask & SCRIPTINFO_IUNKNOWN)
	{
		int len = (int)wcslen(pstrName);
		if (len >= MAX_SCRIPT_NAME)
			return TYPE_E_ELEMENTNOTFOUND;

#ifndef _UNICODE
		char pObjectName[MAX_SCRIPT_NAME];
		WideCharToMultiByte(CP_ACP, NULL, pstrName, -1, pObjectName, MAX_SCRIPT_NAME, NULL, NULL);
		IUnknown *pIUnknown = GetNameItemDispatch(pObjectName);
#else
		IUnknown *pIUnknown = GetNameItemDispatch((LPTSTR)pstrName);
#endif

		*ppiunkItem = pIUnknown;
		if (pIUnknown)
			return S_OK;
	}

	return TYPE_E_ELEMENTNOTFOUND;
}

HRESULT WINAPI duJScriptSite::GetDocVersionString(BSTR *pbstrVersion)
{
	*pbstrVersion = NULL;
	return S_OK;
}

HRESULT WINAPI duJScriptSite::OnScriptTerminate(const VARIANT *pvarResult,
	const EXCEPINFO *pexcepinfo)
{
	return S_OK;
}

HRESULT WINAPI duJScriptSite::OnStateChange(SCRIPTSTATE ssScriptState)
{
	return S_OK;
}

HRESULT WINAPI duJScriptSite::OnScriptError(IActiveScriptError *pscripterror)
{
#ifdef _DEBUG
	if (pscripterror)
	{
		EXCEPINFO ei;
		ZeroMemory(&ei, sizeof(EXCEPINFO));
		pscripterror->GetExceptionInfo(&ei);

		DWORD dwSourceContext = 0;
		ULONG ulLineNumber = 0;
		LONG ichCharPosition = 0;
		pscripterror->GetSourcePosition(&dwSourceContext, &ulLineNumber, &ichCharPosition);
		
		_TRACE(_T("JSError: %s line[%d]\n"), ei.bstrDescription, ulLineNumber);

		if (ei.bstrDescription)
			::SysFreeString(ei.bstrDescription);
		if (ei.bstrHelpFile)
			::SysFreeString(ei.bstrHelpFile);
		if (ei.bstrSource)
			::SysFreeString(ei.bstrSource);
	}
#endif //#ifdef _DEBUG

	return S_OK;
}

HRESULT WINAPI duJScriptSite::OnEnterScript()
{
	return S_OK;
}

HRESULT WINAPI duJScriptSite::OnLeaveScript()
{
	return S_OK;
}

IUnknown *duJScriptSite::GetNameItemDispatch(LPTSTR pObjectName)
{
	if (pObjectName == NULL || *pObjectName == NULL)
		return NULL;

	IUnknown *pIUnknown = NULL;
	dustring strObjectName = pObjectName;
	map<dustring, IUnknown *>::iterator Iter = m_mapNameItem.find(strObjectName);
	if (Iter != m_mapNameItem.end())
		pIUnknown = Iter->second;
	
	return pIUnknown;
}

BOOL duJScriptSite::AddNameItem(LPTSTR pObjectName, IDispatch *pObject)
{
	if (pObjectName == NULL || *pObjectName == NULL || pObject == NULL)
		return FALSE;

	dustring strObjectName = pObjectName;
	m_mapNameItem[strObjectName] = (IUnknown *)pObject;
	return TRUE;
}


duJavaScript::duJavaScript() :
		 m_pJScript(NULL)
		,m_pJParser(NULL)
		,m_pJDispatch(NULL)
		,m_pJDispatchEx(NULL)
		,m_nDispGC(NULL)
		,m_hr(S_OK)
{
	::CoInitialize(NULL);
}

duJavaScript::~duJavaScript()
{
	ULONG uRef = 0;
	if (m_pJDispatchEx)
	{
		uRef = m_pJDispatchEx->Release();
		m_pJDispatchEx = NULL;
	}

	if (m_pJDispatch)
	{
		uRef = m_pJDispatch->Release();
		m_pJDispatch = NULL;
	}

	if (m_pJParser)
	{
		uRef = m_pJParser->Release();
		m_pJParser = NULL;
	}

	if (m_pJScript)
	{
		m_pJScript->SetScriptState(SCRIPTSTATE_DISCONNECTED);
		m_pJScript->SetScriptState(SCRIPTSTATE_CLOSED);

		uRef = m_pJScript->Release();
		m_pJScript = NULL;
	}
}

BOOL duJavaScript::InitJavaScript(LPTSTR pstrScriptCode)
{
	if (pstrScriptCode == NULL)
		return FALSE;

	ULONG uRef = 0;
	CLSID clsid;

	m_hr = CLSIDFromProgID(L"JScript", &clsid);

	// 创建JScript IActiveScript脚本引擎实例
	m_hr = CoCreateInstance(clsid, NULL, CLSCTX_ALL, IID_IActiveScript, 
		(void **)&m_pJScript);
	if (m_hr != S_OK)
		goto T_error;

	// 设置ScriptSite
	m_hr = m_pJScript->SetScriptSite(&m_JScriptSite);
	if (m_hr != S_OK)
		goto T_error;

	// 查询脚本分析接口
	m_hr = m_pJScript->QueryInterface(IID_IActiveScriptParse, (void **)&m_pJParser);
	if (m_hr != S_OK)
		goto T_error;

	// 初始化脚本分析接口
	m_hr = m_pJParser->InitNew();
	if (m_hr != S_OK)
		goto T_error;

	// 脚本字符串处理
	int len = (int)_tcslen(pstrScriptCode);
	if (len <= 0)
		goto T_error;

#ifndef _UNICODE
	wchar_t *wscriptCode = new wchar_t[len+1];
	MultiByteToWideChar(CP_ACP, NULL, pstrScriptCode, -1, wscriptCode, len+1);
#else
	wchar_t *wscriptCode = pstrScriptCode;
#endif

	// 分析脚本字符串
	EXCEPINFO expInfo;
	m_hr = m_pJParser->ParseScriptText(wscriptCode, NULL, NULL, NULL, 0, 0, 
		SCRIPTTEXT_ISPERSISTENT|SCRIPTTEXT_ISVISIBLE, NULL, &expInfo);
	if (m_hr != S_OK)
		goto T_error;

	// 设置脚本引擎运行
	m_hr = m_pJScript->SetScriptState(SCRIPTSTATE_CONNECTED);
	if (m_hr != S_OK)
		goto T_error;

	// 获得脚本执行接口
	m_hr = m_pJScript->GetScriptDispatch(NULL, &m_pJDispatch);
	if (m_hr != S_OK)
		goto T_error;
	
	m_hr = m_pJDispatch->QueryInterface(IID_IDispatchEx, (void **)&m_pJDispatchEx);
	if (m_hr != S_OK)
		goto T_error;

	BSTR bstrName;
	DISPID dispid;

	// 获得脚本垃圾回收函数接口标识符
	m_hr = m_pJDispatchEx->GetNextDispID(fdexEnumAll, DISPID_STARTENUM, &dispid);
	while (m_hr == NOERROR)
	{
		m_hr = m_pJDispatchEx->GetMemberName(dispid, &bstrName);
		if (wcscmp(bstrName, OLESTR("CollectGarbage")) == 0)
		{
			m_nDispGC = dispid;
			SysFreeString(bstrName);
			break;
		}

		SysFreeString(bstrName);
		m_hr = m_pJDispatchEx->GetNextDispID(fdexEnumAll, dispid, &dispid);
	}

	if (m_nDispGC == NULL)
		goto T_error;

	// 清理
#ifndef _UNICODE
	SAFE_DELETE_ARRAY(wscriptCode);
#endif

	return TRUE;

T_error:
#ifndef _UNICODE
	SAFE_DELETE_ARRAY(wscriptCode);
#endif

	if (m_pJDispatchEx)
	{
		uRef = m_pJDispatchEx->Release();
		m_pJDispatchEx = NULL;
	}

	if (m_pJDispatch)
	{
		uRef = m_pJDispatch->Release();
		m_pJDispatch = NULL;
	}

	if (m_pJParser)
	{
		uRef = m_pJParser->Release();
		m_pJParser = NULL;
	}

	if (m_pJScript)
	{
		m_pJScript->SetScriptState(SCRIPTSTATE_DISCONNECTED);
		m_pJScript->SetScriptState(SCRIPTSTATE_CLOSED);

		uRef = m_pJScript->Release();	
		m_pJScript = NULL;
	}

	return FALSE;
}

BOOL duJavaScript::AddObject(LPTSTR pObjectName, IDispatch *pObject)
{
	if (pObjectName == NULL || *pObjectName == NULL || pObject == NULL)
		return FALSE;

	if (m_pJScript == NULL)
		return FALSE;

	int len = (int)_tcslen(pObjectName);
	if (len >= MAX_SCRIPT_NAME)
		return FALSE;

#ifndef _UNICODE
	wchar_t pwObjectName[MAX_SCRIPT_NAME];
	MultiByteToWideChar(CP_ACP, NULL, pObjectName, -1, pwObjectName, MAX_SCRIPT_NAME);
#else
	const wchar_t *pwObjectName = pObjectName;
#endif

	if (!m_JScriptSite.AddNameItem(pObjectName, pObject))
		return FALSE;
	
	m_hr = m_pJScript->AddNamedItem(pwObjectName, SCRIPTITEM_GLOBALMEMBERS|SCRIPTITEM_ISPERSISTENT|SCRIPTITEM_ISVISIBLE);
	if (m_hr != S_OK)
		return FALSE;
	
	return TRUE;
}

BOOL duJavaScript::ExecScript(LPCTSTR pFuncName, int nParamCount, VARIANT *pParams)
{
	if (m_pJDispatch == NULL || pFuncName == NULL || *pFuncName == NULL)
		return FALSE;

	int len = (int)_tcslen(pFuncName);
	if (len <= 0)
		return FALSE;

	wchar_t wszFuncName[MAX_SCRIPT_NAME];

#ifndef _UNICODE
	MultiByteToWideChar(CP_ACP, NULL, pFuncName, -1, wszFuncName, MAX_SCRIPT_NAME);
#else
	wcscpy(wszFuncName, pFuncName);
#endif

	OLECHAR *pwszFuncName = wszFuncName;
	DISPID dispId = 0;
	m_hr = m_pJDispatch->GetIDsOfNames(IID_NULL, &pwszFuncName, 1, LOCALE_SYSTEM_DEFAULT, &dispId);

	if (m_hr != S_OK)
		return FALSE;

	VARIANT  varResult;
	EXCEPINFO ExcepInfo;
	unsigned int puArgErr;
	DISPPARAMS dispparams;

	try
	{
		dispparams.cArgs             = nParamCount;
		dispparams.cNamedArgs        = 0;
		dispparams.rgvarg            = pParams;
		dispparams.rgdispidNamedArgs = NULL;
	
		varResult.vt = VT_EMPTY;

		m_hr = m_pJDispatch->Invoke(dispId, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispparams, &varResult, &ExcepInfo, &puArgErr);

		dispparams.cArgs             = 0;
		dispparams.cNamedArgs        = 0;
		dispparams.rgvarg            = NULL;
		dispparams.rgdispidNamedArgs = NULL;

		// 执行脚本函数之后,立即进行垃圾回收.防止JavaScript缓存函数局部变量.
		m_hr = m_pJDispatchEx->InvokeEx(m_nDispGC, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD,&dispparams, &varResult, &ExcepInfo, NULL);
		if (m_hr != S_OK)
			return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}

	return TRUE;
}
