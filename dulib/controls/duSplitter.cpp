//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duSplitter.cpp
//
//  History:    Mar-08-2010   Denny Chen  Created
//
//--------------------------------------------------------------------------
#include "duSplitter.h"

duSplitter::duSplitter() :
	 m_nFMinPixel(0)
	,m_nSMinPixel(0)
	,m_fVert(FALSE)
	,m_fDockFirst(FALSE)
	,m_fTracking(FALSE)
	,m_ptStart(0,0)
{
	ZeroMemory(m_szFName, MAX_NAME * sizeof(TCHAR));
	ZeroMemory(m_szSName, MAX_NAME * sizeof(TCHAR));
	m_rectLast.SetRectEmpty();
}

duSplitter::~duSplitter()
{
}

void duSplitter::RegisterControlProperty()
{
	RegisterProperty(_T("firstminpixel"),    DU_PROPERTY_LONG,   &m_nFMinPixel);
	RegisterProperty(_T("secondminpixel"),   DU_PROPERTY_LONG,   &m_nSMinPixel);
	RegisterProperty(_T("firstpluginname"),  DU_PROPERTY_STRING, m_szFName);
	RegisterProperty(_T("secondpluginname"), DU_PROPERTY_STRING, m_szSName);
	RegisterProperty(_T("vert"),             DU_PROPERTY_BOOL,   &m_fVert);
	RegisterProperty(_T("dockfirst"),        DU_PROPERTY_BOOL,   &m_fDockFirst);
}

void duSplitter::OnCreate()
{
}

void duSplitter::DrawObject(HDC hDC)
{
	duRect rectSplitter;
	Plugin_GetRect(this, &rectSplitter);
	rectSplitter.OffsetRect(-rectSplitter.left, -rectSplitter.top);

	DrawByStyle(this, GetStyle(), hDC, &rectSplitter, GetState(), GetText(), GetAlpha());
}

void duSplitter::OnMouseLeave(POINT pt)
{
	Plugin_SetState(this, DU_STATE_NORMAL);
	Plugin_Redraw(this, TRUE);
}

void duSplitter::OnMouseLDown(POINT pt)
{
	duPlugin *pFPlugin = GetPluginByName(m_szFName);
	duPlugin *pSPlugin = GetPluginByName(m_szSName);
	
	if (!Plugin_IsValid(pFPlugin) || !Plugin_IsValid(pSPlugin))
		return;

	duRect rectSplitter;
	Plugin_GetRect(this, &rectSplitter);
	if (rectSplitter.PtInRect(pt))
	{
		m_ptStart = pt;
		m_rectLast.SetRectEmpty();
		m_fTracking = TRUE;
		Plugin_SetState(this, DU_STATE_PRESS);
		Plugin_SetCapture(this);
		Plugin_Redraw(this, TRUE);
	}
}

void duSplitter::OnMouseLUp(POINT pt)
{
	duPlugin *pFPlugin = GetPluginByName(m_szFName);
	duPlugin *pSPlugin = GetPluginByName(m_szSName);
	
	if (!Plugin_IsValid(pFPlugin) || !Plugin_IsValid(pSPlugin))
		return;

	if (m_fTracking)
	{
		Plugin_ReleaseCapture(this);
		m_fTracking = FALSE;

		duRect rectFirst;
		Plugin_GetRect(pFPlugin, &rectFirst);
		
		duRect rectSplitter;
		Plugin_GetRect(this, &rectSplitter);
		
		duRect rectSecond;
		Plugin_GetRect(pSPlugin, &rectSecond);

		int nDistance = 0;
		if (m_fVert)
		{
			int nFirstMinY  = rectFirst.top + m_nFMinPixel + (m_ptStart.y - rectSplitter.top);
			int nSecondMinY = rectSecond.bottom - m_nSMinPixel - (rectSplitter.bottom - m_ptStart.y);

			if (pt.y < nFirstMinY)
				nDistance = m_ptStart.y - nFirstMinY;
			else if (pt.y >= nFirstMinY && pt.y <= m_ptStart.y)
				nDistance = m_ptStart.y - pt.y;
			else if (pt.y >= m_ptStart.y && pt.y <= nSecondMinY)
				nDistance = m_ptStart.y - pt.y;
			else if (pt.y > nSecondMinY)
				nDistance =  m_ptStart.y - nSecondMinY;

			MoveVert(nDistance);
		}
		else
		{
			int nFirstMinX  = rectFirst.left   + m_nFMinPixel + (m_ptStart.x - rectSplitter.left);
			int nSecondMinX = rectSecond.right - m_nSMinPixel - (rectSplitter.right - m_ptStart.x);

			if (pt.x < nFirstMinX)
				nDistance = m_ptStart.x - nFirstMinX;
			else if (pt.x >= nFirstMinX && pt.x <= m_ptStart.x)
				nDistance = m_ptStart.x - pt.x;
			else if (pt.x >= m_ptStart.x && pt.x <= nSecondMinX)
				nDistance = m_ptStart.x - pt.x;
			else if (pt.x > nSecondMinX)
				nDistance =  m_ptStart.x - nSecondMinX;

			MoveHorz(nDistance);
		}
	}

	Plugin_SetState(this, DU_STATE_NORMAL);
	::InvalidateRect(m_hWnd, NULL, TRUE);
}

