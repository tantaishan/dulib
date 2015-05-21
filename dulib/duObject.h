//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duObject.h
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "oaidl.h"
#include <tchar.h>
#include <map>
using namespace std;

// duObject
//
//
class duObject : public IDispatch
{
public:
	duObject();
	~duObject();

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
		DISPPARAMS * pdispparams, VARIANT *pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duObject"); }

public:
	void SetTypeInfo(ITypeInfo *pTypeInfo) { m_pTypeInfo = pTypeInfo; }
	FUNCDESC *GetFuncDesc(DISPID dispId);
	
protected:
	ITypeInfo *m_pTypeInfo;
	ULONG m_lRef;
};
