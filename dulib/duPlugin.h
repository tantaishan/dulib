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
#include "duObject.h"
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
class duPlugin : public duObject
{
public:
	virtual LPCTSTR WINAPI GetType() { return m_strType; }
	virtual LPCTSTR WINAPI GetName() { return m_strName; }
	virtual LPCTSTR WINAPI GetStyle() { return m_strStyle; }
	virtual int WINAPI GetTextLength() { return (int)m_strText.size(); }
	virtual LPCTSTR WINAPI GetText() { return m_strText.c_str(); }
	virtual LPCTSTR WINAPI GetCursor() { return m_strCursor; }
	virtual BOOL WINAPI IsVisible() { return m_fVisible; }
	virtual BOOL WINAPI IsFocus() { return m_fFocus; }
	virtual BOOL WINAPI IsDisable() { return m_fDisable; }
	virtual BOOL WINAPI IsCached() { return m_fCached; }
	virtual BOOL WINAPI IsOpaque() { return m_fOpaque; }
	virtual HWND WINAPI GetHwnd() { return m_hWnd; }
	virtual UINT WINAPI GetState();
	virtual int WINAPI GetZorder() { return m_uZorder; }
	virtual void WINAPI GetRect(LPRECT lpRect) { ::CopyRect(lpRect, m_rect); }
	virtual int WINAPI GetPosTop() { return m_nTop; }
	virtual int WINAPI GetPosLeft() { return m_nLeft; }
	virtual int WINAPI GetPosRight() { return m_nRight; }
	virtual int WINAPI GetPosBottom() { return m_nBottom; }
	virtual int WINAPI GetPosHorzAnchor() { return m_nHorzanchor; }
	virtual int WINAPI GetPosVertAnchor() { return m_nVertanchor; }
	virtual int WINAPI GetPosWidth() { return m_nWidth; }
	virtual int WINAPI GetPosHeight() { return m_nHeight; }
	virtual int WINAPI GetAlpha() { return m_nAlpha; }
	virtual duPlugin *WINAPI GetParent() { return m_pParent; }
	virtual duPlugin *WINAPI GetFirstChild() { return m_pFirstChild; }
	virtual duPlugin *WINAPI GetLastChild() { return m_pLastChild; }
	virtual duPlugin *WINAPI GetPreSibling() { return m_pPreSibling; }
	virtual duPlugin *WINAPI GetNextSibling() { return m_pNextSibling; }
	virtual INT_PTR WINAPI GetParam() { return m_lParam; }
	virtual HBITMAP WINAPI GetCachedBitmap() { return m_hBitmap; }
	
	virtual void WINAPI SetName(LPCTSTR lpszName);
	virtual void WINAPI SetStyle(LPCTSTR lpszStyle);
	virtual void WINAPI SetText(LPCTSTR lpszText);
	virtual void WINAPI SetCursor(LPCTSTR lpszCursor);
	virtual void WINAPI SetVisible(BOOL fVisible) { m_fVisible = fVisible; }
	virtual void WINAPI SetFocus(BOOL fFocus) { m_fFocus = fFocus; }
	virtual void WINAPI SetDisable(BOOL fDisable) { m_fDisable = fDisable; }
	virtual void WINAPI SetCached(BOOL fCached) { m_fCached = fCached; }
	virtual void WINAPI SetOpaque(BOOL fOpaque) { m_fOpaque = fOpaque; }
	virtual void WINAPI SetHwnd(HWND hWnd) { m_hWnd = hWnd; }
	virtual void WINAPI SetState(UINT uState) { m_uState = uState; }
	virtual void WINAPI SetZorder(int nZorder) { m_uZorder = nZorder; }
	virtual void WINAPI SetRect(LPRECT lpRect);
	virtual void WINAPI SetPosTop(int nTop) { m_nTop = nTop; }
	virtual void WINAPI SetPosLeft(int nLeft) { m_nLeft = nLeft; }
	virtual void WINAPI SetPosRight(int nRight) { m_nRight = nRight; }
	virtual void WINAPI SetPosBottom(int nBottom) { m_nBottom = nBottom; }
	virtual void WINAPI SetPosHorzAnchor(int nHorzanchor) { m_nHorzanchor = nHorzanchor; }
	virtual void WINAPI SetPosVertAnchor(int nVertanchor) { m_nVertanchor = nVertanchor; }
	virtual void WINAPI SetPosWidth(int nWidth) { m_nWidth = nWidth; }
	virtual void WINAPI SetPosHeight(int nHeight) { m_nHeight = nHeight; }
	virtual void WINAPI SetAlpha(int nAlpha) { m_nAlpha = nAlpha; }
	virtual void WINAPI SetParent(duPlugin *pParent) { m_pParent = pParent; }
	virtual void WINAPI SetFirstChild(duPlugin *pFirstChild) { m_pFirstChild = pFirstChild; }
	virtual void WINAPI SetLastChild(duPlugin *pLastChild) { m_pLastChild = pLastChild; }
	virtual void WINAPI SetPreSibling(duPlugin *pPreSibling) { m_pPreSibling = pPreSibling; }
	virtual void WINAPI SetNextSibling(duPlugin *pNextSibling) { m_pNextSibling = pNextSibling; }
	virtual void WINAPI SetParam(INT_PTR lParam) { m_lParam = lParam; }
	virtual void WINAPI SetCachedBitmap(HBITMAP hBitmap);
	
public:
	virtual void WINAPI Resize(LPRECT lpRect/*=NULL*/);
	virtual duPlugin *WINAPI GetPluginByName(LPCTSTR lpszName);

