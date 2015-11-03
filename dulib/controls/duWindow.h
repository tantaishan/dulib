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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);
	
	virtual void OnMouseLDown(POINT pt);

	virtual int GetCaptionHeight() { return m_nCaptionHeight; }
	virtual void SetCaptionHeight(int nCaptionHeight) { m_nCaptionHeight = nCaptionHeight; }

	virtual UINT GetMinWidth() { return m_uMinWidth; }
	virtual void SetMinWidth(UINT uMinWidth) { m_uMinWidth = uMinWidth; }

	virtual UINT GetMinHeight() { return m_uMinHeight; }
	virtual void SetMinHeight(UINT uMinHeight) { m_uMinHeight = uMinHeight; }
	
	virtual UINT GetMaxWidth() { return m_uMaxWidth; }
	virtual void SetMaxWidth(UINT uMaxWidth) { m_uMaxWidth = uMaxWidth; }
	
	virtual UINT GetMaxHeight() { return m_uMaxHeight; }
	virtual void SetMaxHeight(UINT uMaxHeight) { m_uMaxHeight = uMaxHeight; }

	virtual int GetStartX() { return m_nStartX; }
	virtual void SetStartX(int nStartX) { m_nStartX = nStartX; }

	virtual int GetStartY() { return m_nStartY; }
	virtual void SetStartY(int nStartY) { m_nStartY = nStartY; }

	virtual UINT GetBorderSize() { return m_uBorderSize; }
	virtual void SetBorderSize(UINT uBorderSize) { m_uBorderSize = uBorderSize; }

	virtual LPCTSTR GetRgnName() { return m_strRgnName; }
	virtual void SetRgnName(LPCTSTR lpszRgnName);

	virtual void SetCaptionLeft(int nCaptionLeft) { m_nCaptionLeft = nCaptionLeft; }
	virtual void SetCaptionRight(int nCaptionRight) { m_nCaptionRight = nCaptionRight; }
	virtual void SetCaptionTop(int nCaptionTop) { m_nCaptionTop = nCaptionTop; }
	
	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duWindow"); }	

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