void duSplitter::OnMouseMove(POINT pt)
{
	duPlugin *pFPlugin = GetPluginByName(m_szFName);
	duPlugin *pSPlugin = GetPluginByName(m_szSName);
	
	if (!Plugin_IsValid(pFPlugin) || !Plugin_IsValid(pSPlugin))
		return;

	if (GetState() == DU_STATE_NORMAL)
	{
		Plugin_SetState(this, DU_STATE_OVER);
		Plugin_Redraw(this, TRUE);
	}

	if (m_fTracking)
	{
		duRect rectFirst;
		Plugin_GetRect(pFPlugin, &rectFirst);
		
		duRect rectSplitter;
		Plugin_GetRect(this, &rectSplitter);
		
		duRect rectSecond;
		Plugin_GetRect(pSPlugin, &rectSecond);
		duRect rectTracking = rectSplitter;

		if (m_fVert)
		{
			int yOffset = pt.y - m_ptStart.y;
			rectTracking.OffsetRect(0, yOffset);

			int nFirstMinY = rectFirst.top + m_nFMinPixel + (m_ptStart.y - rectSplitter.top);
			int nSecondMinY = rectSecond.bottom - m_nSMinPixel - (rectSplitter.bottom - m_ptStart.y);
            if (pt.y >= nFirstMinY && pt.y <= nSecondMinY)
			{
				OnInvertTracker(m_rectLast);
				OnInvertTracker(rectTracking);
				m_rectLast = rectTracking;
			}
			else
			{
				if (pt.y < nFirstMinY && m_rectLast.top != (rectFirst.top + m_nFMinPixel))
				{
					int nSplitterHeight = m_rectLast.Height();
					rectTracking.top = rectFirst.top + m_nFMinPixel;
					rectTracking.bottom = rectTracking.top + nSplitterHeight;
					OnInvertTracker(m_rectLast);
					OnInvertTracker(rectTracking);
					m_rectLast = rectTracking;
				}

				if (pt.y > nSecondMinY && m_rectLast.bottom != (rectSecond.bottom - m_nSMinPixel))
				{
					int nSplitterHeight = m_rectLast.Height();
					rectTracking.bottom = rectSecond.bottom - m_nSMinPixel;
					rectTracking.top = rectTracking.bottom - nSplitterHeight;
					OnInvertTracker(m_rectLast);
					OnInvertTracker(rectTracking);
					m_rectLast = rectTracking;
				}
			}
		}
		else
		{
			int xOffset = pt.x - m_ptStart.x;
			rectTracking.OffsetRect(xOffset, 0);
			int nFirstMinX  = rectFirst.left   + m_nFMinPixel + (m_ptStart.x - rectSplitter.left);
			int nSecondMinX = rectSecond.right - m_nSMinPixel - (rectSplitter.right - m_ptStart.x);
			if (pt.x >= nFirstMinX && pt.x <= nSecondMinX)
			{
				OnInvertTracker(m_rectLast);
				OnInvertTracker(rectTracking);
				m_rectLast = rectTracking;
			}
			else
			{
				if (pt.x < nFirstMinX && m_rectLast.left != (rectFirst.left + m_nFMinPixel))
				{
					int nSplitterWidth = m_rectLast.Width();
					rectTracking.left = rectFirst.left + m_nFMinPixel;
					rectTracking.right = rectTracking.left + nSplitterWidth;
					OnInvertTracker(m_rectLast);
					OnInvertTracker(rectTracking);
					m_rectLast = rectTracking;
				}

				if (pt.x > nSecondMinX && m_rectLast.right != (rectSecond.right - m_nSMinPixel))
				{
					int nSplitterWidth = m_rectLast.Width();
					rectTracking.right = rectSecond.right - m_nSMinPixel;
					rectTracking.left = rectTracking.right - nSplitterWidth;
					OnInvertTracker(m_rectLast);
					OnInvertTracker(rectTracking);
					m_rectLast = rectTracking;
				}
			}
		}
	
		//static int count = 0;

		//if (rectTracking.Height() != 10)
		//{
		//	__asm int 3;

		//_TRACE("[%d] pointClient  (%d, %d)\r\n", count, pointClient.x, pointClient.y);
		//_TRACE("[%d] m_ptStart (%d, %d)\r\n", count, m_ptStart.x, m_ptStart.y);

		//_TRACE("[%d] rectFirst   (%d, %d, %d, %d)\r\n", count, rectFirst.left, rectFirst.top, rectFirst.right, rectFirst.bottom);
		//_TRACE("[%d] rectSecond  (%d, %d, %d, %d)\r\n", count, rectSecond.left, rectSecond.top, rectSecond.right, rectSecond.bottom);
		//_TRACE("[%d] rectTracking(%d, %d, %d, %d,) Width : %d, Height : %d\r\n", 
		//	count, rectTracking.left, rectTracking.top, rectTracking.right, rectTracking.bottom, rectTracking.Width(), rectTracking.Height());

		//	count++;
		//}
	}
}