	virtual void WINAPI OnMouseIn(POINT pt) {} //{ _TRACE(_T("OnMouseIn %s\n"), GetName()); }
	virtual void WINAPI OnMouseLeave(POINT pt) {} //{ _TRACE(_T("OnMouseLeave %s\n"), GetName()); }
	virtual void WINAPI OnMouseLDown(POINT pt) {} //{ _TRACE(_T("OnMouseLDown %s\n"), GetName()); }
	virtual void WINAPI OnMouseLUp(POINT pt) {} //{ _TRACE(_T("OnMouseLUp %s\n"), GetName()); }
	virtual void WINAPI OnMouseRDown(POINT pt) {} //{ _TRACE(_T("OnMouseRDown %s\n"), GetName()); }
	virtual void WINAPI OnMouseRUp(POINT pt) {} //{ _TRACE(_T("OnMouseRUp %s\n"), GetName()); }
	virtual void WINAPI OnMouseHover(POINT pt) {} //{ _TRACE(_T("OnMouseHOver %s\n"), GetName()); }
	virtual void WINAPI OnMouseMove(POINT pt) {} //{ _TRACE(_T("OnMouseMove %s\n"), GetName()); }
	virtual void WINAPI OnMouseDbClick(POINT pt) {} //{ _TRACE(_T("OnMouseDbClick %s\n"), GetName()); }
	virtual void WINAPI OnMouseWheel(UINT fwKeys, int zDelta, POINT pt) {} //{ _TRACE(_T("OnMouseWheel %s\n"), GetName()); }
	virtual void WINAPI OnCaptureChanged(duPlugin *newCaptured) {} //{ _TRACE(_T("OnCaptureChanged %s\n"), GetName()); }
	virtual BOOL WINAPI OnKillFocus(duPlugin *pNewFocus) { return FALSE; } //{ _TRACE(_T("OnKillFocus %s\n"), GetName()); }
	virtual BOOL WINAPI OnSetFocus(duPlugin *pOldFocus) { return FALSE; } //{ _TRACE(_T("OnSetFocus %s\n"), GetName()); }
	virtual void WINAPI OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {} //{ _TRACE(_T("OnKeyDown %s\n"), GetName()); }
	virtual void WINAPI OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {} //{ _TRACE(_T("OnKeyUp %s\n"), GetName()); }
	virtual void WINAPI OnTimer(UINT nEventId) {}
	virtual void WINAPI OnChildShow(duPlugin *pChild, BOOL fVisible) {}
	virtual void WINAPI OnChildResize(duPlugin *pChild) {}
	virtual void WINAPI OnRedraw() {}
	virtual void WINAPI OnVScroll(ScrollBarAction sbAction, int nPos) {}
	virtual void WINAPI OnHScroll(ScrollBarAction sbAction, int nPos) {}
	virtual LRESULT WINAPI OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return 0; }
	virtual BOOL WINAPI OnSetCursor();
	virtual void WINAPI OnChildDelete(duPlugin *pChild) {}

	virtual void WINAPI RegisterControlProperty() = 0;
	virtual void WINAPI OnCreate() = 0;
	virtual void WINAPI DrawObject(HDC hDC) = 0;

	virtual void WINAPI FinalRelease() { delete this; }
	virtual LPCTSTR WINAPI GetTypeInfoName() { return _T("duPlugin"); }

