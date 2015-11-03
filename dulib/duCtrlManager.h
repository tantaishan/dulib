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
#pragma once
#include "duWindowManager.h"
#include "duZorderList.h"
#include "duRedrawLayer.h"

// duCtrlManager
//
//
class duCtrlManager
{
public:
	duCtrlManager(duWindowManager *pWinManager);
	~duCtrlManager();
	
public:
	BOOL Create(HWND hWnd, TiXmlElement *pElement);
	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL DispatchMsg(duPlugin *pPlugin, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnPaint(HDC hDC);
	BOOL RedrawPlugin(duPlugin *pPlugin);
	duPlugin *GetFocusPlugin() { return m_pFocus; }
	void SetFocusPlugin(duPlugin *pPlugin, BOOL fFocus);
	duWindowManager *GetWindowManager() { return m_pWinManager; }
	void SetWindowManager(duWindowManager *pWinManager) { m_pWinManager = pWinManager; }
	HWND GetHwnd() { return m_hWnd; }
	BOOL SetTimer(duPlugin *pPlugin, UINT uTimerId, UINT uElapse);
	BOOL KillTimer(duPlugin *pPlugin, UINT uTimerId);
	duPlugin *GetCapture() { return m_pCapture; }
	duPlugin *SetCapture(duPlugin *pPlugin);
	BOOL ReleaseCapture(duPlugin *pPlugin);
	BOOL HookWindowMessage(duPlugin *pPlugin);
	BOOL UnHookWindowMessage(duPlugin *pPlugin);
	BOOL AddName(duPlugin *pPlugin);
	BOOL DeleteName(duPlugin *pPlugin);
	duPlugin *GetPluginByName(LPCTSTR lpszName);
	BOOL _CreateCaret(int nWidth, int nHeight);
	BOOL _DestroyCaret();
	BOOL _HideCaret();
	BOOL _SetCaretPos(int nX, int nY);
	BOOL _ShowCaret();
	void AddToolTip(LPTSTR lpszToolTip);
	void DelToolTip();
	HBITMAP GetBkHBitmap(duPlugin *pPlugin);
	HBITMAP GetSnapshot(duPlugin *pPlugin);

	BOOL DeletePlugin(duPlugin *pPlugin);
	duPlugin *Clone(duPlugin *pTemplate, duPlugin *pParent, duPlugin *pPrevious, UINT nSuffix);
	//BOOL CallScript(duPlugin *pPlugin, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL IsSizeMoving() { return m_fSizeMoving; }
	void DrawTrialBitmap(HDC hDC);

	void ResetEditHFont();

protected:
	BOOL IsLayeredWindow();
	void DirectDrawPlugin(HDC hDC, duPlugin *pPlugin, duRect &rcPluginVisible);
	duPlugin *GetTimer(UINT uTimerId);
	BOOL IsPluginHook(duPlugin *pPlugin);
	int GetHookCount() { return (int)m_setHookMsg.size(); }
	void OnCaretTimer();
	void ClearGraphics(HDC hDC, duRect &rectClear);
	
	void DrawDownLayer(HDC hDC, const duRect &rcPlugin, const duRect &rcPluginVisible, duPlugin *pPlugin);
	void DrawUpLayer(HDC hDC, const duRect &rcPlugin, const duRect &rcPluginVisible, duPlugin *pPlugin);
	void DrawDownLayerPlguin(HDC hDC, duPlugin *pLayerPlugin, const duRect &rcPlugin, const duRect &rcPluginVisible);
	void DrawUpLayerPlguin(HDC hDC, duPlugin *pLayerPlugin, const duRect &rcPlugin, const duRect &rcPluginVisible);
	
	void GetChildSnapshot(HDC hDC, const duRect &rcPlugin, const duRect &rcPluginVisible, duPlugin *pPlugin);
	void AddSuffix(duPlugin *pPlugin, UINT nSuffix);
	void OnTabKey();

public:
	HWND m_hWndToolTip;
	duPlugin *m_pFocus;
	duPlugin *m_pHot;
	duPlugin *m_pCapture;

protected:
	duZorderList *m_pZorderList;
	duWindowManager *m_pWinManager;
	HWND m_hWnd;
	WNDPROC m_wndProc;
	HBITMAP m_hBmpUpdate;
	HBITMAP m_hBmpCaret;
	UINT m_nCaretTimer;
	int m_nCaretPosX;
	int m_nCaretPosY;
	BOOL m_fCaretDrawed;
	map<UINT, duPlugin *> m_mapTimer;
	set<duPlugin *> m_setHookMsg;
	map<dustring, duPlugin *> m_mapName;
	BOOL m_fSizeMoving;
};

LRESULT DirectUIWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
duPlugin* CreateDirectUIControl(LPCTSTR lpszType);
