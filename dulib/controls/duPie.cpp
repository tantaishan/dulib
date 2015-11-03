//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duPie.cpp
//
//  History:    JAN-09-2010   Denny Chen  Created
//
//--------------------------------------------------------------------------
#include "duPie.h"

duPie::duPie() :
	 m_fPercent(0.0)
	,m_clrAll(RGB(255,255,255))
	,m_clrPercent(RGB(255,0,0))
{

}

duPie::~duPie()
{

}

void duPie::RegisterControlProperty()
{
	RegisterProperty(_T("percent"), DU_PROPERTY_FLOAT, &m_fPercent);
	RegisterProperty(_T("colorall"), DU_PROPERTY_COLOR, &m_clrAll);
	RegisterProperty(_T("colorpercent"), DU_PROPERTY_COLOR, &m_clrPercent);
}

void duPie::OnCreate()
{
	
}

void duPie::DrawObject(HDC hDC)
{
	duRect rcPie;
	GetRect(&rcPie);
	rcPie.OffsetRect(-rcPie.left, -rcPie.top);
	
	Graphics *gs = new Graphics(hDC);
	Color clrAll = Color(255, GetRValue(m_clrAll), GetGValue(m_clrAll), GetBValue(m_clrAll));
	Color clrPercent = Color(255, GetRValue(m_clrPercent), GetGValue(m_clrPercent), GetBValue(m_clrPercent));
	
	SolidBrush allBrush(clrAll);
	SolidBrush percentBrush(clrPercent);
	Rect rect(3, 3, rcPie.Width() - 6, rcPie.Height() - 6); //将饼图区域缩小3个像素，就能画圆了
	
	gs->SetSmoothingMode(SmoothingModeHighQuality);
	gs->FillPie(&allBrush, rect, m_fPercent * 360.0f / 100.0f +270.0f, 360.0f);

	gs->SetSmoothingMode(SmoothingModeHighQuality);
	gs->FillPie(&percentBrush, rect, 270.0f, m_fPercent * 360.0f / 100.0f);
	
	delete gs;
}
