//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duPlugin.h
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duProperty.h"
#include "duTypes.h"
#include "duResBase.h"
#include "duFont.h"
#include "duImage.h"
#include "duStyleGroup.h"
#include "duRgn.h"
#include "duCursor.h"

class duWindowManager;
class duCtrlManager;

// duPlugin
//
// 
class duPlugin 
{
public:
	virtual LPCTSTR GetType() { return m_strType; }
	virtual LPCTSTR GetName() { return m_strName; }
	virtual LPCTSTR GetStyle() { return m_strStyle; }
	virtual int GetTextLength() { return (int)m_strText.size(); }
	virtual LPCTSTR GetText() { return m_strText.c_str(); }
	virtual LPCTSTR GetCursor() { return m_strCursor; }
	virtual BOOL IsVisible() { return m_fVisible; }
	virtual BOOL IsFocus() { return m_fFocus; }
	virtual BOOL IsDisable() { return m_fDisable; }
	virtual BOOL IsCached() { return m_fCached; }
	virtual BOOL IsOpaque() { return m_fOpaque; }
	virtual HWND GetHwnd() { return m_hWnd; }
	virtual UINT GetState();
	virtual int GetZorder() { return m_uZorder; }
	virtual void GetRect(LPRECT lpRect) { ::CopyRect(lpRect, m_rect); }
	virtual int GetPosTop() { return m_nTop; }
	virtual int GetPosLeft() { return m_nLeft; }
	virtual int GetPosRight() { return m_nRight; }
	virtual int GetPosBottom() { return m_nBottom; }
	virtual int GetPosHorzAnchor() { return m_nHorzanchor; }
	virtual int GetPosVertAnchor() { return m_nVertanchor; }
	virtual int GetPosWidth() { return m_nWidth; }
	virtual int GetPosHeight() { return m_nHeight; }
	virtual int GetAlpha() { return m_nAlpha; }
	virtual duPlugin* GetParent() { return m_pParent; }
	virtual duPlugin* GetFirstChild() { return m_pFirstChild; }
	virtual duPlugin* GetLastChild() { return m_pLastChild; }
	virtual duPlugin* GetPreSibling() { return m_pPreSibling; }
	virtual duPlugin* GetNextSibling() { return m_pNextSibling; }
	virtual INT_PTR GetParam() { return m_lParam; }
	virtual HBITMAP GetCachedBitmap() { return m_hBitmap; }
	
	virtual void SetName(LPCTSTR lpszName);
	virtual void SetStyle(LPCTSTR lpszStyle);
	virtual void SetText(LPCTSTR lpszText);
	virtual void SetCursor(LPCTSTR lpszCursor);
	virtual void SetVisible(BOOL fVisible) { m_fVisible = fVisible; }
	virtual void SetFocus(BOOL fFocus) { m_fFocus = fFocus; }
	virtual void SetDisable(BOOL fDisable) { m_fDisable = fDisable; }
	virtual void SetCached(BOOL fCached) { m_fCached = fCached; }
	virtual void SetOpaque(BOOL fOpaque) { m_fOpaque = fOpaque; }
	virtual void SetHwnd(HWND hWnd) { m_hWnd = hWnd; }
	virtual void SetState(UINT uState) { m_uState = uState; }
	virtual void SetZorder(int nZorder) { m_uZorder = nZorder; }
	virtual void SetRect(LPRECT lpRect);
	virtual void SetPosTop(int nTop) { m_nTop = nTop; }
	virtual void SetPosLeft(int nLeft) { m_nLeft = nLeft; }
	virtual void SetPosRight(int nRight) { m_nRight = nRight; }
	virtual void SetPosBottom(int nBottom) { m_nBottom = nBottom; }
	virtual void SetPosHorzAnchor(int nHorzanchor) { m_nHorzanchor = nHorzanchor; }
	virtual void SetPosVertAnchor(int nVertanchor) { m_nVertanchor = nVertanchor; }
	virtual void SetPosWidth(int nWidth) { m_nWidth = nWidth; }
	virtual void SetPosHeight(int nHeight) { m_nHeight = nHeight; }
	virtual void SetAlpha(int nAlpha) { m_nAlpha = nAlpha; }
	virtual void SetParent(duPlugin *pParent) { m_pParent = pParent; }
	virtual void SetFirstChild(duPlugin *pFirstChild) { m_pFirstChild = pFirstChild; }
	virtual void SetLastChild(duPlugin *pLastChild) { m_pLastChild = pLastChild; }
	virtual void SetPreSibling(duPlugin *pPreSibling) { m_pPreSibling = pPreSibling; }
	virtual void SetNextSibling(duPlugin *pNextSibling) { m_pNextSibling = pNextSibling; }
	virtual void SetParam(INT_PTR lParam) { m_lParam = lParam; }
	virtual void SetCachedBitmap(HBITMAP hBitmap);
	
public:
	virtual void Resize(LPRECT lpRect/*=NULL*/);
	virtual duPlugin* GetPluginByName(LPCTSTR lpszName);

