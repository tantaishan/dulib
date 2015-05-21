//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duFlashEventSink.h
//
//  History:
//
//--------------------------------------------------------------------------
#pragma once

#include "duUtility.h"
#include <oaidl.h>
#include <tchar.h>
#include <string>
#include <map>

using namespace std;

// duFlashEventSink
//
//
class duFlashEventSink : public IDispatch
{
public:
	duFlashEventSink();
	~duFlashEventSink();

	// IUnknown接口
	virtual ULONG WINAPI AddRef();
	virtual ULONG WINAPI Release();
	virtual HRESULT WINAPI QueryInterface(REFIID iid, LPVOID* ppvObj);

	// IDispatch接口
	virtual HRESULT WINAPI GetTypeInfoCount(UINT *pctinfo);
	virtual HRESULT WINAPI GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
	virtual HRESULT WINAPI GetIDsOfNames(REFIID riid, OLECHAR ** rgszNames, UINT cNames, 
			LCID lcid, DISPID * rgDispId);

	virtual HRESULT WINAPI Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, 
		DISPPARAMS *pdispparams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
	{
		wstring name;
		wstring param_list;

		map<MEMBERID, wstring>::iterator iterFind = m_MemberNameMap.find(dispIdMember);
		if (iterFind == m_MemberNameMap.end())
		{
			//name.Format(L"<%d>",dispIdMember);
			TCHAR szBuf[1024];
			_stprintf(szBuf, _T("%d"), dispIdMember);
			name = szBuf;
		}

		for(unsigned int i = 0;i < pdispparams->cArgs; ++i)
		{
			wstring pname;
			VARIANT Var = pdispparams->rgvarg[i];
			VariantToString(Var, pname);
			if (i == 0)
				param_list = pname;
			else
			{
				TCHAR szBuf[1024];
				_stprintf(szBuf, _T("%s"), pname.c_str());
				param_list += szBuf;
			}
		}

		NotifyFlashCtrl(dispIdMember, name, param_list);
		return E_NOTIMPL;
	}

public:
	HRESULT GetIIDTypeInfo(IID iid, ITypeInfo **ppInfo, IUnknown *pRelateObj);
	HRESULT VariantToString(VARIANT var, wstring& out);
	HRESULT IIDToString(IID iid, wstring& out);
	
public:
	void Init(IID iid, void *pFlash, IUnknown *pUnk);
	void NotifyFlashCtrl(DISPID dispId, wstring name, wstring param_list);

public:
	ULONG m_refCnt;
	IID m_iid;
	map<MEMBERID, wstring> m_MemberNameMap;
	wstring m_InterfaceName;
	void *m_pFlash;
};
