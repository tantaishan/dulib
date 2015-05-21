//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duWindow.h
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"

// duWindow
//
// 
class duWindow : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);
	
	virtual void WINAPI OnMouseLDown(POINT pt);

	virtual int WINAPI GetCaptionHeight() { return m_nCaptionHeight; }
	virtual void WINAPI SetCaptionHeight(int nCaptionHeight) { m_nCaptionHeight = nCaptionHeight; }

	virtual UINT WINAPI GetMinWidth() { return m_uMinWidth; }
	virtual void WINAPI SetMinWidth(UINT uMinWidth) { m_uMinWidth = uMinWidth; }

	virtual UINT WINAPI GetMinHeight() { return m_uMinHeight; }
	virtual void WINAPI SetMinHeight(UINT uMinHeight) { m_uMinHeight = uMinHeight; }
	
	virtual UINT WINAPI GetMaxWidth() { return m_uMaxWidth; }
	virtual void WINAPI SetMaxWidth(UINT uMaxWidth) { m_uMaxWidth = uMaxWidth; }
	
	virtual UINT WINAPI GetMaxHeight() { return m_uMaxHeight; }
	virtual void WINAPI SetMaxHeight(UINT uMaxHeight) { m_uMaxHeight = uMaxHeight; }

	virtual int WINAPI GetStartX() { return m_nStartX; }
	virtual void WINAPI SetStartX(int nStartX) { m_nStartX = nStartX; }

	virtual int WINAPI GetStartY() { return m_nStartY; }
	virtual void WINAPI SetStartY(int nStartY) { m_nStartY = nStartY; }

	virtual UINT WINAPI GetBorderSize() { return m_uBorderSize; }
	virtual void WINAPI SetBorderSize(UINT uBorderSize) { m_uBorderSize = uBorderSize; }

	virtual LPCTSTR WINAPI GetRgnName() { return m_strRgnName; }
	virtual void WINAPI SetRgnName(LPCTSTR lpszRgnName);

	virtual void WINAPI SetCaptionLeft(int nCaptionLeft) { m_nCaptionLeft = nCaptionLeft; }
	virtual void WINAPI SetCaptionRight(int nCaptionRight) { m_nCaptionRight = nCaptionRight; }
	virtual void WINAPI SetCaptionTop(int nCaptionTop) { m_nCaptionTop = nCaptionTop; }
	
	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duWindow"); }	

public:
	void InitWindowPos();
	UINT OnHitTest(POINT pt);
	void OnMinMaxInfo(MINMAXINFO *pMinMaxInfo);
	void RefreshWindowRgn();

public:
	duWindow();
	~duWindow();

protected:
	int   m_nCaptionHeight;
	int	  m_nCaptionLeft;
	int	  m_nCaptionRight;
	int	  m_nCaptionTop;

	UINT  m_uMinWidth;
	UINT  m_uMinHeight;
	UINT  m_uMaxWidth;
	UINT  m_uMaxHeight;
	int   m_nStartX;
	int   m_nStartY;
	UINT  m_uBorderSize;
	TCHAR m_strRgnName[MAX_NAME];
	BOOL m_fLayered;
	
private:
	UINT m_uHitCode;
};
