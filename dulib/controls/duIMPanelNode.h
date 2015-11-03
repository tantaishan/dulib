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
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);

	virtual void OnMouseLDown(POINT pt);
	virtual void OnMouseLUp(POINT pt);
	virtual void OnMouseDbClick(POINT pt);

	virtual void Expand(BOOL fExpand);
	virtual BOOL IsExpand();

	virtual int GetLevel();
	//virtual void AddControl(LPCTSTR lpszName, BOOL fVisible);
	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duIMPanelNode"); }	

public:
	duIMPanelNode();
	~duIMPanelNode();

protected:
	BOOL m_fExpand;
	int m_nLevel;
};
