//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duFlashEventSink.cpp
//
//  History:
//
//--------------------------------------------------------------------------
#include "duFlashEventSink.h"
#include "duFlash.h"

extern void FlashEvent(void *pFlash, wstring name, wstring param_list);

duFlashEventSink::duFlashEventSink()
{
	m_refCnt = 0;
	m_iid = IID_NULL;
	m_MemberNameMap.clear();
	m_InterfaceName = _T("");
	m_pFlash = NULL;
}

duFlashEventSink::~duFlashEventSink()
{
}

ULONG WINAPI duFlashEventSink::AddRef()
{
	return ++m_refCnt;
}

ULONG WINAPI duFlashEventSink::Release()
{
	if (--m_refCnt == 0)
	{
		delete this;
		return 0;
	}
	return m_refCnt;
}

HRESULT WINAPI duFlashEventSink::QueryInterface(REFIID riid, LPVOID* ppvObject)
{
	*ppvObject = NULL;
	if (IID_IUnknown == riid)
		*ppvObject = (IUnknown*)this;
	else if (IID_IDispatch == riid || m_iid == riid)
		*ppvObject = (IDispatch*)this;
	else
		return E_NOINTERFACE;

	AddRef();
	return S_OK;
}

HRESULT WINAPI duFlashEventSink::GetTypeInfoCount(UINT *pctinfo)
{
	return E_NOTIMPL;
}

HRESULT WINAPI duFlashEventSink::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

HRESULT WINAPI duFlashEventSink::GetIDsOfNames(REFIID riid, OLECHAR ** rgszNames, UINT cNames, 
											  LCID lcid, DISPID * rgDispId)
{
	return E_NOTIMPL;
}

void duFlashEventSink::Init(IID iid, void *pFlash, IUnknown *pRelateObj)
{
	bool isDispatch = false;
	m_pFlash = pFlash;
	m_refCnt = 1;
	m_iid = iid;
	ITypeInfo *pInfo;
	HRESULT hr = GetIIDTypeInfo(iid,&pInfo,pRelateObj);
	if (SUCCEEDED(hr))
	{		
		TYPEATTR *attr;
		if (SUCCEEDED(pInfo->GetTypeAttr(&attr)))
		{
			if (attr->typekind == TKIND_DISPATCH) 
				isDispatch = true;
			pInfo->ReleaseTypeAttr(attr);
		}

		if (isDispatch)
		{
			unsigned int uIndex;
			for(uIndex = 0;;++uIndex)
			{
				FUNCDESC * fd;
				BSTR funcName;
				unsigned int cNames;
				if (FAILED(pInfo->GetFuncDesc(uIndex,&fd))) 
					break;

				if (SUCCEEDED(pInfo->GetNames(fd->memid, &funcName, 1, &cNames)) && cNames>=1)
				{
					m_MemberNameMap[fd->memid] = wstring(funcName);
					SysFreeString(funcName);
				}
			}
		}
		pInfo->Release();
	}
	
	if (!isDispatch)
		m_iid = IID_NULL;
}

void duFlashEventSink::NotifyFlashCtrl(DISPID dispId, wstring name, wstring param_list)
{
	FlashEvent(m_pFlash, name, param_list);
}

