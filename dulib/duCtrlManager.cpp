//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duCtrlManager.h
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#include "duCtrlManager.h"
#include "controls/duWindow.h"
#include "controls/duButton.h"
#include "controls/duCheckBox.h"
#include "controls/duLayout.h"
#include "controls/duStatic.h"
#include "controls/duScroll.h"
#include "controls/duEdit.h"
#include "controls/duScrollView.h"
#include "controls/duReportView.h"
#include "controls/duListBox.h"
#include "controls/duComboBox.h"
#include "controls/duTabButton.h"
#include "controls/duListBoxEx.h"
#include "controls/duIMPanelNode.h"
#include "controls/duRadio.h"
#include "controls/duScrollContainer.h"
#include "controls/duSpin.h"
#include "controls/duHwndObj.h"
#include "controls/duGifCtrl.h"
#include "controls/duLEDTimer.h"
#include "controls/duMenuBar.h"
#include "controls/duOutlookBar.h"
#include "controls/duProgress.h"
#include "controls/duSlider.h"
#include "controls/duSplitter.h"
#include "controls/duStaticEx.h"
#include "controls/duTreeCtrl.h"
#include "controls/duLink.h"
#include "controls/duXmlStatic.h"
#include "controls/duMenuItem.h"
#include "controls/duPage.h"
#include "controls/duHeaderCtrl.h"
#include "controls/duListCtrl.h"
#include "controls/duTreeListCtrl.h"
#include "controls/duTreeListItem.h"
#include "controls/duAnimation.h"
#include "controls/duGridLayout.h"
#include "controls/duPie.h"
#include "controls/duMenu.h"
#include "controls/duMLEdit.h"
#include "controls/duRemoteCtrl.h"
#include "controls/duFlash.h"

extern MessageStruct g_messageStruct[];

duCtrlManager::duCtrlManager(duWindowManager *pWinManager) :
	 m_pWinManager(pWinManager)
	,m_hWnd(NULL)
	,m_wndProc(NULL)
	,m_pFocus(NULL)
	,m_pHot(NULL)
	,m_pCapture(NULL)
	,m_hBmpUpdate(NULL)
	,m_hBmpCaret(NULL)
	,m_nCaretPosX(0)
	,m_nCaretPosY(0)
	,m_fCaretDrawed(FALSE)
	,m_hWndToolTip(NULL)
	,m_fSizeMoving(FALSE)
{
	m_mapTimer.clear();
	m_setHookMsg.clear();
	m_mapName.clear();
	m_nCaretTimer = GetUniTimer();
}

duCtrlManager::~duCtrlManager()
{
	if (m_hBmpUpdate)
	{
		::DeleteObject(m_hBmpUpdate);
		m_hBmpUpdate = NULL;
	}
	
	if (m_hBmpCaret)
	{
		::DeleteObject(m_hBmpCaret);
		m_hBmpCaret = NULL;
	}

	if (m_hWndToolTip)
	{
		::DestroyWindow(m_hWndToolTip);
		m_hWndToolTip = NULL;
	}
	
	vector<duPlugin *> vtPlugin;
	vtPlugin.clear();
	
	int i, j;
	int nLevelCount = m_pZorderList->GetLayerCount();
	for (i = 0;i < nLevelCount; i++)
	{
		int nCtrlCount = m_pZorderList->GetLayerChildCount(i);
		for (j = nCtrlCount - 1;j >= 0; j--)
		{
			duPlugin *pPlugin = m_pZorderList->GetPlugin(i, j);
			if (pPlugin == NULL || !Plugin_IsValid(pPlugin))
				continue;
			
			vtPlugin.push_back(pPlugin);
		}
	}

	if (m_pZorderList)
	{
		delete m_pZorderList;
		m_pZorderList = NULL;
	}
	
	m_mapTimer.clear();
	m_setHookMsg.clear();
	m_mapName.clear();
	
	for (i = 0;i < (int)vtPlugin.size(); i++)
	{
		duPlugin *pPlugin = vtPlugin[i];
		Plugin_DeleteValid(pPlugin);
		pPlugin->FinalRelease();
	}
	vtPlugin.clear();

	::SetProp(m_hWnd, ATOMPLUGIN, (HANDLE)NULL);
}

BOOL duCtrlManager::Create(HWND hWnd, TiXmlElement *pElement)
{
	duPlugin *pCtrl = CreateDirectUIControl(pElement->Value());
	if (pCtrl == NULL)
		return FALSE;

	//pCtrl->SetTypeInfo(m_pWinManager->GetTypeInfoByName(pCtrl->GetTypeInfoName()));

	::SetProp(hWnd, ATOMPLUGIN, (HANDLE)this);

	m_hWnd = hWnd;
	pCtrl->SetZorder(0);
	if (!pCtrl->OnCreate(pElement, this))
		return FALSE;

	m_pZorderList = new duZorderList(pCtrl);
	m_pZorderList->DispatchOnCreate();

	::SetClassLong(hWnd, GCL_STYLE, ::GetClassLong(hWnd, GCL_STYLE)|CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW);

	DWORD dwStyle = ::GetWindowLong(hWnd, GWL_STYLE);
	dwStyle &= ~(WS_CAPTION|WS_BORDER|WS_SIZEBOX|WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
	::SetWindowLong(hWnd, GWL_STYLE, dwStyle|WS_CLIPCHILDREN|WS_CLIPSIBLINGS);

	DWORD dwStyleEx;
	dwStyleEx = ::GetWindowLong(hWnd, GWL_EXSTYLE);
	dwStyleEx &= ~(WS_EX_CLIENTEDGE|WS_EX_STATICEDGE|WS_EX_WINDOWEDGE|WS_EX_DLGMODALFRAME);
	::SetWindowLong(hWnd, GWL_EXSTYLE, dwStyleEx);
	
	m_wndProc = (WNDPROC)::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)DirectUIWinProc);

	::SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOOWNERZORDER|SWP_NOZORDER|SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_FRAMECHANGED);
	duWindow *pWin = (duWindow *)m_pZorderList->GetRoot();
	if (pWin == NULL)
	{
		::RemoveProp(hWnd, ATOMPLUGIN);
		return FALSE;
	}
	
	pWin->InitWindowPos();
	if (IsLayeredWindow())
	{
		HDC hDC = ::GetDC(m_hWnd);
		OnPaint(hDC);
		::ReleaseDC(m_hWnd, hDC);
	}

	m_hWndToolTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, _T(""), WS_POPUP, 
		0, 0, 0, 0, m_hWnd, NULL, ::GetModuleHandle(NULL), NULL);
	SetWindowPos(m_hWndToolTip, HWND_TOPMOST,0, 0, 0, 0, SWP_NOOWNERZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
	
	return TRUE;
}

LRESULT duCtrlManager::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TRACKMOUSEEVENT tme;
	POINT pt;
	duRect rectPlugin;
	duRect rectVisible;
	duRect rectClient;
	duPlugin *pPlugin = NULL;
	HDC hDC = NULL;
	PAINTSTRUCT ps;
	duRect rectUpdate;
	set<duPlugin *>::iterator iterHook;
	
	if (GetActiveWindow() != hWnd && !IsWindowEnabled(hWnd))
	{	//当前窗口弹出模态对话框之后调用系统缺省窗口过程
		//以避免出现鼠标点击窗口无法正常激活的bug
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST)
	{
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);

		tme.cbSize      = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags     = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 200;
		tme.hwndTrack   = m_hWnd;

		_TrackMouseEvent(&tme);

		if (m_pCapture && ::GetCapture() == m_hWnd)
		{
			switch(uMsg)
			{
			case WM_MOUSEMOVE:
				DispatchMsg(m_pCapture, DUMI_MOUSEMOVE, pt.x, pt.y);
				break;
			case WM_LBUTTONDOWN:
				DispatchMsg(m_pCapture, DUMI_MOUSELDOWN, pt.x, pt.y);
				break;
			case WM_LBUTTONUP:
				DispatchMsg(m_pCapture, DUMI_MOUSELUP, pt.x, pt.y);
				break;
			case WM_LBUTTONDBLCLK:
				DispatchMsg(m_pCapture, DUMI_MOUSEDBCLICK, pt.x, pt.y);
				break;
			case WM_RBUTTONDOWN:
				DispatchMsg(m_pCapture, DUMI_MOUSERDOWN, pt.x, pt.y);
				break;
			case WM_RBUTTONUP:
				DispatchMsg(m_pCapture, DUMI_MOUSERUP, pt.x, pt.y);
				break;
			}
			
			goto Proc_Exit;
		}
	}
		

	iterHook = m_setHookMsg.begin();
	for (; iterHook != m_setHookMsg.end(); iterHook++)
	{
		pPlugin = *iterHook;
		if (Plugin_IsValid(pPlugin))
		{
			if (!pPlugin->IsDisable())
				pPlugin->OnWndProc(hWnd, uMsg, wParam, lParam);
		}
	}

	if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST)
	{
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);

		MSG msg;
		msg.hwnd = hWnd;
		msg.wParam = wParam;
		msg.lParam = lParam;
		msg.pt = pt;
		msg.message = uMsg;
		msg.time = 0;

		::SendMessage(m_hWndToolTip, TTM_RELAYEVENT, 0, (LPARAM)&msg);


		int i, j;
		int nLevelCount = m_pZorderList->GetLayerCount();
		for (i = 0;i < nLevelCount; i++)
		{
			int nCtrlCount = m_pZorderList->GetLayerChildCount(i);
			for (j = nCtrlCount - 1;j >= 0; j--)
			{
				pPlugin = m_pZorderList->GetPlugin(i, j);
				if (pPlugin == NULL || !Plugin_IsValid(pPlugin) || !Plugin_IsVisible(pPlugin))
					continue;

				rectPlugin.SetRectEmpty();
				pPlugin->GetRect(&rectPlugin);
				if (!rectPlugin.PtInRect(pt))
					continue;				

				rectVisible.SetRectEmpty();
				Plugin_GetVisibleRect(pPlugin, &rectVisible);
				if (!rectVisible.PtInRect(pt))
					continue;

				if (pPlugin != m_pHot)
				{
					DispatchMsg(m_pHot, DUMI_MOUSELEAVE, pt.x, pt.y);
					DispatchMsg(pPlugin, DUMI_MOUSEIN, pt.x, pt.y);
					m_pHot = pPlugin;
				}
				goto MouseMove_Exit;
			}
		}

