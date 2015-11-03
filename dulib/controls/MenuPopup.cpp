//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       MenuPopup.cpp
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#include "MenuPopup.h"
#include "duMenuItem.h"
#include "duMenu.h"

extern "C" duPlugin *GetPluginByName(HWND hWnd, LPCTSTR lpszName);
extern "C" BOOL WindowManager_Attach(duWindowManager *pWinManager, HWND hWnd, LPCTSTR lpWindowName);
HWND g_hWndTopMenu = NULL;

//
// 注册菜单窗口类
//
ATOM RegisterMenuClass()
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style = CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)Menu_WndProc;
	wcex.cbClsExtra	= 0;
	wcex.cbWndExtra	= 0;
	wcex.hInstance = (HINSTANCE)::GetModuleHandle(NULL);
	wcex.hIcon = NULL;
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = g_szMenuClassName;
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex);
}

//
// 菜单窗口过程
//
LRESULT Menu_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DIRECTUIMENU *pMenuStruct = (DIRECTUIMENU *)GetWindowLong(hWnd, GWL_USERDATA);
	if (pMenuStruct == NULL && uMsg == WM_NCCREATE)
	{
		pMenuStruct = (DIRECTUIMENU *)((LPCREATESTRUCT)lParam)->lpCreateParams;
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)pMenuStruct);
	}
	
	switch (uMsg)
	{
	case WM_CREATE:
	{
		WindowManager_Attach(pMenuStruct->pWinManager, hWnd, pMenuStruct->szWinName);
		duMenu *pMenu = (duMenu *)GetPluginByName(hWnd, pMenuStruct->szMenuName);
		if (pMenu == NULL)
			return FALSE;
		
		pMenu->SetMenuHandle(hWnd);
		pMenu->SetOwnerHwnd(pMenuStruct->hWndOwner);
		pMenu->SetParentMenu(pMenuStruct->pParentMenu);
		
		if (pMenuStruct->pOwnerMenuItem)
			pMenuStruct->pOwnerMenuItem->SetPopupMenu(pMenu);

		pMenu->NotifyMenuUser(DUM_MENUINIT, NULL, NULL);

		duPlugin *pPlugin = pMenu->GetFirstChild();
		while (pPlugin)
		{
			if (lstrcmpi(pPlugin->GetType(), _T("MenuItem")) == 0)
				((duMenuItem *)pPlugin)->NotifyMenuUser(DUM_MENUITEMINIT, NULL, NULL);

			pPlugin = pPlugin->GetNextSibling();
		}

		Plugin_Redraw(GetPluginByName(hWnd, pMenuStruct->szWinName), TRUE);
	}
	break;

	case WM_MOUSEACTIVATE:
		return MA_NOACTIVATE;

	case WM_DESTROY:
	{
	}
	break;
	}

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//
// 判断窗口是否为菜单
//
BOOL IsMenuWindow(HWND hWnd)
{
	TCHAR szClassName[256];
	ZeroMemory(szClassName, 256 * sizeof(TCHAR));
	GetClassName(hWnd, szClassName, 256);

	return (lstrcmpi(szClassName, g_szMenuClassName) == 0) ? TRUE : FALSE;
}

//
// 获得最顶层菜单
//
duMenu *GetRootMenu(HWND hWnd)
{
	if (!IsMenuWindow(hWnd))
		return NULL;

	DIRECTUIMENU *pMenuStruct = (DIRECTUIMENU *)GetWindowLong(hWnd, GWL_USERDATA);
	duMenu *pMenu = (duMenu *)GetPluginByName(hWnd, pMenuStruct->szMenuName);
	if (pMenu == NULL)
		return NULL;

	while (pMenu->GetParentMenu())
		pMenu = pMenu->GetParentMenu();

	return pMenu;
}

//
// lpRect坐标转换为屏幕坐标
//
void MenuClientToScreen(HWND hWnd, LPRECT lpRect)
{
	LPPOINT lpPoint = (LPPOINT)lpRect;
	ClientToScreen(hWnd, lpPoint);
	lpPoint++;
	ClientToScreen(hWnd, lpPoint);
}

