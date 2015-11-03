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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);

	virtual float GetPercent() { return m_fPercent; }
	virtual void SetPercent(float fPercent) { m_fPercent = fPercent; }
	
	virtual COLORREF GetAllColor() { return m_clrAll; }
	virtual void SetAllColor(COLORREF clrAll) { m_clrAll = clrAll; }
	
	virtual COLORREF GetPercentColor() { return m_clrPercent; }
	virtual void SetPercentColor(COLORREF clrPercent) { m_clrPercent = clrPercent; }
	
public:
	duPie();
	~duPie();
	
protected:
	float m_fPercent;
	COLORREF m_clrAll;
	COLORREF m_clrPercent;
};
