//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duTabButton.h
//
//  History:    Apr-03-2010   Denny Chen  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

// duTabButton
// 
// 
class duTabButton : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);

	virtual void WINAPI OnMouseIn(POINT ppt);
	virtual void WINAPI OnMouseLeave(POINT ppt);
	virtual void WINAPI OnMouseLDown(POINT ppt);
	virtual void WINAPI OnMouseLUp(POINT ppt);
	virtual void WINAPI OnRedraw();
	virtual void WINAPI OnTimer(UINT nEventId);
	virtual void WINAPI OnKeyDown(UINT key, UINT nRepCnt, UINT nFlags);

	virtual BOOL WINAPI IsSelected();
	virtual void WINAPI SetSelected(BOOL fSel);

	virtual BOOL WINAPI IsAutoSize() { return m_fAutoSize; }
	virtual void WINAPI SetAutoSize(BOOL fAutoSize) { m_fAutoSize = fAutoSize; }

	virtual int WINAPI GetFixLeft() { return m_nFixLeft; } 
	virtual void WINAPI SetFixLeft(int nFixLeft) { m_nFixLeft = nFixLeft; }

	virtual int WINAPI GetFixRight() { return m_nFixRight; }
	virtual void WINAPI SetFixRight(int nFixRight) { m_nFixRight = nFixRight; }

	virtual int WINAPI GetIconX() { return m_nIconX; }
	virtual void WINAPI SetIconX(int nIconX) { m_nIconX = nIconX; }

	virtual int WINAPI GetIconY() { return m_nIconY; }
	virtual void WINAPI SetIconY(int nIconY) { m_nIconY = nIconY; }

	virtual int WINAPI GetIconWidth() { return m_nIconWidth; }
	virtual void WINAPI SetIconWidth(int nIconWidth) { m_nIconWidth = nIconWidth; }

	virtual int WINAPI GetIconHeight() { return m_nIconHeight; }
	virtual void WINAPI SetIconHeight(int nIconHeight) { m_nIconHeight = nIconHeight; }

	virtual LPCTSTR WINAPI GetIcon() { return m_szIcon; }
	virtual void WINAPI SetIcon(LPCTSTR lpszIconName);

	virtual BOOL WINAPI IsFade() { return m_fFade; }
	virtual void WINAPI SetFade(BOOL fFade) { m_fFade = fFade; }

	virtual int WINAPI GetFadeInSpeed() { return m_nFadeInSpeed; }
	virtual void WINAPI SetFadeInSpeed(int nFadeInSpeed) { m_nFadeInSpeed = nFadeInSpeed; }

	virtual int WINAPI GetFadeOutSpeed() { return m_nFadeOutSpeed; }
	virtual void WINAPI SetFadeOutSpeed(int nFadeOutSpeed) { m_nFadeOutSpeed = nFadeOutSpeed; }

	virtual LPCTSTR WINAPI GetTooltipText(){return (LPCTSTR)m_szToolTip;}
	virtual void WINAPI SetTooltipText(LPCTSTR lpszText);
	
	virtual LPCTSTR WINAPI GetTabPage() { return m_szTabPage; }
	virtual void WINAPI SetTabPage(LPCTSTR lpszTabPage);

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duTabButton"); }	

public:
	duTabButton();
	~duTabButton();

	void _SetSelected(BOOL bSelected);	

protected:
	void ResizeByText(LPCTSTR lpszText);
	void GetIconRect(duRect &rcIcon);
	void FadeRedraw(UINT uFadeInState, UINT uFadeOutState);

protected:
	TCHAR m_szIcon[MAX_NAME];
	TCHAR m_szTabPage[MAX_NAME];
	BOOL m_fAutoSize;
	int m_nFixLeft;
	int m_nFixRight;
	int m_nIconX;
	int m_nIconY;
	int m_nIconWidth;
	int m_nIconHeight;
	BOOL m_fFade;
	int m_nFadeInSpeed;
	int m_nFadeOutSpeed;
	TCHAR m_szToolTip[MAX_NAME];

protected:
	BOOL m_fSelected;

protected:
	BOOL m_fFadeDrawing;
	UINT m_nFadeTimer;
	UINT m_uFadeInState;
	UINT m_uFadeOutState;
	int m_nFadeInAlpha;
	int m_nFadeOutAlpha;
};