//
// 屏幕坐标点返回菜单对象
//
duMenu *MenuFromPoint(duMenu *pMenu, POINT ptScreen)
{
	if (pMenu == NULL)
		return NULL;

	if (pMenu->GetNextPopupMenu())
	{
		duMenu *pSubMenu = MenuFromPoint(pMenu->GetNextPopupMenu(), ptScreen);
		if (pSubMenu)
			return pSubMenu;
	}
	
	duRect rcMenu;
	pMenu->GetRect(&rcMenu);
	MenuClientToScreen(pMenu->GetHwnd(), &rcMenu);
	if (rcMenu.PtInRect(ptScreen))
		return pMenu;

	return NULL;
}

HWND MenuWindowFromPoint(duMenu *pMenu, POINT ptScreen)
{
	if (pMenu == NULL)
		return NULL;

	if (pMenu->GetNextPopupMenu())
	{
		HWND hWnd = MenuWindowFromPoint(pMenu->GetNextPopupMenu(), ptScreen);
		if (hWnd)
			return hWnd;
	}
	
	duRect rcMenu;
	GetWindowRect(pMenu->GetHwnd(), &rcMenu);
	if (rcMenu.PtInRect(ptScreen))
		return pMenu->GetHwnd();

	return NULL;
}

//
// 销毁菜单及其子菜单
//
BOOL MyDestroyMenu(duMenu *pMenu)
{
	if (pMenu == NULL)
		return FALSE;

	if (pMenu->GetNextPopupMenu())
		MyDestroyMenu(pMenu->GetNextPopupMenu());
	
	DestroyWindow(pMenu->GetHwnd());
	return TRUE;
}

//
// 找到合适的菜单位置
//
void FindMenuBestPos(int x, int y, int cx, int cy, LPRECT prcExclude, UINT wFlags, MONITORINFO *pMonitor, LPPOINT ppt)
{
	int iRect;
    int iT;
    UINT awRect[4];
    POINT ptT;
    RECT rcExclude;

    if (prcExclude!=NULL)
	{
        CopyRect(&rcExclude, prcExclude);
		IntersectRect(&rcExclude, &rcExclude, &pMonitor->rcMonitor);
    }
	else 
        SetRect(&rcExclude, x, y, x, y);

    if (x + cx > pMonitor->rcMonitor.right)
	{
        if ((wFlags & TPM_CENTERALIGN) || (x - cx < pMonitor->rcMonitor.left) || (x >= pMonitor->rcMonitor.right))
            x = pMonitor->rcMonitor.right - cx;
		else 
            x -= cx;
    }

    if (x < pMonitor->rcMonitor.left) 
	{
        x += cx;
        if ((wFlags & TPM_CENTERALIGN) || (x >= pMonitor->rcMonitor.right) || (x < pMonitor->rcMonitor.left)) 
            x = pMonitor->rcMonitor.left;
    }

    if (y + cy > pMonitor->rcMonitor.bottom)
	{
        if ((wFlags & TPM_VCENTERALIGN) || (y - cy < pMonitor->rcMonitor.top) || (y >= pMonitor->rcMonitor.bottom))
            y = pMonitor->rcMonitor.bottom - cy;
		else 
            y -= cy;
    }

    if (y < pMonitor->rcMonitor.top)
	{
        y += cy;
        if ((wFlags & TPM_VCENTERALIGN) || (y >= pMonitor->rcMonitor.bottom) || (y < pMonitor->rcMonitor.top))
            y = pMonitor->rcMonitor.top;
    }

    if (TryRect(RECT_ORG, x, y, cx, cy, &rcExclude, &ptT, pMonitor))
        goto FindMenuBestPos_end;

    iRect = (wFlags & TPM_VERTICAL) ? 2 : 0;
    iT = (wFlags & TPM_RIGHTALIGN) ? 0 : 2;

    awRect[0 + iRect] = RECT_ONLEFT + iT;
    awRect[1 + iRect] = RECT_ONRIGHT - iT;

    iT = (wFlags & TPM_BOTTOMALIGN) ? 0 : 2;

    awRect[2 - iRect] = RECT_ONTOP + iT;
    awRect[3 - iRect] = RECT_ONBOTTOM - iT;

    for (iRect = 0; iRect < 4; iRect++)
	{
        if (TryRect(awRect[iRect], x, y, cx, cy, &rcExclude, &ptT, pMonitor))
		{
            x = ptT.x;
            y = ptT.y;
            break;
        }
    }

FindMenuBestPos_end:
    ppt->x = x;
	ppt->y = y;
}

