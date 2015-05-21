//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duSplitter.h
//
//  History:    Mar-08-2010   Denny Chen  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

// duSplitter
// 
// 
class duSplitter : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);

	virtual void WINAPI OnMouseLeave(POINT pt);
	virtual void WINAPI OnMouseLDown(POINT pt);
	virtual void WINAPI OnMouseLUp(POINT pt);
	virtual void WINAPI OnMouseMove(POINT pt);

	virtual void WINAPI MoveHorz(int nDistance);
	virtual void WINAPI MoveVert(int nDistance);

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duSplitter"); }	

public:
	duSplitter();
	~duSplitter();

protected:
	void OnInvertTracker(duRect &rect);
	HBRUSH GetHalftoneBrush();

protected:
	TCHAR m_szFName[MAX_NAME];
	TCHAR m_szSName[MAX_NAME];
	int  m_nFMinPixel;
	int  m_nSMinPixel;
	BOOL m_fVert;
	BOOL m_fDockFirst;

protected:
	BOOL   m_fTracking;
	duPoint m_ptStart;
	duRect  m_rectLast;
};
