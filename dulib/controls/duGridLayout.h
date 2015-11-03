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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);
	
	virtual BOOL IsGridLayout() { return m_fGrid; }
	virtual void SetGridLayout(BOOL fGrid) { m_fGrid = fGrid; }

	virtual int GetStartX() { return m_nStartX; }
	virtual void SetStartX(int nStartX) { m_nStartX = nStartX; }

	virtual int GetStartY() { return m_nStartY; }
	virtual void SetStartY(int nStartY) { m_nStartY = nStartY; }

	virtual int GetXSpace() { return m_nXSpace; }
	virtual void SetXSpace(int nXSpace) { m_nXSpace = nXSpace; }

	virtual int GetYSpace() { return m_nYSpace; }
	virtual void SetYSpace(int nYSpace) { m_nYSpace = nYSpace; }

	virtual int GetMaxCount() { return m_nMaxCount; }

	virtual int GetItemWidth() { return m_nItemWidth; }
	virtual void SetItemWidth(int nItemWidth) { m_nItemWidth = nItemWidth; }

	virtual int GetItemHeight() { return m_nItemHeight; }
	virtual void SetItemHeight(int nItemHeight) { m_nItemHeight = nItemHeight; }

	virtual void AdjustGrid();
	
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
