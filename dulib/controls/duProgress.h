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
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);

	virtual int WINAPI GetPos() { return (int)m_flPos; }
	virtual void WINAPI SetPos(int nPos) { m_flPos = (float)nPos; }
	
	virtual int WINAPI GetLeftBorder() { return m_nLeftBorder; }
	virtual void WINAPI SetLeftBorder(int nLeftBorder) { m_nLeftBorder = nLeftBorder; }

	virtual int WINAPI GetRightBorder() { return m_nRightBorder; }
	virtual void WINAPI SetRightBorder(int nRightBorder) { m_nRightBorder = nRightBorder; }

	virtual int WINAPI GetTopBorder() { return m_nTopBorder; }
	virtual void WINAPI SetTopBorder(int nTopBorder) { m_nTopBorder = nTopBorder; }

	virtual int WINAPI GetBottomBorder() { return m_nBottomBorder; }
	virtual void WINAPI SetBottomBorder(int nBottomBorder) { m_nBottomBorder = nBottomBorder; }

	virtual LPCTSTR WINAPI GetProgressStyle() { return m_szProgressStyle; }
	virtual void WINAPI SetProgressStyle(LPCTSTR lpszStyle);

	virtual float WINAPI GetPosfloat() { return m_flPos; }
	virtual void WINAPI SetPosfloat(float nPos) { m_flPos = nPos; }

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duProgress"); }	

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

