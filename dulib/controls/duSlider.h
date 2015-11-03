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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);

	virtual void OnMouseLeave(POINT pt);
	virtual void OnMouseLDown(POINT pt);
	virtual void OnMouseLUp(POINT pt);
	virtual void OnMouseMove(POINT pt);

	virtual void Resize(LPRECT lpRect);

	virtual int  GetPos();
	virtual void SetPos(int nPos);

	virtual int  GetMaxPos();
	virtual void SetMaxPos(int nMaxPos);

	virtual int  GetThumbWidth();
	virtual void SetThumbWidth(int nWidth);

	virtual int  GetThumbHeight();
	virtual void SetThumbHeight(int nHeight);

	virtual BOOL  IsVert();
	virtual void  SetVert(BOOL bVert);

	virtual LPCTSTR GetForeStyle();
	virtual void SetForeStyle(LPCTSTR lpszStyle);

	virtual LPCTSTR GetThumbStyle();
	virtual void SetThumbStyle(LPCTSTR lpszStyle);

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duSlider"); }	
	
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
