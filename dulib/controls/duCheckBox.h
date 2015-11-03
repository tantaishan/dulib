//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duCheckBox.h
//
//  History:    Nov-13-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

#define DUMI_CKCLICK   1

class duCheckBox : public duPlugin
{
public:
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);
	virtual void OnMouseIn(POINT pt);
	virtual void OnMouseLeave(POINT pt);
	virtual void OnMouseLDown(POINT pt);
	virtual void OnMouseLUp(POINT pt);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnRedraw();
	virtual void OnTimer(UINT nEventId);
	virtual void SetText(LPCTSTR lpszText);
	
	virtual BOOL IsChecked();
	virtual void SetChecked(BOOL fChecked);

	virtual BOOL IsAutoSize() { return m_fAutoSize; }
	virtual void SetAutoSize(BOOL fAutoSize) { m_fAutoSize = fAutoSize; }


	virtual int GetFixLeft() { return m_nFixLeft; } 
	virtual void SetFixLeft(int nFixLeft) { m_nFixLeft = nFixLeft; }

	virtual int GetFixRight() { return m_nFixRight; }
	virtual void SetFixRight(int nFixRight) { m_nFixRight = nFixRight; }

	virtual int GetIconX() { return m_nIconX; }
	virtual void SetIconX(int nIconX) { m_nIconX = nIconX; }

	virtual int GetIconY() { return m_nIconY; }
	virtual void SetIconY(int nIconY) { m_nIconY = nIconY; }

	virtual int GetIconWidth() { return m_nIconWidth; }
	virtual void SetIconWidth(int nIconWidth) { m_nIconWidth = nIconWidth; }

	virtual int GetIconHeight() { return m_nIconHeight; }
	virtual void SetIconHeight(int nIconHeight) { m_nIconHeight = nIconHeight; }

	virtual LPCTSTR GetIcon() { return m_szIcon; }
	virtual void SetIcon(LPCTSTR lpszIconName);

	virtual BOOL IsFade() { return m_fFade; }
	virtual void SetFade(BOOL fFade) { m_fFade = fFade; }

	virtual int GetFadeInSpeed() { return m_nFadeInSpeed; }
	virtual void SetFadeInSpeed(int nFadeInSpeed) { m_nFadeInSpeed = nFadeInSpeed; }

	virtual int GetFadeOutSpeed() { return m_nFadeOutSpeed; }
	virtual void SetFadeOutSpeed(int nFadeOutSpeed) { m_nFadeOutSpeed = nFadeOutSpeed; }

	virtual LPCTSTR GetTooltipText(){return (LPCTSTR)m_szToolTip;}
	virtual void SetTooltipText(LPCTSTR lpszText);
	
	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duCheckBox"); }

protected:
	void GetIconRect(duRect &rcIcon);
	void ResizeByText(LPCTSTR lpszText);
	void FadeRedraw(UINT uFadeInState, UINT uFadeOutState);
	

public:
	duCheckBox(void);
	~duCheckBox(void);

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
	BOOL m_fChecked;

protected:
	BOOL m_fFadeDrawing;
	UINT m_nFadeTimer;
	UINT m_uFadeInState;
	UINT m_uFadeOutState;
	int m_nFadeInAlpha;
	int m_nFadeOutAlpha;
};
