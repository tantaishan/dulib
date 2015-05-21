//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duSlider.h
//
//  History:    Dec-29-2009   Steven Li  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

// duSlider
//
//
class duSlider : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);

	virtual void WINAPI OnMouseLeave(POINT pt);
	virtual void WINAPI OnMouseLDown(POINT pt);
	virtual void WINAPI OnMouseLUp(POINT pt);
	virtual void WINAPI OnMouseMove(POINT pt);

	virtual void WINAPI Resize(LPRECT lpRect);

	virtual int  WINAPI GetPos();
	virtual void WINAPI SetPos(int nPos);

	virtual int  WINAPI GetMaxPos();
	virtual void WINAPI SetMaxPos(int nMaxPos);

	virtual int  WINAPI GetThumbWidth();
	virtual void WINAPI SetThumbWidth(int nWidth);

	virtual int  WINAPI GetThumbHeight();
	virtual void WINAPI SetThumbHeight(int nHeight);

	virtual BOOL  WINAPI IsVert();
	virtual void  WINAPI SetVert(BOOL bVert);

	virtual LPCTSTR WINAPI GetForeStyle();
	virtual void WINAPI SetForeStyle(LPCTSTR lpszStyle);

	virtual LPCTSTR WINAPI GetThumbStyle();
	virtual void WINAPI SetThumbStyle(LPCTSTR lpszStyle);

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duSlider"); }	
	
public:
	duSlider();
	~duSlider();

protected:
	void calcRect(duRect &rect);
	void calcThumbRect(duRect &rect);
	void calcRectByPt(POINT &pt, duRect &rect);

protected:
	TCHAR m_szForeStyle[MAX_NAME];
	TCHAR m_szThumbStyle[MAX_NAME];
	int m_nThumbWidth;
	int m_nThumbHeight;
	int m_nMaxPos;
	int m_nPos;
	BOOL m_fVert;

private:
	duRect m_rcFore;
	duRect m_rcThumb;
	UINT m_uThumbState;
	float m_fMaxPos;
	float m_fPos;
	BOOL m_fTracking;
};
