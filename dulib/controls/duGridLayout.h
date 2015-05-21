//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duGridLayout.h
//
//  History:    JAN-09-2010   Denny Chen  Created
//
//--------------------------------------------------------------------------
#pragma once

#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

// duGridLayout
//
// 
class duGridLayout : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);
	
	virtual BOOL WINAPI IsGridLayout() { return m_fGrid; }
	virtual void WINAPI SetGridLayout(BOOL fGrid) { m_fGrid = fGrid; }

	virtual int WINAPI GetStartX() { return m_nStartX; }
	virtual void WINAPI SetStartX(int nStartX) { m_nStartX = nStartX; }

	virtual int WINAPI GetStartY() { return m_nStartY; }
	virtual void WINAPI SetStartY(int nStartY) { m_nStartY = nStartY; }

	virtual int WINAPI GetXSpace() { return m_nXSpace; }
	virtual void WINAPI SetXSpace(int nXSpace) { m_nXSpace = nXSpace; }

	virtual int WINAPI GetYSpace() { return m_nYSpace; }
	virtual void WINAPI SetYSpace(int nYSpace) { m_nYSpace = nYSpace; }

	virtual int WINAPI GetMaxCount() { return m_nMaxCount; }

	virtual int WINAPI GetItemWidth() { return m_nItemWidth; }
	virtual void WINAPI SetItemWidth(int nItemWidth) { m_nItemWidth = nItemWidth; }

	virtual int WINAPI GetItemHeight() { return m_nItemHeight; }
	virtual void WINAPI SetItemHeight(int nItemHeight) { m_nItemHeight = nItemHeight; }

	virtual void WINAPI AdjustGrid();
	
public:
	duGridLayout();
	~duGridLayout();
	
protected:
	BOOL m_fGrid;
	int m_nStartX;
	int m_nStartY;
	int m_nXSpace;
	int m_nYSpace;
	int m_nMaxCount;
	int m_nItemWidth;
	int m_nItemHeight;
};
