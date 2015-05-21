//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duRgn.h
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duResBase.h"
#include "duResManager.h"

// duRgn
//
//
class duRgn : public duResBase
{
public:
	virtual UINT WINAPI GetType() { return DU_RES_RGN; }
	virtual BOOL WINAPI OnCreate(TiXmlElement *pElement);

	virtual LPCTSTR WINAPI GetRgnType() { return m_szType; }
	virtual void WINAPI SetRgnType(LPCTSTR lpszType);

	virtual int WINAPI GetRoundX() { return m_nRoundX; }
	virtual void WINAPI SetRoundX(int nRoundX) { m_nRoundX = nRoundX; }
	virtual int WINAPI GetRoundY() { return m_nRoundY; }
	virtual void WINAPI SetRoundY(int nRoundY) { m_nRoundY = nRoundY; }
	
	virtual LPCTSTR WINAPI GetStyle() { return m_szStyle; }
	virtual void WINAPI SetStyle(LPCTSTR lpszStyle);

	virtual HRGN WINAPI GetHRgn(int nWidth, int nHeight);

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duRgn"); }

public:
	duRgn();
	~duRgn();

protected:
	HRGN WINAPI GetStyleHRgn(int nWidth, int nHeight);
	HRGN WINAPI GetNormalStyleHRgn(duImageStyle *pStyle, int nWidth, int nHeight);
	HRGN WINAPI GetStretchStyleHRgn(duImageStyle *pStyle, int nWidth, int nHeight);
	HRGN WINAPI GetStretchVertStyleHRgn(duImageStyle *pStyle, int nWidth, int nHeight);
	HRGN WINAPI GetStretchHorzStyleHRgn(duImageStyle *pStyle, int nWidth, int nHeight);
	HRGN WINAPI GetStretchPartAllStyleHRgn(duImageStyle *pStyle, int nWidth, int nHeight);
		
protected:
	TCHAR m_szType[MAX_NAME];
	int m_nRoundX;
	int m_nRoundY;
	TCHAR m_szStyle[MAX_NAME];
	HRGN m_hRgn;
	int m_nRgnWidth;
	int m_nRgnHeight;
};