MouseMove_Exit:
		DispatchMsg(m_pHot, DUMI_MOUSEMOVE, pt.x, pt.y);


	}
	
	switch(uMsg)
	{
	case WM_MOUSEMOVE:
	{
	}
	break;
	
	case WM_TIMER:
	{
		if (wParam == m_nCaretTimer)
		{
			OnCaretTimer();
			break;
		}
	
		pPlugin = GetTimer((UINT)wParam);
		DispatchMsg(pPlugin, DUMI_TIMER, wParam, 0);
	}
	break;

	case WM_MOUSELEAVE:
	{
		if (m_pCapture && ::GetCapture() == m_hWnd)
		{
			DispatchMsg(m_pCapture, DUMI_MOUSELEAVE, pt.x, pt.y);
			goto Proc_Exit;
		}
		
		if (Plugin_IsValid(m_pHot))
			DispatchMsg(m_pHot, DUMI_MOUSELEAVE, pt.x, pt.y);
		m_pHot = m_pZorderList->GetRoot();	
	}
	break;

	case WM_MOUSEHOVER:
	{
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);

		if (m_pCapture && ::GetCapture() == m_hWnd)
		{
			DispatchMsg(m_pCapture, DUMI_MOUSEHOVER, pt.x, pt.y);
			goto Proc_Exit;
		}
		
		if (Plugin_IsValid(m_pHot))
			DispatchMsg(m_pHot, DUMI_MOUSEHOVER, pt.x, pt.y);		
	}
	break;

	case WM_LBUTTONDOWN:
	{
		TCHAR szClassName[256];
		ZeroMemory(szClassName, 256 * sizeof(TCHAR));
		GetClassName(m_hWnd, szClassName, 256);

		if (lstrcmpi(szClassName, _T("dulibMenuClass")) != 0)
			::SetFocus(m_hWnd);

		if (Plugin_IsValid(m_pHot))
			DispatchMsg(m_pHot, DUMI_MOUSELDOWN, pt.x, pt.y);

		if (m_pFocus != m_pHot)
		{
			if (m_pHot != m_pZorderList->GetRoot())
			{
				SetFocusPlugin(m_pHot, TRUE);
			}
		}
	}
	break;

	case WM_LBUTTONUP:
	{
		if (Plugin_IsValid(m_pHot))
			DispatchMsg(m_pHot, DUMI_MOUSELUP, pt.x, pt.y);
	}
	break;

	case WM_LBUTTONDBLCLK:
	{
		TCHAR szClassName[256];
		ZeroMemory(szClassName, 256 * sizeof(TCHAR));
		GetClassName(m_hWnd, szClassName, 256);

		if (lstrcmpi(szClassName, _T("dulibMenuClass")) != 0)
			::SetFocus(m_hWnd);

		if (Plugin_IsValid(m_pHot))
			DispatchMsg(m_pHot, DUMI_MOUSEDBCLICK, pt.x, pt.y);

		if (m_pFocus != m_pHot)
		{
			if (m_pHot != m_pZorderList->GetRoot())
			{
				SetFocusPlugin(m_pHot, TRUE);
			}
		}
	}
	break;

	case WM_RBUTTONDOWN:
	{
		TCHAR szClassName[256];
		ZeroMemory(szClassName, 256 * sizeof(TCHAR));
		GetClassName(m_hWnd, szClassName, 256);

		if (lstrcmpi(szClassName, _T("dulibMenuClass")) != 0)
			::SetFocus(m_hWnd);

		if (Plugin_IsValid(m_pHot))
			DispatchMsg(m_pHot, DUMI_MOUSERDOWN, pt.x, pt.y);

		if (m_pFocus != m_pHot)
		{
			if (m_pHot != m_pZorderList->GetRoot())
				SetFocusPlugin(m_pHot, TRUE);
		}	
	}
	break;

	case WM_RBUTTONUP:
	{
		if (Plugin_IsValid(m_pHot))
			DispatchMsg(m_pHot, DUMI_MOUSERUP, pt.x, pt.y);
	}
	break;
	
	case WM_MOUSEWHEEL:
	{
		if (Plugin_IsValid(m_pFocus))
			DispatchMsg(m_pFocus, DUMI_MOUSEWHEEL, wParam, lParam);
	}
	break;

	case WM_CAPTURECHANGED:
		break;

	case WM_KEYDOWN:
		{
			if (wParam == VK_TAB)
			{
				OnTabKey();
			}

			if (Plugin_IsValid(m_pFocus))
				DispatchMsg(m_pFocus, DUMI_KEYDOWN, wParam, lParam);
		}
		break;

	case WM_KEYUP:
		if (Plugin_IsValid(m_pFocus))
			DispatchMsg(m_pFocus, DUMI_KEYUP, wParam, lParam);
		break;

	case WM_PAINT:
	{
		if (wParam)
		{
			hDC = (HDC)wParam;
			OnPaint(hDC);
		}
		else
		{
			hDC = ::BeginPaint(hWnd, &ps);
			OnPaint(hDC);
			::EndPaint(hWnd, &ps);
		}
	}
	return 0;

	case WM_ERASEBKGND:
		return TRUE;

	case WM_NCHITTEST:
	{
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
		
		
		duWindow *pWin = (duWindow *)m_pZorderList->GetRoot();
		return pWin->OnHitTest(pt);
	}
	break;

	case WM_SETCURSOR:
	{
		int nCodeHitTest = LOWORD(lParam);

		if(nCodeHitTest == HTBOTTOM || nCodeHitTest == HTTOP)
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
		else if(nCodeHitTest == HTBOTTOMLEFT || nCodeHitTest == HTTOPRIGHT)
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENESW)));
		else if(nCodeHitTest == HTBOTTOMRIGHT || nCodeHitTest == HTTOPLEFT)
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENWSE)));
		else if(nCodeHitTest == HTLEFT || nCodeHitTest == HTRIGHT)
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
		else
		{
			if (!Plugin_IsValid(m_pHot))
				return FALSE;

			if (m_pHot->IsDisable())
			{
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
				return TRUE;
			}
			
			return m_pHot->OnSetCursor();
		}
		return FALSE;
	}
	break;

	case WM_NCLBUTTONDOWN:
	{
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
		ScreenToClient(hWnd, &pt);

		if (wParam == HTTOPLEFT || wParam ==HTTOPRIGHT || wParam ==HTBOTTOMLEFT || wParam ==HTBOTTOMRIGHT  || wParam ==HTTOP || wParam ==HTLEFT ||  wParam == HTRIGHT ||  wParam == HTBOTTOM)
		{
			::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_SIZE + wParam-9, lParam);
		}
	}
	break;

	case WM_GETMINMAXINFO:
	{
		MINMAXINFO *pMinMaxInfo = (MINMAXINFO *)lParam;

		duWindow *pWin = (duWindow *)m_pZorderList->GetRoot();
		pWin->OnMinMaxInfo(pMinMaxInfo);
	}
	break;
		
	case WM_WINDOWPOSCHANGED:
	{
		LPWINDOWPOS lpwpos = (LPWINDOWPOS)lParam;
		if (lpwpos->flags & SWP_NOSIZE)
			goto Proc_Exit;
		
		duRect rcClient(0, 0, lpwpos->cx, lpwpos->cy);
		duWindow *pWin = (duWindow *)m_pZorderList->GetRoot();
		pWin->SetRect(&rcClient);
		pWin->Resize(NULL);
		
		if (IsLayeredWindow())
		{
			HDC hDC = ::GetDC(m_hWnd);
			OnPaint(hDC);
			::ReleaseDC(m_hWnd, hDC);
		}
		else
			pWin->RefreshWindowRgn();
	}
	break;
	
	case WM_NCACTIVATE:
	{
		duWindow *pWin = (duWindow *)m_pZorderList->GetRoot();
		if (wParam)
			pWin->SetState(DU_STATE_NORMAL_ALL);
		else
			pWin->SetState(DU_STATE_CHECKED_ALL);
		
		::InvalidateRect(m_hWnd, NULL, TRUE);
	}
	break;
	
	case WM_NCDESTROY:
	{
		LRESULT lRes = ::CallWindowProc(m_wndProc, hWnd, uMsg, wParam, lParam);

		WNDPROC wndProc = m_wndProc;
		::RemoveProp(hWnd, ATOMPLUGIN);
		m_pWinManager->RemoveHwnd(hWnd);
		delete this;
		
		return lRes;
	}
	break;
	
	case WM_COMMAND:
	{
		if (HIWORD(wParam) == EN_SETFOCUS)
			SetFocusPlugin(m_pZorderList->GetRoot(), TRUE);
	}
	break;

    case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS;

	case WM_ENTERSIZEMOVE:
		m_fSizeMoving = TRUE;
		break;

	case WM_EXITSIZEMOVE:
		m_fSizeMoving = FALSE;
		break;

	default:
		break;
	}
	
	if (!GetCtrlManager(hWnd))
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	
Proc_Exit:
	return ::CallWindowProc(m_wndProc, hWnd, uMsg, wParam, lParam);
}

