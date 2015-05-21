//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duButton.h
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

// duButton
// 
// 
class duButton : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);
	virtual void WINAPI OnMouseIn(POINT pt);
	virtual void WINAPI OnMouseLeave(POINT pt);
	virtual void WINAPI OnMouseLDown(POINT pt);
	virtual void WINAPI OnMouseLUp(POINT pt);
	virtual void WINAPI OnMouseDbClick(POINT pt);
	virtual void WINAPI OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void WINAPI OnRedraw();
	virtual void WINAPI OnTimer(UINT nEventId);
	virtual BOOL WINAPI OnKillFocus(duPlugin *pNewFocus) { return TRUE; }
	virtual BOOL WINAPI OnSetFocus(duPlugin *pOldFocus) { return TRUE; }
	
	virtual void WINAPI SetText(LPCTSTR lpszText);
	
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

	virtual int WINAPI GetStatusIconX() { return m_nStatusIconX; }
	virtual void WINAPI SetStatusIconX(int nStatusIconX) { m_nStatusIconX = nStatusIconX; }

	virtual int WINAPI GetStatusIconY() { return m_nStatusIconY; }
	virtual void WINAPI SetStatusIconY(int nStatusIconY) { m_nStatusIconY = nStatusIconY; }

	virtual int WINAPI GetStatusIconWidth() { return m_nStatusIconWidth; }
	virtual void WINAPI SetStatusIconWidth(int nStatusIconWidth) { m_nStatusIconWidth = nStatusIconWidth; }

	virtual int WINAPI GetStatusIconHeight() { return m_nStatusIconHeight; }
	virtual void WINAPI SeStatustIconHeight(int nStatusIconHeight) { m_nStatusIconHeight = nStatusIconHeight; }

	virtual LPCTSTR WINAPI GetStatusIcon() { return m_szStatusIcon; }
	virtual void WINAPI SetStatusIcon(LPCTSTR lpszStatusIconName);

	virtual BOOL WINAPI IsGrayDisable();
	virtual void WINAPI SetGrayDisable(BOOL fGrayDisable);

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duButton"); }	

public:
	duButton();
	~duButton();
	
protected:
	void ResizeByText(LPCTSTR lpszText);
	void GetIconRect(duRect &rcIcon);
	void GetStatusIconRect(duRect &rcStatusIcon);
	void FadeRedraw(UINT uFadeInState, UINT uFadeOutState);
	HBITMAP GetCurrentBitmap();

protected:
	TCHAR m_szIcon[MAX_NAME];
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
	TCHAR m_szStatusIcon[MAX_NAME];
	int m_nStatusIconX;
	int m_nStatusIconY;
	int m_nStatusIconWidth;
	int m_nStatusIconHeight;
	BOOL m_fGrayDisable;
	
protected:
	BOOL m_fFadeDrawing;
	UINT m_nFadeTimer;
	UINT m_uFadeInState;
	UINT m_uFadeOutState;
	int m_nFadeInAlpha;
	int m_nFadeOutAlpha;

};