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

ATOM RegisterMenuClass();
LRESULT Menu_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL Plugin_TrackPopupMenuEx(int x, int y, UINT uFlags, DIRECTUIMENU *pMenuStruct, BOOL fTopMenu);
extern "C" BOOL Plugin_TrackPopupMenu(HWND hWndOwner, duWindowManager *pWinManager, LPCTSTR lpszWinName, LPCTSTR lpszMenuName, int x, int y, UINT uFlags);
BOOL IsMenuWindow(HWND hWnd);
duMenu *GetRootMenu(HWND hWnd);
void MenuClientToScreen(HWND hWnd, LPRECT lpRect);
duMenu *MenuFromPoint(duMenu *pMenu, POINT ptScreen);
BOOL MyDestroyMenu(duMenu *pMenu);
void FindMenuBestPos(int x, int y, int cx, int cy, LPRECT prcExclude, UINT wFlags, MONITORINFO *pMonitor, LPPOINT ppt);
BOOL TryRect(UINT wRect, int x, int y, int cx, int cy, LPRECT prcExclude, LPPOINT ppt, MONITORINFO *pMonitor);

void MenuItemMouseLeave(duMenu *pMenu);
void MenuItemMouseMove(duMenuItem *pMenuItem);
BOOL HandleMenuMessage(HWND hWndMenu, LPMSG lpMsg);
BOOL MenuTrackPopup(HWND hWndMenu, HWND hWndOldActive);