void duCtrlManager::OnPaint(HDC hDC)
{
	duPlugin *pPlugin;
	duRect rcClient;
	duRect rcWindow;
	duRect rcInter;
	duRect rcVisible;
	HDC hMemDC;
	HBITMAP hOldBitmap;
	BITMAPINFO bmInfo;
	PBYTE pBits;
	
	if (hDC == NULL)
		return;

	if (m_hBmpUpdate)
	{
		::DeleteObject(m_hBmpUpdate);
		m_hBmpUpdate = NULL;
	}
	
	::GetClientRect(m_hWnd, &rcClient);
	hMemDC = ::CreateCompatibleDC(hDC);
	InitBitmapInfo(&bmInfo, rcClient.Width(), rcClient.Height());
	m_hBmpUpdate = ::CreateDIBSection(hDC, &bmInfo, DIB_RGB_COLORS,(void**)&pBits, 0, 0);
	hOldBitmap = (HBITMAP)::SelectObject(hMemDC, m_hBmpUpdate);

	int i, j;
	int nLevelCount = m_pZorderList->GetLayerCount();
	for (i = nLevelCount-1;i >= 0; i--)
	{
		int nCtrlCount = m_pZorderList->GetLayerChildCount(i);
		for (j = 0;j < nCtrlCount; j++)
		{
			pPlugin = m_pZorderList->GetPlugin(i, j);
			if (pPlugin == NULL || !Plugin_IsValid(pPlugin) || !Plugin_IsVisible(pPlugin))
				continue;

			rcVisible.SetRectEmpty();
			Plugin_GetVisibleRect(pPlugin, &rcVisible);
			
			DispatchMsg(pPlugin, DUMI_PAINT, (WPARAM)hMemDC, (LPARAM)&rcVisible);
		}
	}
	
	if (IsLayeredWindow())
	{
		::GetWindowRect(m_hWnd, &rcWindow);
		
		BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
		POINT ptDst = {rcWindow.left, rcWindow.top};
		SIZE sizeWnd = {rcWindow.Width(), rcWindow.Height()};
		POINT ptSrc = {0, 0};
		
		_HideCaret();
		//DrawTrialBitmap(hMemDC);
		::UpdateLayeredWindow(m_hWnd, hDC, &ptDst, &sizeWnd, hMemDC, &ptSrc, 0, &blendPixelFunction, ULW_ALPHA);
		_ShowCaret();
	}
	else
	{
		//DrawTrialBitmap(hMemDC);
		::BitBlt(hDC, 0, 0, rcClient.Width(), rcClient.Height(), hMemDC, 0, 0, SRCCOPY);
	}

	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);
}

BOOL duCtrlManager::DispatchMsg(duPlugin *pPlugin, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT pt;

	if (!Plugin_IsValid(pPlugin))
		return FALSE;

	if (uMsg >= DUMI_MOUSEMOVE && uMsg <= DUMI_MOUSEWHEEL)
	{
		if (pPlugin->IsDisable())
			return TRUE;
		
		pt.x = wParam;
		pt.y = lParam;
	}
	
	switch(uMsg)
	{
	case DUMI_PAINT:
	{
		HDC hDC = (HDC)wParam;
		duRect rcVisible((LPRECT)lParam);
		DirectDrawPlugin(hDC, pPlugin, rcVisible);
	}
	break;
	
	case DUMI_RESIZE:
		pPlugin->OnChildResize((duPlugin *)wParam);
		break;
	
	case DUMI_SHOWCONTROL:
		pPlugin->OnChildShow((duPlugin *)wParam, (BOOL)lParam);
		break;
	
	case DUMI_MOUSEIN:
	{
		pPlugin->OnMouseIn(pt);
		if (Plugin_IsValid(pPlugin))
			pPlugin->NotifyUser(DUM_MOUSEIN, (WPARAM)pt.x, (LPARAM)pt.y);
	}
	break;
		
	case DUMI_MOUSEMOVE:
		pPlugin->OnMouseMove(pt);
		break;
	
	case DUMI_MOUSELEAVE:
	{
		pPlugin->OnMouseLeave(pt);
		if (Plugin_IsValid(pPlugin))
			pPlugin->NotifyUser(DUM_MOUSELEAVE, NULL, NULL);
	}
	break;
	
	case DUMI_MOUSELDOWN:
	{
		pPlugin->OnMouseLDown(pt);
		if (Plugin_IsValid(pPlugin))
			pPlugin->NotifyUser(DUM_MOUSELDOWN, (WPARAM)pt.x, (LPARAM)pt.y);
	}
	break;
	
	case DUMI_MOUSELUP:
	{
		pPlugin->OnMouseLUp(pt);
		if (Plugin_IsValid(pPlugin))
			pPlugin->NotifyUser(DUM_MOUSELUP, (WPARAM)pt.x, (LPARAM)pt.y);
	}
	break;
		
	case DUMI_MOUSERDOWN:
	{
		pPlugin->OnMouseRDown(pt);
		if (Plugin_IsValid(pPlugin))
			pPlugin->NotifyUser(DUM_MOUSERDOWN, (WPARAM)pt.x, (LPARAM)pt.y);
	}
	break;

	case DUMI_MOUSERUP:
	{
		pPlugin->OnMouseRUp(pt);
		if (Plugin_IsValid(pPlugin))
			pPlugin->NotifyUser(DUM_MOUSERUP, (WPARAM)pt.x, (LPARAM)pt.y);
	}
	break;

	case DUMI_MOUSEHOVER:
	{
		pPlugin->OnMouseHover(pt);
		if (Plugin_IsValid(pPlugin))
			pPlugin->NotifyUser(DUM_MOUSEHOVER, (WPARAM)pt.x, (LPARAM)pt.y);
	}
	break;
	
	case DUMI_MOUSEDBCLICK:
	{
		pPlugin->OnMouseDbClick(pt);
		if (Plugin_IsValid(pPlugin))
			pPlugin->NotifyUser(DUM_MOUSEDBCLICK, (WPARAM)pt.x, (LPARAM)pt.y);
	}
	break;
	
	case DUMI_MOUSEWHEEL:
	{
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);

		UINT fwKeys = (UINT)(short)LOWORD(wParam);
		int  zDelta = (int)(short)HIWORD(wParam);
		pPlugin->OnMouseWheel(fwKeys, zDelta, pt);
	}
	break;

	case DUMI_CAPTURECHANGED:
		break;

	case DUMI_TIMER:
		pPlugin->OnTimer((UINT)wParam);
		break;

	case DUMI_KILLFOCUS:
	{
		pPlugin->SetFocus(FALSE);
		pPlugin->NotifyUser(DUM_KILLFOCUS, NULL, NULL);

		if (Plugin_IsValid(pPlugin))
		{
			pPlugin->OnKillFocus((duPlugin *)wParam);
			Plugin_Redraw(pPlugin, TRUE);
		}	
	}
	break;

	case DUMI_SETFOCUS:
	{
		pPlugin->SetFocus(TRUE);
		pPlugin->NotifyUser(DUM_SETFOCUS, NULL, NULL);

		if (Plugin_IsValid(pPlugin))
		{
			pPlugin->OnSetFocus((duPlugin *)wParam);
			Plugin_Redraw(pPlugin, TRUE);
		}
	}
	break;

	case DUMI_CREATE:
		break;
		
	case DUMI_KEYDOWN:
		if (!pPlugin->IsDisable())
			pPlugin->OnKeyDown((UINT)wParam, LOWORD(lParam), HIWORD(lParam));
		break;
	
	case DUMI_KEYUP:
		if (!pPlugin->IsDisable())
			pPlugin->OnKeyUp((UINT)wParam, LOWORD(lParam), HIWORD(lParam));
		break;
	}
	return TRUE;
}

void duCtrlManager::ClearGraphics(HDC hDC, duRect &rectClear)
{
	TRIVERTEX vert[2] ;
	GRADIENT_RECT gRect;
	vert [0].x = rectClear.left;
	vert [0].y = rectClear.top;
	vert [0].Red = 0;
	vert [0].Green = 0;
	vert [0].Blue = 0;
	vert [0].Alpha = 0;
	
	vert [1].x = rectClear.right;
	vert [1].y = rectClear.bottom; 
	vert [1].Red = 0;
	vert [1].Green = 0;
	vert [1].Blue = 0;
	vert [1].Alpha = 0;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	::GradientFill(hDC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_H);

/*
	Graphics *graphics = new Graphics(hDC);
	Rect rect;
	rect.X = rectClear.left;
	rect.Y = rectClear.top;
	rect.Width = rectClear.Width();
	rect.Height = rectClear.Height();
	graphics->SetClip(rect, CombineModeReplace);
	Color clr(0, 0, 0, 0);
	graphics->Clear(clr);

	graphics->SetClip((HRGN)NULL, CombineModeReplace);
	delete graphics;
*/
}

