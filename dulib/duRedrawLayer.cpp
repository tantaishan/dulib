//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duRedrawLayer.cpp
//
//--------------------------------------------------------------------------
#include "stdafx.h"
#include "duRedrawLayer.h"

duRedrawLayer::duRedrawLayer()
{
}

duRedrawLayer::~duRedrawLayer()
{
}

void duRedrawLayer::InitDown(duZorderList *pZorderList, duPlugin *pPlugin, const duRect &rcPluginVisible)
{
	duPlugin *pTemp = NULL;
	UINT uZorder = pPlugin->GetZorder();
	int i, j;
	int nLevelCount = pZorderList->GetLayerCount();
	for (i = nLevelCount-1;i >= 0; i--)
	{
		UINT uLayerZorder = pZorderList->GetLayerZorder(i);
		if (uZorder < uLayerZorder)
			break;
		
		int nCtrlCount = pZorderList->GetLayerChildCount(i);
		for (j = 0;j < nCtrlCount; j++)
		{
			pTemp = pZorderList->GetPlugin(i, j);
			if (!Plugin_IsValid(pTemp) || !Plugin_IsVisible(pTemp))
				continue;
			
			if (pTemp == pPlugin)
				return;

			duRect rcVisible;
			Plugin_GetVisibleRect(pTemp, &rcVisible);

			duRect rcInter;
			if (::IntersectRect(&rcInter, &rcPluginVisible, &rcVisible))
				Add(pTemp);
		}
	}
}

void duRedrawLayer::InitUp(duZorderList *pZorderList, duPlugin *pPlugin, const duRect &rcPluginVisible)
{
	duPlugin *pTemp = NULL;
	UINT uZorder = pPlugin->GetZorder();
	int i, j;
	int nLevelCount = pZorderList->GetLayerCount();
	for (i = 0;i < nLevelCount; i++)
	{
		UINT uLayerZorder = pZorderList->GetLayerZorder(i);
		if (uZorder > uLayerZorder)
			break;
		
		int nCtrlStart = 0;
		int nCtrlCount = pZorderList->GetLayerChildCount(i);
		if (uZorder == uLayerZorder)
		{
			for (j = 0;j < nCtrlCount; j++)
			{
				pTemp = pZorderList->GetPlugin(i, j);
				if (!Plugin_IsValid(pTemp) || !Plugin_IsVisible(pTemp))
					continue;

				if (pTemp == pPlugin)
				{
					nCtrlStart = ++j;
					break;
				}
			}
		}

		for (j = nCtrlStart;j < nCtrlCount; j++)
		{
			pTemp = pZorderList->GetPlugin(i, j);
			if (!Plugin_IsValid(pTemp) || !Plugin_IsVisible(pTemp))
				continue;
			
			duRect rcVisible;
			Plugin_GetVisibleRect(pTemp, &rcVisible);
        
			duRect rcInter;
			if (::IntersectRect(&rcInter, &rcPluginVisible, &rcVisible))
				Add(pTemp);
		}
	}
}
