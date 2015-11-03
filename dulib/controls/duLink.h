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
	virtual void RegisterControlProperty();
	virtual void OnCreate() {}
	virtual void DrawObject(HDC hDC);

	virtual void OnMouseIn(POINT pt);
	virtual void OnMouseLeave(POINT pt);
	virtual void OnMouseLDown(POINT pt);
	virtual void OnMouseLUp(POINT pt);

	virtual LPCTSTR GetURL();
	virtual void SetURL(LPCTSTR lpszURL);
	virtual void OpenURL();

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duLink"); }	

public:
	duLink();
	~duLink();

protected:
	TCHAR m_szURL[MAX_URL_LENGTH];
};
