//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duRedrawLayer.h
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duZorderList.h"

// duRedrawLayer
//
//
class duRedrawLayer : public duZorderList
{
public:
	duRedrawLayer();
	virtual ~duRedrawLayer();
	
public:
	void InitDown(duZorderList *pZorderList, duPlugin *pPlugin, const duRect &rcPluginVisible);
	void InitUp(duZorderList *pZorderList, duPlugin *pPlugin, const duRect &rcPluginVisible);
};