HRESULT duFlashEventSink::GetIIDTypeInfo(IID iid, ITypeInfo **ppInfo, IUnknown *pRelateObj)
{
	ITypeInfo *pObjTypeInfo;
	HRESULT hr;
	hr = pRelateObj->QueryInterface(IID_ITypeInfo,(void**)&pObjTypeInfo);
	if (FAILED(hr))
	{
		IProvideClassInfo * pProvideInfo;
		hr = pRelateObj->QueryInterface(IID_IProvideClassInfo,(void**)&pProvideInfo);
		if (FAILED(hr)) 
			return hr;

		hr = pProvideInfo->GetClassInfo(&pObjTypeInfo);
		pProvideInfo->Release();
		if (FAILED(hr))
			return hr;
	}
	
	unsigned int uIndex;
	for (uIndex = 0; ;++uIndex)
	{
		HREFTYPE refType;
		hr = pObjTypeInfo->GetRefTypeOfImplType(uIndex,&refType);
		if (FAILED(hr)) break;
		hr = pObjTypeInfo->GetRefTypeInfo(refType,ppInfo);
		if (SUCCEEDED(hr))
		{
			TYPEATTR *attr;
			ITypeInfo *ti = *ppInfo;
			if (SUCCEEDED(ti->GetTypeAttr(&attr)) && attr->guid == iid)
			{
				pObjTypeInfo->Release();
				ti->ReleaseTypeAttr(attr);
				return S_OK;
			}
			else
			{
				ti->ReleaseTypeAttr(attr);
				ti->Release();
				*ppInfo = 0;
			}
		}
	}
	hr = E_FAIL;
	ITypeLib *pLib = NULL;
	hr = pObjTypeInfo->GetContainingTypeLib(&pLib,&uIndex);
	if(SUCCEEDED(hr))
		hr = pLib->GetTypeInfoOfGuid(iid,ppInfo);

	if (pLib) 
		pLib->Release();

	pObjTypeInfo->Release();
	return hr;
}

HRESULT duFlashEventSink::VariantToString(VARIANT var, wstring& out)
{
	TCHAR szBuf[1024];

	if (var.vt == VT_BSTR)
	{
		_stprintf(szBuf, _T("\"%s\""), var.bstrVal);
		out = szBuf;
		return S_OK;
	}

	if (var.vt == VT_EMPTY)
	{
		out = _T("<EMPTY>");
		return S_OK;
	}

	if (var.vt == VT_NULL)
	{
		out = _T("<NULL>");
		return S_OK;
	}

	VARIANT outvar;
	VariantInit(&outvar);
	HRESULT hr = VariantChangeType(&outvar, &var, VARIANT_ALPHABOOL|VARIANT_NOUSEROVERRIDE, VT_BSTR);
	if (SUCCEEDED(hr))
	{
		out = outvar.bstrVal;
		::VariantClear(&outvar);
		return S_OK;
	}

	_stprintf(szBuf, _T("<Unknown:%d>"), var.vt);
	out = szBuf;
	return S_OK;
}

HRESULT duFlashEventSink::IIDToString(IID iid, wstring& out)
{
	LPOLESTR striid;
	StringFromIID(iid,&striid);

	TCHAR szRegPath[1024];
	_stprintf(szRegPath, _T("Interface\\%s"), striid);

	WCHAR szRegValue[512];
	LONG ValueLen=512;
	if (ERROR_SUCCESS == RegQueryValueW(HKEY_CLASSES_ROOT, szRegPath, szRegValue, &ValueLen))
		out = szRegValue;
	else
		out = striid;

	CoTaskMemFree(striid);
	return S_OK;
}

HRESULT AxAdviseAll(duFlash *pFlash, IUnknown *pUnk)
{
	HRESULT hr;
	IConnectionPointContainer * pContainer = NULL;
	IConnectionPoint * pConnectionPoint=NULL;
	IEnumConnectionPoints * pEnum = NULL;
	hr = pUnk->QueryInterface(IID_IConnectionPointContainer,(void**)&pContainer);
	if (FAILED(hr))
		goto error1;
		
	hr = pContainer->EnumConnectionPoints(&pEnum);
	if (FAILED(hr))
		goto error1;

	ULONG uFetched;
	while (S_OK == (pEnum->Next(1,&pConnectionPoint,&uFetched)) && uFetched >= 1)
	{
		DWORD dwCookie;
		IID iid;
		hr = pConnectionPoint->GetConnectionInterface(&iid);
		if (FAILED(hr))
			iid = IID_NULL;
			
		duFlashEventSink * pSink = new duFlashEventSink;
		pSink->Init(iid, pFlash, pUnk);
		hr = pConnectionPoint->Advise(pSink,&dwCookie);
		pSink->Release();
		pConnectionPoint->Release();
		pConnectionPoint = NULL;
		pSink = NULL;
	}
	hr = S_OK;

error1:
	if (pEnum)
		pEnum->Release();
	
	if (pContainer)
		pContainer->Release();
	
	if (pConnectionPoint)
		pConnectionPoint->Release();
	
	return hr;
}