BOOL duCtrlManager::RedrawPlugin(duPlugin *pPlugin)
{
	if (!Plugin_IsValid(pPlugin))
		return FALSE;

	duRect rcPluginVisible;
	Plugin_GetVisibleRect(pPlugin, &rcPluginVisible);
	if (rcPluginVisible.IsRectEmpty())
		return FALSE;

	duRect rcPlugin;
	Plugin_GetRect(pPlugin, &rcPlugin);

	duRect rcWindow;
	GetClientRect(m_hWnd, &rcWindow);

	HDC hMemDC = ::CreateCompatibleDC(NULL);
	BITMAPINFO bmInfo;
	InitBitmapInfo(&bmInfo, rcWindow.Width(), rcWindow.Height());
	
	PBYTE pBits = NULL;
	HBITMAP hBitmap = ::CreateDIBSection(hMemDC, &bmInfo, DIB_RGB_COLORS,(void**)&pBits, 0, 0);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);
	
	DrawDownLayer(hMemDC, rcPlugin, rcPluginVisible, pPlugin);
	
	if (Plugin_IsVisible(pPlugin))
	{
		DispatchMsg(pPlugin, DUMI_PAINT, (WPARAM)hMemDC, (LPARAM)&rcPluginVisible);
	}

	DrawUpLayer(hMemDC, rcPlugin, rcPluginVisible, pPlugin);
	
	if (IsLayeredWindow())
	{
		if (m_hBmpUpdate == NULL)
		{
			//_TRACE(_T("Error: Layered window cached is Null\n"));
			return FALSE;
		}
		
		HDC hUpdateDC = ::CreateCompatibleDC(NULL);
		HBITMAP hOldUpdateBitmap = (HBITMAP)::SelectObject(hUpdateDC, m_hBmpUpdate);
		ClearGraphics(hUpdateDC, rcPluginVisible);
		
		BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
		::AlphaBlend(hUpdateDC, rcPluginVisible.left, rcPluginVisible.top, rcPluginVisible.Width(), rcPluginVisible.Height(), 
				hMemDC, rcPluginVisible.left, rcPluginVisible.top, rcPluginVisible.Width(), rcPluginVisible.Height(), bf);

		duRect rcWindow;
		::GetWindowRect(m_hWnd, &rcWindow);
		
		BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
		POINT ptDst = {rcWindow.left, rcWindow.top};
		SIZE sizeWnd = {rcWindow.Width(), rcWindow.Height()};
		POINT ptSrc = {0, 0};
		
		HDC hDC = ::GetDC(m_hWnd);

		//_HideCaret();
		::UpdateLayeredWindow(m_hWnd, hDC, &ptDst, &sizeWnd, hUpdateDC, &ptSrc, 0, &blendPixelFunction, ULW_ALPHA);
		//_ShowCaret();

		::SelectObject(hUpdateDC, hOldUpdateBitmap);
		::DeleteDC(hUpdateDC);

		::ReleaseDC(m_hWnd, hDC);
	}
	else
	{
		HDC hDC = ::GetDC(m_hWnd);
		
		//_TRACE(_T("rcPluginVisible[%d %d %d %d]\n"), rcPluginVisible.left, rcPluginVisible.top, rcPluginVisible.Width(), rcPluginVisible.Height());
		_HideCaret();
		::BitBlt(hDC, rcPluginVisible.left, rcPluginVisible.top, rcPluginVisible.Width(), rcPluginVisible.Height(), 
			hMemDC, rcPluginVisible.left, rcPluginVisible.top, SRCCOPY);
		_ShowCaret();
		::ReleaseDC(m_hWnd, hDC);
	}
	
	::SelectObject(hMemDC, hOldBitmap);
	::DeleteObject(hBitmap);
	::DeleteDC(hMemDC);
	return TRUE;
}

void duCtrlManager::DrawUpLayer(HDC hDC, const duRect &rcPlugin, const duRect &rcPluginVisible, duPlugin *pPlugin)
{
	duRedrawLayer *pRedrawLayer = new duRedrawLayer;
	if (pRedrawLayer == NULL)
		return;

	//_TRACE(_T("DrawUpLayer begin [%s]%s\n"), pPlugin->GetType(), pPlugin->GetName());
	
	pRedrawLayer->InitUp(m_pZorderList, pPlugin, rcPluginVisible);
	UINT uZorder = pPlugin->GetZorder();
	int i, j;
	int nLevelCount = pRedrawLayer->GetLayerCount();
	for (i = nLevelCount-1;i >= 0; i--)
	{		
		int nCtrlCount = pRedrawLayer->GetLayerChildCount(i);
		for (j = 0;j < nCtrlCount; j++)
		{
			duPlugin *pTemp = pRedrawLayer->GetPlugin(i, j);

			//_TRACE(_T("    [%d][%s]%s\n"), pTemp->GetZorder(), pTemp->GetType(), pTemp->GetName());

			duRect rcVisible;
			rcVisible.SetRectEmpty();
			Plugin_GetVisibleRect(pTemp, &rcVisible);

			DispatchMsg(pTemp, DUMI_PAINT, (WPARAM)hDC, (LPARAM)&rcVisible);

		}
	}

	//_TRACE(_T("DrawUpLayer end [%s]%s\n"), pPlugin->GetType(), pPlugin->GetName());
	
	delete pRedrawLayer;
}

void duCtrlManager::DrawUpLayerPlguin(HDC hDC, duPlugin *pLayerPlugin, const duRect &rcPlugin, const duRect &rcPluginVisible)
{
	duRect rcLayerPlugin;
	Plugin_GetRect(pLayerPlugin, &rcLayerPlugin);

	duRect rcLayerPluginVisible;
	Plugin_GetVisibleRect(pLayerPlugin, &rcLayerPluginVisible);
		
	duRect rcInter;
	if (!::IntersectRect(&rcInter, &rcPluginVisible, &rcLayerPluginVisible))
	{
		_TRACE(_T("error DrawUpLayerPlguin IntersectRect NULL\n"));
		return;
	}

	HDC hMemDC = ::CreateCompatibleDC(NULL);
	BITMAPINFO bmInfo;
	InitBitmapInfo(&bmInfo, rcLayerPlugin.Width(), rcLayerPlugin.Height());
	
	PBYTE pBits = NULL;
	HBITMAP hBitmap = ::CreateDIBSection(hMemDC, &bmInfo, DIB_RGB_COLORS,(void**)&pBits, 0, 0);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);
		
	pLayerPlugin->DrawObject(hMemDC);
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
		
	::AlphaBlend(hDC, rcInter.left - rcPlugin.left, rcInter.top - rcPlugin.top,
			rcInter.Width(), rcInter.Height(), 
			hMemDC, rcInter.left - rcLayerPlugin.left, rcInter.top - rcLayerPlugin.top,
			rcInter.Width(), rcInter.Height(), bf);
				
	::SelectObject(hMemDC, hOldBitmap);
	
	if (pLayerPlugin->IsCached())
		pLayerPlugin->SetCachedBitmap(hBitmap);
	else
		::DeleteObject(hBitmap);
	
	::DeleteDC(hMemDC);
}

void duCtrlManager::DrawDownLayer(HDC hDC, const duRect &rcPlugin, const duRect &rcPluginVisible, duPlugin *pPlugin)
{
	duRedrawLayer *pRedrawLayer = new duRedrawLayer;
	if (pRedrawLayer == NULL)
		return;
	
	pRedrawLayer->InitDown(m_pZorderList, pPlugin, rcPluginVisible);

	UINT uZorder = pPlugin->GetZorder();
	int i, j;
	int nLevelCount = pRedrawLayer->GetLayerCount();
	int nStartLayer = nLevelCount - 1;
	for (i = nStartLayer;i >= 0; i--)
	{
	//	UINT uLayerZorder = pRedrawLayer->GetLayerZorder(i);
	//	if (uZorder < uLayerZorder)
	//		break;
		
		int nCtrlCount = pRedrawLayer->GetLayerChildCount(i);
		for (j = 0;j < nCtrlCount; j++)
		{
			duPlugin *pTemp = pRedrawLayer->GetPlugin(i, j);

			//_TRACE(_T("    Draw Down Layer [%s]%s\n"), pTemp->GetType(), pTemp->GetName());
			//DrawDownLayerPlguin(hDC, pTemp, rcPlugin, rcPluginVisible);

			duRect rcVisible;
			rcVisible.SetRectEmpty();
			Plugin_GetVisibleRect(pTemp, &rcVisible);

			DispatchMsg(pTemp, DUMI_PAINT, (WPARAM)hDC, (LPARAM)&rcVisible);
		}
	}
	
	delete pRedrawLayer;
	
	//_TRACE(_T("DrawDownLayer end [%s]%s\n"), pPlugin->GetType(), pPlugin->GetName());
}