public:
	BOOL OnCreate(TiXmlElement *pElement, duCtrlManager *pCtrlManager);
	BOOL CreateChildPlugin(TiXmlElement *pElement, duCtrlManager *pCtrlManager);
	BOOL CallProperty(TiXmlElement *pElement);
	TiXmlElement *GetXmlElement();
	BOOL CallScript(duPlugin *pPluginFrom, UINT uMsg, WPARAM wParam, LPARAM lParam);
	int WINAPI GetPropertyCount();
	LPCTSTR WINAPI GetPropertyName(int nNameIndex);
	int WINAPI GetChildCount();	
	BOOL WINAPI RegisterProperty(LPCTSTR lpszPropName,  duVARIANT_TYPE vt, void *duVariant);
	duResBase *WINAPI GetResObj(LPCTSTR lpszResName, UINT nResType);
	HBITMAP WINAPI GetBkHBitmap();
	HBITMAP WINAPI GetSnapshot();
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
duPlugin *WINAPI Plugin_SetParent(duPlugin *pPlugin, duPlugin *pParent, LPCTSTR lpszPre);
BOOL WINAPI Plugin_IsValid(duPlugin *pPlugin);
void WINAPI Plugin_AddValid(duPlugin *pPlugin);
void WINAPI Plugin_DeleteValid(duPlugin *pPlugin);
BOOL WINAPI Plugin_IsVisible(duPlugin *pPlugin);
BOOL WINAPI Plugin_GetVisibleRect(duPlugin *pPlugin, LPRECT lpRect);
BOOL WINAPI Plugin_Redraw(duPlugin *pPlugin, BOOL fNotify);
BOOL WINAPI Plugin_SetCached(duPlugin *pPlugin, BOOL fCached);

BOOL WINAPI Plugin_GetType(duPlugin *pPlugin, LPTSTR lpString, int nMaxCount);
BOOL WINAPI Plugin_GetName(duPlugin *pPlugin, LPTSTR lpString, int nMaxCount);
BOOL WINAPI Plugin_GetStyle(duPlugin *pPlugin, LPTSTR lpString, int nMaxCount);
BOOL WINAPI Plugin_GetCursor(duPlugin *pPlugin, LPTSTR lpString, int nMaxCount);
int WINAPI Plugin_GetTextLength(duPlugin *pPlugin);
BOOL WINAPI Plugin_GetText(duPlugin *pPlugin, LPTSTR lpString, int nMaxCount);
void WINAPI Plugin_GetRect(duPlugin *pPlugin, LPRECT lpRect);
void WINAPI Plugin_SetFocus(duPlugin *pPlugin, BOOL fFocus);
void WINAPI Plugin_SetVisible(duPlugin *pPlugin, BOOL fVisible);
void WINAPI Plugin_Resize(duPlugin *pPlugin, LPRECT lpRect);

UINT WINAPI Plugin_GetState(duPlugin *pPlugin);
void WINAPI Plugin_SetState(duPlugin *pPlugin, UINT uState);

BOOL WINAPI Plugin_SetTimer(duPlugin *pPlugin, UINT uTimerId, UINT uElapse);
BOOL WINAPI Plugin_KillTimer(duPlugin *pPlugin, UINT uTimerId);

duPlugin *WINAPI Plugin_GetCapture(duPlugin *pPlugin);
duPlugin *WINAPI Plugin_SetCapture(duPlugin *pPlugin);
BOOL WINAPI Plugin_ReleaseCapture(duPlugin *pPlugin);

BOOL WINAPI Plugin_HookWindowMessage(duPlugin *pPlugin);
BOOL WINAPI Plugin_UnHookWindowMessage(duPlugin *pPlugin);

BOOL WINAPI Plugin_Delete(duPlugin *pPlugin);
duPlugin *WINAPI Plugin_Clone(duPlugin *pTemplate, duPlugin *pParent, duPlugin *pPrevious, UINT nSuffix);
BOOL WINAPI Plugin_SetDisable(duPlugin *pPlugin, BOOL fDisable);

void WINAPI DrawByStyle(duPlugin *pPlugin, LPCTSTR lpszStyle, HDC hDC, LPRECT lpDstRect, UINT uState, LPCTSTR lpszText, int nAlpha);
UINT64 WINAPI Plugin_GetParam64(duPlugin *pPlugin);
void WINAPI Plugin_SetParam64(duPlugin *pPlugin, UINT64 nlParam);
void WINAPI Plugin_SetTabOrder(duPlugin *pPlugin, int nTabOrder);
int WINAPI Plugin_GetTabOrder(duPlugin *pPlugin);
}