BOOL TryRect(UINT wRect, int x, int y, int cx, int cy, LPRECT prcExclude, LPPOINT ppt, MONITORINFO *pMonitor)
{
    RECT rcTry;

    switch (wRect)
	{
        case RECT_ONRIGHT:
            x = prcExclude->right;
            if (x + cx > pMonitor->rcMonitor.right)
                return FALSE;
            break;

        case RECT_ONBOTTOM:
            y = prcExclude->bottom;
            if (y + cy > pMonitor->rcMonitor.bottom)
                return FALSE;
            break;

        case RECT_ONLEFT:
            x = prcExclude->left - cx;
            if (x < pMonitor->rcMonitor.left)
                return FALSE;
            break;

        case RECT_ONTOP:
            y = prcExclude->top - cy;
            if (y < pMonitor->rcMonitor.top)
                return FALSE;
            break;
    }

    ppt->x = x;
    ppt->y = y;

    rcTry.left      = x;
    rcTry.top       = y;
    rcTry.right     = x + cx;
    rcTry.bottom    = y + cy;

    return (!::IntersectRect(&rcTry, &rcTry, prcExclude));
}

BOOL Plugin_TrackPopupMenuEx(int x, int y, UINT uFlags, DIRECTUIMENU *pMenuStruct, BOOL fTopMenu)
{
	HINSTANCE hInstance;
	HWND hWndMenu;
	MONITORINFO mii;
	HMONITOR hMonitor;
	duRect rcWindow;
	POINT pt;

	hInstance = (HINSTANCE)::GetWindowLong(pMenuStruct->hWndOwner, GWL_HINSTANCE);
	hWndMenu = ::CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST, g_szMenuClassName, NULL, WS_POPUP, 0, 0, 0, 0, pMenuStruct->hWndOwner, NULL, hInstance, pMenuStruct);
	if (hWndMenu == NULL)
		return FALSE;

	// allow user to resize menu
	duMenu *pMenu = (duMenu *)GetPluginByName(hWndMenu, pMenuStruct->szMenuName);
	if (pMenu == NULL)
		return FALSE;

	if (fTopMenu)
	{
		if (::IsWindow(g_hWndTopMenu))
		{
			DIRECTUIMENU *pDirectUIMenu = (DIRECTUIMENU *)GetWindowLong(g_hWndTopMenu, GWL_USERDATA);
			if (pDirectUIMenu)
			{
				duMenu *pMenu = (duMenu *)GetPluginByName(g_hWndTopMenu, pDirectUIMenu->szMenuName);
				MyDestroyMenu(pMenu);
			}

			g_hWndTopMenu = NULL;
		}

		g_hWndTopMenu = pMenu->GetHwnd();
	}

	LRESULT lRes = pMenu->NotifyMenuUser(DUM_MENUSIZE, NULL, NULL);
	if (lRes != 0)
	{
		int cx = (int)(short)LOWORD(lRes);
		int cy = (int)(short)HIWORD(lRes);
		SetWindowPos(hWndMenu, NULL, 0, 0, cx, cy, SWP_NOMOVE|SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOACTIVATE);
	}

	mii.cbSize = sizeof(MONITORINFO);
	pt.x = x;
	pt.y = y;
	hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
	GetMonitorInfo(hMonitor, &mii);
	
	GetWindowRect(hWndMenu, &rcWindow);
	FindMenuBestPos(x, y, rcWindow.Width(), rcWindow.Height(), pMenuStruct->prcExclude, uFlags,  &mii, &pt);
	SetWindowPos(hWndMenu, NULL, pt.x, pt.y, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOOWNERZORDER);

	return MenuTrackPopup(hWndMenu, GetActiveWindow());
}

extern "C" BOOL Plugin_TrackPopupMenu(HWND hWndOwner, duWindowManager *pWinManager, LPCTSTR lpszWinName, LPCTSTR lpszMenuName, int x, int y, UINT uFlags)
{
	DIRECTUIMENU menuStruct;
	
	menuStruct.hWndOwner = hWndOwner;
	menuStruct.pWinManager = pWinManager;
	menuStruct.prcExclude = NULL;
	menuStruct.pOwnerMenuItem = NULL;
	menuStruct.pParentMenu = NULL;
	
	_tcsncpy(menuStruct.szWinName, lpszWinName, 256);
	_tcsncpy(menuStruct.szMenuName, lpszMenuName, 256);
	
	return Plugin_TrackPopupMenuEx(x, y, uFlags, &menuStruct, TRUE);
}

