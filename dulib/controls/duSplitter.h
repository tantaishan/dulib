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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);

	virtual void OnMouseLeave(POINT pt);
	virtual void OnMouseLDown(POINT pt);
	virtual void OnMouseLUp(POINT pt);
	virtual void OnMouseMove(POINT pt);

	virtual void MoveHorz(int nDistance);
	virtual void MoveVert(int nDistance);

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duSplitter"); }	

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