void duCtrlManager::DrawDownLayerPlguin(HDC hDC, duPlugin *pLayerPlugin, const duRect &rcPlugin, const duRect &rcPluginVisible)
{
	duRect rcLayerPlugin;
	Plugin_GetRect(pLayerPlugin, &rcLayerPlugin);

	duRect rcLayerPluginVisible;
	Plugin_GetVisibleRect(pLayerPlugin, &rcLayerPluginVisible);
		
	duRect rcInter;
	if (!::IntersectRect(&rcInter, &rcPluginVisible, &rcLayerPluginVisible))
	{
		_TRACE(_T("error DrawDownLayerPlguin IntersectRect NULL\n"));
		return;
	}
	
	//if (pLayerPlugin->IsCached() && pLayerPlugin->GetCachedBitmap() != NULL)
	//{
	//	HDC hMemDC = ::CreateCompatibleDC(NULL);
	//	HBITMAP hBitmap = pLayerPlugin->GetCachedBitmap();
	//	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);
	//	
	//	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	//	
	//	::AlphaBlend(hDC, rcInter.left - rcPlugin.left, rcInter.top - rcPlugin.top,
	//			rcInter.Width(), rcInter.Height(), 
	//			hMemDC, rcInter.left - rcLayerPlugin.left, rcInter.top - rcLayerPlugin.top,
	//			rcInter.Width(), rcInter.Height(), bf);
	//
	//	::SelectObject(hMemDC, hOldBitmap);
	//	::DeleteDC(hMemDC);
	//}
	//else
	{
		HDC hMemDC = ::CreateCompatibleDC(NULL);
		BITMAPINFO bmInfo;
		InitBitmapInfo(&bmInfo, rcLayerPlugin.Width(), rcLayerPlugin.Height());
	
		PBYTE pBits = NULL;
		HBITMAP hBitmap = ::CreateDIBSection(hMemDC, &bmInfo, DIB_RGB_COLORS,(void**)&pBits, 0, 0);
		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);
		
		pLayerPlugin->DrawObject(hMemDC);
		BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
		
		::AlphaBlend(hDC, rcInter.left - rcPlugin.left, rcInter.top - rcPlugin.top,
				rcInter.Width(), rcInter.Height(), 
				hMemDC, rcInter.left - rcLayerPlugin.left, rcInter.top - rcLayerPlugin.top,
				rcInter.Width(), rcInter.Height(), bf);
				
		::SelectObject(hMemDC, hOldBitmap);
		::DeleteObject(hBitmap);
		::DeleteDC(hMemDC);
	}
}

BOOL duCtrlManager::IsLayeredWindow()
{
	if ((m_hWnd != NULL) && ::IsWindow(m_hWnd))
	{
		DWORD dwStyleEx = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
		if (dwStyleEx & WS_EX_LAYERED)
			return TRUE;
	}
	return FALSE;
}

void duCtrlManager::DirectDrawPlugin(HDC hDC, duPlugin *pPlugin, duRect &rcPluginVisible)
{
	duRect rcPlugin;
	Plugin_GetRect(pPlugin, &rcPlugin);

	//if (pPlugin->IsCached())
	//{
	//	HDC hMemDC = ::CreateCompatibleDC(NULL);
	//	BITMAPINFO bmInfo;
	//	PBYTE pBits = NULL;
	//	InitBitmapInfo(&bmInfo, rcPlugin.Width(), rcPlugin.Height());
	//
	//	HBITMAP hBitmap = ::CreateDIBSection(hMemDC, &bmInfo, DIB_RGB_COLORS,(void**)&pBits, 0, 0);
	//	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);
	//
	//	pPlugin->DrawObject(hMemDC);
	//	pPlugin->SetCachedBitmap(hBitmap);
	//		
	//	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	//	::AlphaBlend(hDC, rcPluginVisible.left, rcPluginVisible.top, rcPluginVisible.Width(), rcPluginVisible.Height(), 
	//		hMemDC, rcPluginVisible.left-rcPlugin.left, rcPluginVisible.top-rcPlugin.top, rcPluginVisible.Width(), rcPluginVisible.Height(), bf);
	//
	//	::SelectObject(hMemDC, hOldBitmap);
	//	::DeleteDC(hMemDC);
	//}
	//else
	{
		HRGN hRgnClip = ::CreateRectRgn(rcPluginVisible.left, rcPluginVisible.top, rcPluginVisible.right, rcPluginVisible.bottom);
		POINT ptOrg = {0, 0};
		
		::SelectClipRgn(hDC, hRgnClip);
		::SetViewportOrgEx(hDC, rcPlugin.left, rcPlugin.top, &ptOrg);
		
		pPlugin->DrawObject(hDC);
		
		::SetViewportOrgEx(hDC, ptOrg.x, ptOrg.y, &ptOrg);
		::SelectClipRgn(hDC, NULL);
		::DeleteObject(hRgnClip);
	}
}

void duCtrlManager::SetFocusPlugin(duPlugin *pPlugin, BOOL fFocus)
{
	if (!Plugin_IsValid(pPlugin) || pPlugin->IsDisable())
		return;

	duPlugin *pRoot = m_pZorderList->GetRoot();
	if (fFocus)
	{
		if (pPlugin == m_pFocus)
			return;
		
		if (Plugin_IsValid(m_pFocus))
			DispatchMsg(m_pFocus, DUMI_KILLFOCUS, (WPARAM)pPlugin, NULL);
		
		if (Plugin_IsValid(pPlugin))
			DispatchMsg(pPlugin, DUMI_SETFOCUS, (WPARAM)m_pFocus, NULL);

		if (Plugin_IsValid(pPlugin))
			m_pFocus = pPlugin;
		else
			m_pFocus = pRoot;
	}
	else
	{
		if (pPlugin == m_pFocus)
		{
			if (pRoot == pPlugin)
				return;
			
			if (Plugin_IsValid(m_pFocus))
				DispatchMsg(m_pFocus, DUMI_KILLFOCUS, (WPARAM)pPlugin, NULL);
			
			pRoot->SetFocus(TRUE);			
			m_pFocus = pRoot;
		}
	}
}

BOOL duCtrlManager::SetTimer(duPlugin *pPlugin, UINT uTimerId, UINT uElapse)
{
	map<UINT, duPlugin *>::iterator iterTimer = m_mapTimer.find(uTimerId);
	if (iterTimer != m_mapTimer.end())
		return FALSE;

	pair<UINT, duPlugin *> newPair(uTimerId, pPlugin);
	m_mapTimer.insert(newPair);
	::SetTimer(m_hWnd, uTimerId, uElapse, NULL);
	return TRUE;
}

BOOL duCtrlManager::KillTimer(duPlugin *pPlugin, UINT uTimerId)
{
	map<UINT, duPlugin *>::iterator iterTimer = m_mapTimer.find(uTimerId);
	if (iterTimer != m_mapTimer.end())
	{
		::KillTimer(m_hWnd, uTimerId);
		m_mapTimer.erase(iterTimer);
		return TRUE;
	}
	
	return FALSE;
}

duPlugin *duCtrlManager::GetTimer(UINT uTimerId)
{
	map<UINT, duPlugin *>::iterator iterTimer = m_mapTimer.find(uTimerId);
	if (iterTimer != m_mapTimer.end())
	{
		duPlugin *pPlugin = iterTimer->second;
		if (Plugin_IsValid(pPlugin))
			return pPlugin;
		else
		{
			::KillTimer(m_hWnd, uTimerId);
			m_mapTimer.erase(iterTimer);
		}
	}
	return NULL;
}

duPlugin *duCtrlManager::SetCapture(duPlugin *pPlugin)
{
	duPlugin *pOldCapture = m_pCapture;
	::SetCapture(m_hWnd);
		
	if (Plugin_IsValid(m_pCapture))
	{
		m_pCapture->OnCaptureChanged(pPlugin);
		m_pCapture = pPlugin;
		return pOldCapture;
	}
	
	m_pCapture = pPlugin;
	return NULL;
}

BOOL duCtrlManager::ReleaseCapture(duPlugin *pPlugin)
{
	::ReleaseCapture();
	
	if (Plugin_IsValid(pPlugin))
		pPlugin->OnCaptureChanged(NULL);
	
	m_pCapture = NULL;
	return TRUE;
}

BOOL duCtrlManager::HookWindowMessage(duPlugin *pPlugin)
{
	if (!Plugin_IsValid(pPlugin) || IsPluginHook(pPlugin))
		return FALSE;
		
	m_setHookMsg.insert(pPlugin);
	return TRUE;
}

BOOL duCtrlManager::UnHookWindowMessage(duPlugin *pPlugin)
{
	return (BOOL)m_setHookMsg.erase(pPlugin);
}

BOOL duCtrlManager::IsPluginHook(duPlugin *pPlugin)
{
	return m_setHookMsg.find(pPlugin) == m_setHookMsg.end() ? FALSE : TRUE;
}

BOOL duCtrlManager::AddName(duPlugin *pPlugin)
{
	if (!Plugin_IsValid(pPlugin))
		return FALSE;
	
	LPCTSTR lpszName = pPlugin->GetName();
	if (lpszName == NULL || GetPluginByName(lpszName))
	{
		_TRACE(_T("error: [%s] is repeated\n"), lpszName);
		return FALSE;
	}
	
	dustring strName = lpszName;
	pair<dustring, duPlugin *> newPair(strName, pPlugin);
	m_mapName.insert(newPair);
	return TRUE;
}

BOOL duCtrlManager::DeleteName(duPlugin *pPlugin)
{
	if (!Plugin_IsValid(pPlugin))
		return FALSE;
	
	LPCTSTR lpszName = pPlugin->GetName();
	if (lpszName == NULL)
		return FALSE;

	dustring strName = lpszName;
	return (BOOL)m_mapName.erase(strName);
}

duPlugin *duCtrlManager::GetPluginByName(LPCTSTR lpszName)
{
	if (lpszName == NULL)
		return NULL;
	
	duPlugin *pPlugin = NULL;
	dustring strName = lpszName;
	map<dustring, duPlugin *>::iterator iterName = m_mapName.find(strName);
	if (iterName != m_mapName.end())
		pPlugin = iterName->second;
	
	return pPlugin;
}

