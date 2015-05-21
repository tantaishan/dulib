//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duIMPanelNode.h
//
//  History:    Dec-21-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

// duIMPanelNode
//
//
class duIMPanelNode : public duPlugin
{
public:
	virtual void WINAPI RegisterControlProperty();
	virtual void WINAPI OnCreate();
	virtual void WINAPI DrawObject(HDC hDC);

	virtual void WINAPI OnMouseLDown(POINT pt);
	virtual void WINAPI OnMouseLUp(POINT pt);
	virtual void WINAPI OnMouseDbClick(POINT pt);

	virtual void WINAPI Expand(BOOL fExpand);
	virtual BOOL WINAPI IsExpand();

	virtual int WINAPI GetLevel();
	//virtual void WINAPI AddControl(LPCTSTR lpszName, BOOL fVisible);
	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duIMPanelNode"); }	

public:
	duIMPanelNode();
	~duIMPanelNode();

protected:
	BOOL m_fExpand;
	int m_nLevel;
};
