//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duObject.cpp
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duObject.h"
#include "duTypeLib.h"

extern duTypeLib *g_pTypeLib;

HRESULT InvokeHelper(void *pThis, FUNCDESC *funcDesc, DISPPARAMS* pdispparams, VARIANT* pvarResult);
extern "C" DWORD WINAPI InvokeVTableFunc(void *pThis, FARPROC funcAddr, DWORD *pParam, int nParamCount);


static const GUID IDD_duPlugin = 
{ 0x6F76069F, 0x2BD3, 0x4653, {0xBF, 0x40, 0x45, 0x1A, 0x24, 0x61, 0xD0, 0xB0} };

duObject::duObject() :
	 m_lRef(1)
	,m_pTypeInfo(NULL)
{
}

duObject::~duObject()
{
}

ULONG WINAPI duObject::AddRef()
{
	m_lRef++;
	return m_lRef;
}

ULONG WINAPI duObject::Release()
{
	m_lRef--;
	return m_lRef;
}

HRESULT WINAPI duObject::QueryInterface(REFIID iid, LPVOID* ppvObj)
{
	if (IsEqualIID(iid, IID_IDispatch) || IsEqualIID(iid, IID_IUnknown) || IsEqualIID(iid, IDD_duPlugin))
	{
		*ppvObj = this;
		return S_OK;
	}
	
	if (m_pTypeInfo)
	{
		TYPEATTR *ta = NULL;
		m_pTypeInfo->GetTypeAttr(&ta);
		if (IsEqualIID(ta->guid, iid))
		{
			*ppvObj = this;
			m_pTypeInfo->ReleaseTypeAttr(ta);
			return S_OK;
		}
		m_pTypeInfo->ReleaseTypeAttr(ta);
	}

	return E_NOINTERFACE; 
}

HRESULT WINAPI duObject::GetTypeInfoCount(UINT *pctinfo)
{
	if (pctinfo == NULL)
      return E_INVALIDARG;

	*pctinfo = 1;
	return NOERROR;
}

HRESULT WINAPI duObject::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	if (ppTInfo == NULL)
		return E_INVALIDARG;

   *ppTInfo = NULL;
	if (iTInfo != 0)
		return DISP_E_BADINDEX;

	if (!m_pTypeInfo)
		return E_NOTIMPL;
		
	m_pTypeInfo->AddRef();
	*ppTInfo = m_pTypeInfo;
	return NOERROR;
}

HRESULT WINAPI duObject::GetIDsOfNames(REFIID riid, OLECHAR ** rgszNames, UINT cNames, 
											  LCID lcid, DISPID * rgDispId)
{	
	if (m_pTypeInfo == NULL)
		return DISP_E_UNKNOWNNAME;

	HRESULT hr = ::DispGetIDsOfNames(m_pTypeInfo, rgszNames, cNames, rgDispId);
	return hr;
}

HRESULT WINAPI duObject::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, 
									   DISPPARAMS * pdispparams, VARIANT *pVarResult, 
									   EXCEPINFO * pExcepInfo, UINT * puArgErr)
{	
	if (m_pTypeInfo == NULL)
		return DISP_E_MEMBERNOTFOUND;
	
	FUNCDESC *funcDesc = GetFuncDesc(dispIdMember);
	if (funcDesc == NULL)
		return DISP_E_MEMBERNOTFOUND;
	
	HRESULT hr = InvokeHelper(this, funcDesc, pdispparams, pVarResult);
	//HRESULT hr = ::DispInvoke(this, m_pTypeInfo, dispIdMember, wFlags, pdispparams, pVarResult, pExcepInfo, puArgErr);
	return hr;
}

FUNCDESC *duObject::GetFuncDesc(DISPID dispId)
{
	mapFuncDesc *pDesc = g_pTypeLib->GetMapCache(m_pTypeInfo);
	if (pDesc)
	{
		mapFuncDesc::iterator iterFuncDesc = pDesc->find(dispId);
		if (iterFuncDesc != pDesc->end())
			return iterFuncDesc->second;
	}

	return NULL;
}

#define MAX_PARAM_NUMBER      32

HRESULT InvokeHelper(void *pThis, FUNCDESC *funcDesc, DISPPARAMS* pdispparams, VARIANT* pvarResult)
{
	if (!pThis || !funcDesc)
		return DISP_E_MEMBERNOTFOUND;

	int nParamCount = (int)funcDesc->cParams;
	if (pdispparams->cArgs != nParamCount) 
		return DISP_E_BADPARAMCOUNT;

	DWORD _result = 0;
	DWORD *pParam = new DWORD[MAX_PARAM_NUMBER];
	
	UINT nIndex;
	for (nIndex = 0; nIndex < pdispparams->cArgs; nIndex++)
	{
		DWORD param = 0;
		VARIANT *pVar = &pdispparams->rgvarg[nIndex];
		if (pVar->vt == VT_R8)
		{
			float r4 = (float)pVar->dblVal;
			int *p = (int *)&r4;
			param = (DWORD)*p;
		}
		else if (pVar->vt == VT_BOOL) //VARIANT_TRUE
			param = pVar->boolVal ? TRUE : FALSE;
		else
			param = (DWORD)pVar->ulVal;
		
		pParam[pdispparams->cArgs - 1 - nIndex] = param;
	}
	
	FARPROC *pVtable = *(FARPROC**)pThis;
	FARPROC funcAddr = pVtable[funcDesc->oVft/sizeof(void *)];	
	_result = InvokeVTableFunc(pThis, funcAddr, pParam, nParamCount);
	delete [] pParam;

	if (pvarResult)
	{
		VARTYPE vt = funcDesc->elemdescFunc.tdesc.vt;
		if (vt == VT_LPWSTR && _result != 0)
		{
			pvarResult->vt = VT_BSTR;
			pvarResult->bstrVal = ::SysAllocString((const OLECHAR *)_result);
		}
		else if (vt == VT_EMPTY)
			pvarResult->vt = VT_EMPTY;
		else if (vt == VT_BOOL)
		{
			pvarResult->vt = VT_BOOL;
			pvarResult->boolVal = _result ? VARIANT_TRUE : VARIANT_FALSE;
		}
		else if (vt == VT_PTR)
		{
			pvarResult->vt = VT_DISPATCH;
			pvarResult->ulVal = _result;
		}
		else
		{
			pvarResult->vt = funcDesc->elemdescFunc.tdesc.vt;
			pvarResult->ulVal = _result;
		}
	}

	return S_OK;
}