void duSplitter::MoveHorz(int nDistance)
{
	duPlugin *pFPlugin = GetPluginByName(m_szFName);
	duPlugin *pSPlugin = GetPluginByName(m_szSName);
	duPlugin *pParent = GetParent();
	
	if (!Plugin_IsValid(pFPlugin) || !Plugin_IsValid(pSPlugin) || !Plugin_IsValid(pParent))
		return;

	duRect rectFirst, rectSplitter, rectSecond, rectParent;
	Plugin_GetRect(pFPlugin, &rectFirst);
	Plugin_GetRect(pSPlugin, &rectSecond);
	Plugin_GetRect(this, &rectSplitter);
	Plugin_GetRect(pParent, &rectParent);
	
	// Move first
	int nFirstWidth = rectFirst.Width() - nDistance;
	int nFirstRight = rectParent.right - (rectFirst.left + nFirstWidth);
 	int nFirstRatio = 0;
	if (rectParent.Width() > 0)
		nFirstRatio = (int)nFirstWidth / (float)rectParent.Width();
	
	int first_HorzType = pFPlugin->GetPosHorzAnchor();
	if (first_HorzType == 0)
		pFPlugin->SetPosWidth(nFirstWidth);
	else if (first_HorzType == 1)
		pFPlugin->SetPosWidth(nFirstRatio);
	else if (first_HorzType == 2)
		pFPlugin->SetPosRight(nFirstRight);
	
	Plugin_Resize(pFPlugin, NULL);

	// Move splitter
	Plugin_GetRect(pFPlugin, &rectFirst);
	int splitter_HorzType = GetPosHorzAnchor();
	if (splitter_HorzType != 0)
	{
		SetPosHorzAnchor(0);
		SetPosWidth(rectSplitter.Width());
	}

	int nSplitterLeft = rectFirst.right - rectParent.left;
	SetPosLeft(nSplitterLeft);
	Plugin_Resize(this, NULL);

	// Move second
	Plugin_GetRect(this, &rectSplitter);
	int nSecondLeft_positive = max(0, rectSplitter.right - rectParent.left);
	int nSecondLeft_negative = min(0, nSecondLeft_positive - rectParent.Width());

	int nSecondWidth = rectSecond.right - rectSplitter.right;
	int nSecondRight = rectParent.right - (rectSplitter.right + nSecondWidth);
	int nSecondRatio = 0;
	if (rectSecond.Width())
		nSecondRatio = (int)nSecondWidth / (float)rectSecond.Width();

	if (pSPlugin->GetPosLeft() < 0)
		pSPlugin->SetPosLeft(nSecondLeft_negative);
	else
		pSPlugin->SetPosLeft(nSecondLeft_positive);

	int second_HorzType = pSPlugin->GetPosHorzAnchor();
	if (second_HorzType == 0)
		pSPlugin->SetPosWidth(nSecondWidth);
	else if (second_HorzType == 1)
		pSPlugin->SetPosWidth(nSecondRatio);
	else if (second_HorzType == 2)
		pSPlugin->SetPosRight(nSecondRight);
	
	Plugin_Resize(pSPlugin, NULL);
	NotifyUser(DUM_SPLITTERMOVED, NULL, NULL);
}

