//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duSpin.h
//
//  History:    Nov-17-2009   Steven Li  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duEdit.h"

// duSpin
//
//
class duSpin : public duEdit
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);
	virtual void WINAPI OnMouseLDown(POINT pt);
	virtual void WINAPI OnMouseLUp(POINT pt);
	virtual void WINAPI OnMouseDbClick(POINT pt);
	virtual void WINAPI OnMouseLeave(POINT pt);

	virtual int WINAPI GetValue();
	virtual void WINAPI SetValue(int nVal);

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duSpin"); }	

public:
	duSpin();
	~duSpin();
	
protected:
	int m_nVal;
	int m_nSpinLeft;
	int n_nSpinWidth;
	int m_nSpinTop;
	int m_nSpinBottom;
	TCHAR m_szUpStyle[MAX_NAME];
	TCHAR m_szDownStyle[MAX_NAME];
	
protected:
	UINT m_uUpState;
	UINT m_uDownState;
};