extern "C" void Plugin_EndMenu()
{
	if (g_hWndTopMenu == NULL)
		return;

	DIRECTUIMENU *pDirectUIMenu = (DIRECTUIMENU *)GetWindowLong(g_hWndTopMenu, GWL_USERDATA);
	if (pDirectUIMenu)
	{
		duMenu *pMenu = (duMenu *)GetPluginByName(g_hWndTopMenu, pDirectUIMenu->szMenuName);
		MyDestroyMenu(pMenu);
	}
}

//
// 菜单消息循环
// 
BOOL MenuTrackPopup(HWND hWndMenu, HWND hWndOldActive)
{
	if (!::IsWindow(hWndMenu))
		return FALSE;

	MSG  msg;
	HWND hwndActive = NULL;
	::ShowWindow(hWndMenu, SW_SHOWNOACTIVATE);

	while (::GetMessage(&msg, NULL, 0, 0) && IsWindow(hWndMenu))
	{
		hwndActive = ::GetActiveWindow();
		if (hwndActive != hWndOldActive)
		{
			//_TRACE(_T("hwndActive exit 0 %x\n"), hWndMenu);
			break;
		}

		if (!HandleMenuMessage(hWndMenu, &msg))
		{
			if (!IsWindow(hWndMenu))
				break;

			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		if (!IsWindow(hWndMenu))
			break;

		hwndActive = ::GetActiveWindow();
		if (hwndActive != hWndOldActive)
		{
			//_TRACE(_T("hwndActive exit 1 %x\n"), hWndMenu);
			break;
		}
	}

	if (::IsWindow(hWndMenu))
		::DestroyWindow(hWndMenu);

	if (msg.message == WM_QUIT) {
		PostQuitMessage((int)msg.wParam);
	}
	
	return TRUE;
}

//
// 处理菜单消息
//
BOOL HandleMenuMessage(HWND hWndMenu, LPMSG lpMsg)
{
	switch (lpMsg->message)
	{
	case WM_MOUSEMOVE:
		{
			POINT pt;
			pt.x = (int)(short)LOWORD(lpMsg->lParam);
			pt.y = (int)(short)HIWORD(lpMsg->lParam);

			duMenu *pTopMenu = GetRootMenu(hWndMenu);
			ClientToScreen(lpMsg->hwnd, &pt);
			duMenu *pMenu = MenuFromPoint(pTopMenu, pt);
			if (pMenu == NULL)
			{
				MenuItemMouseLeave(pTopMenu);
				return FALSE;
			}

			ScreenToClient(pMenu->GetHwnd(), &pt);
			duMenuItem *pMenuItem = pMenu->MenuItemFromClientPoint(&pt);
			if (pMenuItem == NULL)
			{
				MenuItemMouseLeave(pTopMenu);
				return FALSE;
			}

			MenuItemMouseMove(pMenuItem);
		}
		break;

	case WM_MOUSELEAVE:
		{
			if (IsMenuWindow(lpMsg->hwnd))
			{
				DIRECTUIMENU *pMenuStruct = (DIRECTUIMENU *)GetWindowLong(lpMsg->hwnd, GWL_USERDATA);
				if (pMenuStruct)
				{
					duMenu *pMenu = (duMenu *)GetPluginByName(lpMsg->hwnd, pMenuStruct->szMenuName);
					MenuItemMouseLeave(pMenu);
				}
			}
		}
		break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
		{
			POINT pt;
			pt.x = (int)(short)LOWORD(lpMsg->lParam);
			pt.y = (int)(short)HIWORD(lpMsg->lParam);

			duMenu *pTopMenu = GetRootMenu(hWndMenu);
			ClientToScreen(lpMsg->hwnd, &pt);
			HWND hWndHot = MenuWindowFromPoint(pTopMenu, pt);
			if (hWndHot == NULL)
				MyDestroyMenu(pTopMenu);
			else
			{
				pt.x = (short)LOWORD(lpMsg->lParam);
				pt.y = (short)HIWORD(lpMsg->lParam);
				::MapWindowPoints(lpMsg->hwnd, hWndHot, &pt, 1);
				lpMsg->lParam = MAKELPARAM(pt.x, pt.y);
				lpMsg->hwnd = hWndHot;
			}
		}
		break;

	case WM_NCLBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCLBUTTONDBLCLK:
	case WM_NCRBUTTONDOWN:
	case WM_NCRBUTTONUP:
	case WM_NCRBUTTONDBLCLK:
	case WM_NCMBUTTONDOWN:
	case WM_NCMBUTTONUP:
	case WM_NCMBUTTONDBLCLK:
		{
			duMenu *pTopMenu = GetRootMenu(hWndMenu);
			if (pTopMenu)
			{
				lpMsg->hwnd = pTopMenu->GetHwnd();
				MyDestroyMenu(pTopMenu);
			}
		}
		break;

	case WM_NCMOUSEMOVE:
		{
		}
		return TRUE;

	default:
		return FALSE;
	}
	
	return FALSE;
}

//
// 处理鼠标离开菜单事件
//
void MenuItemMouseLeave(duMenu *pMenu)
{
	if (pMenu == NULL)
		return;

	if (pMenu->GetNextPopupMenu())
		MenuItemMouseLeave(pMenu->GetNextPopupMenu());

	duPlugin *pPlugin = pMenu->GetFirstChild();
	while (pPlugin)
	{
		if (pPlugin->IsVisible() && lstrcmpi(pPlugin->GetType(), _T("MenuItem")) == 0)
		{
			duMenuItem *pMenuItem = (duMenuItem *)pPlugin;
			if (pMenuItem->GetState() == DU_STATE_OVER && !pMenuItem->GetPopupMenu())
			{
				pMenuItem->SetState(DU_STATE_NORMAL);
				pMenuItem->SetPress(FALSE);
				Plugin_Redraw(pMenuItem, TRUE);
				//_TRACE(_T("MouseLeave %s\n"), pMenuItem->GetText());
				break;
			}
		}

		pPlugin = pPlugin->GetNextSibling();
	}
}

//
// 处理WM_MOUSEMOVE消息
//
void MenuItemMouseMove(duMenuItem *pMenuItem)
{
	if (pMenuItem == NULL)
		return;

	if (pMenuItem->GetState() == DU_STATE_OVER)
		return;

	duMenu *pMenu = (duMenu *)pMenuItem->GetParent();
	duMenuItem *pHotMenuItem = pMenu->GetHotMenu();
	if (pHotMenuItem != pMenuItem)
	{
		if (pHotMenuItem)
		{
			pHotMenuItem->SetState(DU_STATE_NORMAL);
			pHotMenuItem->SetPress(FALSE);
			if (pHotMenuItem->IsPopupItem() && pHotMenuItem->GetPopupMenu())
			{
				duMenu *pPopupMenu = pHotMenuItem->GetPopupMenu();
				pHotMenuItem->SetPopupMenu(NULL);
				MyDestroyMenu(pPopupMenu);
			}

			Plugin_Redraw(pHotMenuItem, TRUE);
		}

		pMenuItem->SetState(DU_STATE_OVER);
		Plugin_Redraw(pMenuItem, TRUE);

		if (pMenuItem->IsPopupItem())
		{
			duMenu *pMenu = (duMenu *)pMenuItem->GetParent();
			HWND hWndOwner = pMenu->GetOwnerHwnd();

			
			DIRECTUIMENU *pMenuStruct = (DIRECTUIMENU *)GetWindowLong(pMenuItem->GetHwnd(), GWL_USERDATA);

			duRect rcMenuItem;
			pMenuItem->GetRect(&rcMenuItem);

			POINT ptPopup;
			ptPopup.x = rcMenuItem.right;
			ptPopup.y = rcMenuItem.top;
			ClientToScreen(pMenuItem->GetHwnd(), &ptPopup);

			duRect rcWindow;
			GetWindowRect(pMenuItem->GetHwnd(), &rcWindow);

			
			DIRECTUIMENU menuStruct;
			menuStruct.hWndOwner = hWndOwner;
			menuStruct.pOwnerMenuItem = pMenuItem;
			menuStruct.pParentMenu = (duMenu *)pMenuItem->GetParent();
			menuStruct.prcExclude = &rcWindow;
			menuStruct.pWinManager = pMenuStruct->pWinManager;
			_tcsncpy(menuStruct.szMenuName, pMenuItem->GetChildMenu(), 256);
			_tcsncpy(menuStruct.szWinName, pMenuItem->GetPopupWindow(), 256);

			Plugin_TrackPopupMenuEx(ptPopup.x, ptPopup.y, TPM_LEFTALIGN|TPM_TOPALIGN, &menuStruct, FALSE);
		}
	}
}


