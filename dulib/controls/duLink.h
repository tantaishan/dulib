//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duLink.h
//
//  History:    Dec-18-2010   Tony Chen  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

#define MAX_URL_LENGTH 	1024

class duLink : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate() {}
	virtual void WINAPI DrawObject(HDC hDC);

	virtual void WINAPI OnMouseIn(POINT pt);
	virtual void WINAPI OnMouseLeave(POINT pt);
	virtual void WINAPI OnMouseLDown(POINT pt);
	virtual void WINAPI OnMouseLUp(POINT pt);

	virtual LPCTSTR WINAPI GetURL();
	virtual void WINAPI SetURL(LPCTSTR lpszURL);
	virtual void WINAPI OpenURL();

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duLink"); }	

public:
	duLink();
	~duLink();

protected:
	TCHAR m_szURL[MAX_URL_LENGTH];
};