	virtual void OnMouseIn(POINT pt) {} //{ _TRACE(_T("OnMouseIn %s\n"), GetName()); }
	virtual void OnMouseLeave(POINT pt) {} //{ _TRACE(_T("OnMouseLeave %s\n"), GetName()); }
	virtual void OnMouseLDown(POINT pt) {} //{ _TRACE(_T("OnMouseLDown %s\n"), GetName()); }
	virtual void OnMouseLUp(POINT pt) {} //{ _TRACE(_T("OnMouseLUp %s\n"), GetName()); }
	virtual void OnMouseRDown(POINT pt) {} //{ _TRACE(_T("OnMouseRDown %s\n"), GetName()); }
	virtual void OnMouseRUp(POINT pt) {} //{ _TRACE(_T("OnMouseRUp %s\n"), GetName()); }
	virtual void OnMouseHover(POINT pt) {} //{ _TRACE(_T("OnMouseHOver %s\n"), GetName()); }
	virtual void OnMouseMove(POINT pt) {} //{ _TRACE(_T("OnMouseMove %s\n"), GetName()); }
	virtual void OnMouseDbClick(POINT pt) {} //{ _TRACE(_T("OnMouseDbClick %s\n"), GetName()); }
	virtual void OnMouseWheel(UINT fwKeys, int zDelta, POINT pt) {} //{ _TRACE(_T("OnMouseWheel %s\n"), GetName()); }
	virtual void OnCaptureChanged(duPlugin *newCaptured) {} //{ _TRACE(_T("OnCaptureChanged %s\n"), GetName()); }
	virtual BOOL OnKillFocus(duPlugin *pNewFocus) { return FALSE; } //{ _TRACE(_T("OnKillFocus %s\n"), GetName()); }
	virtual BOOL OnSetFocus(duPlugin *pOldFocus) { return FALSE; } //{ _TRACE(_T("OnSetFocus %s\n"), GetName()); }
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {} //{ _TRACE(_T("OnKeyDown %s\n"), GetName()); }
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {} //{ _TRACE(_T("OnKeyUp %s\n"), GetName()); }
	virtual void OnTimer(UINT nEventId) {}
	virtual void OnChildShow(duPlugin *pChild, BOOL fVisible) {}
	virtual void OnChildResize(duPlugin *pChild) {}
	virtual void OnRedraw() {}
	virtual void OnVScroll(ScrollBarAction sbAction, int nPos) {}
	virtual void OnHScroll(ScrollBarAction sbAction, int nPos) {}
	virtual LRESULT OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return 0; }
	virtual BOOL OnSetCursor();
	virtual void OnChildDelete(duPlugin *pChild) {}

	virtual void RegisterControlProperty() = 0;
	virtual void OnCreate() = 0;
	virtual void DrawObject(HDC hDC) = 0;

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duPlugin"); }

