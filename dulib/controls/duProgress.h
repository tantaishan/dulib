//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duProgress.h
//
//  History:    MAR-16-2010   Denny Chen  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

// duProgress
// 
// 
class duProgress : public duPlugin
{
public:
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);

	virtual int GetPos() { return (int)m_flPos; }
	virtual void SetPos(int nPos) { m_flPos = (float)nPos; }
	
	virtual int GetLeftBorder() { return m_nLeftBorder; }
	virtual void SetLeftBorder(int nLeftBorder) { m_nLeftBorder = nLeftBorder; }

	virtual int GetRightBorder() { return m_nRightBorder; }
	virtual void SetRightBorder(int nRightBorder) { m_nRightBorder = nRightBorder; }

	virtual int GetTopBorder() { return m_nTopBorder; }
	virtual void SetTopBorder(int nTopBorder) { m_nTopBorder = nTopBorder; }

	virtual int GetBottomBorder() { return m_nBottomBorder; }
	virtual void SetBottomBorder(int nBottomBorder) { m_nBottomBorder = nBottomBorder; }

	virtual LPCTSTR GetProgressStyle() { return m_szProgressStyle; }
	virtual void SetProgressStyle(LPCTSTR lpszStyle);

	virtual float GetPosfloat() { return m_flPos; }
	virtual void SetPosfloat(float nPos) { m_flPos = nPos; }

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duProgress"); }	

public:
	duProgress();
	~duProgress();
	
protected:
	duRect GetInnerRect(duRect &rect, float pos);
	
protected:
	float m_flPos;
	int m_nLeftBorder;
	int m_nRightBorder;
	int m_nTopBorder;
	int m_nBottomBorder;
	TCHAR m_szProgressStyle[MAX_NAME];
};