BOOL duCtrlManager::_CreateCaret(int nWidth, int nHeight)
{
	if (!IsLayeredWindow())
		return CreateCaret(m_hWnd, 0, nWidth, nHeight);
		
	if (m_hBmpCaret)
	{
		::DeleteObject(m_hBmpCaret);
		m_hBmpCaret = NULL;
	}
	
	HDC hMemDC = ::CreateCompatibleDC(NULL);
	BITMAPINFO bmInfo;
	int *pInt = NULL;
	InitBitmapInfo(&bmInfo, nWidth, nHeight);
	m_hBmpCaret = ::CreateDIBSection(hMemDC, &bmInfo, DIB_RGB_COLORS,(void**)&pInt, 0, 0);
	if (pInt == NULL)
	{
		::DeleteDC(hMemDC);
		return FALSE;
	}
	
	int i;
	int nSize = nWidth * nHeight;
	for (i = 0;i < nSize;i++)
		pInt[i] = 0xFF000000;

	::DeleteDC(hMemDC);
	return TRUE;
}

BOOL duCtrlManager::_DestroyCaret()
{
	if (!IsLayeredWindow())
		return DestroyCaret();
		
	if (m_hBmpCaret)
	{
		::DeleteObject(m_hBmpCaret);
		m_hBmpCaret = NULL;
	}
	return TRUE;
}

BOOL duCtrlManager::_HideCaret()
{
	if (!IsLayeredWindow())
		return HideCaret(m_hWnd);
		
	::KillTimer(m_hWnd, m_nCaretTimer);
	return TRUE;
}

BOOL duCtrlManager::_SetCaretPos(int nX, int nY)
{
	if (!IsLayeredWindow())
		return SetCaretPos(nX, nY);
		
	if (m_hBmpCaret == NULL)
		return FALSE;
	
	m_fCaretDrawed = FALSE;
	m_nCaretPosX = nX;
	m_nCaretPosY = nY;
	OnCaretTimer();
	return TRUE;
}

BOOL duCtrlManager::_ShowCaret()
{
	if (!IsLayeredWindow())
		return ShowCaret(m_hWnd);
	
	::SetTimer(m_hWnd, m_nCaretTimer, GetCaretBlinkTime(), NULL);
	return TRUE;
}

void duCtrlManager::OnCaretTimer()
{
	if (!IsLayeredWindow())
	{
		::KillTimer(m_hWnd, m_nCaretTimer);
		return;
	}

	if (m_fSizeMoving)
		return;
	
	if (m_hBmpUpdate == NULL || m_hBmpCaret == NULL)
		return;
	
	if (!m_fCaretDrawed)
	{
		duRect rcClient;
		::GetClientRect(m_hWnd, &rcClient);
		
		HDC hDC = ::CreateCompatibleDC(NULL);
		BITMAPINFO bmInfo;
		PBYTE pBits = NULL;
		InitBitmapInfo(&bmInfo, rcClient.Width(), rcClient.Height());
		HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmInfo, DIB_RGB_COLORS,(void**)&pBits, 0, 0);
		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hDC, hBitmap);

		HDC hTmpDC = ::CreateCompatibleDC(NULL);
		HBITMAP hOldUpdateBmp = (HBITMAP)::SelectObject(hTmpDC, m_hBmpUpdate);
		
		BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
		::AlphaBlend(hDC, 0, 0, rcClient.Width(), rcClient.Height(), hTmpDC, 0, 0, rcClient.Width(), rcClient.Height(), bf);
		
		::SelectObject(hTmpDC, hOldUpdateBmp);
	
		hOldUpdateBmp = (HBITMAP)::SelectObject(hTmpDC, m_hBmpCaret);
		
		BITMAP bm;
		ZeroMemory(&bm, sizeof(BITMAP));
		
		::GetObject(m_hBmpCaret, sizeof(BITMAP), &bm);
		::AlphaBlend(hDC, m_nCaretPosX, m_nCaretPosY, bm.bmWidth, bm.bmHeight, hTmpDC, 0, 0, bm.bmWidth, bm.bmHeight, bf);
		
		::SelectObject(hTmpDC, hOldUpdateBmp);
		::DeleteDC(hTmpDC);
		
		
		duRect rcWindow;
		::GetWindowRect(m_hWnd, &rcWindow);
		
		BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
		POINT ptDst = {rcWindow.left, rcWindow.top};
		SIZE sizeWnd = {rcWindow.Width(), rcWindow.Height()};
		POINT ptSrc = {0, 0};
		::UpdateLayeredWindow(m_hWnd, NULL, &ptDst, &sizeWnd, hDC, &ptSrc, 0, &blendPixelFunction, ULW_ALPHA);
		
		::SelectObject(hDC, hOldBitmap);
		::DeleteObject(hBitmap);
		::DeleteDC(hDC);
		
		m_fCaretDrawed = TRUE;
	}
	else
	{
		HDC hDC = ::CreateCompatibleDC(NULL);
		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hDC, m_hBmpUpdate);
	
		duRect rcWindow;
		::GetWindowRect(m_hWnd, &rcWindow);
		
		BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
		POINT ptDst = {rcWindow.left, rcWindow.top};
		SIZE sizeWnd = {rcWindow.Width(), rcWindow.Height()};
		POINT ptSrc = {0, 0};
		::UpdateLayeredWindow(m_hWnd, NULL, &ptDst, &sizeWnd, hDC, &ptSrc, 0, &blendPixelFunction, ULW_ALPHA);
		
		::SelectObject(hDC, hOldBitmap);
		::DeleteDC(hDC);
		
		m_fCaretDrawed = FALSE;
	}
}

void duCtrlManager::AddToolTip(LPTSTR lpszToolTip)
{
	if (m_hWndToolTip == NULL)
		return;

	if (lpszToolTip == NULL || *lpszToolTip == 0)
		return;

	if ((::GetWindowLong(m_hWnd, GWL_STYLE) & WS_CHILD) && ::GetParent(m_hWnd) == ::GetDesktopWindow())
	{
		TOOLINFO ti;
		ZeroMemory(&ti, sizeof(TOOLINFO));

		ti.cbSize    = sizeof(TOOLINFO) - sizeof(void*);
		ti.uFlags    = TTF_IDISHWND|TTF_TRACK|TTF_ABSOLUTE;
		ti.hwnd     = m_hWnd;
		ti.uId       = (UINT)0;
		ti.hinst     = ::GetModuleHandle(NULL);
		ti.lpszText  = lpszToolTip;

		::SendMessage(m_hWndToolTip, TTM_ADDTOOL, 0,(LPARAM)&ti);

		POINT ptScreen;
		::GetCursorPos(&ptScreen);
		::SendMessage(m_hWndToolTip, TTM_TRACKPOSITION, 0, (LPARAM)MAKELPARAM(ptScreen.x + 15, ptScreen.y + 15));
		::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE,(WPARAM)TRUE,(LPARAM)&ti);
	}
	else
	{
		TOOLINFO ti;
		ZeroMemory(&ti, sizeof(TOOLINFO));

		ti.cbSize    = sizeof(TOOLINFO) - sizeof(void*);
		ti.uFlags    = 0;
		ti.hwnd     = m_hWnd;
		ti.uId       = (UINT)1;
		ti.hinst     = ::GetModuleHandle(NULL);
		ti.lpszText  = lpszToolTip;
		::GetClientRect(m_hWnd, &ti.rect);

		::SendMessage(m_hWndToolTip, TTM_ADDTOOL, 0,(LPARAM)&ti);

		POINT ptScreen;
		::GetCursorPos(&ptScreen);
		::SendMessage(m_hWndToolTip, TTM_POP,(WPARAM)NULL,(LPARAM)NULL);
	}
}

void duCtrlManager::DelToolTip()
{
	if ((::GetWindowLong(m_hWnd, GWL_STYLE) & WS_CHILD) && ::GetParent(m_hWnd) == ::GetDesktopWindow())
	{
		TOOLINFO ti;
		ZeroMemory(&ti, sizeof(TOOLINFO));

		ti.cbSize    = sizeof(TOOLINFO) - sizeof(void*);
		ti.uFlags    = TTF_IDISHWND|TTF_TRACK|TTF_ABSOLUTE;
		ti.hwnd      = m_hWnd;
		ti.uId       = (UINT)0;
		ti.hinst     = ::GetModuleHandle(NULL);
		ti.lpszText  = NULL;

		::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE,(WPARAM)FALSE,(LPARAM)&ti);
		::SendMessage(m_hWndToolTip, TTM_DELTOOL, 0,(LPARAM)&ti);
	}
	else
	{
		TOOLINFO ti;
		ZeroMemory(&ti, sizeof(TOOLINFO));

		ti.cbSize    = sizeof(TOOLINFO) - sizeof(void*);
		ti.uFlags    = 0;
		ti.hwnd      = m_hWnd;
		ti.uId       = (UINT)1;
		ti.hinst     = ::GetModuleHandle(NULL);
		ti.lpszText  = NULL;

		::SendMessage(m_hWndToolTip, TTM_DELTOOL, 0,(LPARAM)&ti);
	}
}

