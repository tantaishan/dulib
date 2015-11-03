//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duWindow.cpp
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#include "duWindow.h"

duWindow::duWindow() :
	 m_nCaptionHeight(0)
	,m_nCaptionLeft(0)
	,m_nCaptionRight(0)
	,m_nCaptionTop(0)
	,m_uMinWidth(0)
	,m_uMinHeight(0)
	,m_uMaxWidth(0)
	,m_uMaxHeight(0)
	,m_nStartX(0)
	,m_nStartY(0)
	,m_uBorderSize(0)
	,m_uHitCode(HTCLIENT)
	,m_fLayered(FALSE)
{
	ZeroMemory(m_strRgnName, MAX_NAME * sizeof(TCHAR));
}

duWindow::~duWindow()
{
	//_TRACE(_T("name [%s] ~duWindow\n"), GetName());
}

void duWindow::RegisterControlProperty()
{
	RegisterProperty(_T("captionheight"), DU_PROPERTY_LONG, &m_nCaptionHeight);
	RegisterProperty(_T("minwidth"), DU_PROPERTY_LONG, &m_uMinWidth);
	RegisterProperty(_T("minheight"), DU_PROPERTY_LONG, &m_uMinHeight);
	RegisterProperty(_T("maxwidth"), DU_PROPERTY_LONG, &m_uMaxWidth);
	RegisterProperty(_T("maxheight"), DU_PROPERTY_LONG, &m_uMaxHeight);
	RegisterProperty(_T("x"), DU_PROPERTY_LONG, &m_nStartX);
	RegisterProperty(_T("y"), DU_PROPERTY_LONG, &m_nStartY);
	RegisterProperty(_T("bordersize"), DU_PROPERTY_LONG, &m_uBorderSize);
	RegisterProperty(_T("rgn"), DU_PROPERTY_RGN, &m_strRgnName);
	RegisterProperty(_T("layered"), DU_PROPERTY_BOOL, &m_fLayered);
}

void  duWindow::OnCreate()
{
	SetCached(TRUE);

	if (m_fLayered)
	{
		DWORD dwStyleEx = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
		dwStyleEx |= WS_EX_LAYERED;
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwStyleEx);
	}
}

void duWindow::DrawObject(HDC hDC)
{
	duStyleGroup *pStyleGroup = (duStyleGroup *)GetResObj(GetStyle(), DU_RES_STYLEGROUP);
	if (pStyleGroup == NULL)
		return;

	duRect rcCtrl;
	rcCtrl.SetRectEmpty();
	GetRect(&rcCtrl);
	rcCtrl.OffsetRect(-rcCtrl.left, -rcCtrl.top);
	
	pStyleGroup->Draw(hDC, &rcCtrl, GetState(), GetText(), GetAlpha());
}

void duWindow::OnMouseLDown(POINT pt)
{
	if (m_nCaptionHeight == 0)
		return;
	
	duRect rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	rcClient.InflateRect(m_uBorderSize, m_uBorderSize, m_uBorderSize, m_uBorderSize);
	if (!rcClient.PtInRect(pt))
		return;

	rcClient.left = rcClient.left + m_nCaptionLeft;
	rcClient.right = rcClient.right - m_nCaptionRight;

	rcClient.top = m_nCaptionTop;
	rcClient.bottom = m_nCaptionHeight;
	if (rcClient.PtInRect(pt) && !IsZoomed(m_hWnd))
	{
		POINT ptScreen(pt);
		::ClientToScreen(m_hWnd, &ptScreen);
		if (::GetCapture() == m_hWnd)
			::ReleaseCapture();

		::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_MOVE+9, MAKELPARAM(ptScreen.x, ptScreen.y));
	}

	//if (IsZoomed(m_hWnd))
	//{
	//	::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
	//}
}

void duWindow::SetRgnName(LPCTSTR lpszRgnName)
{
	if (lpszRgnName)
		_tcsncpy(m_strRgnName, lpszRgnName, MAX_NAME);
	else
		ZeroMemory(m_strRgnName, MAX_NAME * sizeof(TCHAR));
}

void duWindow::InitWindowPos()
{	
	if (m_nWidth <= 0 || m_nHeight <= 0)
		return;
	
	duRect rcWork;
	rcWork.SetRectEmpty();
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, NULL);
	
	POINT ptWin = {0, 0};
	if (m_nStartX == -1)
		ptWin.x = (rcWork.Width() - m_nWidth) / 2;
	else
		ptWin.x = m_nStartX;
		
	if (m_nStartY == -1)
		ptWin.y = (rcWork.Height() - m_nHeight) / 2;
	else
		ptWin.y = m_nStartY;
	
	::SetWindowPos(m_hWnd, NULL, ptWin.x, ptWin.y, m_nWidth, m_nHeight, SWP_NOOWNERZORDER|SWP_NOZORDER|SWP_NOACTIVATE);
}

