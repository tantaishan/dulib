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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);
	
	virtual void OnMouseIn(POINT pt);
	virtual void OnMouseLeave(POINT pt);
	virtual void OnMouseLDown(POINT pt);
	virtual void OnMouseLUp(POINT pt);

	virtual BOOL IsChecked();
	virtual void SetChecked(BOOL fChecked);
	
	virtual int GetGroupId() { return m_nGroupId; }

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

	virtual LPCTSTR GetTooltipText(){return (LPCTSTR)m_szToolTip;}
	virtual void SetTooltipText(LPCTSTR lpszText);
	

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duRadio"); }	

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