HBITMAP duCtrlManager::GetBkHBitmap(duPlugin *pPlugin)
{
	if (!Plugin_IsValid(pPlugin))
		return FALSE;

	duRect rcPluginVisible;
	Plugin_GetVisibleRect(pPlugin, &rcPluginVisible);
	if (rcPluginVisible.IsRectEmpty())
		return FALSE;

	duRect rcPlugin;
	Plugin_GetRect(pPlugin, &rcPlugin);

	HDC hMemDC = ::CreateCompatibleDC(NULL);
	BITMAPINFO bmInfo;
	InitBitmapInfo(&bmInfo, rcPlugin.Width(), rcPlugin.Height());
	
	PBYTE pBits = NULL;
	HBITMAP hBitmap = ::CreateDIBSection(hMemDC, &bmInfo, DIB_RGB_COLORS,(void**)&pBits, 0, 0);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);
	
	if (!pPlugin->IsOpaque())
		DrawDownLayer(hMemDC, rcPlugin, rcPluginVisible, pPlugin);

	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);

	return hBitmap;
}

HBITMAP duCtrlManager::GetSnapshot(duPlugin *pPlugin)
{
	if (!Plugin_IsValid(pPlugin))
		return NULL;

	duRect rcPluginVisible;
	Plugin_GetVisibleRect(pPlugin, &rcPluginVisible);
	if (rcPluginVisible.IsRectEmpty())
		return NULL;

	duRect rcPlugin;
	Plugin_GetRect(pPlugin, &rcPlugin);

	HDC hMemDC = ::CreateCompatibleDC(NULL);
	BITMAPINFO bmInfo;
	InitBitmapInfo(&bmInfo, rcPlugin.Width(), rcPlugin.Height());
	
	PBYTE pBits = NULL;
	HBITMAP hBitmap = ::CreateDIBSection(hMemDC, &bmInfo, DIB_RGB_COLORS,(void**)&pBits, 0, 0);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);

	if (pPlugin->IsCached() && !pPlugin->GetCachedBitmap())
	{
		HDC hCachedDC = ::CreateCompatibleDC(NULL);
		HBITMAP hCachedBitmap = pPlugin->GetCachedBitmap();
		HBITMAP hOldCahcedBitmap = (HBITMAP)::SelectObject(hCachedDC, hCachedBitmap);
		BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
		::AlphaBlend(hMemDC, 0, 0, rcPlugin.Width(), rcPlugin.Height(), hCachedDC, 0, 0, rcPlugin.Width(), rcPlugin.Height(), bf);
		
		::SelectObject(hCachedDC, hOldCahcedBitmap);
		::DeleteDC(hCachedDC);
	}
	else
		pPlugin->DrawObject(hMemDC);

	duPlugin *pChildPlugin = pPlugin->GetFirstChild();
	while (pChildPlugin)
	{
		GetChildSnapshot(hMemDC, rcPlugin, rcPluginVisible, pChildPlugin);
		pChildPlugin = pChildPlugin->GetNextSibling();
	}

	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);

	return hBitmap;
}

void duCtrlManager::GetChildSnapshot(HDC hDC, const duRect &rcPlugin, const duRect &rcPluginVisible, duPlugin *pPlugin)
{
	if (hDC == NULL || !Plugin_IsValid(pPlugin))
		return;

	DrawUpLayerPlguin(hDC, pPlugin, rcPlugin, rcPluginVisible);

	duPlugin *pChildPlugin = pPlugin->GetFirstChild();
	while (pChildPlugin)
	{
		GetChildSnapshot(hDC, rcPlugin, rcPluginVisible, pChildPlugin);
		pChildPlugin = pChildPlugin->GetNextSibling();
	}
}

BOOL duCtrlManager::DeletePlugin(duPlugin *pPlugin)
{
	if (pPlugin == m_pZorderList->GetRoot())
		return FALSE;

	duPlugin *pChild = NULL;
	deque<duPlugin *> dqPlugin;
	set<duPlugin *> setPlugin;

	dqPlugin.clear();
	setPlugin.clear();
	dqPlugin.push_back(pPlugin);
	while (!dqPlugin.empty())
	{
		pChild = dqPlugin.at(0);
		dqPlugin.pop_front();

		setPlugin.insert(pChild);
		duPlugin *pParent = pChild->GetParent();
		pParent->OnChildDelete(pChild);
		
		pChild = pChild->GetFirstChild();
		while (pChild)
		{
			dqPlugin.push_back(pChild);
			pChild = pChild->GetNextSibling();
		}
	}
	
	duPlugin *pPre = pPlugin->GetPreSibling();
	duPlugin *pNext = pPlugin->GetNextSibling();
	duPlugin *pParent = pPlugin->GetParent();

	if (pPre)
		pPre->SetNextSibling(pNext);

	if (pNext)
		pNext->SetPreSibling(pPre);

	if (pPlugin == pParent->GetFirstChild())
		pParent->SetFirstChild(pNext);

	if (pPlugin == pParent->GetLastChild())
		pParent->SetLastChild(pPlugin->GetPreSibling());

	set<duPlugin *>::iterator iterDelete = setPlugin.begin();
	for (; iterDelete != setPlugin.end(); iterDelete++)
	{
		pChild = *iterDelete;

		if (m_pFocus == pChild) 
			m_pFocus = NULL;
		if (m_pHot == pChild)
			m_pHot = NULL;
		if (m_pCapture == pChild)
			ReleaseCapture(pChild);

		m_pZorderList->Delete(pChild);
		DeleteName(pChild);
		UnHookWindowMessage(pChild);
		Plugin_DeleteValid(pChild);
		pChild->FinalRelease();
	}

	//_TRACE(_T("Delete [%d] Control\n"), setPlugin.size());
	setPlugin.clear();
	return FALSE;
}

duPlugin *duCtrlManager::Clone(duPlugin *pTemplate, duPlugin *pParent, duPlugin *pPrevious, UINT nSuffix)
{
	TiXmlElement *pXmlTemplate = pTemplate->GetXmlElement();
	if (pXmlTemplate == NULL)
		return NULL;

	duPlugin *pNewPlugin = CreateDirectUIControl(pXmlTemplate->Value());
	if (pNewPlugin == NULL)
		return NULL;

	//pNewPlugin->SetTypeInfo(m_pWinManager->GetTypeInfoByName(pNewPlugin->GetTypeInfoName()));

	pNewPlugin->SetParent(pParent);
	if (!pNewPlugin->OnCreate(pXmlTemplate, this))
	{
		pNewPlugin->FinalRelease();
		return NULL;
	}

	AddSuffix(pNewPlugin, nSuffix);

	if (pPrevious)
	{
		duPlugin *pNext = pPrevious->GetNextSibling();
		
		if (pNext)
			pNext->SetPreSibling(pNewPlugin);
		pPrevious->SetNextSibling(pNewPlugin);
		pNewPlugin->SetPreSibling(pPrevious);
		pNewPlugin->SetNextSibling(pNext);

		if (pPrevious == pParent->GetLastChild())
			pParent->SetLastChild(pNewPlugin);
	}
	else
	{
		duPlugin *pNext = pParent->GetFirstChild();

		if (pNext)
			pNext->SetPreSibling(pNewPlugin);
		else
			pParent->SetLastChild(pNewPlugin);

		pNewPlugin->SetNextSibling(pNext);
		pParent->SetFirstChild(pNewPlugin);
	}

	return pNewPlugin;
}

void duCtrlManager::AddSuffix(duPlugin *pPlugin, UINT nSuffix)
{
	duPlugin *pChild = NULL;
	deque<duPlugin *> dqClone;
	TCHAR szName[MAX_NAME];

	dqClone.clear();
	dqClone.push_back(pPlugin);
	while (!dqClone.empty())
	{
		pChild = dqClone.at(0); 
		dqClone.pop_front();

		_stprintf(szName, _T("%s%d"), pChild->GetName(), nSuffix);
		pChild->SetName(szName);
		//_TRACE(_T("%s\n"), szName);

		m_pZorderList->Add(pChild);
		AddName(pChild);
		Plugin_AddValid(pChild);
		
		pChild = pChild->GetFirstChild();
		while(pChild)
		{
			dqClone.push_back(pChild);
			pChild = pChild->GetNextSibling();
		}
	}

	dqClone.clear();
	dqClone.push_back(pPlugin);
	while (!dqClone.empty())
	{
		pChild = dqClone.at(0); 
		dqClone.pop_front();

		pChild->OnCreate();
		
		pChild = pChild->GetFirstChild();
		while(pChild)
		{
			dqClone.push_back(pChild);
			pChild = pChild->GetNextSibling();
		}
	}
}

// BOOL duCtrlManager::CallScript(duPlugin *pPlugin, UINT uMsg, WPARAM wParam, LPARAM lParam)
// {
// 	if (!Plugin_IsValid(pPlugin) || !m_pZorderList || !m_pWinManager)
// 		return FALSE;
// 
// 	duPlugin *pRoot = m_pZorderList->GetRoot();
// 	if (!Plugin_IsValid(pRoot))
// 		return FALSE;
// 
// 	TCHAR szFunName[MAX_SCRIPT_NAME];
// 	_stprintf(szFunName, _T("%s_%s_%s"), pRoot->GetName(), pPlugin->GetName(), g_messageStruct[uMsg].szMessage);
// 
// 	VARIANTARG varParams[3];
// 
// 	varParams[2].vt       = VT_DISPATCH;
// 	varParams[2].pdispVal = pPlugin;
// 	varParams[1].vt       = g_messageStruct[uMsg].wParamType;
// 	varParams[1].ulVal    = (ULONG)wParam;
// 	varParams[0].vt       = g_messageStruct[uMsg].lParamType;
// 	varParams[0].ulVal    = (ULONG)lParam;
// 
// 	return m_pWinManager->ExecScript(szFunName, 3, varParams);
// }

