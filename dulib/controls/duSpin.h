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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);
	virtual void OnMouseLDown(POINT pt);
	virtual void OnMouseLUp(POINT pt);
	virtual void OnMouseDbClick(POINT pt);
	virtual void OnMouseLeave(POINT pt);

	virtual int GetValue();
	virtual void SetValue(int nVal);

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duSpin"); }	

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

