//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       MenuPopup.h
//
//  History:    Nov-10-2009   Eric Qian  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"

class duMenuItem;
class duMenu;

typedef struct tagDIRECTUIMENU
{
	HWND hWndOwner;
	duWindowManager *pWinManager;
	TCHAR szWinName[256];
	TCHAR szMenuName[256];
	LPRECT prcExclude;
	duMenuItem *pOwnerMenuItem;
	duMenu *pParentMenu;
}DIRECTUIMENU;

static TCHAR g_szMenuClassName[] = _T("dulibMenuClass");

#define RECT_ONLEFT     0
#define RECT_ONTOP      1
#define RECT_ONRIGHT    2
#define RECT_ONBOTTOM   3
#define RECT_ORG        4

ATOM WINAPI RegisterMenuClass();
LRESULT WINAPI Menu_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI Plugin_TrackPopupMenuEx(int x, int y, UINT uFlags, DIRECTUIMENU *pMenuStruct, BOOL fTopMenu);
extern "C" BOOL WINAPI Plugin_TrackPopupMenu(HWND hWndOwner, duWindowManager *pWinManager, LPCTSTR lpszWinName, LPCTSTR lpszMenuName, int x, int y, UINT uFlags);
BOOL WINAPI IsMenuWindow(HWND hWnd);
duMenu *WINAPI GetRootMenu(HWND hWnd);
void WINAPI MenuClientToScreen(HWND hWnd, LPRECT lpRect);
duMenu *WINAPI MenuFromPoint(duMenu *pMenu, POINT ptScreen);
BOOL WINAPI MyDestroyMenu(duMenu *pMenu);
void WINAPI FindMenuBestPos(int x, int y, int cx, int cy, LPRECT prcExclude, UINT wFlags, MONITORINFO *pMonitor, LPPOINT ppt);
BOOL WINAPI TryRect(UINT wRect, int x, int y, int cx, int cy, LPRECT prcExclude, LPPOINT ppt, MONITORINFO *pMonitor);

void WINAPI MenuItemMouseLeave(duMenu *pMenu);
void WINAPI MenuItemMouseMove(duMenuItem *pMenuItem);
BOOL WINAPI HandleMenuMessage(HWND hWndMenu, LPMSG lpMsg);
BOOL WINAPI MenuTrackPopup(HWND hWndMenu, HWND hWndOldActive);
