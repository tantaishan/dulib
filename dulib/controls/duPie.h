//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duPie.h
//
//  History:    JAN-09-2010   Denny Chen  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

// duPie
//
//
class duPie : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);

	virtual float WINAPI GetPercent() { return m_fPercent; }
	virtual void WINAPI SetPercent(float fPercent) { m_fPercent = fPercent; }
	
	virtual COLORREF WINAPI GetAllColor() { return m_clrAll; }
	virtual void WINAPI SetAllColor(COLORREF clrAll) { m_clrAll = clrAll; }
	
	virtual COLORREF WINAPI GetPercentColor() { return m_clrPercent; }
	virtual void WINAPI SetPercentColor(COLORREF clrPercent) { m_clrPercent = clrPercent; }
	
public:
	duPie();
	~duPie();
	
protected:
	float m_fPercent;
	COLORREF m_clrAll;
	COLORREF m_clrPercent;
};