void duSplitter::MoveVert(int nDistance)
{
	duPlugin *pFPlugin = GetPluginByName(m_szFName);
	duPlugin *pSPlugin = GetPluginByName(m_szSName);
	duPlugin *pParent = GetParent();
	
	if (!Plugin_IsValid(pFPlugin) || !Plugin_IsValid(pSPlugin) || !Plugin_IsValid(pParent))
		return;

	duRect rectFirst, rectSplitter, rectSecond, rectParent;
	Plugin_GetRect(pFPlugin, &rectFirst);
	Plugin_GetRect(pSPlugin, &rectSecond);
	Plugin_GetRect(this, &rectSplitter);
	Plugin_GetRect(pParent, &rectParent);

	// Move first
	int nFirstHeight = rectFirst.Height() - nDistance;
	int nFirstBottom = rectParent.bottom - (rectFirst.top + nFirstHeight);
	int nFirstRatio = 0;
	if (rectParent.Height() > 0)
		nFirstRatio = (int) nFirstHeight / (float)rectParent.Height();

	int first_VertType = pFPlugin->GetPosVertAnchor();
	if (first_VertType == 0)
		pFPlugin->SetPosHeight(nFirstHeight);
	else if (first_VertType == 1)
		pFPlugin->SetPosHeight(nFirstRatio);
	else if (first_VertType == 2)
		pFPlugin->SetPosBottom(nFirstBottom);

	Plugin_Resize(pFPlugin, NULL);

	// Move splitter
	Plugin_GetRect(pFPlugin, &rectFirst);
	int splitter_VertType = GetPosVertAnchor();
	if (splitter_VertType != 0)
	{
		SetPosVertAnchor(0);
		SetPosHeight(rectSplitter.Height());
	}

	int nSplitterTop = rectFirst.bottom - rectParent.top;
	SetPosTop(nSplitterTop);
	Plugin_Resize(this, NULL);

	// Move second
	Plugin_GetRect(this, &rectSplitter);

	int nSecondTop_positive = max(0, rectSplitter.bottom - rectParent.top);
	int nSecondTop_negative = min(0, nSecondTop_positive - rectParent.Height());
	int nSecondHeight = rectSecond.bottom - rectSplitter.bottom;
	int nSecondBottom = rectParent.bottom - (rectSplitter.bottom + nSecondHeight);
	int nSecondRatio = 0;
	if (rectSecond.Height())
		nSecondRatio = (int)nSecondHeight / (float)rectSecond.Height();

	if (pSPlugin->GetPosTop() < 0)
		pSPlugin->SetPosTop( nSecondTop_negative );
	else
		pSPlugin->SetPosTop( nSecondTop_positive );

	int second_VertType = pSPlugin->GetPosVertAnchor();
	if (second_VertType == 0)
		pSPlugin->SetPosHeight(nSecondHeight);
	else if (second_VertType == 1)
		pSPlugin->SetPosHeight(nSecondRatio);
	else if (second_VertType == 2)
		pSPlugin->SetPosBottom(nSecondBottom);

	Plugin_Resize(pSPlugin, NULL);
	NotifyUser(DUM_SPLITTERMOVED, NULL, NULL);
}

void duSplitter::OnInvertTracker(duRect &rect)
{
	if (rect.IsRectEmpty())
		return;

	// pat-blt without clip children on
	HDC hdc = GetDC(NULL);
	duRect rectWindow;
	rectWindow.SetRectEmpty();
	GetWindowRect(m_hWnd, &rectWindow);
	
	// invert the brush pattern (looks just like frame window sizing)
	HBRUSH hBrush =   GetHalftoneBrush();
	HBRUSH hOldBrush = NULL;
	if (hBrush != NULL)
		hOldBrush = (HBRUSH)SelectObject(hdc,hBrush);
	PatBlt(hdc,rect.left+rectWindow.left, rect.top+rectWindow.top, rect.Width(), rect.Height(), PATINVERT);
	if (hOldBrush != NULL)
		SelectObject(hdc, hOldBrush);
	DeleteObject(hBrush);
	ReleaseDC(NULL,hdc);
}

HBRUSH duSplitter::GetHalftoneBrush()
{
	HBRUSH hBrush = NULL;

	WORD grayPattern[8];
	int i = 0;
	for (i = 0; i < 8; i++)
		grayPattern[i] = (WORD)(0x5555 << (i & 1));
	HBITMAP grayBitmap = CreateBitmap(8, 8, 1, 1, &grayPattern);
	if (grayBitmap != NULL)
	{
		hBrush = ::CreatePatternBrush(grayBitmap);
		DeleteObject(grayBitmap);
	}

	return hBrush;
}