public:
	BOOL OnCreate(TiXmlElement *pElement, duCtrlManager *pCtrlManager);
	BOOL CreateChildPlugin(TiXmlElement *pElement, duCtrlManager *pCtrlManager);
	BOOL CallProperty(TiXmlElement *pElement);
	TiXmlElement *GetXmlElement();
	//BOOL CallScript(duPlugin *pPluginFrom, UINT uMsg, WPARAM wParam, LPARAM lParam);
	int GetPropertyCount();
	LPCTSTR GetPropertyName(int nNameIndex);
	int GetChildCount();	
	BOOL RegisterProperty(LPCTSTR lpszPropName,  duVARIANT_TYPE vt, void *duVariant);
	duResBase* GetResObj(LPCTSTR lpszResName, UINT nResType);
	HBITMAP GetBkHBitmap();
	HBITMAP GetSnapshot();
	LRESULT NotifyUser(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void  SetParam64(UINT64 nlParam) { m_nParam64 = nlParam; }
	UINT64  GetParam64() { return m_nParam64;}
	void SetTabOrder(int nTabOrder) { m_nTabOrder = nTabOrder; }
	int GetTabOrder() { return m_nTabOrder; }

public:
	duPlugin();
	~duPlugin();

protected:
	TCHAR m_strType[MAX_NAME];
	TCHAR m_strName[MAX_NAME];
	TCHAR m_strStyle[MAX_NAME];
	TCHAR m_strCursor[MAX_NAME];
	dustring m_strText;
	BOOL m_fVisible;
	BOOL m_fFocus;
	BOOL m_fDisable;
	BOOL m_fCached;
	BOOL m_fOpaque;
	HWND m_hWnd;
	UINT m_uZorder;
	UINT m_uState;
	int m_nAlpha;
	int m_nLeft;
	int m_nTop;
	int m_nRight;
	int m_nBottom;
	int m_nHorzanchor;
	int m_nVertanchor;
	int m_nWidth;
	int m_nHeight;
	INT_PTR m_lParam;
	INT64 m_nParam64;
	duPlugin *m_pParent;
	duPlugin *m_pFirstChild;
	duPlugin *m_pLastChild;
	duPlugin *m_pNextSibling;
	duPlugin *m_pPreSibling;
	duPropertyGroup *m_pProperty;
	duRect m_rect;
	long m_lRef;
	HBITMAP m_hBitmap;
	int m_nTabOrder;
};

extern "C"
{
	duPlugin* Plugin_SetParent(duPlugin *pPlugin, duPlugin *pParent, LPCTSTR lpszPre);
	BOOL Plugin_IsValid(duPlugin *pPlugin);
	void Plugin_AddValid(duPlugin *pPlugin);
	void Plugin_DeleteValid(duPlugin *pPlugin);
	BOOL Plugin_IsVisible(duPlugin *pPlugin);
	BOOL Plugin_GetVisibleRect(duPlugin *pPlugin, LPRECT lpRect);
	BOOL Plugin_Redraw(duPlugin *pPlugin, BOOL fNotify);
	BOOL Plugin_SetCached(duPlugin *pPlugin, BOOL fCached);
	BOOL Plugin_GetType(duPlugin *pPlugin, LPTSTR lpString, int nMaxCount);
	BOOL Plugin_GetName(duPlugin *pPlugin, LPTSTR lpString, int nMaxCount);
	BOOL Plugin_GetStyle(duPlugin *pPlugin, LPTSTR lpString, int nMaxCount);
	BOOL Plugin_GetCursor(duPlugin *pPlugin, LPTSTR lpString, int nMaxCount);
	int Wlugin_GetTextLength(duPlugin *pPlugin);
	BOOL Plugin_GetText(duPlugin *pPlugin, LPTSTR lpString, int nMaxCount);
	void Plugin_GetRect(duPlugin *pPlugin, LPRECT lpRect);
	void Plugin_SetFocus(duPlugin *pPlugin, BOOL fFocus);
	void Plugin_SetVisible(duPlugin *pPlugin, BOOL fVisible);
	void Plugin_Resize(duPlugin *pPlugin, LPRECT lpRect);
	UINT Plugin_GetState(duPlugin *pPlugin);
	void Plugin_SetState(duPlugin *pPlugin, UINT uState);
	BOOL Plugin_SetTimer(duPlugin *pPlugin, UINT uTimerId, UINT uElapse);
	BOOL Plugin_KillTimer(duPlugin *pPlugin, UINT uTimerId);
	duPlugin* Plugin_GetCapture(duPlugin *pPlugin);
	duPlugin* Plugin_SetCapture(duPlugin *pPlugin);
	BOOL Plugin_ReleaseCapture(duPlugin *pPlugin);
	BOOL Plugin_HookWindowMessage(duPlugin *pPlugin);
	BOOL Plugin_UnHookWindowMessage(duPlugin *pPlugin);
	BOOL Plugin_Delete(duPlugin *pPlugin);
	duPlugin* Plugin_Clone(duPlugin *pTemplate, duPlugin *pParent, duPlugin *pPrevious, UINT nSuffix);
	BOOL Plugin_SetDisable(duPlugin *pPlugin, BOOL fDisable);
	void DrawByStyle(duPlugin *pPlugin, LPCTSTR lpszStyle, HDC hDC, LPRECT lpDstRect, UINT uState, LPCTSTR lpszText, int nAlpha);
	UINT64 Plugin_GetParam64(duPlugin *pPlugin);
	void Plugin_SetParam64(duPlugin *pPlugin, UINT64 nlParam);
	void Plugin_SetTabOrder(duPlugin *pPlugin, int nTabOrder);
	int Plugin_GetTabOrder(duPlugin *pPlugin);
}
