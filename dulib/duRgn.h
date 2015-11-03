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
	virtual UINT GetType() { return DU_RES_RGN; }
	virtual BOOL OnCreate(TiXmlElement *pElement);

	virtual LPCTSTR GetRgnType() { return m_szType; }
	virtual void SetRgnType(LPCTSTR lpszType);

	virtual int GetRoundX() { return m_nRoundX; }
	virtual void SetRoundX(int nRoundX) { m_nRoundX = nRoundX; }
	virtual int GetRoundY() { return m_nRoundY; }
	virtual void SetRoundY(int nRoundY) { m_nRoundY = nRoundY; }
	
	virtual LPCTSTR GetStyle() { return m_szStyle; }
	virtual void SetStyle(LPCTSTR lpszStyle);

	virtual HRGN GetHRgn(int nWidth, int nHeight);

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duRgn"); }

public:
	duRgn();
	~duRgn();

protected:
	HRGN GetStyleHRgn(int nWidth, int nHeight);
	HRGN GetNormalStyleHRgn(duImageStyle *pStyle, int nWidth, int nHeight);
	HRGN GetStretchStyleHRgn(duImageStyle *pStyle, int nWidth, int nHeight);
	HRGN GetStretchVertStyleHRgn(duImageStyle *pStyle, int nWidth, int nHeight);
	HRGN GetStretchHorzStyleHRgn(duImageStyle *pStyle, int nWidth, int nHeight);
	HRGN GetStretchPartAllStyleHRgn(duImageStyle *pStyle, int nWidth, int nHeight);
		
protected:
	TCHAR m_szType[MAX_NAME];
	int m_nRoundX;
	int m_nRoundY;
	TCHAR m_szStyle[MAX_NAME];
	HRGN m_hRgn;
	int m_nRgnWidth;
	int m_nRgnHeight;
};
