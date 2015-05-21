//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duResBase.h
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duObject.h"
#include "tinyXmlW/tinyxml.h"

#define disp_duResBase_name                     0x0F01
#define disp_duResBase_type						0x0F02

class duResManager;

// duResBase
//
//
class duResBase : public duObject
{
public:
	virtual LPCTSTR WINAPI GetName() { return m_szName; }
	virtual void WINAPI SetName(LPCTSTR lpszName)
	{
		if (lpszName)
			_tcsncpy(m_szName, lpszName, MAX_NAME);
		else
			ZeroMemory(m_szName, MAX_NAME * sizeof(TCHAR));
	}
	virtual UINT WINAPI GetType() = 0;
	virtual BOOL WINAPI OnCreate(TiXmlElement *pElement) { return TRUE; }

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duResBase"); }

public:
	duResBase()
	{
		ZeroMemory(m_szName, MAX_NAME);
	}
	
	~duResBase() {}
	
	duResManager *GetResManager() { return m_pResManager; }
	void SetResManager(duResManager *pResManager) { m_pResManager = pResManager; }

protected:
	TCHAR m_szName[MAX_NAME];
	duResManager *m_pResManager;
};