UINT duWindow::OnHitTest(POINT pt)
{
	//if (::IsZoomed(m_hWnd))
	//{
	//	return HTCAPTION;
	//}

	//if (::IsZoomed(m_hWnd))
	//{
	//	if(m_uHitCode == HTCLIENT && ::GetAsyncKeyState(MK_LBUTTON) & 0x1)
	//	{
	//		return  HTCAPTION;
	//	}
	//	else
	//	{
	//		return  HTCLIENT;
	//	}
	//}


	POINT pointClient(pt);
	ScreenToClient(m_hWnd, &pointClient);

	duRect rectWin, rectClient, rectLeft, rectTop, rectRight, rectBottom;
	::GetClientRect(m_hWnd, &rectWin);
	rectClient = rectWin;
	rectClient.InflateRect(m_uBorderSize, m_uBorderSize, m_uBorderSize, m_uBorderSize); 
	
	if (m_uBorderSize > 0)
	{
		rectTop = rectWin;
		rectTop.bottom = m_uBorderSize;
		rectLeft = rectWin;
		rectLeft.right = m_uBorderSize;
		rectRight = rectWin;
		rectRight.left = rectRight.right - m_uBorderSize;
		rectBottom = rectWin; 
		rectBottom.top = rectBottom.bottom - m_uBorderSize;

		if (rectTop.PtInRect(pointClient) && rectLeft.PtInRect(pointClient) )
		{
			return m_uHitCode = HTTOPLEFT;
		}
		else if (rectTop.PtInRect(pointClient) && rectRight.PtInRect(pointClient) )
		{
			return m_uHitCode = HTTOPRIGHT;
		}
		else if (rectBottom.PtInRect(pointClient) && rectLeft.PtInRect(pointClient) )
		{
			return m_uHitCode = HTBOTTOMLEFT;
		}
		else if (rectBottom.PtInRect(pointClient) && rectRight.PtInRect(pointClient) )
		{
			return m_uHitCode = HTBOTTOMRIGHT;
		}
		else if (rectTop.PtInRect(pointClient))
		{
			return m_uHitCode = HTTOP;
		}
		else if (rectLeft.PtInRect(pointClient))
		{
			return m_uHitCode = HTLEFT;
		}
		else if (rectRight.PtInRect(pointClient))
		{
			return m_uHitCode = HTRIGHT;
		}
		else if (rectBottom.PtInRect(pointClient))
		{
			return m_uHitCode = HTBOTTOM;
		}
	}


	m_uHitCode = HTCLIENT;
	return m_uHitCode;
}

void duWindow::OnMinMaxInfo(MINMAXINFO *pMinMaxInfo)
{
	pMinMaxInfo->ptMinTrackSize.x = m_uMinWidth;
	pMinMaxInfo->ptMinTrackSize.y = m_uMinHeight;

	duRect rectWorkArea; 
	rectWorkArea.SetRectEmpty();
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWorkArea, NULL);
	
	if (m_uMaxWidth > (UINT)rectWorkArea.Width())
	{
		pMinMaxInfo->ptMaxSize.x = rectWorkArea.Width();
		pMinMaxInfo->ptMaxTrackSize.x = rectWorkArea.Width();
	}
	else
	{
		pMinMaxInfo->ptMaxSize.x = m_uMaxWidth;
		pMinMaxInfo->ptMaxTrackSize.x = m_uMaxWidth;
	}

	if (m_uMaxHeight > (UINT)rectWorkArea.Height())
	{
		pMinMaxInfo->ptMaxSize.y = rectWorkArea.Height();
		pMinMaxInfo->ptMaxTrackSize.y = rectWorkArea.Height();
	}
	else
	{
		pMinMaxInfo->ptMaxSize.y = m_uMaxHeight;
		pMinMaxInfo->ptMaxTrackSize.y = m_uMaxHeight;
	}
}

void duWindow::RefreshWindowRgn()
{
	duRgn *pRgn = (duRgn *)GetResObj(m_strRgnName, DU_RES_RGN);
	if (pRgn)
	{
		HRGN hRgn = pRgn->GetHRgn(m_rect.Width(), m_rect.Height());
		if (hRgn)
			::SetWindowRgn(GetHwnd(), hRgn, TRUE);
	}
}