void duCtrlManager::DrawTrialBitmap(HDC hDC)
{
	duImage *pImage = m_pWinManager->GetTrialBitmap();
	if (pImage == NULL)
		return;
	
	duRect rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	
	int nWidth = pImage->GetWidth();
	int nHeight = pImage->GetHeight();
	DrawNormal(hDC, (rcClient.Width() - nWidth) / 2, 6, nWidth, nHeight, pImage, 0, 0, 255);
}

void duCtrlManager::ResetEditHFont()
{
	int i, j;
	int nLevelCount = m_pZorderList->GetLayerCount();
	for (i = 0;i < nLevelCount; i++)
	{
		int nCtrlCount = m_pZorderList->GetLayerChildCount(i);
		for (j = nCtrlCount - 1;j >= 0; j--)
		{
			duPlugin *pPlugin = m_pZorderList->GetPlugin(i, j);
			if (pPlugin == NULL || !Plugin_IsValid(pPlugin))
				continue;
			if(lstrcmpi(pPlugin->GetType(), _T("edit")) == 0)
			{
				duEdit *pEdit = (duEdit *)pPlugin;
				pEdit->ResetHFont();
			}
		}
	}
}

bool tabless(duPlugin *&s1, duPlugin *&s2)
{
	return s1->GetTabOrder() > s2->GetTabOrder();
}

void duCtrlManager::OnTabKey()
{
	vector<duPlugin *> vtTabPlugin;
	vtTabPlugin.clear();

	duPlugin *pPlugin = NULL;
	int i, j;
	int nLevelCount = m_pZorderList->GetLayerCount();
	for (i = 0;i < nLevelCount; i++)
	{
		int nCtrlCount = m_pZorderList->GetLayerChildCount(i);
		for (j = 0;j < nCtrlCount; j++)
		{
			pPlugin = m_pZorderList->GetPlugin(i, j);
			if (pPlugin == NULL || !Plugin_IsValid(pPlugin) || !Plugin_IsVisible(pPlugin))
				continue;

			if (pPlugin->GetTabOrder() > 0)
				vtTabPlugin.push_back(pPlugin);
		}
	}

	sort(vtTabPlugin.begin(), vtTabPlugin.end(), tabless);
	reverse(vtTabPlugin.begin(), vtTabPlugin.end());

	int nIndex = -1;
	for (i = 0;i < (int)vtTabPlugin.size(); i++)
	{
		if (vtTabPlugin[i] == m_pFocus)
		{
			nIndex = i;
			break;
		}
	}
	
	nIndex++;
	if (nIndex >= vtTabPlugin.size())
		nIndex = 0;

	if (vtTabPlugin.size())
		SetFocusPlugin(vtTabPlugin[nIndex], TRUE);
}

duPlugin *CreateDirectUIControl(LPCTSTR lpszType)
{
	if (lpszType == NULL)
		return NULL;
	
	duPlugin *pIPlugin = NULL;
	
	if (lstrcmpi(lpszType, _T("duwindow")) == 0)
		pIPlugin = new duWindow;
	else if (lstrcmpi(lpszType, _T("button")) == 0)
		pIPlugin = new duButton;
	else if (lstrcmpi(lpszType, _T("layout")) == 0)
		pIPlugin = new duLayout;
	else if (lstrcmpi(lpszType, _T("tabbutton")) == 0)
		pIPlugin = new duTabButton; 
	else if (lstrcmpi(lpszType, _T("checkbox")) == 0)
		pIPlugin = new duCheckBox; 
	else if (lstrcmpi(lpszType, _T("radio")) == 0)
		pIPlugin = new duRadio;

	else if (lstrcmpi(lpszType, _T("static")) == 0)
		pIPlugin = new duStatic;
	else if (lstrcmpi(lpszType, _T("edit")) == 0)
		pIPlugin = new duEdit;
	else if (lstrcmpi(lpszType, _T("scrollbar")) == 0)
		pIPlugin = new duScroll; 
	else if (lstrcmpi(lpszType, _T("scrollview")) == 0)
		pIPlugin = new duScrollView;
	else if (lstrcmpi(lpszType, _T("reportview")) == 0)
		pIPlugin = new duReportView;
	else if (lstrcmpi(lpszType, _T("listbox")) == 0)
		pIPlugin = new duListBox;		
	else if (lstrcmpi(lpszType, _T("combobox")) == 0)
		pIPlugin = new duComboBox; 
	else if (lstrcmpi(lpszType, _T("listboxex")) == 0)
		pIPlugin = new duListBoxEx;
	else if (lstrcmpi(lpszType, _T("impanelnode")) == 0)
		pIPlugin = new duIMPanelNode;
	else if (lstrcmpi(lpszType, _T("scrollcontainer")) == 0)
		pIPlugin = new duScrollContainer;
	else if (lstrcmpi(lpszType, _T("spin")) == 0)
		pIPlugin = new duSpin;
	else if (lstrcmpi(lpszType, _T("hwndobj")) == 0)
		pIPlugin = new duHwndObj;
	else if (lstrcmpi(lpszType, _T("gifctrl")) == 0)
		pIPlugin = new duGifCtrl;
	else if (lstrcmpi(lpszType, _T("ledtimer")) == 0)
		pIPlugin = new duLEDTimer;
	else if (lstrcmpi(lpszType, _T("menubar")) == 0)
		pIPlugin = new duMenuBar;
	else if (lstrcmpi(lpszType, _T("outlookbar")) == 0)
		pIPlugin = new duOutlookBar;
	else if (lstrcmpi(lpszType, _T("progress")) == 0)
		pIPlugin = new duProgress;
	else if (lstrcmpi(lpszType, _T("slider")) == 0)
		pIPlugin = new duSlider;
	else if (lstrcmpi(lpszType, _T("splitter")) == 0)
		pIPlugin = new duSplitter;
	else if (lstrcmpi(lpszType, _T("treectrl")) == 0)
		pIPlugin = new duTreeCtrl;
	else if (lstrcmpi(lpszType, _T("link")) == 0)
		pIPlugin = new duLink;
	else if (lstrcmpi(lpszType, _T("xmlstatic")) == 0)
		pIPlugin = new duXmlStatic;
	else if (lstrcmpi(lpszType, _T("staticex")) == 0)
		pIPlugin = new duStaticEx;
	else if (lstrcmpi(lpszType, _T("menuitem")) == 0)
		pIPlugin = new duMenuItem;
	else if (lstrcmpi(lpszType, _T("page")) == 0)
		pIPlugin = new duPage;
	else if (lstrcmpi(lpszType, _T("headerctrl")) == 0)
		pIPlugin = new duHeaderCtrl;
	else if (lstrcmpi(lpszType, _T("listctrl")) == 0)
		pIPlugin = new duListCtrl;
	else if (lstrcmpi(lpszType, _T("treelistctrl")) == 0)
		pIPlugin = new duTreeListCtrl;
	else if (lstrcmpi(lpszType, _T("treelistitem")) == 0)
		pIPlugin = new duTreeListItem;
	else if (lstrcmpi(lpszType, _T("flash")) == 0)
		pIPlugin = new duFlash;
	else if (lstrcmpi(lpszType, _T("animation")) == 0)
		pIPlugin = new duAnimation;
	else if (lstrcmpi(lpszType, _T("gridlayout")) == 0)
		pIPlugin = new duGridLayout;
	else if (lstrcmpi(lpszType, _T("pie")) == 0)
		pIPlugin = new duPie;
	else if (lstrcmpi(lpszType, _T("menu")) == 0)
		pIPlugin = new duMenu;
	else if (lstrcmpi(lpszType, _T("mledit")) == 0)
		pIPlugin = new duMLEdit;
	else if (lstrcmpi(lpszType, _T("remotectrl")) == 0)
		pIPlugin = new duRemoteCtrl;

	return pIPlugin;
}

LRESULT DirectUIWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	duCtrlManager *pCtrlManager = GetCtrlManager(hWnd);
	if (pCtrlManager == NULL)
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	
	return pCtrlManager->WndProc(hWnd, uMsg, wParam, lParam);
}

extern "C" duPlugin *GetPluginByName(HWND hWnd, LPCTSTR lpszName)
{
	duCtrlManager *pCtrlManager = GetCtrlManager(hWnd);
	if (pCtrlManager == NULL)
		return NULL;

	return pCtrlManager->GetPluginByName(lpszName);
}

extern "C" duPlugin *GetFocusPlugin(HWND hWnd)
{
	duCtrlManager *pCtrlManager = GetCtrlManager(hWnd);
	if (pCtrlManager == NULL)
		return NULL;

	return pCtrlManager->GetFocusPlugin();
}

extern "C" void SetFocusPlugin(HWND hWnd, duPlugin *pPlugin)
{
	duCtrlManager *pCtrlManager = GetCtrlManager(hWnd);
	if (pCtrlManager == NULL)
		return;

	pCtrlManager->SetFocusPlugin(pPlugin, TRUE);
}

extern "C" int SetMaxTipWidth(HWND hWnd, int nWidth)
{
	duCtrlManager *pCtrlManager = GetCtrlManager(hWnd);
	if (pCtrlManager == NULL)
		return -1;

	return ::SendMessage(pCtrlManager->m_hWndToolTip, TTM_SETMAXTIPWIDTH, 0, nWidth);
}
