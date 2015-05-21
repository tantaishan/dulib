//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duTypeLib.h
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duTypeLib.h"

duTypeLib::duTypeLib()
{
	m_mapTypeInfo.clear();
	m_mapCache.clear();
}

duTypeLib::~duTypeLib()
{
	map<wstring, ITypeInfo *>::iterator iter = m_mapTypeInfo.begin();
	for (; iter != m_mapTypeInfo.end(); iter++)
	{
		ITypeInfo *pTypeInfo = iter->second;
		if (pTypeInfo)
		{
			pTypeInfo->Release();
			pTypeInfo = NULL;
		}
	}
	m_mapTypeInfo.clear();


	//map<ITypeInfo *, mapFuncDesc *> m_mapCache;
	//mapFuncDesc

	map<ITypeInfo *, mapFuncDesc *>::iterator iterCache = m_mapCache.begin();
	for (; iterCache != m_mapCache.end(); iterCache++)
	{
		mapFuncDesc *pFuncDesc = iterCache->second;
		if (pFuncDesc)
		{
			mapFuncDesc::iterator iterFunc = pFuncDesc->begin();
			for (; iterFunc != pFuncDesc->end(); iterFunc++)
			{
				FUNCDESC *p = iterFunc->second;
				if (p)
				{
					delete p;
				}
			}

			pFuncDesc->clear();
		}
	}

	m_mapCache.clear();
}

void duTypeLib::InitTypeInfo(HMODULE hModule)
{
	wchar_t szLibPath[MAX_PATH];
	ZeroMemory(szLibPath, MAX_PATH * sizeof(wchar_t));
	::GetModuleFileNameW(hModule, szLibPath, MAX_PATH);
	
	ITypeLib *pTypeLib = NULL;
	HRESULT hr = ::LoadTypeLib((OLECHAR *)szLibPath, &pTypeLib);
	if (hr != S_OK || pTypeLib == NULL)
		return;
	
	int nTypeInfoCount = pTypeLib->GetTypeInfoCount();
	int i = 0;
	for (; i < nTypeInfoCount; i++)
	{
		BSTR bstrName = NULL;
		pTypeLib->GetDocumentation(i, &bstrName, NULL, NULL, NULL);
		
		ITypeInfo *pTypeInfo = NULL;
		pTypeLib->GetTypeInfo(i, &pTypeInfo);
		
		wstring strName = bstrName;
		m_mapTypeInfo[strName] = pTypeInfo;
		::SysFreeString(bstrName);
	}
	
	pTypeLib->Release();
}

ITypeInfo *duTypeLib::GetTypeInfo(LPWSTR lpszTypeName)
{
	wstring strName = lpszTypeName;
	map<wstring, ITypeInfo *>::iterator iter = m_mapTypeInfo.find(strName);
	if (iter != m_mapTypeInfo.end())
		return iter->second;

	return NULL;
}

mapFuncDesc *duTypeLib::GetMapCache(ITypeInfo *pTypeInfo)
{
	if (pTypeInfo == NULL)
		return NULL;

	map<ITypeInfo *, mapFuncDesc *>::iterator iterFind = m_mapCache.find(pTypeInfo);
	if (iterFind != m_mapCache.end())
		return iterFind->second;

	mapFuncDesc *pMapDesc = new mapFuncDesc;
	pMapDesc->clear();

	TYPEATTR *typeAttr = NULL;
	pTypeInfo->GetTypeAttr(&typeAttr);
	int i;
	for (i = 0;i < typeAttr->cFuncs; i++)
	{
		FUNCDESC *funcDesc = NULL;
		pTypeInfo->GetFuncDesc(i, &funcDesc);

		FUNCDESC *newFuncDesc = new FUNCDESC;
		memcpy(newFuncDesc, funcDesc, sizeof(FUNCDESC));
		pMapDesc->insert(pair<DISPID, FUNCDESC *>(funcDesc->memid, newFuncDesc));
		pTypeInfo->ReleaseFuncDesc(funcDesc);
	}
	pTypeInfo->ReleaseTypeAttr(typeAttr);

	m_mapCache[pTypeInfo] = pMapDesc;
	return pMapDesc;
}
