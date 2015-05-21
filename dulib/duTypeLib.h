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
#pragma once
#include "duUtility.h"

typedef map<DISPID, FUNCDESC *> mapFuncDesc;

// duTypeLib
//
//
class duTypeLib
{
public:
	duTypeLib();
	~duTypeLib();
	
public:
	void InitTypeInfo(HMODULE hModule);
	ITypeInfo *GetTypeInfo(LPWSTR lpszTypeName);
	mapFuncDesc *GetMapCache(ITypeInfo *pTypeInfo);
	
protected:
	map<wstring, ITypeInfo *> m_mapTypeInfo;
	map<ITypeInfo *, mapFuncDesc *> m_mapCache;
};

