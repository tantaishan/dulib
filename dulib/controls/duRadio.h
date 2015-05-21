//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duRadio.h
//
//  History:    Nov-17-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

// duRadio
// 
// 
class duRadio : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);
	
	virtual void WINAPI OnMouseIn(POINT pt);
	virtual void WINAPI OnMouseLeave(POINT pt);
	virtual void WINAPI OnMouseLDown(POINT pt);
	virtual void WINAPI OnMouseLUp(POINT pt);

	virtual BOOL WINAPI IsChecked();
	virtual void WINAPI SetChecked(BOOL fChecked);
	
	virtual int WINAPI GetGroupId() { return m_nGroupId; }

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

	virtual LPCTSTR WINAPI GetTooltipText(){return (LPCTSTR)m_szToolTip;}
	virtual void WINAPI SetTooltipText(LPCTSTR lpszText);
	

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duRadio"); }	

public:
	duRadio();
	~duRadio();
	
protected:
	void GetIconRect(duRect &rcIcon);

protected:
	int m_nGroupId;
	int m_nIconX;
	int m_nIconY;
	int m_nIconWidth;
	int m_nIconHeight;
	TCHAR m_szIcon[MAX_NAME];
	TCHAR m_szToolTip[MAX_NAME];
	BOOL m_fChecked;
};
